//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//libraries for mSD
#include <mySD.h>

//Libraries for ablocks
#include "ABlocks_TinyGPS.h"
#include <HardwareSerial.h>
#include <Adafruit_BMP280.h>
//#include "ABlocks_DHT.h"
#include "ADXL345.h"

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 867.5E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

//creacion de pantalla (objeto display)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//objetos y variables para mSD
ext::File myFile;
const int chipSelect = 13;

//contador de tiempo
int counter = 0;

//variables creadas por ablocks
double presion;
double altitud;
double temperatura;
//double humedad;
double longitud;
double latitud;
double velocidad;
//String s_aceleracion_Z;
String s_aceleracion_Z;
String s_registro;
TinyGPS gps;
float gps_lat = 0;
float gps_long = 0;
float gps_speed_kmph = 0;
float gps_speed_mph = 0;
float gps_altitude = 0;
float gps_course = 0;
bool gps_fixed = false;
byte gps_day = 0;
byte gps_month = 0;
int gps_year = 0;
byte gps_hour = 0;
byte gps_min = 0;
byte gps_sec = 0;
byte gps_hund = 0;
HardwareSerial &gps_serial = Serial;
Adafruit_BMP280 bmp280;
ADXL345 adxl345_accel;
//DHT dht2(34, DHT22);




//funciones

void envia_lora() {  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(s_registro);
  LoRa.endPacket();
}

void envia_oled() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("IES FGL");
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("Paquete LoRa enviado.");
  display.setCursor(0, 30);
  display.print("Contador: ");
  display.setCursor(60, 30);
  display.print(counter);
  display.display();
}

//funciones creadas en ablocks
void fnc_gps_update() {
  unsigned long fix_age;
  while (gps_serial.available()) {
    if (gps.encode(gps_serial.read())) {
      gps_fixed = false;
      gps_lat = gps_long = gps_speed_kmph = gps_speed_mph = gps_course = 0;
      gps_day = gps_month = gps_year = gps_hour = gps_min = gps_sec = 0;
      gps.f_get_position(&gps_lat, &gps_long, &fix_age);
      if (fix_age != TinyGPS::GPS_INVALID_AGE && fix_age < 5000) {
        gps_fixed = true;
        gps.crack_datetime(&gps_year, &gps_month, &gps_day, &gps_hour, &gps_min, &gps_sec, &gps_hund, &fix_age);
        gps_altitude = gps.f_altitude();
        gps_course = gps.f_course();
        gps_speed_kmph = gps.f_speed_kmph();
        gps_speed_mph = gps.f_speed_mph();
      }
    }
  }
}

double fnc_adxl345(int _type)
{
  double v=0.0;
  if(_type>=1 && _type<=3){
    //raw data
    Vector data_raw = adxl345_accel.readRaw();
    if(_type==1)v=data_raw.XAxis;
    if(_type==2)v=data_raw.YAxis;
    if(_type==3)v=data_raw.ZAxis;
  }
  else{
    //noramlized data g=gravity
    Vector data_norm = adxl345_accel.readNormalize();
    if(_type>=4 && _type<=6){
      if(_type==4)v=data_norm.XAxis;
      if(_type==5)v=data_norm.YAxis;
      if(_type==6)v=data_norm.ZAxis;
    }
    else{
      //pitch,roll degrees
      Vector data_filtered = adxl345_accel.lowPassFilter(data_norm, 0.5);
      if(_type==7)v=(atan2(data_filtered.YAxis, data_filtered.ZAxis)*180.0)/M_PI;
      if(_type==8)v=-(atan2(data_filtered.XAxis, sqrt(data_filtered.YAxis*data_filtered.YAxis + data_filtered.ZAxis*data_filtered.ZAxis))*180.0)/M_PI;
    }
  }
  return v;
}  


void leesensores() {
  presion = (bmp280.readPressure() / 100.0);
  altitud = bmp280.readAltitude();
  temperatura = bmp280.readTemperature();
//  humedad = dht2.readHumidity();
  if (gps_fixed) {
    longitud = gps_long;
    latitud = gps_lat;
    velocidad = gps_speed_kmph;
  }
  	if (false) {
		s_aceleracion_Z = String(fnc_adxl345(6),2);
	}
  s_registro = String(counter) + String(";") + String(presion) + String(";") + String(altitud) + String(";") + String(temperatura) + String(";") + String(longitud) + String(";") + String(latitud) + String(";") + String(velocidad) + String(";") +String(s_aceleracion_Z) + String(";");
}

void envia_sd() {
  myFile = SD.open("datos.csv", FILE_WRITE);
  if (myFile) {
    myFile.println(s_registro);
    // close the file:
    myFile.close();
  }
}

void setup() {
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {  // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA SENDER ");
  display.display();

  /*initialize Serial Monitor
  Serial.begin(115200);
  
  Serial.println("Prueba de envio LoRa");*/

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    display.setCursor(0, 10);
    display.print("Starting LoRa failed!");
    display.display();
    //Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  //Serial.println("LoRa Inicializacion OK!");
  display.setCursor(0, 10);
  display.print("Inicializacion OK!");
  display.display();
  delay(2000);

  //setup mSD
  pinMode(SS, OUTPUT);
  SD.begin(13, 15, 2, 14);  //definición de pines CS,MOSI,MISO,SCK

  //setup de ablocks
  pinMode(2, INPUT);

  bmp280.begin(0x76);
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);
  //dht2.begin();
  adxl345_accel.begin(); adxl345_accel.setRange(ADXL345_RANGE_16G);

  gps_serial.begin(9600);
  gps_serial.println("$PMTK220,1000*1F");
}

void loop() {
  fnc_gps_update();
  leesensores();
  envia_lora();
  envia_oled();
  envia_sd();
  counter++;
  delay(1000);
}
