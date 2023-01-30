/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/
#define BLYNK_PRINT Serial
// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLBd7N7Ekt"
#define BLYNK_DEVICE_NAME "AUTOMATIC FISH FEEDER"
#define BLYNK_AUTH_TOKEN "F5tea000taml3hPu9BJMqQUxISo2baIq"

// DEFINISI LIBRARY YG AKAN DIGUNAKAN
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <OneWire.h>
#include <DallasTemperature.h>


char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// MENDEFINISIKAN PIN YANG AKAN DIPAKAI
#define POMPA_ISI   D3
#define POMPA_KURAS D4
#define POMPA_PAKAN D5

//#define BUZZER  D10
//#define pinTurb A0

WiFiManager wifiManager;

#define echoPin1 D8 // Echo Pin
#define trigPin1 D0 // Trigger Pin
#define echoPin2 D6 // Echo Pin
#define trigPin2 D7 // Trigger Pin

long duration1; // Duration used to calculate distance
long duration2; // Duration used to calculate distance




// int waktu = 0;
// int mulai = 0;
BlynkTimer timer;
// bool UV = 0, PIR = 0, STAT = 0;


int tinggi_sensor_air = 31;
int tinggi_sensor_pakan = 13;
int tinggi_kolam = 24;
int batas_isi = 22;
int batas_kuras = 26;
int kuras = 0;
bool otomatis = 1;
bool pompa_isi = 0;
bool pompa_kuras = 0;
bool pompa_pakan = 0;
long tinggi_air = 0;
long sensor_air = 0;
long tinggi_pakan = 0;
long sensor_pakan = 0;
float tempC = 0;



BLYNK_WRITE(V2)
{
  // Set incoming value from pin V8 to a variable
  int value = param.asInt();
  Blynk.virtualWrite(V2, value);
  if ((value == 1) && (otomatis == 0))
  {
    pompa_isi = 1;
    digitalWrite(POMPA_ISI,   LOW);//pompa nyala
  }
  else
  {
    pompa_isi = 0;
    digitalWrite(POMPA_ISI,   HIGH);//pompa mati
  }
}

BLYNK_WRITE(V3)
{
  // Set incoming value from pin V8 to a variable
  int value = param.asInt();
  Blynk.virtualWrite(V3, value);
  if ((value == 1) && (otomatis == 0))
  {
    pompa_kuras = 1;
    digitalWrite(POMPA_KURAS, LOW);//pompa nyala
  }
  else
  {
    pompa_kuras = 0;
    digitalWrite(POMPA_KURAS, HIGH);//pompa mati
  }
}

BLYNK_WRITE(V4)
{
  // Set incoming value from pin V8 to a variable
  int value = param.asInt();
  Blynk.virtualWrite(V5, value);
  if (value == 1)
  {
    pompa_pakan = 1;
    digitalWrite(POMPA_PAKAN, LOW);//pompa nyala
  }
  else
  {
    pompa_pakan = 0;
    digitalWrite(POMPA_PAKAN, HIGH);//pompa mati
  }
}

BLYNK_WRITE(V6)
{
  // Set incoming value from pin V8 to a variable
  int value = param.asInt();

  if (value == 1)
  {
    otomatis = 1;
  }
  else if (value == 0)
  {
    otomatis = 0;
  }
}

BLYNK_WRITE(V7)
{
  // Set incoming value from pin V6 to a variable
  int value = param.asInt();

  if (value == 1)
  {
    Serial.print("MEMULAI ULANG   ");
    Serial.print("PERANGKAT       ");
    delay(1000);
    ESP.reset();
    delay(1000);
  }
}

BLYNK_WRITE(V8)
{
  // Set incoming value from pin V8 to a variable
  int value = param.asInt();

  if (value == 1)
  {
    cek_sonar();
    tinggi_sensor_air = sensor_air;
    tinggi_sensor_pakan = sensor_pakan;
  }
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  Blynk.virtualWrite(V0, tinggi_pakan);
  Blynk.virtualWrite(V1, tinggi_air);
  Blynk.virtualWrite(V2, pompa_isi);
  Blynk.virtualWrite(V3, pompa_kuras);
  Blynk.virtualWrite(V4, pompa_pakan);
  Blynk.virtualWrite(V5, tempC);
  Blynk.virtualWrite(V6, otomatis);
  Serial.print("TERHUBUNG... ^_^");
}

void cek_sonar()
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  sensor_air = duration1 / 58.2;
  //  Serial.print("sonar 2 = ");
  //  Serial.print(sensor_air);
  //Delay 50ms before next reading.
  delay(50);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  sensor_pakan = duration2 / 58.2;
  //  Serial.print("sonar 2 = ");
  //  Serial.println(sensor_pakan);
  delay(50);
  tinggi_air = tinggi_sensor_air - sensor_air;
  tinggi_pakan = tinggi_sensor_pakan - sensor_pakan;
  cek_suhu();

}
// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{


  //  if(tinggi_air <= 0)
  //  {
  //   tinggi_air = 0;
  //  }
  //  if(tinggi_pakan <= 0)
  //  {
  //   tinggi_pakan = 0;
  //  }

  cek_sonar();

  if (otomatis == 1)
  {
    if (tinggi_air <= batas_isi)
    {
      pompa_isi = 1;
      pompa_kuras = 0;
      digitalWrite(POMPA_ISI, LOW);//pompa isi nyala
      digitalWrite(POMPA_KURAS, HIGH);//pompa kuras mati
    }
    else if (tinggi_air >= batas_kuras)
    {
      pompa_isi = 0;
      pompa_kuras = 1;
      digitalWrite(POMPA_ISI, HIGH);//pompa isi mati
      digitalWrite(POMPA_KURAS, LOW);//pompa kuras nyala
    }
    else if (tinggi_air == tinggi_kolam)
    {
      pompa_isi = 0;
      pompa_kuras = 0;
      digitalWrite(POMPA_ISI, HIGH);//pompa isi mati
      digitalWrite(POMPA_KURAS, HIGH);//pompa kuras mati
    }
  }

  Serial.print("Suhu air :");
  Serial.print(tempC);
  Serial.println(" °C");
  Serial.print("sensor_pakan :");
  Serial.println(sensor_pakan);
  Serial.print("tinggi_air :");
  Serial.println(tinggi_air);
  Serial.print("tinggi_pakan :");
  Serial.println(tinggi_pakan);

  Blynk.virtualWrite(V0, tinggi_pakan);
  Blynk.virtualWrite(V1, tinggi_air);
  Blynk.virtualWrite(V2, pompa_isi);
  Blynk.virtualWrite(V3, pompa_kuras);
  Blynk.virtualWrite(V4, pompa_pakan);
  Blynk.virtualWrite(V5, tempC);
  Blynk.virtualWrite(V6, otomatis);



}

void cek_suhu()
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  tempC = sensors.getTempCByIndex(0);

  if (tempC != DEVICE_DISCONNECTED_C)
  {
    //Serial.print("Temperature for the device 1 (index 0)");

  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("SMART FISH FEEDER");
  Serial.println("MENGHUBUNGKAN...");

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect("FISH_FEEDER_AP")) {
    Serial.println("GAGAL TERHUBUNG");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected to the network... )");
  pinMode(POMPA_ISI,   OUTPUT);
  pinMode(POMPA_KURAS, OUTPUT);
  pinMode(POMPA_PAKAN, OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  digitalWrite(POMPA_ISI,   HIGH);
  digitalWrite(POMPA_KURAS, HIGH);
  digitalWrite(POMPA_PAKAN, HIGH);
  sensors.begin();
  delay(1000);
  Blynk.begin(auth, ssid, pass);

  //timer.setInterval(5000L, cek_sonar);
  timer.setInterval(1000L, myTimerEvent);

}

void loop()
{
  Blynk.run();
  timer.run();
}

//JIKA ALAT BELUM DAPAT TERKONEKSI WIFI MAKA PERLU MENGESET WIFI YANG AKAN DIHUBUNGKAN
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.print("BUKA AKSES POINT");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  //  Serial.println(myWiFiManager->getConfigPortalSSID());
}
