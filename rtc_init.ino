/*
 <datalogger.ino> Copyright (C) <2015> <Sneha Kopallu>,<Prasad Mehendale> 
         This program comes with ABSOLUTELY NO WARRANTY; 
         for details, refer to the GPL-3.0.txt on www.gnu.org .
         This is free software, and you are welcome to redistribute 
         it under the conditions of GNU-GPL V3.0
*/
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include "DS1307.h" // written by  mattt on the Arduino forum and modified by D. Sjunnesson
//#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#define edit_date 0x00
#define edit_month 0x01
#define edit_year 0x02
#define edit_hrs  0x03
#define edit_mins  0x04
#define edit_sec 0x05
#define init_rtc 0x06
#define discard_process 0x07
#define display_datetime 0x08
#define edit_datetime 0x09

#define ENTER 0x07
#define BACK 0x0B
#define DECR  0x0D
#define INCR  0x0E
//inital process
int process= edit_datetime;

//define output pins
const int relay1= 9;
const int relay2= 10;
//keys for time setting
const int dec=A3;
const int inc=A2;
const int back=A1;
const int enter=A0;

int hr,minu,sec,date,month,yr;
int datetime[6]={
  0,0,0,0,0,0};
  
int read_keys()
{
  int key_status;
  key_status = PINC;
  key_status &= 0x0F;
  if(key_status != 0x0F)
  {
    delay(10);
    if((PINC & 0x0F) == key_status)
    {
      if(((PINC & 0x0F) == ENTER) ||((PINC & 0x0F) ==  BACK))
      {while((PINC & 0xF) != 0x0F);}else{delay(200);}
      
      return(key_status);
      
    }
    else
      return(0x0F);
  }
  return(0x0F);  
}
void setup()
{
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();
  lcd.print("    Suntime    ");
  lcd.setCursor(0,1);
  lcd.print("SET Date & Time");

  //pins as an input 
  pinMode(enter,INPUT);
  pinMode(back,INPUT);
  pinMode(inc,INPUT);
  pinMode(dec,INPUT);


  delay(2000);  
}

void loop()
{
  byte week,days,total_days ;
  byte port_c,curpos;
  byte date=0;
  char buf[50];
  int key=0;
  int i=0;
  // byte edit_date=1;
   
 
      lcd.setCursor(0,0);
    
     lcd.print("Date:00/00/00  ");
     lcd.setCursor(0,1);
     lcd.print("Time=00:00:00   "); 
    while(process == edit_datetime){
      
      key = read_keys() & 0x0F;
      

      switch(key)
      {
      case INCR:
      {
        datetime[i]++;
        
        switch(i)
        {
        case edit_date:
          if(datetime[i] > 31)
            datetime[i]=0;
          break;

        case edit_month:
          if(datetime[i] > 12)
            datetime[i]=0;
          break;
        case edit_year:
          if(datetime[i] > 99)
            datetime[i]=0;
          break;
        case edit_hrs:
          if(datetime[i] > 23)
            datetime[i]=0;
          break;
        case edit_mins:
          if(datetime[i] > 59)
            datetime[i]=0;
          break;
        case edit_sec:
          if(datetime[i] > 59)
            datetime[i]=0;
          break;
        }
      }
        break;

      case DECR:
      {
        if(datetime[i] <= 0)
        {
          switch(i)
          {
          case edit_date:
            datetime[i]=31;
            break;       
          case edit_month:
            datetime[i]=12;
            break;       
          case edit_year:       
            datetime[i]=99;
            break;
          case edit_hrs:
            datetime[i]=23;
            break;
          case edit_mins:
            datetime[i]=59;
            break;
          case edit_sec:
            datetime[i]=59;
            break;
          }
        }
        else
          datetime[i]--;
      }
        break;

      case BACK:
      {
        if(i<=0){
        process=display_datetime;
        lcd.clear();
        }
        else
          i--;
      }
        break;

      case ENTER:
      {
        i++;
        if(i>=6)
        process=init_rtc;
        
      }break;
        
      default:
        break; 
      }
      if(i>=0 && i<=2)
      {
      lcd.setCursor((5+(i*3)),0);
      }
      else
      {
      
      lcd.setCursor((5+((i-3)*3)),1);
    
      }
      if(datetime[i] < 10)
     // lcd.setCursor(5,0);
      lcd.print('0');
      
      lcd.print(datetime[i]);
   
       if(i>=0 && i<=2)
      {
      lcd.setCursor((5+(i*3))+1,0);
      }
      else
      {
      
      lcd.setCursor((5+((i-3)*3))+1,1);
    
      }
      lcd.cursor();
    }
  
  
  if(process==init_rtc)
  {
      lcd.clear();
      lcd.print("ENTER=Initialize");
      lcd.setCursor(0,1);
      lcd.print("BACK=Discard    "); 
      delay(500);
  }
  while(process == init_rtc)
  {
    
    key=(read_keys() & 0x0F);
    switch(key)
    {
      case ENTER:
    {
      lcd.clear();
      lcd.print("Initializing RTC");
      lcd.setCursor(0,1);
      lcd.print("      wait      ");
       RTC.stop();
    RTC.set(DS1307_SEC,datetime[5]);        //set the seconds
    RTC.set(DS1307_MIN,datetime[4]);     //set the minutes
    RTC.set(DS1307_HR,datetime[3]);       //set the hours
    RTC.set(DS1307_DOW,1);       //set the day of the week
    RTC.set(DS1307_DATE,datetime[0]);       //set the date
    RTC.set(DS1307_MTH,datetime[1]);        //set the month
    RTC.set(DS1307_YR,datetime[2]);         //set the year
    RTC.start();
    }
    delay(1000);
    lcd.clear();
    process = display_datetime;
    break;
    case BACK:
    process = display_datetime;
    break;
    default :
   // process=init_rtc;
    break;
    }
  }
  //---------------------------------------------------------------
  while(process == display_datetime)
  {      //get the time from RTC
    lcd.noCursor(); 
    lcd.setCursor(0,0);
    //lcd.print("Date=");
     date=RTC.get(DS1307_DATE,true);
  //  Serial.println(hr);
    month=RTC.get(DS1307_MTH,false);
 //   Serial.println(minu);
    yr=RTC.get(DS1307_YR,false);
  //  Serial.println(sec);
    //display the time
    lcd.setCursor(0,0);
    lcd.print("Date=");
    lcd.print(date);
    lcd.print("/"); 
    lcd.print(month);
    lcd.print("/");
    lcd.print(yr);
    lcd.print("    ");
    //lcd.print("          ");
    hr=RTC.get(DS1307_HR,true);
  //  Serial.println(hr);
    minu=RTC.get(DS1307_MIN,false);
  //  Serial.println(minu);
    sec=RTC.get(DS1307_SEC,false);
  //  Serial.println(sec);
    //display the time
    lcd.setCursor(0,1);
    lcd.print("Time=");
    lcd.print(hr);
    lcd.print(":"); 
    lcd.print(minu);
    lcd.print(":");
    lcd.print(sec);
    lcd.print(" ");
    // delay(1);   
    delay(1000);    
  }  
  while(1);
}






