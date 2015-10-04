 
 /*  METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    eigene Methoden zur Berechnung und Ausfürhung 
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


  


 
void processRelais()
{//Aquaduino.print(".");

//who is WHO:
/* 
f11on //Pump1 ON
f24on  //Pump2 ON
f13on //MainLight ON
f21on    //Heater ON
f22on    //CO2 ON
f23on //Coolpump ON

*/
//erst funken
//dann schalten 




///XXXXXXXXXX
//VALUES ARE ALSO USED IN THE POWERSCREEN()
//XXXXXXXX

 if (!light230Value){mySwitch.send(f13on, 24);}
 else {mySwitch.send(f13off, 24);}
 sx1509.writePin(light1Pin, light1Value);
 sx1509.writePin(light2Pin, light2Value);
 

  if (!heaterValue){mySwitch.send(f21on, 24);}
 else {mySwitch.send(f21off, 24);}
 if (!co2Value){mySwitch.send(f22on, 24);}
 else {mySwitch.send(f22off, 24);}
 if (!coolValue){mySwitch.send(f23on, 24);}
 else {mySwitch.send(f23off, 24);}
// sx1509.writePin(light230Pin, light230Value);
 sx1509.writePin(dPump1Pin, dPump1Value); 
 sx1509.writePin(dPump2Pin, dPump2Value); 
 sx1509.writePin(dPump3Pin, dPump3Value); 

// sx1509.writePin(co2Pin, co2Value); 
// sx1509.writePin(heaterPin, heaterValue); 

// sx1509.writePin(coolPin, coolValue); 

}


void processPump() //2 times cause sometimes it doesnt switch
{ delay(100);
if (!pump1Value){mySwitch.send(f11on, 24);}
 else {mySwitch.send(f11off, 24);}
 delay(100);
 if (!pump2Value){mySwitch.send(f42on, 24);}
 else {mySwitch.send(f42off, 24);}
 
  delay(100);
if (!pump1Value){mySwitch.send(f11on, 24);}
 else {mySwitch.send(f11off, 24);}
 delay(100);
 if (!pump2Value){mySwitch.send(f42on, 24);}
 else {mySwitch.send(f42off, 24);}
 
}


/*
void processSingleRelais(int SR)
{switch (SR)
    {   case 0:
       sx1509.writePin(pump1Pin, pump1Value);
       break;
       case 1:
       sx1509.writePin(pump2Pin, pump2Value);
       break;
       case 2:
       sx1509.writePin(light230Pin, light230Value);
       break;
       case 3:
       sx1509.writePin(light2Pin, light2Value);
       break;
       case 4:
       sx1509.writePin(co2Pin, co2Value); 
       break;
       case 5:
       sx1509.writePin(heaterPin, heaterValue); 
       break;
       case 6:
       sx1509.writePin(dPump1Pin, dPump1Value); 
       break;
       case 7:
       sx1509.writePin(dPump2Pin, dPump2Value); 
       break;
       case 8:
       sx1509.writePin(dPump3Pin, dPump3Value); 
       break;
       case 9:
       sx1509.writePin(coolPin, coolValue); 
       break;
    }
}
*/



void UpdateClockAndLight()
 {
    now = rtc.now();
    //calculatedPWM=calculatedPWM+255;
    analogWrite(lightPwmPin, calculatedPWM);
 }
 


void processRFInput()
{switch (mySwitch.getReceivedValue())
 {case f44on:
  { TVMode();
    lightCalculator();
    AI();
  break;
  }
  case f44off:
   { TVModeState=false;
     lightCalculator();
     AI();
    break;
   }
  }
 }




/* void SerialOutput()    //for debugging - not called
{   
    

    Serial.println("XXXXXX Serial Output XXXXXXXXX");
    Serial.print("Time: ");

    Serial.print("PH: ");
    Serial.print(PhWert);
    Serial.print("          Temp:  ");
    Serial.println(Temp);
    Serial.println();
    

    
}
 */
   void serialEvent() {                                                         //if the hardware serial port_0 receives a char              
               char inchar = (char)Serial.read();                               //get the char we just received
               inputstring += inchar;                                           //add it to the inputString
               if(inchar == '\r') {input_stringcomplete = true;}                //if the incoming character is a <CR>, set the flag
              }  


  void serialEvent3(){                                                         //if the hardware serial port_3 receives a char 
              char inchar = (char)Serial3.read();                              //get the char we just received
              PhWertString += inchar;                                                    //add it to the inputString
                if(inchar == '\r') 
                {                                             //if the incoming character is a <CR>, set the flag 
                  sensor_stringcomplete = true;  
                
                }               
             }

void GetTemperature()
{sensors.requestTemperatures();
 Temp=sensors.getTempCByIndex(0);
}

void CleanMode()
{if (!cleaningInProcess)
 {cleanEnd=now.unixtime()+(60*cleanMinutes);}
 cleaningInProcess=true;
 calculatedPWM=229.5;
 pump1Value=pump1Clean;
 pump2Value=pump2Clean;
 light230Value=light230Clean;
 light1Value=light2Clean;
 light2Value=light2Clean;
 co2Value=co2Clean;
 heaterValue=heaterClean;
 coolValue=coolClean;
 drawScreen();
processRelais();
processPump();

}

void getPHValue()
{
      if (input_stringcomplete){                                                   //if a string from the PC has been received in its entirety 
      Serial3.print(inputstring);                                              //send that string to the Atlas Scientific product
      inputstring = "";                                                        //clear the string:
      input_stringcomplete = false;                                            //reset the flag used to tell if we have received a completed string from the PC
      }
  
 if (sensor_stringcomplete){                                                   //if a string from the Atlas Scientific product has been received in its entierty 

      char FloatBufferForPH[5];
      PhWertString.toCharArray(FloatBufferForPH, sizeof(FloatBufferForPH));
      PhWert = atof(FloatBufferForPH);      //String is ready - set the PhWert
      
     /*  ONLY UNCOMMENT FOR PH DEBUGGING
      Serial.println(PhWertString);  
      Serial.println(PhWert);
      */
      PhWertString = "";        //clear the string:
      
      sensor_stringcomplete = false;      //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
      }
      
}

void TVMode()   //to start the TV Mode. the TV Mode resets as soon as the CalculatedPWM hit
{TVModeStart=now;
 TVModeState=true;
 //TVModeBrightness in globalConfig is the Point Brightness starts
}

void lightCalculator()
{  if (!manualOverride && !cleaningInProcess)
{  TimeSpan helpSpan  = now.unixtime() - (now.unixtime()-86400*7);  //set it to 7 days as fallback
 timeSinceLastLight  = now.unixtime() - (now.unixtime()+86400*7);  //set it to -7 days as fallback
 timeToNextLight = now.unixtime() - (now.unixtime()-86400*7);  //set it to 7 days as fallback
 float oldPWM=255;
 float newPWM=255;


  //int helpSpanSeconds;
 for (int i=0; i<int(sizeof(lightPWM)/3); i++)
 {DateTime helpDT (now.year(),now.month(),now.day(),int(lightPWM[i].Hour),int(lightPWM[i].Minute),0);
  helpSpan=helpDT-now;
   if (!TVModeState)
       {if(timeToNextLight.totalseconds()>helpSpan.totalseconds() && int(helpSpan.totalseconds()>=0))
            {timeToNextLight=helpSpan;
             newPWM=int(lightPWM[i].pwmValue);
            }
        if(timeSinceLastLight.totalseconds()<helpSpan.totalseconds() && int(helpSpan.totalseconds()<0))
            {timeSinceLastLight=helpSpan;
             oldPWM=int(lightPWM[i].pwmValue);
            }
       }
   else
    {if(timeToNextLight.totalseconds()>helpSpan.totalseconds() && int(helpSpan.totalseconds()>=0) && int(lightPWM[i].pwmValue)<3)   //find the first point with 0 light  (<2)
      {timeToNextLight=helpSpan;
       newPWM=int(lightPWM[i].pwmValue);
      }
    timeSinceLastLight=TVModeStart-now;
    oldPWM=TVModeBrightness;
    if (calculatedPWM<3)   //disable TVMode once hitting 0
       {TVModeState=false;
       }
    }
 


  calculatedPWM=oldPWM+(int(((oldPWM-newPWM)/((timeToNextLight.totalseconds())+abs(timeSinceLastLight.totalseconds())))*timeSinceLastLight.totalseconds()));
 
   if(calculatedPWM>90)  //over 35% light - coolpump on
   {coolValue=false;}
   else
   {coolValue=true;}   
 }
}





/*  Serial.print(i);
  Serial.print("   ");
  Serial.print(helpSpan.days(), DEC);
  Serial.print("d    ");
  Serial.print(helpSpan.hours(), DEC);
  Serial.print(':');
  Serial.print(helpSpan.minutes(), DEC);
  Serial.print("      PWM:");
 Serial.println(lightPWM[i].pwmValue);


  Serial.print("Shortest TIME:    ");
  Serial.print("   ");
  Serial.print(timeToNextLight.days(), DEC);
  Serial.print("d    "); 
  Serial.print(timeToNextLight.hours(), DEC);
  Serial.print(':');
  Serial.println(timeToNextLight.minutes(), DEC);
 
  Serial.print("Shortest NEGATIVE TIME:    ");
    Serial.print("   ");
  Serial.print(timeSinceLastLight.days(), DEC);
  Serial.print("d    "); 
Serial.print(timeSinceLastLight.hours(), DEC);
 Serial.print(':');
 Serial.println(timeSinceLastLight.minutes(), DEC);
 
  Serial.print("calculated PWM:    ");
  Serial.println(calculatedPWM);
 
 */
  /*helpSpanSeconds=abs(int(helpSpan.totalseconds()));
  if((helpSpanSeconds < timeToNextLight.totalseconds()) && (int(helpSpan.totalseconds()>0)))
   {timeToNextLight=helpSpan;
    currentPWM=lightPWM[i].pwmValue;
    }
    Serial.println(i);
    Serial.print(timeToNextLight.days(), DEC);
    Serial.print(' ');
    Serial.print(timeToNextLight.hours(), DEC);
    Serial.print(':');
    Serial.println(timeToNextLight.minutes(), DEC);
    Serial.println(currentPWM);
   
*/


}
/*
 lightPWM[0].startHour=21;
 lightPWM[0].startMinute=0;
 lightPWM[0].stopHour=6;
 lightPWM[0].stopMinute=40;
 lightPWM[0].pwmValue=255;
 
*/




void AI()
{wdt_reset();
  if (!manualOverride && !cleaningInProcess)
  { DateTime CompareLightOnTime (now.year(), now.month(),now.day(), int(powLightOnHour), int(powLightOnMinute),0);
    DateTime CompareLightOffTime (now.year(), now.month(),now.day(), int(powLightOffHour), int(powLightOffMinute),0);
    DateTime CompareCO2OnTime (now.year(), now.month(),now.day(), int(powCo2OnHour), int(powCo2OnMinute),0);
    DateTime CompareCO2OffTime (now.year(), now.month(),now.day(), int(powCo2OffHour), int(powCo2OffMinute),0);
    DateTime CompareScreenOffTime (now.year(), now.month(),now.day(), int(screenOnHour), int(screenOnMinute),0);
    DateTime CompareScreenOnTime (now.year(), now.month(),now.day(), int(screenOffHour), int(screenOffMinute),0);
     //turn lights on or off
     if (now.unixtime() >CompareLightOnTime.unixtime() && now.unixtime() <CompareLightOffTime.unixtime())
       {light230Value=false;
        light1Value=false; 
        light2Value=false;  
       }
    else
     {light230Value=true;
       light1Value=true; 
       light2Value=true;
     }
           //turn Heater on or off
      if(Temp<TempLowerLimit)
          {heaterValue=false;
          }
        else if (TempUpperLimit<Temp)
          {heaterValue= true;
          } 
   
     //turn  Co2 on or off
     if (now.unixtime() >CompareCO2OnTime.unixtime() && now.unixtime() <CompareCO2OffTime.unixtime())
      {if(PhWert>PHUpperLimit)
          {co2Value=false;
          }
        else if (PhWert<PHLowerLimit)
          {co2Value=true;
          } 
      }
      else
       {co2Value=true;
       } 

  processRelais();
  updateHomeScreen();
  }
  else if(!manualOverride && (now.unixtime() >= cleanEnd.unixtime()))
  {      cleaningInProcess=false;
          pump1Value=false;
          pump2Value=false;
          dispScreen=0;
          drawScreen();
          //processRelais();          
          lightCalculator();
          AI();
          processPump();
  }
  
  
}


void writeFile()
{Aquaduino = SD.open("aqua.txt", FILE_WRITE);
 if (Aquaduino)
 {  Aquaduino.print(now.day(), DEC);
    Aquaduino.print(".");    
    Aquaduino.print(now.month(), DEC);
    Aquaduino.print(".");
    Aquaduino.print(now.year(), DEC);  
    Aquaduino.print(";");
    Aquaduino.print(now.hour(), DEC);
    Aquaduino.print(':');
    Aquaduino.print(now.minute(), DEC);
    Aquaduino.print(':');
    Aquaduino.print(now.second(), DEC);
    Aquaduino.print(";");
    Aquaduino.print(Temp);
    Aquaduino.print(";");
    Aquaduino.print(PhWert);
    Aquaduino.print(";");
    Aquaduino.print(calculatedPWM);
    Aquaduino.print(";");
    Aquaduino.print(pump1Value);
    Aquaduino.print(";");
    Aquaduino.print(pump2Value);
    Aquaduino.print(";");    
    Aquaduino.print(light230Value);
    Aquaduino.print(";");
    Aquaduino.print(light1Value);
    Aquaduino.print(";");
    Aquaduino.print(light2Value);
    Aquaduino.print(";");
    Aquaduino.print(co2Value);
    Aquaduino.print(";");
    Aquaduino.print(heaterValue);
    Aquaduino.print(";");
    Aquaduino.print(dPump1Value);
    Aquaduino.print(";");
    Aquaduino.print(dPump2Value);
    Aquaduino.print(";");
    Aquaduino.print(dPump3Value);
    Aquaduino.print(";");
    Aquaduino.print(coolValue);
    Aquaduino.println(";");
    //Aquaduino.println();
  Aquaduino.close();
 }
}
/*
void writeFertToCard()
{Aquaduino = SD.open("fert.txt", FILE_WRITE);
 if (Aquaduino)
 {  Aquaduino.print(lastFert.day(), DEC);
    Aquaduino.print(".");    
    Aquaduino.print(lastFert.month(), DEC);
    Aquaduino.print(".");
    Aquaduino.print(lastFert.year(), DEC);  
    Aquaduino.print(";");
    Aquaduino.print(lastFert.hour(), DEC);
    Aquaduino.print(':');
    Aquaduino.print(lastFert.minute(), DEC);
    Aquaduino.print(':');
    Aquaduino.print(lastFert.second(), DEC);
    Aquaduino.print(";");
 }
}




*/



void fertilize()
{   if(now.unixtime()-lastFert.unixtime()>82800)  // no fertilizing for 23 hours (82800) - just to be sure
      {if((now.hour()==doseHour) && (now.minute()>=doseMinute) && (now.minute()<=doseMinute+10))  //timeframe of 10 Minutes to fertilize - in case that another task runs at the exact dose time
        {lastFert=now.unixtime();
          Aquaduino = SD.open("fert.txt", FILE_WRITE);
          if (Aquaduino)   //write to file
           {Aquaduino.print(lastFert.day(), DEC);
            Aquaduino.print(".");    
            Aquaduino.print(lastFert.month(), DEC);
            Aquaduino.print(".");
            Aquaduino.print(lastFert.year(), DEC);  
            Aquaduino.print(";");
            Aquaduino.print(lastFert.hour(), DEC);
            Aquaduino.print(':');
            Aquaduino.print(lastFert.minute(), DEC);
            Aquaduino.print(':');
            Aquaduino.print(lastFert.second(), DEC);
            Aquaduino.print("; ");
            Aquaduino.print("Fertilizing ");  
           }
           fertmillis=millis();
            
            //fertilize N
            if (dayN[now.dayOfWeek()]  && (FLeft[0]-FDose[0]>=0))
            { Aquaduino.print("N:");
              Aquaduino.print(FDose[0]);
              FLeft[0]=FLeft[0]-FDose[0];
              fertmillis=millis();
           
              wdt_disable();
              
              while(millis() - fertmillis < 1000*FDose[0]/(FRate[0]/60))
              {dPump1Value=false;
               processRelais();
               if (dispScreen<1) {myFiles.load(393, 449,48,48, "1nN.raw"); }
              }
              dPump1Value=true;
              processRelais();
              
              wdt_enable(WDTO_8S);
              updateHomeScreen();
            }
            //fertilize NPK 
            if (dayNPK[now.dayOfWeek()] && (FLeft[1]-FDose[1]>=0))
            {Aquaduino.print("  NPK: ");
             Aquaduino.print(FDose[1]);
             FLeft[1]=FLeft[1]-FDose[1];
             fertmillis=millis();
             wdt_disable();
             while(millis() - fertmillis < 1000*FDose[1]/(FRate[1]/60))
             {dPump2Value=false;
             processRelais();
             if (dispScreen<1) {myFiles.load(340, 502,48,48, "1npkN.raw");}
             }
             dPump2Value=true;
             processRelais();
             wdt_enable(WDTO_8S);
             updateHomeScreen();
            }
            //fertilize FE 
            if (dayFE[now.dayOfWeek()] && (FLeft[2]-FDose[2]>=0))
            {Aquaduino.print("  FE: ");
             Aquaduino.print(FDose[2]);
             FLeft[2]=FLeft[2]-FDose[2];
             fertmillis=millis();
             wdt_disable();
             while(millis() - fertmillis < 1000*FDose[2]/(FRate[2]/60))
             {dPump3Value=false;
              processRelais();
              if (dispScreen<1) { myFiles.load(393, 502,48,48, "1feN.raw"); }
             }
             dPump3Value=true;
             processRelais();
             wdt_enable(WDTO_8S);
             updateHomeScreen();
             
            } 
           Aquaduino.println();
           Aquaduino.close();
           drawFertilizer();
           saveFerti();
   
     }
   }
 }

/*
void Testcalculation()
{byte Lightmodes[]={20,00,21,10,23,10,22,30,22,53};
 DateTime dt99 (now.unixtime()-86400*7);
 TimeSpan ts1;
 TimeSpan shortestTime=now-dt99;
  DateTime zwischenzeit;

 for (int i=0; i<sizeof(Lightmodes); i+=2)
 {DateTime zwischenzeit (now.year(),now.month(),now.day(),int(Lightmodes[i]),int(Lightmodes[i+1]),0);
Serial.print("ZWISCHENZEIT:");
Serial.print(zwischenzeit.year(), DEC);
    Serial.print('/');
    Serial.print(zwischenzeit.month(), DEC);
    Serial.print('/');
    Serial.print(zwischenzeit.day(), DEC);
    Serial.print(' ');
    Serial.print(zwischenzeit.hour(), DEC);
    Serial.print(':');
    Serial.print(zwischenzeit.minute(), DEC);
    Serial.print(':');
    Serial.print(zwischenzeit.second(), DEC);
    Serial.println();
    
    
  ts1=zwischenzeit-now;
  if((ts1.totalseconds() < shortestTime.totalseconds()) && (int(ts1.totalseconds())>0))
   {shortestTime=ts1;
    Serial.print("Next Time:");
    Serial.print(int(Lightmodes[i]));
    Serial.print(":");
     Serial.println(int(Lightmodes[i+1]));
}
 }
  
 
 
   Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    
 /*       Serial.print(zwischenzeit.year(), DEC);
    Serial.print('/');
    Serial.print(zwischenzeit.month(), DEC);
    Serial.print('/');
    Serial.print(zwischenzeit.day(), DEC);
    Serial.print(' ');
    Serial.print(zwischenzeit.hour(), DEC);
    Serial.print(':');
    Serial.print(zwischenzeit.minute(), DEC);
    Serial.print(':');
    Serial.print(zwischenzeit.second(), DEC);
    Serial.println();

    Serial.print("days  ");
    Serial.print(shortestTime.days(), DEC);
    Serial.print("hours   ");
    Serial.print(shortestTime.hours(), DEC);
    Serial.print("h");
    Serial.print(shortestTime.minutes(), DEC);
    Serial.print("m");
    Serial.println(shortestTime.seconds(), DEC);
        Serial.println("s");
     
    Serial.print(shortestTime.totalseconds(), DEC);
   // Serial.print(ts1);
   
   
    Serial.println();
    Serial.println();
    Serial.println();
    
    
   */ 

  
 
   /* for (i=0; i<9; i++)
  {zwischenzeit
  }
  


 
powLightOnHour;
powLightOnMinute;
powLightOffHour;
powLightOffMinute;

powCo2OnHour;
powCo2OnMinute;
powCo2OffHour;
powCo2OffMinute;
 */

  
  


void readLightPWM()
{lightPWM[0].Hour=EEPROM.read(20);
lightPWM[0].Minute=EEPROM.read(21);
lightPWM[0].pwmValue=EEPROM.read(22);
lightPWM[1].Hour=EEPROM.read(23);
lightPWM[1].Minute=EEPROM.read(24);
lightPWM[1].pwmValue=EEPROM.read(25);
lightPWM[2].Hour=EEPROM.read(26);
lightPWM[2].Minute=EEPROM.read(27);
lightPWM[2].pwmValue=EEPROM.read(28);
lightPWM[3].Hour=EEPROM.read(29);
lightPWM[3].Minute=EEPROM.read(30);
lightPWM[3].pwmValue=EEPROM.read(31);
lightPWM[4].Hour=EEPROM.read(32);
lightPWM[4].Minute=EEPROM.read(33);
lightPWM[4].pwmValue=EEPROM.read(34);
lightPWM[5].Hour=EEPROM.read(35);
lightPWM[5].Minute=EEPROM.read(36);
lightPWM[5].pwmValue=EEPROM.read(37);
lightPWM[6].Hour=EEPROM.read(38);
lightPWM[6].Minute=EEPROM.read(39);
lightPWM[6].pwmValue=EEPROM.read(40);
lightPWM[7].Hour=EEPROM.read(41);
lightPWM[7].Minute=EEPROM.read(42);
lightPWM[7].pwmValue=EEPROM.read(43);
lightPWM[8].Hour=EEPROM.read(44);
lightPWM[8].Minute=EEPROM.read(45);
lightPWM[8].pwmValue=EEPROM.read(46);
lightPWM[9].Hour=EEPROM.read(47);
lightPWM[9].Minute=EEPROM.read(48);
lightPWM[9].pwmValue=EEPROM.read(49);
lightPWM[10].Hour=EEPROM.read(50);
lightPWM[10].Minute=EEPROM.read(51);
lightPWM[10].pwmValue=EEPROM.read(52);
lightPWM[11].Hour=EEPROM.read(53);
lightPWM[11].Minute=EEPROM.read(54);
lightPWM[11].pwmValue=EEPROM.read(55);
}

void saveLightPWM()
{EEPROM.write(20,lightPWM[0].Hour);
EEPROM.write(21,lightPWM[0].Minute);
EEPROM.write(22,byte(lightPWM[0].pwmValue));
EEPROM.write(23,lightPWM[1].Hour);
EEPROM.write(24,lightPWM[1].Minute);
EEPROM.write(25,byte(lightPWM[1].pwmValue));
EEPROM.write(26,lightPWM[2].Hour);
EEPROM.write(27,lightPWM[2].Minute);
EEPROM.write(28,byte(lightPWM[2].pwmValue));
EEPROM.write(29,lightPWM[3].Hour);
EEPROM.write(30,lightPWM[3].Minute);
EEPROM.write(31,byte(lightPWM[3].pwmValue));
EEPROM.write(32,lightPWM[4].Hour);
EEPROM.write(33,lightPWM[4].Minute);
EEPROM.write(34,byte(lightPWM[4].pwmValue));
EEPROM.write(35,lightPWM[5].Hour);
EEPROM.write(36,lightPWM[5].Minute);
EEPROM.write(37,byte(lightPWM[5].pwmValue));
EEPROM.write(38,lightPWM[6].Hour);
EEPROM.write(39,lightPWM[6].Minute);
EEPROM.write(40,byte(lightPWM[6].pwmValue));
EEPROM.write(41,lightPWM[7].Hour);
EEPROM.write(42,lightPWM[7].Minute);
EEPROM.write(43,byte(lightPWM[7].pwmValue));
EEPROM.write(44,lightPWM[8].Hour);
EEPROM.write(45,lightPWM[8].Minute);
EEPROM.write(46,byte(lightPWM[8].pwmValue));
EEPROM.write(47,lightPWM[9].Hour);
EEPROM.write(48,lightPWM[9].Minute);
EEPROM.write(49,byte(lightPWM[9].pwmValue));
EEPROM.write(50,lightPWM[10].Hour);
EEPROM.write(51,lightPWM[10].Minute);
EEPROM.write(52,byte(lightPWM[10].pwmValue));
EEPROM.write(53,lightPWM[11].Hour);
EEPROM.write(54,lightPWM[11].Minute);
EEPROM.write(55,byte(lightPWM[11].pwmValue));

}


void readCleanSched()
{
 pump1Clean=EEPROM.read(56);
 pump2Clean=EEPROM.read(57);
 light230Clean=EEPROM.read(58);
 light2Clean=EEPROM.read(59);
 co2Clean=EEPROM.read(60);
 heaterClean=EEPROM.read(61);
 coolClean=EEPROM.read(62);
 cleanMinutes=EEPROM.read(91);
 }


void saveCleanSched()
{
 EEPROM.write(56,pump1Clean);
 EEPROM.write(57,pump2Clean);
 EEPROM.write(58,light230Clean);
 EEPROM.write(59,light2Clean);
 EEPROM.write(60,co2Clean);
 EEPROM.write(61,heaterClean);
 EEPROM.write(62,coolClean);
 EEPROM.write(91,cleanMinutes);
 }



void readPHValue()
{PHUpperLimit=(EEPROM.read(63));
 PHLowerLimit=(EEPROM.read(64));
 PHUpperLimit=PHUpperLimit/20;
 PHLowerLimit=PHLowerLimit/20;
 }

void savePHValue()
{EEPROM.write(63,PHUpperLimit*20);
EEPROM.write(64,PHLowerLimit*20);
}


void readTempValue()
{TempUpperLimit=(EEPROM.read(65));
 TempLowerLimit=(EEPROM.read(66));

 }
 
void saveTempValue()
{EEPROM.write(65,TempUpperLimit);
EEPROM.write(66,TempLowerLimit);
}



void readFerti()
{  FDose[0]=EEPROM.read(0);
  FDose[1]=EEPROM.read(1);
  FDose[2]=EEPROM.read(2);
  FMax[0]=(EEPROM.read(3)*5);  //max 1,2 Liter max amount
  FMax[1]=(EEPROM.read(4)*5);//max 1,2 Liter max amount
  FMax[2]=(EEPROM.read(5)*5);//max 1,2 Liter max amount
  FLeft[0]=(EEPROM.read(6)*5);//max 1,2 Liter max amount
  FLeft[1]=(EEPROM.read(7)*5);//max 1,2 Liter max amount
  FLeft[2]=(EEPROM.read(8)*5);//max 1,2 Liter max amount
  FRate[0]=EEPROM.read(67);
  FRate[1]=EEPROM.read(68);
  FRate[2]=EEPROM.read(69);
  MoF[0]=EEPROM.read(70);
  MoF[1]=EEPROM.read(71);
  MoF[2]=EEPROM.read(72);
  TuF[0]=EEPROM.read(73);
  TuF[1]=EEPROM.read(74);
  TuF[2]=EEPROM.read(75);
  WeF[0]=EEPROM.read(76);
  WeF[1]=EEPROM.read(77);
  WeF[2]=EEPROM.read(78);
  ThF[0]=EEPROM.read(79);
  ThF[1]=EEPROM.read(80);
  ThF[2]=EEPROM.read(81);
  FrF[0]=EEPROM.read(82);
  FrF[1]=EEPROM.read(83);
  FrF[2]=EEPROM.read(84);
  SaF[0]=EEPROM.read(85);
  SaF[1]=EEPROM.read(86);
  SaF[2]=EEPROM.read(87);
  SuF[0]=EEPROM.read(88);
  SuF[1]=EEPROM.read(89);
  SuF[2]=EEPROM.read(90);
  doseHour=EEPROM.read(92);  
  doseMinute=EEPROM.read(93);  
  
  
  dayN[1]=MoF[0];
  dayN[2]=TuF[0];
  dayN[3]=WeF[0];
  dayN[4]=ThF[0];
  dayN[5]=FrF[0];
  dayN[6]=SaF[0];
  dayN[0]=SuF[0];
  
  dayNPK[1]=MoF[1];
  dayNPK[2]=TuF[1];
  dayNPK[3]=WeF[1];
  dayNPK[4]=ThF[1];
  dayNPK[5]=FrF[1];
  dayNPK[6]=SaF[1];
  dayNPK[0]=SuF[1];
  
  dayFE[1]=MoF[2];
  dayFE[2]=TuF[2];
  dayFE[3]=WeF[2];
  dayFE[4]=ThF[2];
  dayFE[5]=FrF[2];
  dayFE[6]=SaF[2];
  dayFE[0]=SuF[2];
  
  
  
  
  
 
}

void saveFerti()
{EEPROM.write(0,FDose[0]);
EEPROM.write(1,FDose[1]);
EEPROM.write(2,FDose[2]);
EEPROM.write(3,int(FMax[0]/5));
EEPROM.write(4,int(FMax[1]/5));
EEPROM.write(5,int(FMax[2]/5));
EEPROM.write(6,int(FLeft[0]/5));
EEPROM.write(7,int(FLeft[1]/5));
EEPROM.write(8,int(FLeft[2]/5));
EEPROM.write(67,FRate[0]);
EEPROM.write(68,FRate[1]);
EEPROM.write(69,FRate[2]);
EEPROM.write(70,MoF[0]);
EEPROM.write(71,MoF[1]);
EEPROM.write(72,MoF[2]);
EEPROM.write(73,TuF[0]);
EEPROM.write(74,TuF[1]);
EEPROM.write(75,TuF[2]);
EEPROM.write(76,WeF[0]);
EEPROM.write(77,WeF[1]);
EEPROM.write(78,WeF[2]);
EEPROM.write(79,ThF[0]);
EEPROM.write(80,ThF[1]);
EEPROM.write(81,ThF[2]);
EEPROM.write(82,FrF[0]);
EEPROM.write(83,FrF[1]);
EEPROM.write(84,FrF[2]);
EEPROM.write(85,SaF[0]);
EEPROM.write(86,SaF[1]);
EEPROM.write(87,SaF[2]);
EEPROM.write(88,SuF[0]);
EEPROM.write(89,SuF[1]);
EEPROM.write(90,SuF[2]);
EEPROM.write(92,doseHour);
EEPROM.write(93,doseMinute);
 
  dayN[1]=MoF[0];
  dayN[2]=TuF[0];
  dayN[3]=WeF[0];
  dayN[4]=ThF[0];
  dayN[5]=FrF[0];
  dayN[6]=SaF[0];
  dayN[0]=SuF[0];
  
  dayNPK[1]=MoF[1];
  dayNPK[2]=TuF[1];
  dayNPK[3]=WeF[1];
  dayNPK[4]=ThF[1];
  dayNPK[5]=FrF[1];
  dayNPK[6]=SaF[1];
  dayNPK[0]=SuF[1];
  
  dayFE[1]=MoF[2];
  dayFE[2]=TuF[2];
  dayFE[3]=WeF[2];
  dayFE[4]=ThF[2];
  dayFE[5]=FrF[2];
  dayFE[6]=SaF[2];
  dayFE[0]=SuF[2];
}



void readReminder()
{//tankCleanDay=EEPROM.read(94);
 /*tankCleanMonth=EEPROM.read(95);
 co2BottleDay=EEPROM.read(96);
 co2BottleMonth=EEPROM.read(97);
 cleanFilter1Day=EEPROM.read(98);
 cleanFilter1Month=EEPROM.read(99);
 cleanFilter2Day=EEPROM.read(100);
 cleanFilter2Month=EEPROM.read(101); 
 */
 tankClean= DateTime (now.year(),EEPROM.read(95),EEPROM.read(94),now.hour(),now.minute(),0);
 co2Bottle= DateTime (now.year(),EEPROM.read(97),EEPROM.read(96),now.hour(),now.minute(),0);
 cleanFilter1= DateTime (now.year(),EEPROM.read(99),EEPROM.read(98),now.hour(),now.minute(),0);
 cleanFilter2= DateTime (now.year(),EEPROM.read(101),EEPROM.read(100),now.hour(),now.minute(),0);
 tankCleandDays=EEPROM.read(102); 
 co2BottleDays=EEPROM.read(103); 
 cleanFilter1Days=EEPROM.read(104); 
 cleanFilter2Days=EEPROM.read(105); 
 // DateTime helpDT (now.year(),now.month(),now.day(),int(lightPWM[i].Hour),int(lightPWM[i].Minute),0);
}

void saveReminder()
{EEPROM.write(94,tankClean.day());
EEPROM.write(95,tankClean.month());
EEPROM.write(96,co2Bottle.day());
EEPROM.write(97,co2Bottle.month());
EEPROM.write(98,cleanFilter1.day());
EEPROM.write(99,cleanFilter1.month());
EEPROM.write(100,cleanFilter2.day());
EEPROM.write(101,cleanFilter2.month());
EEPROM.write(102,tankCleandDays);
EEPROM.write(103,co2BottleDays);
EEPROM.write(104,cleanFilter1Days);
EEPROM.write(105,cleanFilter2Days);

}



void readPowerSchedule()
{powLightOnHour=EEPROM.read(9);
 powLightOnMinute=EEPROM.read(10);
 powLightOffHour=EEPROM.read(11);
 powLightOffMinute=EEPROM.read(12);
 powCo2OnHour=EEPROM.read(13);
 powCo2OnMinute=EEPROM.read(14);
 powCo2OffHour=EEPROM.read(15);
 powCo2OffMinute=EEPROM.read(16);
}

void savePowerSchedule()
{EEPROM.write(9,powLightOnHour);
EEPROM.write(10,powLightOnMinute);
EEPROM.write(11,powLightOffHour);
EEPROM.write(12,powLightOffMinute);
EEPROM.write(13,powCo2OnHour);
EEPROM.write(14,powCo2OnMinute);
EEPROM.write(15,powCo2OffHour);
EEPROM.write(16,powCo2OffMinute);
}    



void readScreenScreen()
{screenOnHour=EEPROM.read(106);
 screenOffHour=EEPROM.read(107);
 screenOnMinute=EEPROM.read(108);
 screenOffMinute=EEPROM.read(109);

 standByMinutes=EEPROM.read(110);

 backlightPWM=EEPROM.read(111);
}

void saveScreenScreen()
{EEPROM.write(106,screenOnHour);
EEPROM.write(107,screenOffHour);
EEPROM.write(108,screenOnMinute);
EEPROM.write(109,screenOffMinute);
EEPROM.write(110,standByMinutes);
EEPROM.write(111,backlightPWM);
}  

