#include "ABlocks_TinyGPS.h"
#include <HardwareSerial.h>

String s_longitud;
String s_latitud;
String s_velocidad;
TinyGPS gps;
float gps_lat=0;
float gps_long=0;
float gps_speed_kmph=0;
float gps_speed_mph=0;
float gps_altitude=0;
float gps_course=0;
bool gps_fixed=false;
byte gps_day=0;
byte gps_month=0;
int gps_year=0;
byte gps_hour=0;
byte gps_min=0;
byte gps_sec=0;
byte gps_hund=0;
HardwareSerial &gps_serial=Serial2;

void fnc_gps_update()
{
	unsigned long fix_age;
	while (gps_serial.available()){
		if(gps.encode(gps_serial.read())){
			gps_fixed=false;
			gps_lat=gps_long=gps_speed_kmph=gps_speed_mph=gps_course=0;
			gps_day=gps_month=gps_year=gps_hour=gps_min=gps_sec=0;
			gps.f_get_position(&gps_lat, &gps_long, &fix_age);
			if (fix_age != TinyGPS::GPS_INVALID_AGE && fix_age<5000){
				gps_fixed=true;
				gps.crack_datetime(&gps_year, &gps_month, &gps_day,&gps_hour, &gps_min, &gps_sec, &gps_hund, &fix_age);
				gps_altitude=gps.f_altitude();
				gps_course=gps.f_course();
				gps_speed_kmph=gps.f_speed_kmph();
				gps_speed_mph=gps.f_speed_mph();
			}
		}
	}
}

void tomar_datos() {
	if (gps_fixed) {
		s_longitud = String(gps_long,6);
		s_latitud = String(gps_lat,6);
		s_velocidad = String(gps_speed_kmph,2);
	}

}

void setup()
{


	gps_serial.begin(9600);
	gps_serial.println("$PMTK220,1000*1F");

}


void loop()
{
	yield();

	fnc_gps_update();
  	tomar_datos();

}