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
//#define BAND 866E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

//array o lista de frecuencias
unsigned long frecuencias[] = { 866E6, 866.1E6 };        // aquí se colocan separadas por coma las frecuencias rastreadas, creando un array
int size = sizeof(frecuencias) / sizeof(unsigned long);  // calcula el nº de elementos del array

//contador de nº de envíos realizados
int counter = 0;

//creacion de pantalla (objeto display)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//funciones

void envia_lora() {  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("problema de seguridad en comunicaciones");
  LoRa.endPacket();
}

void envia_oled_exito() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Club Robotica Granada");
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("Enviando a pardillos...");
  display.setCursor(0, 30);
  display.print("Contador de envíos: ");
  display.setCursor(60, 30);
  display.print(counter);
  display.display();
}

void envia_oled_fallo() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Club Robotica Granada");
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("Comunicación LoRa falla...");
  display.display();
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
}

void loop() {
  for (int i = 0; i < size; i++) {

    if (!LoRa.begin(frecuencias[i])) {
      envia_oled_fallo();
    } else {
      envia_oled_exito();
      envia_lora();
    }
  }
  delay(1000);//espera 1 segundo después de cada envío a todos los receptores
  counter++;//se incrementa en uno el contador de envíos
}
