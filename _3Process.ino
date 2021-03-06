
/*  METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   eigene Methoden zur Berechnung und AusfÃ¼rhung
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/






void processRelais()
{ //Aquaduino.print(".");

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

  if (!light230Value) {
    mySwitch.send(f13on, 24);
  }
  else {
    mySwitch.send(f13off, 24);
  }
  sx1509.writePin(light1Pin, light1Value);
  sx1509.writePin(light2Pin, light2Value);


  if (!heaterValue) {
    mySwitch.send(f21on, 24);
  }
  else {
    mySwitch.send(f21off, 24);
  }
  if (!co2Value) {
    mySwitch.send(f22on, 24);
  }
  else {
    mySwitch.send(f22off, 24);
  }
  if (!coolValue) {
    mySwitch.send(f23on, 24);
  }
  else {
    mySwitch.send(f23off, 24);
  }
  // sx1509.writePin(light230Pin, light230Value);
  sx1509.writePin(dPump1Pin, dPump1Value);
  sx1509.writePin(dPump2Pin, dPump2Value);
  sx1509.writePin(dPump3Pin, dPump3Value);

  // sx1509.writePin(co2Pin, co2Value);
  // sx1509.writePin(heaterPin, heaterValue);

  // sx1509.writePin(coolPin, coolValue);

}


void processPump() //2 times cause sometimes it doesnt switch
{ //delay(100);
  if (!pump1Value) {
    mySwitch.send(f11on, 24);
  }
  else {
    mySwitch.send(f11off, 24);
  }
  delay(100);
  if (!pump2Value) {
    mySwitch.send(f42on, 24);
  }
  else {
    mySwitch.send(f42off, 24);
  }

  //  delay(100);
  //if (!pump1Value){mySwitch.send(f11on, 24);}
  //else {mySwitch.send(f11off, 24);}
  //delay(100);
  //if (!pump2Value){mySwitch.send(f42on, 24);}
  //else {mySwitch.send(f42off, 24);}

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
  if (dispScreen != 141 && dispScreen != 142 && dispScreen != 143 && dispScreen != 144 && dispScreen != 145 && dispScreen != 146 && dispScreen != 16)
  {
    analogWrite(redPin, calculatedRed);
    analogWrite(greenPin, calculatedGreen);
    analogWrite(bluePin, calculatedBlue);
  }
}



void processRFInput()
{ switch (mySwitch.getReceivedValue())
  { case f44on:
      { TVMode();
        lightCalculator();
        UpdateClockAndLight();
        AI();
        break;
      }
    case f44off:
      { TVModeState = false;
        lightCalculator();
        UpdateClockAndLight();
        AI();
        break;
      }
    case f43on:
      { MoonMode();
        lightCalculator();
        UpdateClockAndLight();
        AI();
        break;
      }
    case f43off:
      { MoonModeState = false;
        lightCalculator();
        UpdateClockAndLight();
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
  if (inchar == '\r') {
    input_stringcomplete = true; //if the incoming character is a <CR>, set the flag
  }
}


void serialEvent3() {                                                        //if the hardware serial port_3 receives a char
  char inchar = (char)Serial3.read();                              //get the char we just received
  PhWertString += inchar;                                                    //add it to the inputString
  if (inchar == '\r')
  { //if the incoming character is a <CR>, set the flag
    sensor_stringcomplete = true;

  }
}

void GetTemperature()
{ sensors.requestTemperatures();
  Temp = sensors.getTempCByIndex(0);
}


void MoonMode()
{ if (!MoonModeState)
  {
    MoonEnd = now.unixtime() + (60 * MoonMinutes);
  }
  MoonModeState = true;
  light230Value = false;

  processRelais();
  UpdateClockAndLight();
  drawScreen();
}


void CleanMode()
{ if (!cleaningInProcess)
  {
    cleanEnd = now.unixtime() + (60 * cleanMinutes);
  }
  cleaningInProcess = true;
  calculatedPWM = 229.5;
  pump1Value = pump1Clean;
  pump2Value = pump2Clean;
  light230Value = light230Clean;
  light1Value = light2Clean;
  light2Value = light2Clean;
  co2Value = co2Clean;
  heaterValue = heaterClean;
  coolValue = coolClean;
  drawScreen();
  processRelais();
  processPump();

}

void writeToRingBuffer()
{ TempValues[put_TempIndex] = Temp;
  put_TempIndex = ( put_TempIndex + 1) % 62;

  if ((PhWert <= PHUpperLimit + 0.2) && (PhWert >= PHLowerLimit - 0.2))
  { PHValues[ put_PHindex] = PhWert;
    Co2Values[ put_PHindex] = co2Value;
     put_PHindex = ( put_PHindex + 1) % 96;
  }
}


void getPHValue()
{
  if (input_stringcomplete) {                                                  //if a string from the PC has been received in its entirety
    Serial3.print(inputstring);                                              //send that string to the Atlas Scientific product
    inputstring = "";                                                        //clear the string:
    input_stringcomplete = false;                                            //reset the flag used to tell if we have received a completed string from the PC
  }

  if (sensor_stringcomplete) {                                                  //if a string from the Atlas Scientific product has been received in its entierty

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

void TVMode()   //to start the TV Mode. the TV Mode resets as soon as the CalculatedPWM hit 0
{ TVModeStart = now;
  TVModeState = true;
  //TVModeBrightness is the Point Brightness starts
}

void lightCalculator()
{ if (!manualOverride && !cleaningInProcess)
   {TimeSpan helpSpan  = now.unixtime() - (now.unixtime() - 86400 * 7); //set it to 7 days as fallback
    timeSinceLastLight  = now.unixtime() - (now.unixtime() + 86400 * 7); //set it to -7 days as fallback
    timeToNextLight = now.unixtime() - (now.unixtime() - 86400 * 7); //set it to 7 days as fallback
    float oldPWM = 255;
    float newPWM = 255;

    //for RGB
    TimeSpan helpSpanRGB  = now.unixtime() - (now.unixtime() - 86400 * 7); //set it to 7 days as fallback
    timeSinceLastLightRGB  = now.unixtime() - (now.unixtime() + 86400 * 7); //set it to -7 days as fallback
    timeToNextLightRGB = now.unixtime() - (now.unixtime() - 86400 * 7); //set it to 7 days as fallback
    float oldRed = 0;
    float newRed = 0;
    float oldBlue = 0;
    float newBlue = 0;
    float oldGreen = 0;
    float newGreen = 0;


    //int helpSpanSeconds;
    for (int i = 0; i < 12; i++)
    { DateTime helpDTRGB (now.year(), now.month(), now.day(), int(lightRGB[i].Hour), int(lightRGB[i].Minute), 0);
      helpSpanRGB = helpDTRGB - now;
      DateTime helpDT (now.year(), now.month(), now.day(), int(lightPWM[i].Hour), int(lightPWM[i].Minute), 0);
      helpSpan = helpDT - now;

      //RGB

      if (timeToNextLightRGB.totalseconds() > helpSpanRGB.totalseconds() && int(helpSpanRGB.totalseconds() >= 0))
      { timeToNextLightRGB = helpSpanRGB;
        newRed = int(lightRGB[i].red);
        newGreen = int(lightRGB[i].green);
        newBlue = int(lightRGB[i].blue);
      }
      if (timeSinceLastLightRGB.totalseconds() < helpSpanRGB.totalseconds() && int(helpSpanRGB.totalseconds() < 0))
      { timeSinceLastLightRGB = helpSpanRGB;
        oldRed = int(lightRGB[i].red);
        oldGreen = int(lightRGB[i].green);
        oldBlue = int(lightRGB[i].blue);
      }





      //PWM for white light

      if (!TVModeState)
      { if (timeToNextLight.totalseconds() > helpSpan.totalseconds() && int(helpSpan.totalseconds() >= 0))
        { timeToNextLight = helpSpan;
          newPWM = int(lightPWM[i].pwmValue);
        }
        if (timeSinceLastLight.totalseconds() < helpSpan.totalseconds() && int(helpSpan.totalseconds() < 0))
        { timeSinceLastLight = helpSpan;
          oldPWM = int(lightPWM[i].pwmValue);
        }
      }
      else
      { if (timeToNextLight.totalseconds() > helpSpan.totalseconds() && int(helpSpan.totalseconds() >= 0) && int(lightPWM[i].pwmValue) < 3) //find the first point with 0 light  (<2)
        { timeToNextLight = helpSpan;
          newPWM = 0;
        }
        if (timeSinceLastLight.totalseconds() < helpSpan.totalseconds() && int(helpSpan.totalseconds() < 0))
        { timeSinceLastLight = helpSpan;
          if (oldPWM < 2)
          {
            oldPWM = int(lightPWM[i].pwmValue);
          }
          else
          {
            oldPWM = TVModeBrightness;
          }

          timeSinceLastLight = TVModeStart - now;
        }


        if (calculatedPWM < 3) //disable TVMode once hitting 0
        { TVModeState = false;
        }
      }

      //RGB
      if (!MoonModeState)
      {
      calculatedRed = oldRed + (int(((oldRed - newRed) / ((timeToNextLightRGB.totalseconds()) + abs(timeSinceLastLightRGB.totalseconds()))) * timeSinceLastLightRGB.totalseconds()));
      calculatedGreen = oldGreen + (int(((oldGreen - newGreen) / ((timeToNextLightRGB.totalseconds()) + abs(timeSinceLastLightRGB.totalseconds()))) * timeSinceLastLightRGB.totalseconds()));
      calculatedBlue = oldBlue + (int(((oldBlue - newBlue) / ((timeToNextLightRGB.totalseconds()) + abs(timeSinceLastLightRGB.totalseconds()))) * timeSinceLastLightRGB.totalseconds()));
      calculatedPWM = oldPWM + (int(((oldPWM - newPWM) / ((timeToNextLight.totalseconds()) + abs(timeSinceLastLight.totalseconds()))) * timeSinceLastLight.totalseconds()));
      }
      else
      {calculatedRed=MoonRed;
       calculatedGreen=MoonGreen;
       calculatedBlue=MoonBlue;
       calculatedPWM=0;
      }
      //white




      if (calculatedPWM > 90) //over 35% light - coolpump on
      {
        coolValue = false;
      }
      else
      {
        coolValue = true;
      }



      /*

      Serial.print("i = ");
       Serial.print(i);
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



          Serial.println(i);
          Serial.print(timeToNextLight.days(), DEC);
          Serial.print(' ');
          Serial.print(timeToNextLight.hours(), DEC);
          Serial.print(':');
          Serial.println(timeToNextLight.minutes(), DEC);
          Serial.println(currentPWM);
        */
    }
  }
  else  //if manualoverride is active - turn off the RGB light
  {
    calculatedRed = 0;
    calculatedGreen = 0;
    calculatedBlue = 0;
  }

}
/*
 lightPWM[0].startHour=21;
 lightPWM[0].startMinute=0;
 lightPWM[0].stopHour=6;
 lightPWM[0].stopMinute=40;
 lightPWM[0].pwmValue=255;

*/




void AI()
{ wdt_reset();
  if (!manualOverride && !cleaningInProcess && !MoonModeState)
  { DateTime CompareLightOnTime (now.year(), now.month(), now.day(), int(powLightOnHour), int(powLightOnMinute), 0);
    DateTime CompareLightOffTime (now.year(), now.month(), now.day(), int(powLightOffHour), int(powLightOffMinute), 0);
    DateTime CompareCO2OnTime (now.year(), now.month(), now.day(), int(powCo2OnHour), int(powCo2OnMinute), 0);
    DateTime CompareCO2OffTime (now.year(), now.month(), now.day(), int(powCo2OffHour), int(powCo2OffMinute), 0);
    DateTime CompareScreenOffTime (now.year(), now.month(), now.day(), int(screenOnHour), int(screenOnMinute), 0);
    DateTime CompareScreenOnTime (now.year(), now.month(), now.day(), int(screenOffHour), int(screenOffMinute), 0);
    //turn lights on or off
    if (now.unixtime() > CompareLightOnTime.unixtime() && now.unixtime() < CompareLightOffTime.unixtime())
    { light230Value = false;
      light1Value = false;
      light2Value = false;
    }
    else
    { light230Value = true;
      light1Value = true;
      light2Value = true;
    }
    //turn Heater on or off
    if (Temp < TempLowerLimit)
    { heaterValue = false;
    }
    else if (TempUpperLimit < Temp)
    { heaterValue = true;
    }

    //turn  Co2 on or off
    if (now.unixtime() > CompareCO2OnTime.unixtime() && now.unixtime() < CompareCO2OffTime.unixtime())
    { if (PhWert >= PHUpperLimit)
      { co2Value = false;
      }
      else if (PhWert <= PHLowerLimit)
      { co2Value = true;
      }
    }
    else
    { co2Value = true;
    }

    processRelais();
    updateHomeScreen();
  }
  
  else if (!manualOverride && (now.unixtime() >= cleanEnd.unixtime())&& !MoonModeState)
  { cleaningInProcess = false;
    pump1Value = false;
    pump2Value = false;
    dispScreen = 0;
    drawScreen();
    //processRelais();
    lightCalculator();
    AI();
    processPump();
  }
  else if (now.unixtime() >= MoonEnd.unixtime())
  {MoonModeState = false;    
  }


}


void writeFile()
{    Aquaduino = SD.open("aqua.txt", FILE_WRITE);
  /*    if (Aquaduino)
  { Aquaduino.print(now.day(), DEC);
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
  }*/
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
{ if (now.unixtime() - lastFert.unixtime() > 82800) // no fertilizing for 23 hours (82800) - just to be sure
  { if ((now.hour() == doseHour) && (now.minute() >= doseMinute) && (now.minute() <= doseMinute + 10)) //timeframe of 10 Minutes to fertilize - in case that another task runs at the exact dose time
    { lastFert = now.unixtime();
      Aquaduino = SD.open("fert.txt", FILE_WRITE);
      if (Aquaduino)   //write to file
      { Aquaduino.print(lastFert.day(), DEC);
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
      fertmillis = millis();

      //fertilize N
      if (dayN[now.dayOfWeek()]  && (FLeft[0] - FDose[0] >= 0))
      {   Aquaduino.print("N:");
          Aquaduino.print(FDose[0]);
        FLeft[0] = FLeft[0] - FDose[0];
        fertmillis = millis();

        wdt_disable();

        while (millis() - fertmillis < 1000 * FDose[0] / (FRate[0] / 60))
        { dPump1Value = false;
          processRelais();
          if (dispScreen < 1) {
              myFiles.load(393, 449, 48, 48, "1nN.raw");
          }
        }
        dPump1Value = true;
        processRelais();

        wdt_enable(WDTO_8S);
        updateHomeScreen();
      }
      //fertilize NPK
      if (dayNPK[now.dayOfWeek()] && (FLeft[1] - FDose[1] >= 0))
      {   Aquaduino.print("  NPK: ");
          Aquaduino.print(FDose[1]);
        FLeft[1] = FLeft[1] - FDose[1];
        fertmillis = millis();
        wdt_disable();
        while (millis() - fertmillis < 1000 * FDose[1] / (FRate[1] / 60))
        { dPump2Value = false;
          processRelais();
          if (dispScreen < 1) {
              myFiles.load(340, 502, 48, 48, "1npkN.raw");
          }
        }
        dPump2Value = true;
        processRelais();
        wdt_enable(WDTO_8S);
        updateHomeScreen();
      }
      //fertilize FE
      if (dayFE[now.dayOfWeek()] && (FLeft[2] - FDose[2] >= 0))
      {   Aquaduino.print("  FE: ");
         Aquaduino.print(FDose[2]);
        FLeft[2] = FLeft[2] - FDose[2];
        fertmillis = millis();
        wdt_disable();
        while (millis() - fertmillis < 1000 * FDose[2] / (FRate[2] / 60))
        { dPump3Value = false;
          processRelais();
          if (dispScreen < 1) {
              myFiles.load(393, 502, 48, 48, "1feN.raw");
          }
        }
        dPump3Value = true;
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





void readLightPWM()
{ lightPWM[0].Hour = EEPROM.read(20);
  lightPWM[0].Minute = EEPROM.read(21);
  lightPWM[0].pwmValue = EEPROM.read(22);
  lightPWM[1].Hour = EEPROM.read(23);
  lightPWM[1].Minute = EEPROM.read(24);
  lightPWM[1].pwmValue = EEPROM.read(25);
  lightPWM[2].Hour = EEPROM.read(26);
  lightPWM[2].Minute = EEPROM.read(27);
  lightPWM[2].pwmValue = EEPROM.read(28);
  lightPWM[3].Hour = EEPROM.read(29);
  lightPWM[3].Minute = EEPROM.read(30);
  lightPWM[3].pwmValue = EEPROM.read(31);
  lightPWM[4].Hour = EEPROM.read(32);
  lightPWM[4].Minute = EEPROM.read(33);
  lightPWM[4].pwmValue = EEPROM.read(34);
  lightPWM[5].Hour = EEPROM.read(35);
  lightPWM[5].Minute = EEPROM.read(36);
  lightPWM[5].pwmValue = EEPROM.read(37);
  lightPWM[6].Hour = EEPROM.read(38);
  lightPWM[6].Minute = EEPROM.read(39);
  lightPWM[6].pwmValue = EEPROM.read(40);
  lightPWM[7].Hour = EEPROM.read(41);
  lightPWM[7].Minute = EEPROM.read(42);
  lightPWM[7].pwmValue = EEPROM.read(43);
  lightPWM[8].Hour = EEPROM.read(44);
  lightPWM[8].Minute = EEPROM.read(45);
  lightPWM[8].pwmValue = EEPROM.read(46);
  lightPWM[9].Hour = EEPROM.read(47);
  lightPWM[9].Minute = EEPROM.read(48);
  lightPWM[9].pwmValue = EEPROM.read(49);
  lightPWM[10].Hour = EEPROM.read(50);
  lightPWM[10].Minute = EEPROM.read(51);
  lightPWM[10].pwmValue = EEPROM.read(52);
  lightPWM[11].Hour = EEPROM.read(53);
  lightPWM[11].Minute = EEPROM.read(54);
  lightPWM[11].pwmValue = EEPROM.read(55);
}

void saveLightPWM()
{ EEPROM.write(20, lightPWM[0].Hour);
  EEPROM.write(21, lightPWM[0].Minute);
  EEPROM.write(22, byte(lightPWM[0].pwmValue));
  EEPROM.write(23, lightPWM[1].Hour);
  EEPROM.write(24, lightPWM[1].Minute);
  EEPROM.write(25, byte(lightPWM[1].pwmValue));
  EEPROM.write(26, lightPWM[2].Hour);
  EEPROM.write(27, lightPWM[2].Minute);
  EEPROM.write(28, byte(lightPWM[2].pwmValue));
  EEPROM.write(29, lightPWM[3].Hour);
  EEPROM.write(30, lightPWM[3].Minute);
  EEPROM.write(31, byte(lightPWM[3].pwmValue));
  EEPROM.write(32, lightPWM[4].Hour);
  EEPROM.write(33, lightPWM[4].Minute);
  EEPROM.write(34, byte(lightPWM[4].pwmValue));
  EEPROM.write(35, lightPWM[5].Hour);
  EEPROM.write(36, lightPWM[5].Minute);
  EEPROM.write(37, byte(lightPWM[5].pwmValue));
  EEPROM.write(38, lightPWM[6].Hour);
  EEPROM.write(39, lightPWM[6].Minute);
  EEPROM.write(40, byte(lightPWM[6].pwmValue));
  EEPROM.write(41, lightPWM[7].Hour);
  EEPROM.write(42, lightPWM[7].Minute);
  EEPROM.write(43, byte(lightPWM[7].pwmValue));
  EEPROM.write(44, lightPWM[8].Hour);
  EEPROM.write(45, lightPWM[8].Minute);
  EEPROM.write(46, byte(lightPWM[8].pwmValue));
  EEPROM.write(47, lightPWM[9].Hour);
  EEPROM.write(48, lightPWM[9].Minute);
  EEPROM.write(49, byte(lightPWM[9].pwmValue));
  EEPROM.write(50, lightPWM[10].Hour);
  EEPROM.write(51, lightPWM[10].Minute);
  EEPROM.write(52, byte(lightPWM[10].pwmValue));
  EEPROM.write(53, lightPWM[11].Hour);
  EEPROM.write(54, lightPWM[11].Minute);
  EEPROM.write(55, byte(lightPWM[11].pwmValue));

}

void readMoonMode()
{ MoonRed = EEPROM.read(172);
 MoonGreen = EEPROM.read(173);
  MoonBlue = EEPROM.read(174);
  MoonMinutes = EEPROM.read(175);
}

void saveMoonMode()
{EEPROM.write(172, MoonRed);
  EEPROM.write(173, MoonGreen);
  EEPROM.write(174, MoonBlue);
  EEPROM.write(175, MoonMinutes);
  
}



void readTVMode()
{ TVModeBrightness = EEPROM.read(176);

}

void saveTVMode()
{EEPROM.write(176, TVModeBrightness);

  
}




void readLightRGB()
{ lightRGB[0].Hour = EEPROM.read(112);
  lightRGB[0].Minute = EEPROM.read(113);
  lightRGB[0].red = EEPROM.read(114);
  lightRGB[0].green = EEPROM.read(115);
  lightRGB[0].blue = EEPROM.read(116);
  lightRGB[1].Hour = EEPROM.read(117);
  lightRGB[1].Minute = EEPROM.read(118);
  lightRGB[1].red = EEPROM.read(119);
  lightRGB[1].green = EEPROM.read(120);
  lightRGB[1].blue = EEPROM.read(121);
  lightRGB[2].Hour = EEPROM.read(122);
  lightRGB[2].Minute = EEPROM.read(123);
  lightRGB[2].red = EEPROM.read(124);
  lightRGB[2].green = EEPROM.read(125);
  lightRGB[2].blue = EEPROM.read(126);
  lightRGB[3].Hour = EEPROM.read(127);
  lightRGB[3].Minute = EEPROM.read(128);
  lightRGB[3].red = EEPROM.read(129);
  lightRGB[3].green = EEPROM.read(130);
  lightRGB[3].blue = EEPROM.read(131);
  lightRGB[4].Hour = EEPROM.read(132);
  lightRGB[4].Minute = EEPROM.read(133);
  lightRGB[4].red = EEPROM.read(134);
  lightRGB[4].green = EEPROM.read(135);
  lightRGB[4].blue = EEPROM.read(136);
  lightRGB[5].Hour = EEPROM.read(137);
  lightRGB[5].Minute = EEPROM.read(138);
  lightRGB[5].red = EEPROM.read(139);
  lightRGB[5].green = EEPROM.read(140);
  lightRGB[5].blue = EEPROM.read(141);
  lightRGB[6].Hour = EEPROM.read(142);
  lightRGB[6].Minute = EEPROM.read(143);
  lightRGB[6].red = EEPROM.read(144);
  lightRGB[6].green = EEPROM.read(145);
  lightRGB[6].blue = EEPROM.read(146);
  lightRGB[7].Hour = EEPROM.read(147);
  lightRGB[7].Minute = EEPROM.read(148);
  lightRGB[7].red = EEPROM.read(149);
  lightRGB[7].green = EEPROM.read(150);
  lightRGB[7].blue = EEPROM.read(151);
  lightRGB[8].Hour = EEPROM.read(152);
  lightRGB[8].Minute = EEPROM.read(153);
  lightRGB[8].red = EEPROM.read(154);
  lightRGB[8].green = EEPROM.read(155);
  lightRGB[8].blue = EEPROM.read(156);
  lightRGB[9].Hour = EEPROM.read(157);
  lightRGB[9].Minute = EEPROM.read(158);
  lightRGB[9].red = EEPROM.read(159);
  lightRGB[9].green = EEPROM.read(160);
  lightRGB[9].blue = EEPROM.read(161);
  lightRGB[10].Hour = EEPROM.read(162);
  lightRGB[10].Minute = EEPROM.read(163);
  lightRGB[10].red = EEPROM.read(164);
  lightRGB[10].green = EEPROM.read(165);
  lightRGB[10].blue = EEPROM.read(166);
  lightRGB[11].Hour = EEPROM.read(167);
  lightRGB[11].Minute = EEPROM.read(168);
  lightRGB[11].red = EEPROM.read(168);
  lightRGB[11].green = EEPROM.read(170);
  lightRGB[11].blue = EEPROM.read(171);
}

void saveLightRGB()
{ EEPROM.write(112, lightRGB[0].Hour);
  EEPROM.write(113, lightRGB[0].Minute);
  EEPROM.write(114, lightRGB[0].red);
  EEPROM.write(115, lightRGB[0].green);
  EEPROM.write(116, lightRGB[0].blue);
  EEPROM.write(117, lightRGB[1].Hour);
  EEPROM.write(118, lightRGB[1].Minute);
  EEPROM.write(119, lightRGB[1].red);
  EEPROM.write(120, lightRGB[1].green);
  EEPROM.write(121, lightRGB[1].blue);
  EEPROM.write(122, lightRGB[2].Hour);
  EEPROM.write(123, lightRGB[2].Minute);
  EEPROM.write(124, lightRGB[2].red);
  EEPROM.write(125, lightRGB[2].green);
  EEPROM.write(126, lightRGB[2].blue);
  EEPROM.write(127, lightRGB[3].Hour);
  EEPROM.write(128, lightRGB[3].Minute);
  EEPROM.write(129, lightRGB[3].red);
  EEPROM.write(130, lightRGB[3].green);
  EEPROM.write(131, lightRGB[3].blue);
  EEPROM.write(132, lightRGB[4].Hour);
  EEPROM.write(133, lightRGB[4].Minute);
  EEPROM.write(134, lightRGB[4].red);
  EEPROM.write(135, lightRGB[4].green);
  EEPROM.write(136, lightRGB[4].blue);
  EEPROM.write(137, lightRGB[5].Hour);
  EEPROM.write(138, lightRGB[5].Minute);
  EEPROM.write(139, lightRGB[5].red);
  EEPROM.write(140, lightRGB[5].green);
  EEPROM.write(141, lightRGB[5].blue);
  EEPROM.write(142, lightRGB[6].Hour);
  EEPROM.write(143, lightRGB[6].Minute);
  EEPROM.write(144, lightRGB[6].red);
  EEPROM.write(145, lightRGB[6].green);
  EEPROM.write(146, lightRGB[6].blue);
  EEPROM.write(147, lightRGB[7].Hour);
  EEPROM.write(148, lightRGB[7].Minute);
  EEPROM.write(149, lightRGB[7].red);
  EEPROM.write(150, lightRGB[7].green);
  EEPROM.write(151, lightRGB[7].blue);
  EEPROM.write(152, lightRGB[8].Hour);
  EEPROM.write(153, lightRGB[8].Minute);
  EEPROM.write(154, lightRGB[8].red);
  EEPROM.write(155, lightRGB[8].green);
  EEPROM.write(156, lightRGB[8].blue);
  EEPROM.write(157, lightRGB[9].Hour);
  EEPROM.write(158, lightRGB[9].Minute);
  EEPROM.write(159, lightRGB[9].red);
  EEPROM.write(160, lightRGB[9].green);
  EEPROM.write(161, lightRGB[9].blue);
  EEPROM.write(162, lightRGB[10].Hour);
  EEPROM.write(163, lightRGB[10].Minute);
  EEPROM.write(164, lightRGB[10].red);
  EEPROM.write(165, lightRGB[10].green);
  EEPROM.write(166, lightRGB[10].blue);
  EEPROM.write(167, lightRGB[11].Hour);
  EEPROM.write(168, lightRGB[11].Minute);
  EEPROM.write(169, lightRGB[11].red);
  EEPROM.write(170, lightRGB[11].green);
  EEPROM.write(171, lightRGB[11].blue);
}





void readCleanSched()
{
  pump1Clean = EEPROM.read(56);
  pump2Clean = EEPROM.read(57);
  light230Clean = EEPROM.read(58);
  light2Clean = EEPROM.read(59);
  co2Clean = EEPROM.read(60);
  heaterClean = EEPROM.read(61);
  coolClean = EEPROM.read(62);
  cleanMinutes = EEPROM.read(91);
}


void saveCleanSched()
{
  EEPROM.write(56, pump1Clean);
  EEPROM.write(57, pump2Clean);
  EEPROM.write(58, light230Clean);
  EEPROM.write(59, light2Clean);
  EEPROM.write(60, co2Clean);
  EEPROM.write(61, heaterClean);
  EEPROM.write(62, coolClean);
  EEPROM.write(91, cleanMinutes);
}



void readPHValue()
{ PHUpperLimit = (EEPROM.read(63));
  PHLowerLimit = (EEPROM.read(64));
  PHUpperLimit = PHUpperLimit / 20;
  PHLowerLimit = PHLowerLimit / 20;
}

void savePHValue()
{ EEPROM.write(63, PHUpperLimit * 20);
  EEPROM.write(64, PHLowerLimit * 20);
}


void readTempValue()
{ TempUpperLimit = (EEPROM.read(65));
  TempLowerLimit = (EEPROM.read(66));

}

void saveTempValue()
{ EEPROM.write(65, TempUpperLimit);
  EEPROM.write(66, TempLowerLimit);
}



void readFerti()
{ FDose[0] = EEPROM.read(0);
  FDose[1] = EEPROM.read(1);
  FDose[2] = EEPROM.read(2);
  FMax[0] = (EEPROM.read(3) * 5); //max 1,2 Liter max amount
  FMax[1] = (EEPROM.read(4) * 5); //max 1,2 Liter max amount
  FMax[2] = (EEPROM.read(5) * 5); //max 1,2 Liter max amount
  FLeft[0] = (EEPROM.read(6) * 5); //max 1,2 Liter max amount
  FLeft[1] = (EEPROM.read(7) * 5); //max 1,2 Liter max amount
  FLeft[2] = (EEPROM.read(8) * 5); //max 1,2 Liter max amount
  FRate[0] = EEPROM.read(67);
  FRate[1] = EEPROM.read(68);
  FRate[2] = EEPROM.read(69);
  MoF[0] = EEPROM.read(70);
  MoF[1] = EEPROM.read(71);
  MoF[2] = EEPROM.read(72);
  TuF[0] = EEPROM.read(73);
  TuF[1] = EEPROM.read(74);
  TuF[2] = EEPROM.read(75);
  WeF[0] = EEPROM.read(76);
  WeF[1] = EEPROM.read(77);
  WeF[2] = EEPROM.read(78);
  ThF[0] = EEPROM.read(79);
  ThF[1] = EEPROM.read(80);
  ThF[2] = EEPROM.read(81);
  FrF[0] = EEPROM.read(82);
  FrF[1] = EEPROM.read(83);
  FrF[2] = EEPROM.read(84);
  SaF[0] = EEPROM.read(85);
  SaF[1] = EEPROM.read(86);
  SaF[2] = EEPROM.read(87);
  SuF[0] = EEPROM.read(88);
  SuF[1] = EEPROM.read(89);
  SuF[2] = EEPROM.read(90);
  doseHour = EEPROM.read(92);
  doseMinute = EEPROM.read(93);


  dayN[1] = MoF[0];
  dayN[2] = TuF[0];
  dayN[3] = WeF[0];
  dayN[4] = ThF[0];
  dayN[5] = FrF[0];
  dayN[6] = SaF[0];
  dayN[0] = SuF[0];

  dayNPK[1] = MoF[1];
  dayNPK[2] = TuF[1];
  dayNPK[3] = WeF[1];
  dayNPK[4] = ThF[1];
  dayNPK[5] = FrF[1];
  dayNPK[6] = SaF[1];
  dayNPK[0] = SuF[1];

  dayFE[1] = MoF[2];
  dayFE[2] = TuF[2];
  dayFE[3] = WeF[2];
  dayFE[4] = ThF[2];
  dayFE[5] = FrF[2];
  dayFE[6] = SaF[2];
  dayFE[0] = SuF[2];






}

void saveFerti()
{ EEPROM.write(0, FDose[0]);
  EEPROM.write(1, FDose[1]);
  EEPROM.write(2, FDose[2]);
  EEPROM.write(3, int(FMax[0] / 5));
  EEPROM.write(4, int(FMax[1] / 5));
  EEPROM.write(5, int(FMax[2] / 5));
  EEPROM.write(6, int(FLeft[0] / 5));
  EEPROM.write(7, int(FLeft[1] / 5));
  EEPROM.write(8, int(FLeft[2] / 5));
  EEPROM.write(67, FRate[0]);
  EEPROM.write(68, FRate[1]);
  EEPROM.write(69, FRate[2]);
  EEPROM.write(70, MoF[0]);
  EEPROM.write(71, MoF[1]);
  EEPROM.write(72, MoF[2]);
  EEPROM.write(73, TuF[0]);
  EEPROM.write(74, TuF[1]);
  EEPROM.write(75, TuF[2]);
  EEPROM.write(76, WeF[0]);
  EEPROM.write(77, WeF[1]);
  EEPROM.write(78, WeF[2]);
  EEPROM.write(79, ThF[0]);
  EEPROM.write(80, ThF[1]);
  EEPROM.write(81, ThF[2]);
  EEPROM.write(82, FrF[0]);
  EEPROM.write(83, FrF[1]);
  EEPROM.write(84, FrF[2]);
  EEPROM.write(85, SaF[0]);
  EEPROM.write(86, SaF[1]);
  EEPROM.write(87, SaF[2]);
  EEPROM.write(88, SuF[0]);
  EEPROM.write(89, SuF[1]);
  EEPROM.write(90, SuF[2]);
  EEPROM.write(92, doseHour);
  EEPROM.write(93, doseMinute);

  dayN[1] = MoF[0];
  dayN[2] = TuF[0];
  dayN[3] = WeF[0];
  dayN[4] = ThF[0];
  dayN[5] = FrF[0];
  dayN[6] = SaF[0];
  dayN[0] = SuF[0];

  dayNPK[1] = MoF[1];
  dayNPK[2] = TuF[1];
  dayNPK[3] = WeF[1];
  dayNPK[4] = ThF[1];
  dayNPK[5] = FrF[1];
  dayNPK[6] = SaF[1];
  dayNPK[0] = SuF[1];

  dayFE[1] = MoF[2];
  dayFE[2] = TuF[2];
  dayFE[3] = WeF[2];
  dayFE[4] = ThF[2];
  dayFE[5] = FrF[2];
  dayFE[6] = SaF[2];
  dayFE[0] = SuF[2];
}



void readReminder()
{ //tankCleanDay=EEPROM.read(94);
  /*tankCleanMonth=EEPROM.read(95);
  co2BottleDay=EEPROM.read(96);
  co2BottleMonth=EEPROM.read(97);
  cleanFilter1Day=EEPROM.read(98);
  cleanFilter1Month=EEPROM.read(99);
  cleanFilter2Day=EEPROM.read(100);
  cleanFilter2Month=EEPROM.read(101);
  */
  tankClean = DateTime (now.year(), EEPROM.read(95), EEPROM.read(94), now.hour(), now.minute(), 0);
  co2Bottle = DateTime (now.year(), EEPROM.read(97), EEPROM.read(96), now.hour(), now.minute(), 0);
  cleanFilter1 = DateTime (now.year(), EEPROM.read(99), EEPROM.read(98), now.hour(), now.minute(), 0);
  cleanFilter2 = DateTime (now.year(), EEPROM.read(101), EEPROM.read(100), now.hour(), now.minute(), 0);
  tankCleandDays = EEPROM.read(102);
  co2BottleDays = EEPROM.read(103);
  cleanFilter1Days = EEPROM.read(104);
  cleanFilter2Days = EEPROM.read(105);
  // DateTime helpDT (now.year(),now.month(),now.day(),int(lightPWM[i].Hour),int(lightPWM[i].Minute),0);
}

void saveReminder()
{ EEPROM.write(94, tankClean.day());
  EEPROM.write(95, tankClean.month());
  EEPROM.write(96, co2Bottle.day());
  EEPROM.write(97, co2Bottle.month());
  EEPROM.write(98, cleanFilter1.day());
  EEPROM.write(99, cleanFilter1.month());
  EEPROM.write(100, cleanFilter2.day());
  EEPROM.write(101, cleanFilter2.month());
  EEPROM.write(102, tankCleandDays);
  EEPROM.write(103, co2BottleDays);
  EEPROM.write(104, cleanFilter1Days);
  EEPROM.write(105, cleanFilter2Days);

}



void readPowerSchedule()
{ powLightOnHour = EEPROM.read(9);
  powLightOnMinute = EEPROM.read(10);
  powLightOffHour = EEPROM.read(11);
  powLightOffMinute = EEPROM.read(12);
  powCo2OnHour = EEPROM.read(13);
  powCo2OnMinute = EEPROM.read(14);
  powCo2OffHour = EEPROM.read(15);
  powCo2OffMinute = EEPROM.read(16);
}

void savePowerSchedule()
{ EEPROM.write(9, powLightOnHour);
  EEPROM.write(10, powLightOnMinute);
  EEPROM.write(11, powLightOffHour);
  EEPROM.write(12, powLightOffMinute);
  EEPROM.write(13, powCo2OnHour);
  EEPROM.write(14, powCo2OnMinute);
  EEPROM.write(15, powCo2OffHour);
  EEPROM.write(16, powCo2OffMinute);
}



void readScreenScreen()
{ screenOnHour = EEPROM.read(106);
  screenOffHour = EEPROM.read(107);
  screenOnMinute = EEPROM.read(108);
  screenOffMinute = EEPROM.read(109);
  standByMinutes = EEPROM.read(110);
  backlightPWM = EEPROM.read(111);
}

void saveScreenScreen()
{ EEPROM.write(106, screenOnHour);
  EEPROM.write(107, screenOffHour);
  EEPROM.write(108, screenOnMinute);
  EEPROM.write(109, screenOffMinute);
  EEPROM.write(110, standByMinutes);
  EEPROM.write(111, backlightPWM);
}







void parseCommand(String com)
{
  String part1;
  // part1 = com.substring(0, com.indexOf("|"));
  if (com.substring(0, com.indexOf("|")).equalsIgnoreCase("toMega"))
  {
    for (int i = com.indexOf("|"); i < com.lastIndexOf("|"); i = com.indexOf("|", i + 1))
    { String part1 = com.substring(i + 1, com.indexOf("|", i + 1));
      int cmdID = findCommand(part1.substring(0, part1.indexOf("_")).c_str());
      switch (cmdID)
      { //case tPhWert : {PhWert=(part1.substring(part1.indexOf("_")+1,part1.length())).toFloat();  break;}
        //case tTemp : {Temp=(part1.substring(part1.indexOf("_")+1,part1.length())).toFloat();    break;}
        case tcalculatedPWM : {calculatedPWM = (part1.substring(part1.indexOf("_") + 1, part1.length())).toFloat();   sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedPWM));break;}
        case tcalculatedRed : {calculatedRed = (part1.substring(part1.indexOf("_") + 1, part1.length())).toFloat(); sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedRed)); break;}
        case tcalculatedGreen : { calculatedGreen = (part1.substring(part1.indexOf("_") + 1, part1.length())).toFloat(); sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedGreen)); break;}
        case tcalculatedBlue : { calculatedBlue = (part1.substring(part1.indexOf("_") + 1, part1.length())).toFloat();sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedBlue)); break;}
        case tTVModeState : { TVModeState = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt(); sendCommand(part1.substring(0, part1.indexOf("_")), String(TVModeState)); break;}
        case tcleaningInProcess : {cleaningInProcess = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt(); sendCommand(part1.substring(0, part1.indexOf("_")), String(cleaningInProcess)); break;}
        case tmanualOverride : {manualOverride = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(manualOverride));break;}
        case tMoonModeState : {MoonModeState = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(MoonModeState));break;}
        case tpump1Value : {pump1Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(pump1Value));break;}
        case tpump2Value : {pump2Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(pump2Value));break;}
        case tlight230Value : {light230Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(light230Value));break;}
        case tlight1Value : {light1Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(light1Value));break;}
        case tlight2Value : {light2Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(light2Value));break;}
        case tco2Value : {co2Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(co2Value));break;}
        case theaterValue : {heaterValue = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(heaterValue));break;}
        case tdPump1Value : {dPump1Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(dPump1Value));break;}
        case tdPump2Value : {dPump2Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(dPump2Value));break;}
        case tdPump3Value : {dPump3Value = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(dPump3Value));break;}
        case tcoolValue : {coolValue = (part1.substring(part1.indexOf("_") + 1, part1.length())).toInt();sendCommand(part1.substring(0, part1.indexOf("_")), String(coolValue));break;}
        //case tnow : {rtc.adjust(part1.substring(part1.indexOf("_") + 1, part1.length()).toInt());sendCommand(part1.substring(0, part1.indexOf("_")), String(now.unixtime()));break;}
        case tprocess : {processRelais(); AI(); break;}
     
      }
#if debug
      {
        printMyValues();
      }
#endif
    }
  }
  else  if (com.substring(0, com.indexOf("|")).equalsIgnoreCase("toESP"))
  {
    for (int i = com.indexOf("|"); i < com.lastIndexOf("|"); i = com.indexOf("|", i + 1))
    { String part1 = com.substring(i + 1, com.indexOf("|", i + 1));
      int cmdID = findCommand(part1.substring(0, part1.indexOf("_")).c_str());
      switch (cmdID)
      { case tPhWert : {sendCommand(part1.substring(0, part1.indexOf("_")), String(PhWert));break;}
        case tTemp : {sendCommand(part1.substring(0, part1.indexOf("_")), String(Temp));break;}
        case tcalculatedPWM : {sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedPWM));break;}
        case tcalculatedRed : {sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedRed));break;}
        case tcalculatedGreen : {sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedGreen));break;}
        case tcalculatedBlue : {sendCommand(part1.substring(0, part1.indexOf("_")), String(calculatedBlue));break;}
        case tTVModeState : {sendCommand(part1.substring(0, part1.indexOf("_")), String(TVModeState));break;}
        case tcleaningInProcess : {sendCommand(part1.substring(0, part1.indexOf("_")), String(cleaningInProcess));break;}
        case tmanualOverride : {sendCommand(part1.substring(0, part1.indexOf("_")), String(manualOverride));break;}
        case tMoonModeState : {sendCommand(part1.substring(0, part1.indexOf("_")), String(MoonModeState));break;}
        case tpump1Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(pump1Value));break;}
        case tpump2Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(pump2Value));break;}
        case tlight230Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(light230Value));break;}
        case tlight1Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(light1Value));break;}
        case tlight2Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(light2Value));break;}
        case tco2Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(co2Value));break;}
        case theaterValue : {sendCommand(part1.substring(0, part1.indexOf("_")), String(heaterValue));break;}
        case tdPump1Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(dPump1Value));break;}
        case tdPump2Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(dPump2Value));break;}
        case tdPump3Value : {sendCommand(part1.substring(0, part1.indexOf("_")), String(dPump3Value));break;}
        case tcoolValue : {sendCommand(part1.substring(0, part1.indexOf("_")), String(coolValue));break;}
          //case tnow : {sendCommand(part1.substring(0, part1.indexOf("_")), String(now)); break;}

      }
    }
  }

  else
  {
#if debug
    { Serial.println(F("-- No known Command -- "));
      Serial.println(com);
      Serial.println(F("-- No known Command -- "));

    }
#endif
  }

}




void giveAllData()
{
  sendSerial( "toESP|PhWert_" + String(PhWert) + "|" + "Temp_" + String(Temp) + "|" + "calculatedPWM_" + String(calculatedPWM) + "|" + "calculatedRed_" + String(calculatedRed) + "|" +
 "calculatedGreen_" + String(calculatedGreen) + "|" + "calculatedBlue_" + String(calculatedBlue) + "|" + "TVModeState_" + String(TVModeState) + "|" +
 "cleaningInProcess_" + String(cleaningInProcess) + "|" + "manualOverride_" + String(manualOverride) + "|" + "MoonModeState_" + String(MoonModeState) + "|" +
 "pump1Value_" + String(pump1Value) + "| \n");

  delay(20);
    sendSerial( "toESP|pump2Value_" + String(pump2Value) + "|" + "light230Value_" + String(light230Value) + "|" +
 "light1Value_" + String(light1Value) + "|" + "light2Value_" + String(light2Value) + "|" + "co2Value_" + String(co2Value) + "|" +
 "heaterValue_" + String(heaterValue) + "|" + "dPump1Value_" + String(dPump1Value) + "|" + "dPump2Value_" + String(dPump2Value) + "|" +
 "dPump3Value_" + String(dPump3Value) + "|" + "coolValue_" + String(coolValue) + "| \n");



 
}




void sendSerial(String sendCom)
{
 String sendThis = sendCom + "| \n" ;
  char charBuf[350];
  sendThis.toCharArray(charBuf, 350);
  Serial2.write(charBuf);
# if debug
  {
    Serial.print(charBuf);
  }
#endif
}



void sendCommand(String variable, String sendCom)
{
  String sendThis = "toESP|" + variable + "_" + sendCom + "| \n" ;
  char charBuf[350];
  sendThis.toCharArray(charBuf, 350);
  Serial2.write(charBuf);
# if debug
  {
    Serial.print(charBuf);
  }
#endif
}





int findCommand(const char* searchText)
{
  int startCount = 0;
  int foundIndex = -1; // -1 = not found
  while (startCount < charCount)
  {
    if (strcmp_P(searchText, (const char*)pgm_read_ptr(Char_table + startCount)) == 0)
    {
      foundIndex = startCount;
      break;
    }
    startCount++;
  }
  return foundIndex;
}








void printMyValues()
{
#if debug
{
  Serial.println(F("-----Date and Time--------"));
  Serial.println(F("NOW: "));
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(' ');
  Serial.print(now.day(), DEC);
  Serial.print('.');
  Serial.print(now.month(), DEC);
  Serial.print('.');
  Serial.println(now.year(), DEC);
  Serial.println(F("------PH and Temp--------"));
  Serial.println(F("PhWert: "));
  Serial.println(PhWert);
  Serial.println(F("Temp: "));
  Serial.println(Temp);
  Serial.println(F("-----------Modes---------"));
  Serial.print(F("TVModeState: "));
  Serial.println(TVModeState);
  Serial.print(F("cleaningInProcess: "));
  Serial.println(cleaningInProcess);
  Serial.print(F("manualOverride: "));
  Serial.println(manualOverride);
  Serial.print(F("MoonModeState: "));
  Serial.println(MoonModeState);
  Serial.println(F("--------Lights----------"));
  Serial.print(F("calculatedPWM: "));
  Serial.println(calculatedPWM);
  Serial.print(F("calculatedRed: "));
  Serial.println(calculatedRed);
  Serial.print(F("calculatedGreen: "));
  Serial.println(calculatedGreen);
  Serial.print(F("calculatedBlue: "));
  Serial.println(calculatedBlue);
  Serial.println(F("-------Booleans--------"));
  Serial.print(F("pump1Value: "));
  Serial.println(pump1Value);
  Serial.print(F("pump2Value: "));
  Serial.println(pump2Value);
  Serial.print(F("light230Value: "));
  Serial.println(light230Value);
  Serial.print(F("light1Value: "));
  Serial.println(light1Value);
  Serial.print(F("light2Value: "));
  Serial.println(light2Value);
  Serial.print(F("co2Value: "));
  Serial.println(co2Value);
  Serial.print(F("heaterValue: "));
  Serial.println(heaterValue);
  Serial.print(F("dPump1Value: "));
  Serial.println(dPump1Value);
  Serial.print(F("dPump2Value: "));
  Serial.println(dPump2Value);
  Serial.print(F("dPump3Value: "));
  Serial.println(dPump3Value);
  Serial.print(F("coolValue: "));
  Serial.println(coolValue);
  Serial.println(F("----------------------"));
}
#endif
}








