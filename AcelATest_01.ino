#include <Wire.h>
#include "ADXL345.h"

String s_aceleracion_Z;
ADXL345 adxl345_accel;

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

void tomar_datos() {
	if (false) {
		s_aceleracion_Z = String(fnc_adxl345(6),2);
	}

}

void setup()
{


	adxl345_accel.begin(); adxl345_accel.setRange(ADXL345_RANGE_16G);

}


void loop()
{
	yield();

  	tomar_datos();

}