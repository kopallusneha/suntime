/*
 <datalogger.ino> Copyright (C) <2015> <Sneha Kopallu>,<Prasad Mehendale> 
         This program comes with ABSOLUTELY NO WARRANTY; 
         for details, refer to the GPL-3.0.txt on www.gnu.org .
         This is free software, and you are welcome to redistribute 
         it under the conditions of GNU-GPL V3.0
*/



#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include "DS1307.h" // written by  mattt on the Arduino forum and modified by D. Sjunnesson
//#include <LiquidCrystal.h>
#define SIX 360
#define EIGHTEEN 1080
//define output pins
const int relay1= 9;
const int relay2= 10;

 // save cumulative days for each month in flash memory 
const PROGMEM  byte cumu_days[13]  = {0,0,15,30,45,60,76,91,106,122,137,152,167};
// save Sunset timings in flash memory
const PROGMEM  byte sunrise[53]  = {  74,75,76,76,74,72,68,64,59,54,
                                      48,43,37,31,26,20,16,12,8,5,3, 
                                      2,2,2,4,5,8,10,12,15,17,20,21,
                                      23,25,26,27,29,30,31,33,35,37,
                                      40,43,47,51,55,60,64,68,71,71 };
                                      
const PROGMEM byte sunset[53] = {     11,16,20,25,29,32,36,39,42,44,
                                      46,47,49,51,53,55,57,59,61,64,
                                      67,69,72,74,76,77,78,77,76,74,
                                      71,68,63,58,53,47,41,35,29,23,
                                      17,12,7,3,0,0,0,0,0,0,3,7,7   };  
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int hr,minu,sec,date,month,yr;
void setup()
{
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  digitalWrite(relay1,LOW);
  digitalWrite(relay2,LOW);
  Serial.begin(9600);
  /* display cursor */
  
//If you want to initialize the RTC uncomment the following statements
//and change the time and date accordingly.
//  RTC.stop();
//  RTC.set(DS1307_SEC,00);        //set the seconds
//  RTC.set(DS1307_MIN,50);     //set the minutes
//  RTC.set(DS1307_HR,12);       //set the hours
//  RTC.set(DS1307_DOW,1);       //set the day of the week
//  RTC.set(DS1307_DATE,29);       //set the date
//  RTC.set(DS1307_MTH,12);        //set the month
//  RTC.set(DS1307_YR,15);         //set the year
//  RTC.start();
  delay(1000);  
}

void loop()
{
  byte week,days,total_days ;
  byte sunset_min,sunrise_min;
  int sunset_time,sunrise_time;
  int hrs2min,current_time;
  byte sunset_flag,sunrise_flag;
  
    while(1)
    {      
      
      //get the date from RTC
        
          date=RTC.get(DS1307_DATE,true);
          month=RTC.get(DS1307_MTH,true);
          yr=RTC.get(DS1307_YR,false);          
          //send the date and time on serial monitor
          Serial.print("\n");
          Serial.println(date);
          Serial.print("/");
          Serial.print(month);
          Serial.print("/");
          Serial.print(yr);
          //read the cumulative days for the running month      
          total_days = pgm_read_byte(&(cumu_days[month])); 
          Serial.print("\nCumulative days=");
          Serial.print(total_days);
          days=date/2;
          Serial.print("\ndays=");
          Serial.print(days);
          total_days += days;
          Serial.print("\nTotal days=");
          Serial.print(total_days);
          //find the week no. 
          week = (total_days/7)*2;
          if((total_days%7)>=4)
          week +=1;
          Serial.print("\nWEEK=");
          Serial.print(week);                   
          //find Sunset and Sunrise timing
          sunrise_min=pgm_read_byte(&(sunrise[week])); 
          Serial.print("\nSunrise_min=");
          Serial.print(sunrise_min);  
          sunset_min=pgm_read_byte(&(sunset[week])); 
          Serial.print("\nSunset_min=");
          Serial.print(sunset_min);  
          //find the Sunset and sunrise timings for current day
          sunrise_time = SIX + sunrise_min;
          Serial.print("\nSunrise_time=");
          Serial.print(sunrise_time); 
          sunset_time = EIGHTEEN + sunset_min;
          Serial.print("\nSunset_time=");
          Serial.print(sunset_time);
          
          // get the current time from RTC
          hr=RTC.get(DS1307_HR,true);       
          minu=RTC.get(DS1307_MIN,false);          
          sec=RTC.get(DS1307_SEC,false);
          // Convert the hours to minutes
          hrs2min = 60*hr;
          Serial.print("\nHours=");
          Serial.print(hr);
          Serial.print("\nHours to minutes=");
          Serial.print(hrs2min);
          current_time = hrs2min+minu;
          Serial.print("\current time=");
          Serial.print(current_time);
          //compare current time with Sunrise and sunset timings
          if(current_time < sunrise_time)          
             sunrise_flag = HIGH;           
          else
            sunrise_flag = LOW;
          if(current_time < sunset_time)
            sunset_flag = HIGH;
          else
            sunset_flag = LOW;
            if((sunset_flag ^ sunrise_flag)== HIGH)
            {
              Serial.print("\n Day time ***************");
              Serial.print("\n RELAY OFF");
              digitalWrite(relay1,LOW);
              digitalWrite(relay2,LOW);
            }
            else
            {
                Serial.print("\n Night time############");
                Serial.print("\n RELAY ON");
                digitalWrite(relay1,HIGH);
                digitalWrite(relay2,HIGH); 
            }
            
//           
//          Serial.print("\n");
//          Serial.print(hr);
//          Serial.print(":");
//          Serial.print(minu);
//          Serial.print(":");
//          Serial.print(sec);
//          Serial.print("\n");
            delay(1000);        
    }  
             
}


