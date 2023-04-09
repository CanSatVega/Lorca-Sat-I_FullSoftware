//Libraries for ablocks
#include <Adafruit_BMP280.h>
#include "ABlocks_TinyGPS.h"
#include <HardwareSerial.h>


//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>


//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
#define BAND 866E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

int counter=0;

//variables de sensores
double temperatura;
double presion;
double altitud;
Adafruit_BMP280 bmp280;


//creacion de pantalla (objeto display)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//funcion de tomas de datos
void tomar_datos() {
	temperatura = bmp280.readTemperature();
	presion = (bmp280.readPressure()/100.0);
	altitud = bmp280.readAltitude();
}

void envia_lora(){//Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(counter);
  LoRa.print(',');
  LoRa.print(temperatura);
  LoRa.print(',');
  LoRa.print(presion);
  LoRa.print(',');
  LoRa.print(altitud);
  LoRa.endPacket();
}

void envia_oled(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("IESFGL");
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Paquete LoRa enviado.");
  display.setCursor(0,30);
  display.print("Contador: ");
  display.setCursor(60,30);
  display.print(counter);      
  display.display();

}


void setup() {
  //configuracion de sensor bmp 280
  bmp280.begin(0x76);
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,Adafruit_BMP280::SAMPLING_X2,Adafruit_BMP280::SAMPLING_X16,Adafruit_BMP280::FILTER_X16,Adafruit_BMP280::STANDBY_MS_500);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER ");
  display.display();
  
  //initialize Serial Monitor
  Serial.begin(115200);
  
  Serial.println("Prueba de envio LoRa");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Inicializacion OK!");
  display.setCursor(0,10);
  display.print("Inicializacion OK!");
  display.display();
  delay(2000);
}

void loop() {
  yield();
  	tomar_datos(); 
  envia_lora();
  envia_oled();

  counter++;
  delay(1000);
}