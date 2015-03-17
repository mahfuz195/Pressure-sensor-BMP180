/*
Reading Two Barometric Pressure Sensor BMP180 using MUX 74HC4052
Author : Mahfuz

The Mux can be found here : https://www.sparkfun.com/products/retired/9907

Hardware connections: 
using Arduino Mega

SCL		-> MUX PIN 3
SDA		-> MUX PIN 13
PIN 19	-> MUX PIN 10 with a pull down 10K resistor.

PIN 19 is used for selecting one of the pressure sensor.

MUX PIN 6 , 7 , 8 , 9 is connected to GND.

*/
#include <SFE_BMP180.h>
#include <Wire.h>
#include "typedef.h"

#define SELECT_PIN 19
#define ALTITUDE 510.0


#define P1_ACTIVE digitalWrite(SELECT_PIN,HIGH); //delay(10);
#define P2_ACTIVE digitalWrite(SELECT_PIN,LOW);  //delay(10);

SFE_BMP180 bmp1,bmp2;
boolean readSensors(Data *data1, Data *data2);

Data newData(char id)
{
  Data d ; 
  d._id = id ;
  return d ;
}
void setup()
{
	boolean failed = false ;
	pinMode(SELECT_PIN, OUTPUT);
	
	Serial.begin(9600);
		
	P1_ACTIVE
	
	if(bmp1.begin()){
		Serial.println("BMP #1 init success.");
	}
	else 
		  failed = true ;

 
	P2_ACTIVE
	if(bmp2.begin()){
		  Serial.println("BMP #2 init success.");
	}
	else 
		failed = true ;

	if(failed){
		Serial.println("Error in BMP180 init.");
		while (1);
	}
}

void loop()
{
  Data d1 = newData('1');
  Data d2 = newData('2');
  
  boolean read = readSensors(&d1,&d2);
  
  
  if(read){
	  /* print the value to serial terminal */
	  Serial.println();
	  Serial.print("provided altitude: ");
	  Serial.print(ALTITUDE,0);
	  Serial.print(" meters, ");
	  Serial.print(ALTITUDE*3.28084,0);
	  Serial.println(" feet");

	  /* Pressure Sensor #1 value */
	  // Print out the measurement:
	  Serial.println("Sensor #1 Data:");
          Serial.print("temperature: ");
	  Serial.print(d1._temp_value,2);
          Serial.print(" deg C, ");
          Serial.print((9.0/5.0)*d1._temp_value+32.0,2);
          Serial.println(" deg F");
	  
	  // Print out the measurement:
	  Serial.print("absolute pressure: ");
	  Serial.print(d1._pres_value,2);
	  Serial.print(" mb, ");
	  Serial.print(d1._pres_value*0.0295333727,2);
	  Serial.println(" inHg");

	  Serial.println();
	  /* Pressure Sensor #2 value */
	  // Print out the measurement:
	  Serial.println("Sensor #2 Data:");
      
	  Serial.print("temperature: ");
	  Serial.print(d2._temp_value,2);
          Serial.print(" deg C, ");
          Serial.print((9.0/5.0)*d2._temp_value+32.0,2);
          Serial.println(" deg F");
	  
	  // Print out the measurement:
	  Serial.print("absolute pressure: ");
	  Serial.print(d2._pres_value,2);
	  Serial.print(" mb, ");
	  Serial.print(d2._pres_value*0.0295333727,2);
	  Serial.println(" inHg");

	  Serial.println();
  }
  else
	  Serial.println("ERROR");
  
  delay(1000);
}
boolean readSensors(Data *data1, Data *data2)
{
	char status1,status2 ;
	double T1 , T2 , P1, P2;
  
	
	P1_ACTIVE		/* make active 1 */
	status1 = bmp1.startTemperature(); 
  
	P2_ACTIVE		/* make active 2 */ 
	status2 = bmp2.startTemperature();
  
	 if (status1 != 0 && status2!=0){
		// Wait for the measurement to complete:
		delay(max(status1,status2));
		
		P1_ACTIVE		/* make active 1*/ 
		status1 = bmp1.getTemperature(T1);
		
		P2_ACTIVE		/* make active 2*/ 
		status2 = bmp2.getTemperature(T2);
    
		if(status1!=0 && status2!=0){
			/* save the temparature */
			data1->_temp_value = T1 ;
			data2->_temp_value = T2 ;
      
			P1_ACTIVE		/* make active 1*/
			status1 = bmp1.startPressure(3);
			
			P2_ACTIVE		/* make active 2*/
			status2 = bmp2.startPressure(3);
            
			if(status1!=0 && status2!=0){
				
				delay(max(status1,status2));
				
				P1_ACTIVE		/* make active 1*/
				status1 = bmp1.getPressure(P1,T1);
				
				P2_ACTIVE		/* make active 2*/
				status2 = bmp2.getPressure(P2,T2);
				
				
				if(status1!=0 && status2!=0){
					/* save pressure value */
					data1->_pres_value = P1 ; 
					data2->_pres_value = P2 ;
					
					return true ;
				}
				else Serial.println("error reading pressure measurement\n");
			}
			else Serial.println("error starting pressure measurement\n");  
		}
		else Serial.println("error reading temperature measurement\n");  
	 }
	 else Serial.println("error starting temperature measurement\n");  
	 
	 return false ;
}
