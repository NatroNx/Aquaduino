// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SCREENS
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void drawScreen() 
{if (dispScreen != lastScreen) {myGLCD.clrScr();} //clear everything on screenchange
lastScreen=dispScreen;
//selection which screen to draw
printDate(now, 5,5);
switch (dispScreen) 
  {case 0:  // home screen
    drawClockPhPWM();
    drawFertilizer();
    HomeScreen();

    break;
    case 1:  // clean screen
    FeedScreen();
    break;
    case 2:  // power screen
    PowerScreen();
    break;
    case 3:  // settings screen
    SettingsScreen();
    break;
    case 4:  // lights screen
    LightsScreen();
    break;
    case 41:
    case 42:   
    case 43: 
    case 44:
    case 45:
    case 46:
    LightScene();
    break;  
    case 5:  // clock screen
    ClockScreen();
    break;
    case 6:  // CleanSchedScreen
    CleanSchedScreen();
    updateCleanSchedScreen();
    quickUpdateCleanSchedScreen();
    break;
    case 7:  // RemindScreen
    RemindScreen();
    break;
    case 8:  // HeaterScreen
    HeaterScreen();
    updateHeaterScreen();
    break;
    case 9:  // DoseScreen
    DoseScreen();
    updateDoseScreen();
    break;
    case 91:  // DoseScreen
    DoseScreenN();
    UpdateDoseScreenN(FSelect);
    quickUpdateDoseScreenN(FSelect);
    break;

    case 10:  // DoseScreen
    PowerSchedScreen();
    break;
    case 11:  // DoseScreen
    ScreenScreen();
    break;
    case 12:  // DoseScreen
    Co2SetScreen();
    updateCO2SetScreen();
    break;
   case 13:
    ScreenScreen();
    break;
  }
}



 /*  DRAW ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    draw Methoden zur Darstellung auf dem Touchscreen
    screens are listed below                       
    0-home, 1-cleaning, , 2-power, 3-extras, 4-lights
    5-clock, 6-feeding sched, 7-schedule, 8-heater
    9-dosing, 10-pwer schedule, 11-schedule item, 13=TestScreen
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


 
  
void printCoords() //for troubleshooting
{ myGLCD.setFont(BigFont);
  myGLCD.setColor(255,255,255);
  myGLCD.printNumI(x, RIGHT,20,3,48);           
  myGLCD.printNumI(y, RIGHT,40,3,48);  
  myGLCD.drawPixel(x,y);
}






//draw write Date to an char Array and draw Time and Date
void printDate(DateTime printnow, int tx, int ty) 
{  char  chDate[25], tmpChar[5];
   strcat(chDate, "     ");
   chDate[0] = '\0'; 
   strcat(chDate, dayName[printnow.dayOfWeek()]);
   strcat(chDate, ",");
   itoa(printnow.day(), tmpChar, 10);
   strcat(chDate, tmpChar);
   strcat(chDate, ".");
   strcat(chDate, monthName[printnow.month()]);
   strcat(chDate, "   ");
   myGLCD.setFont(BigFont);
   myGLCD.setColor(255, 255, 255);
   myGLCD.print(chDate, tx, ty);            //Display date 
   myGLCD.setColor(255, 255, 255);
   myGLCD.printNumI(printnow.hour(), 400,ty,2,48);
   myGLCD.print(":", 430,ty);
   myGLCD.printNumI(printnow.minute(), 445,ty,2,48);
   //printCoords();             //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~for TSHOOT
     myGLCD.setColor(0, 0, 255);
 // myGLCD.fillRoundRect (Button1Cord[0], Button1Cord[1], Button1Cord[2], Button1Cord[3]);
  myGLCD.drawLine(Button1Cord[0], Button1Cord[1], Button1Cord[2], Button1Cord[1]);
  myGLCD.drawLine(Button1Cord[2], Button1Cord[1], Button1Cord[2], Button1Cord[3]);  // -
  myGLCD.drawLine(Button1Cord[0], Button1Cord[1], Button1Cord[0], Button1Cord[3]); // links senkrecht
  myGLCD.drawLine(Button1Cord[0], Button1Cord[3], Button1Cord[2], Button1Cord[3]);   // 
   
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 0. draw Homescreen
void HomeScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(36,0,255);
   myGLCD.print("HOME       ", 77,40);
   myFiles.load(6, 24, 60,60, "60home.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
   myGLCD.drawLine(30,270,450,270);  //2nd line
  //icons - switchin with state
  
updateHomeScreen();
   // lightmode
    myFiles.load(LightUp[0], LightUp[1], 48,48, "48up.raw");
     myFiles.load(LightDown[0], LightDown[1], 48,48, "48down.raw");
     myFiles.load(90, 85, 120,102, "1therm.raw");
   //draw the fertilizer
    myGLCD.drawLine(30,570,450,570); //3rd line
    myFiles.load(21, 615,438,96, "DockNew.raw");  
    myGLCD.setFont(BigFont);
    myGLCD.print("Home", 37, 720);
    myGLCD.print("Feed", 149, 720);
    myGLCD.print("Power", 256, 720);
    myGLCD.print("Setting", 357, 720);


   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "ResetF.raw"); 
  

    
}

void updateHomeScreen()
{  wdt_reset();  
  if(dispScreen<1)
  {if(!pump1Value) { myFiles.load(340, 290,48,48, "3filt_N.raw");}
   else { myFiles.load(340, 290,48,48, "3filt_F.raw");}
   if(!pump2Value)  {myFiles.load(393, 290,48,48, "3filt_N.raw");}
   else   { myFiles.load(393, 290,48,48, "3filt_F.raw");}
   if(!light230Value) {myFiles.load(340, 343,48,48, "3light_N.raw"); }
   else {myFiles.load(340, 343,48,48, "3light_F.raw"); }
   if(!light2Value) { myFiles.load(393, 343,48,48, "3light_N.raw");}
   else { myFiles.load(393, 343,48,48, "3light_F.raw");}
   if(!co2Value) {myFiles.load(340, 396,48,48, "3co2_N.raw"); }
   else  {myFiles.load(340, 396,48,48, "3co2_F.raw"); }
   if(!heaterValue ) { myFiles.load(393, 396,48,48, "3heat_N.raw");}
   else { myFiles.load(393, 396,48,48, "3heat_F.raw");}
   if(!coolValue)  { myFiles.load(340, 449,48,48, "3circ_N.raw"); }
   else { myFiles.load(340, 449,48,48, "3circ_F.raw"); }
   if(!dPump1Value) {myFiles.load(393, 449,48,48, "1nN.raw"); }
   else {myFiles.load(393, 449,48,48, "1nF.raw"); }
   if(!dPump2Value){myFiles.load(340, 502,48,48, "1npkN.raw");}
   else {myFiles.load(340, 502,48,48, "1npkF.raw");}
   if(!dPump3Value)  { myFiles.load(393, 502,48,48, "1feN.raw"); }
   else { myFiles.load(393, 502,48,48, "1feF.raw"); }
    }
}

void drawFertilizer()  //methode zur Berechnung der Düngermenge sowie Darstellung der Düngerreserve
{ if (dispScreen<1)
    {myGLCD.setFont(BigFont);
     myFiles.load(166, 370,46,130, "1ferts.raw");
     myGLCD.setColor(col_FertiN.r,col_FertiN.g,col_FertiN.b);
     myGLCD.fillRect(178,(484-((100/(FMax[0]/FDose[0]))*((FLeft[0]/(FDose[0]))))),199,484);
     myGLCD.printNumI((FLeft[0]/FDose[0]),164,348,2);
     myGLCD.print("N",180,330,2);
     
     myFiles.load(217, 370,46,130, "1ferts.raw");
     myGLCD.setColor(col_FertiNPK.r, col_FertiNPK.g, col_FertiNPK.b);
     myGLCD.fillRect(229,(484-((100/(FMax[1]/FDose[1]))*((FLeft[1]/(FDose[1]))))),250,484);
     myGLCD.printNumI((FLeft[1]/FDose[1]),217,348,2);
     myGLCD.print("NPK",218,330,2);
     
     myFiles.load(268, 370,46,130, "1ferts.raw");
     myGLCD.setColor(col_FertiFE.r, col_FertiFE.g, col_FertiFE.b);
     myGLCD.fillRect(280,(484-((100/(FMax[2]/FDose[2]))*((FLeft[2]/(FDose[2]))))),301,484);
     myGLCD.printNumI((FLeft[2]/FDose[2]),267,348,2);
     myGLCD.print("FE",275,330,2);
    }
}  

void drawClockPhPWM()
{
  if(Temp>=28)
      {myFiles.load(90, 85, 120,102, "1thermR.raw"); }
   else
      {myFiles.load(90, 85, 120,102, "1therm.raw"); }
   myGLCD.setColor(col_white.r, col_white.g,col_white.b);    
   myGLCD.setFont(SevenSegmentFull);
   myGLCD.print("o",410,90);                // Degree icon
   myGLCD.printNumF(Temp,2, 245, 115);
   myGLCD.setColor(0,255,0);
   myGLCD.print("PH:  ", 104, 210);
   myGLCD.setColor(255,255,255);
   myGLCD.printNumF(PhWert,2, 275, 210);
   myGLCD.setFont(BigFont);
   if (cleaningInProcess)
    {myGLCD.setColor(col_red.r, col_red.g, col_red.b);
     myGLCD.print("CLEANING IN PROGRESS", 5, 545);
    }
    else if (manualOverride)
    {myGLCD.setColor(col_red.r, col_red.g, col_red.b);
     myGLCD.print("MANUAL OVERRIDE     ", 5, 545);
    }
    else
   {myGLCD.print("                     ", 5, 545);
   }
     
 drawPWM();
 
           
}

void drawPWM()
{  
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(255,int(calculatedPWM),0);
   if (calculatedPWM>=25.5) 
     {
     // myGLCD.printNumI(int(100-calculatedPWM*100/180+75/1.8), 23,392,3);
      myGLCD.printNumI(int(calculatedPWM*100/255), 23,392,3);
      }
   else 
     {myGLCD.printNumI(int(calculatedPWM*100/255), 27,392,3);
     
     }
     myGLCD.print("%" , 108,392,3);
     
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//1. FeedScreen
void FeedScreen()
{  //Statusline TOP
   wdt_reset();
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(0,219,0);
   myGLCD.print("FEEDING     ", 77,40);
   myFiles.load(6, 24, 60,60, "60Clea.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
      //end of >TOP
   myGLCD.print("FEEDING",20,136);
   myGLCD.print("in progress...",135,180);
   myGLCD.print("RESET", 110, 635);
   myFiles.load(20, 608,74,74, "74Reset.raw");

  
   if(!pump1Value) { myFiles.load(340, 290,48,48, "3filt_N.raw");}
   else { myFiles.load(340, 290,48,48, "3filt_F.raw");}
   if(!pump2Value)  {myFiles.load(393, 290,48,48, "3filt_N.raw");}
   else   { myFiles.load(393, 290,48,48, "3filt_F.raw");}
   if(!light230Value) {myFiles.load(340, 343,48,48, "3light_N.raw"); }
   else {myFiles.load(340, 343,48,48, "3light_F.raw"); }
   if(!light2Value) { myFiles.load(393, 343,48,48, "3light_N.raw");}
   else { myFiles.load(393, 343,48,48, "3light_F.raw");}
   if(!co2Value) {myFiles.load(340, 396,48,48, "3co2_N.raw"); }
   else  {myFiles.load(340, 396,48,48, "3co2_F.raw"); }
   if(!heaterValue ) { myFiles.load(393, 396,48,48, "3heat_N.raw");}
   else { myFiles.load(393, 396,48,48, "3heat_F.raw");}
   wdt_reset();
   if(!dPump1Value) {myFiles.load(340, 449,48,48, "1nN.raw"); }
   else {myFiles.load(340, 449,48,48, "1nF.raw"); }
   if(!dPump2Value){myFiles.load(393, 449,48,48, "1npkN.raw");}
   else {myFiles.load(393, 449,48,48, "1npkF.raw");}
   if(!dPump3Value)  { myFiles.load(340, 502,48,48, "1feN.raw"); }
   else { myFiles.load(340, 502,48,48, "1feF.raw"); }
   if(!coolValue)  { myFiles.load(393, 502,48,48, "3circ_N.raw"); }
   else { myFiles.load(393, 502,48,48, "3circ_F.raw"); }
    
  myGLCD.setColor(col_white.r, col_white.g,col_white.b);
  myFiles.load(80, 310,184,184, "2feeding.raw"); 
  myGLCD.setFont(BigFont);

/*  myGLCD.print("Feed mode ends at:", 20, 520);
  myGLCD.setFont(UbuntuBold);
  myGLCD.printNumI(cleanEnd.hour(), 110,564,2,48);
  myGLCD.print(":", 160,564);
  myGLCD.printNumI(cleanEnd.minute(), 180,564,2,48);   
**/

//footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2. Power
void PowerScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(219,0,0);
   myGLCD.print("POWER       ", 77,40);
   myFiles.load(6, 24, 60,60, "60Powe.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP

   //icons starts here
   //myFiles.load(40, 180,74,74, "74LED_N.raw"); 
   //icosn end here
   updatePowerIcons();

   
   //text for the buttons
   myGLCD.setFont(BigFont);
    myGLCD.setColor(col_red.r, col_red.g, col_red.b);
   if(manualOverride) {myGLCD.print("Manual Override is active", 20,100);}
   else {myGLCD.print("                         ", 20,100);}
   if(cleaningInProcess) 
     {  myGLCD.print("Cleaning ends @", 20,125);
        myGLCD.setColor(col_white.r, col_white.g, col_white.b);
        myGLCD.printNumI(cleanEnd.hour(), 310-30,125,2,48);
        myGLCD.print(":", 360-30,125);
        myGLCD.printNumI(cleanEnd.minute(), 380-30,125,2,48);   
 }
   else {myGLCD.print("                         ", 20,125);}
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.print("Filter 1", 110, 180);
   myGLCD.print("Filter 2", 334, 180);
   myGLCD.print("Light 1", 110, 264);
   myGLCD.print("Light 2", 334, 264);
   myGLCD.print("CO2", 110, 348);
   myGLCD.print("Heater", 334, 348);
   myGLCD.print("Cooling", 110, 432);
   myGLCD.print("ALL OFF", 110, 550);
   myGLCD.print("RESET", 110, 635);
   myGLCD.print("CLEAN", 334, 550);






   
 
   
   
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
}



void updatePowerIcons()
{   if(!pump1Value) { myFiles.load(20, 150,74,74, "74Filt_N.raw");}
   else { myFiles.load(20, 150,74,74, "74Filt_F.raw");}
   if(!pump2Value)  {myFiles.load(250, 150,74,74, "74Filt_N.raw");}
   else   { myFiles.load(250, 150,74,74, "74Filt_F.raw");}
   if(!light230Value) {myFiles.load(20, 234,74,74, "74LED_N.raw"); }
   else {myFiles.load(20, 234,74,74, "74LED_F.raw"); }
   if(!light2Value) { myFiles.load(250, 234,74,74, "74LED_N.raw");}
   else { myFiles.load(250, 234,74,74, "74LED_F.raw");}
   if(!co2Value) {myFiles.load(20, 318,74,74, "74CO2_N.raw"); }
   else  {myFiles.load(20, 318,74,74, "74CO2_F.raw"); }
   if(!heaterValue ) { myFiles.load(250, 318,74,74, "74heat_N.raw");}
   else { myFiles.load(250, 318,74,74, "74heat_F.raw");}
   if(!coolValue)  { myFiles.load(20, 402,74,74, "74Fan_N.raw"); }
   else { myFiles.load(20, 402,74,74, "74Fan_F.raw"); }
   myFiles.load(20, 524,74,74, "74OFF.raw");
   myFiles.load(20, 608,74,74, "74Reset.raw");
    myFiles.load(250, 524,74,74, "74CleanN.raw");  //Cleanmode
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 3. Settings
void SettingsScreen()
{  //Statusline TOP 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(255,36,0);
   myGLCD.print("SETTINGS      ", 77,40);
   myFiles.load(6, 24, 60,60, "60Sett.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   
   //end of >TOP
    myFiles.load(PowerSchedCord[0], PowerSchedCord[1],74,74, "74Powe.raw"); 
    myFiles.load(LightsCord[0], LightsCord[1],74,74, "74Ligh.raw"); 
    myFiles.load(CleanCord[0], CleanCord[1],74,74, "74CleanN.raw"); 
    myFiles.load(ScheCord[0], ScheCord[1],74,74, "74Sche.raw"); 
    myFiles.load(ClockCord[0], ClockCord[1],74,74, "74Cloc.raw"); 
    myFiles.load(Co2SetCord[0], Co2SetCord[1],74,74, "74Co2_N.raw"); 
    myFiles.load(HeatCord[0], HeatCord[1],74,74, "74Heat.raw");
    myFiles.load(DoseCord[0], DoseCord[1],74,74, "74Dose.raw");   
    myFiles.load(ScreenCord[0], ScreenCord[1],74,74, "74Scree.raw");   
      
    //footer starts here
     myGLCD.setColor(col_white.r, col_white.g, col_white.b);
     myGLCD.drawLine(30,770,196,770); 
     myGLCD.drawLine(284,770,450,770); 
     myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 4. Lights
void LightsScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(182,0,255);
   myGLCD.print("LIGHTS            ", 77,40);
   myFiles.load(6, 24, 60,60, "60Ligh.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
   
   myGLCD.setFont(OCR_A_Extended_M);
   
   myGLCD.setColor(255,255,255);
   myGLCD.print("Light Scene 1" , 15,140);  
   myGLCD.printNumI(lightPWM[0].Hour,255,140,2,48);
   myGLCD.print(":",290,140);
   myGLCD.printNumI(lightPWM[0].Minute,305,140,2,48);
   myGLCD.print(" - ",340,140);
   myGLCD.printNumI(lightPWM[1].Hour,385,140,2,48);
   myGLCD.print(":",420,140);
   myGLCD.printNumI(lightPWM[1].Minute,435,140,2,48);
   myGLCD.setColor(182,0,255);
   myGLCD.printNumI(int(lightPWM[0].pwmValue*100/255) , 274,170,3);
   myGLCD.print("%", 320,170);
   myGLCD.printNumI(int(lightPWM[1].pwmValue*100/255) , 404,170,3);
   myGLCD.print("%", 450,170);
   
   
      myGLCD.setColor(255,255,255);
   myGLCD.print("Light Scene 2" , 15,230);  
   myGLCD.printNumI(lightPWM[2].Hour,255,230,2,48);
   myGLCD.print(":",290,230);
   myGLCD.printNumI(lightPWM[2].Minute,305,230,2,48);
   myGLCD.print(" - ",340,230);
   myGLCD.printNumI(lightPWM[3].Hour,385,230,2,48);
   myGLCD.print(":",420,230);
   myGLCD.printNumI(lightPWM[3].Minute,435,230,2,48);
   myGLCD.setColor(182,0,255);
   myGLCD.printNumI(int(lightPWM[2].pwmValue*100/255) , 274,260,3);
   //int(calculatedPWM*100/255
   myGLCD.print("%", 320,260);
   myGLCD.printNumI(int(lightPWM[3].pwmValue*100/255) , 404,260,3);
   myGLCD.print("%", 450,260);
   
   myGLCD.setColor(255,255,255);
   myGLCD.print("Light Scene 3" , 15,320);  
   myGLCD.printNumI(lightPWM[4].Hour,255,320,2,48);
   myGLCD.print(":",290,320);
   myGLCD.printNumI(lightPWM[4].Minute,305,320,2,48);
   myGLCD.print(" - ",340,140);
   myGLCD.printNumI(lightPWM[5].Hour,385,320,2,48);
   myGLCD.print(":",420,320);
   myGLCD.printNumI(lightPWM[5].Minute,435,320,2,48);
   myGLCD.setColor(182,0,255);
   myGLCD.printNumI(int(lightPWM[4].pwmValue*100/255) , 274,350,3);
   myGLCD.print("%", 320,350);
   myGLCD.printNumI(int(lightPWM[5].pwmValue*100/255) , 404,350,3);
   myGLCD.print("%", 450,350);
   
   myGLCD.setColor(255,255,255);
   myGLCD.print("Light Scene 4" , 15,410);  
   myGLCD.printNumI(lightPWM[6].Hour,255,410,2,48);
   myGLCD.print(":",290,410);
   myGLCD.printNumI(lightPWM[6].Minute,305,410,2,48);
   myGLCD.print(" - ",340,410);
   myGLCD.printNumI(lightPWM[7].Hour,385,410,2,48);
   myGLCD.print(":",420,410);
   myGLCD.printNumI(lightPWM[7].Minute,435,410,2,48);
   myGLCD.setColor(182,0,255);
   myGLCD.printNumI(int(lightPWM[6].pwmValue*100/255) , 274,440,3);
   myGLCD.print("%", 320,440);
   myGLCD.printNumI(int(lightPWM[7].pwmValue*100/255) , 404,440,3);
   myGLCD.print("%", 450,440);
   
   myGLCD.setColor(255,255,255);
   myGLCD.print("Light Scene 5" , 15,500);  
   myGLCD.printNumI(lightPWM[8].Hour,255,500,2,48);
   myGLCD.print(":",290,500);
   myGLCD.printNumI(lightPWM[8].Minute,305,500,2,48);
   myGLCD.print(" - ",340,500);
   myGLCD.printNumI(lightPWM[9].Hour,385,500,2,48);
   myGLCD.print(":",420,500);
   myGLCD.printNumI(lightPWM[9].Minute,435,500,2,48);
   myGLCD.setColor(182,0,255);
   myGLCD.printNumI(int(lightPWM[8].pwmValue*100/255) , 274,530,3);
   myGLCD.print("%", 320,530);
   myGLCD.printNumI(int(lightPWM[9].pwmValue*100/255) , 404,530,3);
   myGLCD.print("%", 450,530);
   
      myGLCD.setColor(255,255,255);
   myGLCD.print("Light Scene 6" , 15,590);  
   myGLCD.printNumI(lightPWM[10].Hour,255,590,2,48);
   myGLCD.print(":",290,590);
   myGLCD.printNumI(lightPWM[10].Minute,305,590,2,48);
   myGLCD.print(" - ",340,590);
   myGLCD.printNumI(lightPWM[11].Hour,385,590,2,48);
   myGLCD.print(":",420,590);
   myGLCD.printNumI(lightPWM[11].Minute,435,590,2,48);
   myGLCD.setColor(182,0,255);
   myGLCD.printNumI(int(lightPWM[10].pwmValue*100/255) , 274,620,3);
   myGLCD.print("%", 320,620);
   myGLCD.printNumI(int(lightPWM[11].pwmValue*100/255) , 404,620,3);
   myGLCD.print("%", 450,620);
   
   
   
  /* 
   myGLCD.print("Light Scene 2  07:20 - 11:50" , 15,230);
   myGLCD.print(" 90%" , 400,260);
   myGLCD.print("Light Scene 3  12:00 - 15:50" , 15,320);
   myGLCD.print("  0%" , 400,350);
   myGLCD.print("Light Scene 4  12:00 - 15:50" , 15,410);
   myGLCD.print("100%" , 400,440);
   myGLCD.print("Light Scene 5  00:00 - 00:00" , 15,500);
   myGLCD.print("  0%" , 400,530);
  myGLCD.print("Light Scene 6  00:00 - 00:00" , 15,590);
   myGLCD.print("  0%" , 400,620);
   */
   /*
  myFiles.load(ClockCord[0], ClockCord[1],74,74, "74Cloc.raw"); 
  myFiles.load(LightsCord[0], LightsCord[1],74,74, "74Ligh.raw"); 
  myFiles.load(CleanCord[0], CleanCord[1],74,74, "74Clea.raw"); 
  myFiles.load(ScheCord[0], ScheCord[1],74,74, "74Sche.raw"); 
  
  myFiles.load(ScreenCord[0], ScreenCord[1],74,74, "74Remi.raw"); 
  myFiles.load(HeatCord[0], HeatCord[1],74,74, "74Heat.raw"); 
  myFiles.load(Co2SetCord[0], Co2SetCord[1],74,74, "74Co2_N.raw"); 
  myFiles.load(DoseCord[0], DoseCord[1],74,74, "74Dose.raw"); 
   
  */ 
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 41 . LightScene1
void LightScene()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(182,0,255);
   myGLCD.print("LIGHT SCENE", 77,40);
   myGLCD.printNumI(dispScreen-40, 360,40);
   myFiles.load(6, 24, 60,60, "60Ligh.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
   
    myGLCD.setColor(182,0,255);
  myGLCD.print("SCENE ON", 20,100);
  myGLCD.print("BRIGHTNESS", 20,230);
  myGLCD.print("SCENE OFF", 20,400);
  myGLCD.print("BRIGHTNESS", 20,530);
   
     myFiles.load(powLightOnHourUp[0], powLightOnHourUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnHourDown[0], powLightOnHourDown[1], 48,48, "48down.raw");
      myFiles.load(powLightOnMinuteUp[0], powLightOnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnMinuteDown[0], powLightOnMinuteDown[1], 48,48, "48down.raw");

      myFiles.load(powLightOffMinuteUp[0], powLightOffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffMinuteDown[0], powLightOffMinuteDown[1], 48,48, "48down.raw");
        myFiles.load(powCo2OnHourUp[0], powCo2OnHourUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnHourDown[0], powCo2OnHourDown[1], 48,48, "48down.raw");
      myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
 
      myFiles.load(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], 48,48, "48down.raw");
  
  
   myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
    myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
  UpdateLightScene();
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}


void UpdateLightScene()
{ myGLCD.setFont(UbuntuBold);
   

  myGLCD.setColor(255,255,255);
  myGLCD.printNumI(lightPWM[lightScreenSet].Hour, 220,145,2,48);
  myGLCD.printNumI(lightPWM[lightScreenSet].Minute, 360,145,2,48);
  //myGLCD.printNumI(int(100-(lightPWM[lightScreenSet].pwmValue*100)/255), 335,275,3);
  myGLCD.printNumI(int(lightPWM[lightScreenSet].pwmValue*100/255), 335,275,3);


  myGLCD.printNumI(lightPWM[lightScreenSet+1].Hour, 220,445,2,48);
  myGLCD.printNumI(lightPWM[lightScreenSet+1].Minute, 360,445,2,48);
  //myGLCD.printNumI(int(100-(lightPWM[lightScreenSet+1].pwmValue*100)/255), 335,575,3);
  myGLCD.printNumI(int(lightPWM[lightScreenSet+1].pwmValue*100/255), 335,575,3);

  
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 5. Settings
void ClockScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(0,109,255);
   myGLCD.print("CLOCK        ", 77,40);
   myFiles.load(6, 24, 60,60, "60Cloc.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   updateClockSettings();
   //end of >TOP
   //get the time
  
   

   myFiles.load(HourUp[0], HourUp[1], 48,48, "48up.raw");
   myFiles.load(HourDown[0], HourDown[1], 48,48, "48down.raw");
      myFiles.load(MinuteUp[0], MinuteUp[1], 48,48, "48up.raw");
   myFiles.load(MinuteDown[0], MinuteDown[1], 48,48, "48down.raw");
   
      myFiles.load(DayUp[0], DayUp[1], 48,48, "48up.raw");
   myFiles.load(DayDown[0], DayDown[1], 48,48, "48down.raw");
      myFiles.load(MonthUp[0], MonthUp[1], 48,48, "48up.raw");
   myFiles.load(MonthDown[0], MonthDown[1], 48,48, "48down.raw");
      myFiles.load(YearUp[0], YearUp[1], 48,48, "48up.raw");
   myFiles.load(YearDown[0], YearDown[1], 48,48, "48down.raw");
    myFiles.load(SetClockCord[0], SetClockCord[1], 168,52, "6set.raw");


   
   
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
   
  
}


void updateClockSettings() 
{  myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(255,255,255);
   //end of >TOP
   //get the time
   myGLCD.printNumI(adjustTimer.hour(),100,260,2,48);    
   myGLCD.printNumI(adjustTimer.minute(),270, 260,2,48);
   myGLCD.printNumI(adjustTimer.day(),50,424,2,48);    
   myGLCD.printNumI(adjustTimer.month(),170, 424,2,48);
   myGLCD.printNumI(adjustTimer.year(),280,424,2,48);      
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 6 . CleanSched
void CleanSchedScreen()
{  //Statusline TOP
   wdt_reset();
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(0,219,0);
   myGLCD.print("CLEANING   ", 77,40);
   myFiles.load(6, 24, 60,60, "60CleanN.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
 
   myGLCD.setFont(BigFont);

   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.print("Filter 1", 110, 180);
   myGLCD.print("Filter 2", 334, 180);
   myGLCD.print("Light 1", 110, 264);
   myGLCD.print("Light 2", 334, 264);
   myGLCD.print("CO2", 110, 348);
   myGLCD.print("Heater", 334, 348);
   myGLCD.print("Cooling", 110, 432);
   
        myFiles.load(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], 48,48, "48down.raw");
   myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
   myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
  myGLCD.setFont(UbuntuBold);
  myGLCD.print("Minutes", 70,575);  
   /*
  */ 
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
   
}

void updateCleanSchedScreen()
{  wdt_reset();
   if(!pump1Clean) { myFiles.load(20, 150,74,74, "74Filt_N.raw");}
   else { myFiles.load(20, 150,74,74, "74Filt_F.raw");}
   if(!pump2Clean)  {myFiles.load(250, 150,74,74, "74Filt_N.raw");}
   else   { myFiles.load(250, 150,74,74, "74Filt_F.raw");}
   if(!light230Clean) {myFiles.load(20, 234,74,74, "74LED_N.raw"); }
   else {myFiles.load(20, 234,74,74, "74LED_F.raw"); }
   wdt_reset();
   if(!light2Clean) { myFiles.load(250, 234,74,74, "74LED_N.raw");}
   else { myFiles.load(250, 234,74,74, "74LED_F.raw");}
   if(!co2Clean) {myFiles.load(20, 318,74,74, "74CO2_N.raw"); }
   else  {myFiles.load(20, 318,74,74, "74CO2_F.raw"); }
   if(!heaterClean ) { myFiles.load(250, 318,74,74, "74heat_N.raw");}
   else { myFiles.load(250, 318,74,74, "74heat_F.raw");}
   if(!coolClean)  { myFiles.load(20, 402,74,74, "74Fan_N.raw"); }
   else { myFiles.load(20, 402,74,74, "74Fan_F.raw"); }

}

void quickUpdateCleanSchedScreen()
{myGLCD.setFont(UbuntuBold);
 myGLCD.setColor(0,219,0);
 myGLCD.printNumI(cleanMinutes, 330,575,3);  
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 7. Schedule
void RemindScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(219,0,170);
   myGLCD.print("REMINDER            ", 77,40);
   myFiles.load(6, 24, 60,60, "60Sche.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
 
  myGLCD.setFont(OCR_A_Extended_M);
  myGLCD.print("Cleaned Tank", 120,155);
  myGLCD.print("days ago", 265,190);
  myGLCD.print("(", 190,190);
  myGLCD.print(")", 248,190);
 
  myGLCD.print("new CO2 bottle", 120,239);
  myGLCD.print("days ago", 265,274);
    myGLCD.print("(", 190,274);
  myGLCD.print(")", 248,274);
 
  
  myGLCD.print("Cleaned Filter 1", 120,323);
  myGLCD.print("days ago", 265,358);
    myGLCD.print("(", 190,358);
  myGLCD.print(")", 248,358);
 
 
  myGLCD.print("Cleaned Filter 2", 120,407);
  myGLCD.print("days ago", 265,442);
    myGLCD.print("(", 190,442);
  myGLCD.print(")", 248,442);
 
 
  myFiles.load(20, 150,74,74, "74Clea.raw");
  myFiles.load(20, 234,74,74, "74CO2_N.raw");
  myFiles.load(20, 318,74,74, "74Filt_N.raw");
  myFiles.load(20, 402,74,74, "74Filt_N.raw");
  myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
  myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
  
  
   updateRemindScreen();
   
   

   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}


/*

   myGLCD.printNumI(printnow.hour(), 140,190,3,48);
   
   myGLCD.print(":", 430,ty);
   myGLCD.printNumI(printnow.minute(), 445,ty,2,48);
   
   */
   
   
void updateRemindScreen()
{   myGLCD.setFont(OCR_A_Extended_M);
   myGLCD.setColor(219,0,170);
// myGLCD.printNumI(((now.unixtime()-tankClean.unixtime())/86400), 110,190,3);
// myGLCD.printNumI(tankCleandDays, 125,190,3);
//myGLCD.printNumI(((now.unixtime()-tankClean.unixtime())/86400), 105,190,3);
myGLCD.printNumI(((now.unixtime()-tankClean.unixtime())/86400), 135,190,3);
   myGLCD.setColor(255,255,255);
myGLCD.printNumI(tankCleandDays, 200,190,3);

  //myGLCD.print("66", 140,274);
//  myGLCD.printNumI(((now.unixtime()-co2Bottle.unixtime())/1), 130,274,3);
myGLCD.setColor(219,0,170);
myGLCD.printNumI(((now.unixtime()-co2Bottle.unixtime())/86400), 130,274,3);
   myGLCD.setColor(255,255,255);
myGLCD.printNumI(co2BottleDays, 200,274,3);

myGLCD.setColor(219,0,170);
  myGLCD.printNumI(((now.unixtime()-cleanFilter1.unixtime())/86400), 130,358,3);
   myGLCD.setColor(255,255,255);
myGLCD.printNumI(cleanFilter1Days, 200,358,3);

myGLCD.setColor(219,0,170);
  myGLCD.printNumI(((now.unixtime()-cleanFilter2.unixtime())/86400), 130,442,3);
     myGLCD.setColor(255,255,255);
     myGLCD.printNumI(cleanFilter2Days, 200,442,3);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 8. Heater
void HeaterScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(219,0,85);
   myGLCD.print("HEATER              ", 77,40);
   myFiles.load(6, 24, 60,60, "60Heat.raw");

   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
   myGLCD.print("UPPER LIMIT",  20,215);
  myGLCD.print("LOWER LIMIT", 20,385);
   myFiles.load(powLightOffMinuteUp[0], powLightOffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffMinuteDown[0], powLightOffMinuteDown[1], 48,48, "48down.raw");
   myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
   myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
   myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}

void updateHeaterScreen()
{myGLCD.setFont(UbuntuBold);
 myGLCD.setColor(col_white.r, col_white.g, col_white.b);
 myGLCD.printNumF(TempUpperLimit,2, 280,275);
 myGLCD.printNumF(TempLowerLimit,2, 280,445);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 9. Dose
void DoseScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(73,36,0);
   myGLCD.print("DOSING             ", 77,40);
   myFiles.load(6, 24, 60,60, "60Dose.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP

   myGLCD.setFont(OCR_A_Extended_M);
   
   myGLCD.setColor(255,255,255);
   myGLCD.print("Macro " , 15,140);  
   
   myGLCD.setColor(col_FertiN.r, col_FertiN.g, col_FertiN.b);
   myGLCD.print("N" , 100,140); 
   myGLCD.printNumI(FDose[0],335,140);
   myGLCD.setColor(255,255,255);
   myGLCD.print("ml/day",370,140);


  myGLCD.setColor(col_FertiN.r, col_FertiN.g, col_FertiN.b);
   myGLCD.printNumI(int(FLeft[0]/FDose[0]), 244,170,3);
   myGLCD.setColor(255,255,255);
   myGLCD.print("doses left", 305,170);
  
  
  
  
     
   myGLCD.print("Macro" , 15,230);  
   
 myGLCD.setColor(col_FertiNPK.r, col_FertiNPK.g, col_FertiNPK.b);
  myGLCD.print("NPK" , 100,230); 
   myGLCD.printNumI(FDose[1],335,230);
   myGLCD.setColor(255,255,255);
   myGLCD.print("ml/day",370,230);


   myGLCD.setColor(col_FertiNPK.r, col_FertiNPK.g, col_FertiNPK.b);
   myGLCD.printNumI(int(FLeft[1]/FDose[1]), 244,260,3);
   myGLCD.setColor(255,255,255);
   myGLCD.print("doses left", 305,260);


   myGLCD.print("Micro" , 15,320);  
   myGLCD.setColor(col_FertiFE.r, col_FertiFE.g, col_FertiFE.b);
   myGLCD.print("FE" , 100,320);  
   myGLCD.printNumI(FDose[2],335,320);
   myGLCD.setColor(255,255,255);
   myGLCD.print("ml/day",370,320);


  myGLCD.setColor(col_FertiFE.r, col_FertiFE.g, col_FertiFE.b);
   myGLCD.printNumI(int(FLeft[2]/FDose[2]), 244,350,3);
   myGLCD.setColor(255,255,255);
   myGLCD.print("doses left", 305,350);
 
 myGLCD.setFont(UbuntuBold);  
 myGLCD.print("TIME", 10,445);
 myFiles.load(powCo2OnHourUp[0], powCo2OnHourUp[1], 48,48, "48up.raw");
 myFiles.load(powCo2OnHourDown[0], powCo2OnHourDown[1], 48,48, "48down.raw");
 myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
 myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
 myFiles.load(refillAllCord[0],refillAllCord[1],260,52, "260Ref.raw"); 
 

   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}

void updateDoseScreen()
{ myGLCD.setFont(UbuntuBold);  
 myGLCD.setColor(73,36,0);
 myGLCD.printNumI(doseHour, 220,445,2,48);
 myGLCD.printNumI(doseMinute, 360,445,2,48);
}

// 91. DoseScreeen for N
void DoseScreenN()
{   myGLCD.setFont(UbuntuBold);

   myFiles.load(6, 24, 60,60, "60Dose.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
 myGLCD.setFont(OCR_A_Extended_M);
 
 myGLCD.print("MO  TU  WE", 25, 550);
 myGLCD.print("TH  FR  SA  SO", 225, 550);
 myGLCD.setFont(UbuntuBold);
 myGLCD.print("dose/day", 20,145);
  myGLCD.print("ml", 345,145);
  
  myGLCD.print("volume", 20,275);
  myGLCD.print("ml", 345,275);
  
  myGLCD.print("ml/min", 260,445);
 //445
   myFiles.load(calibrateCord[0], calibrateCord[1], 260,52, "260Cal.raw");
   myFiles.load(powLightOnMinuteUp[0], powLightOnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnMinuteDown[0], powLightOnMinuteDown[1], 48,48, "48down.raw");
   myFiles.load(powLightOffMinuteUp[0], powLightOffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffMinuteDown[0], powLightOffMinuteDown[1], 48,48, "48down.raw");
   
      myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
   
   
      myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
   myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");

   //footer starts here  
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
}

void UpdateDoseScreenN(byte FS)
{if(MoF[FS]){ myFiles.load(MoCord[0], MoCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(MoCord[0], MoCord[1], 48,48, "48unch.raw");}
  if(TuF[FS]){ myFiles.load(TuCord[0], TuCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(TuCord[0], TuCord[1], 48,48, "48unch.raw");}
  if(WeF[FS]){ myFiles.load(WeCord[0], WeCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(WeCord[0], WeCord[1], 48,48, "48unch.raw");}
  if(ThF[FS]){ myFiles.load(ThCord[0], ThCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(ThCord[0], ThCord[1], 48,48, "48unch.raw");}
   if(FrF[FS]){ myFiles.load(FrCord[0], FrCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(FrCord[0], FrCord[1], 48,48, "48unch.raw");}
    if(SaF[FS]){ myFiles.load(SaCord[0], SaCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(SaCord[0], SaCord[1], 48,48, "48unch.raw");}
    if(SuF[FS]){ myFiles.load(SoCord[0], SoCord[1], 48,48, "48chec.raw");}
 else {myFiles.load(SoCord[0], SoCord[1], 48,48, "48unch.raw");}
 }
 
 void quickUpdateDoseScreenN(byte FS)
 {myGLCD.setFont(UbuntuBold);
 switch (FS)
 {case 0:
   myGLCD.setColor(73,36,0);
   myGLCD.print("MACRO", 77,40);
  myGLCD.setColor(col_FertiN.r, col_FertiN.g, col_FertiN.b);
  myGLCD.print("N", 215,40);

  break;
  case 1:
    myGLCD.setColor(73,36,0);
   myGLCD.print("MACRO", 77,40);
  myGLCD.setColor(col_FertiNPK.r, col_FertiNPK.g, col_FertiNPK.b);
    myGLCD.print("NPK", 215,40);

  break;
  case 2:
    myGLCD.setColor(73,36,0);
   myGLCD.print("MICRO", 77,40);
  myGLCD.setColor(col_FertiFE.r, col_FertiFE.g, col_FertiFE.b);
    myGLCD.print("FE", 215,40);

  break;
 }
 myGLCD.printNumI(FDose[FS], 270,145,3);
 myGLCD.printNumI(FMax[FS], 270,275,3);
 myGLCD.printNumI(FRate[FS], 185,445,3);
 }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 10. Powerschedscreen
void PowerSchedScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(219,0,0);
   myGLCD.print("POWER           ", 77,40);
   myFiles.load(6, 24, 60,60, "60Powe.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   UpdatePowerSchedScreen();
   //end of >TOP
  
      myGLCD.setColor(219,0,0);
  myGLCD.print("LIGHTS ON", 20,100);
  myGLCD.print("LIGHTS OFF", 20,230);
  myGLCD.print("CO2 ON", 20,400);
  myGLCD.print("CO2 OFF", 20,530);
     myFiles.load(powLightOnHourUp[0], powLightOnHourUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnHourDown[0], powLightOnHourDown[1], 48,48, "48down.raw");
      myFiles.load(powLightOnMinuteUp[0], powLightOnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnMinuteDown[0], powLightOnMinuteDown[1], 48,48, "48down.raw");
        myFiles.load(powLightOffHourUp[0], powLightOffHourUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffHourDown[0], powLightOffHourDown[1], 48,48, "48down.raw");
      myFiles.load(powLightOffMinuteUp[0], powLightOffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffMinuteDown[0], powLightOffMinuteDown[1], 48,48, "48down.raw");
        myFiles.load(powCo2OnHourUp[0], powCo2OnHourUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnHourDown[0], powCo2OnHourDown[1], 48,48, "48down.raw");
      myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
        myFiles.load(powCo2OffHourUp[0], powCo2OffHourUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OffHourDown[0], powCo2OffHourDown[1], 48,48, "48down.raw");
      myFiles.load(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], 48,48, "48down.raw");

  
  
  
  
   myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
    myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
    
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}


void UpdatePowerSchedScreen()
{ myGLCD.setFont(UbuntuBold);
   
  myGLCD.setColor(255,255,255);
  myGLCD.printNumI(powLightOnHour, 220,145,2,48);
  myGLCD.printNumI(powLightOnMinute, 360,145,2,48);
  myGLCD.printNumI(powLightOffHour, 220,275,2,48);
  myGLCD.printNumI(powLightOffMinute, 360,275,2,48);

  myGLCD.printNumI(powCo2OnHour, 220,445,2,48);
  myGLCD.printNumI(powCo2OnMinute, 360,445,2,48);
  myGLCD.printNumI(powCo2OffHour, 220,575,2,48);
  myGLCD.printNumI(powCo2OffMinute, 360,575,2,48);  
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 13. Dose
void ScreenScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,75,450,75);
   myGLCD.print("SCREEN         ", 77,40);
   myFiles.load(6, 24, 60,60, "60Scree.raw");
   myGLCD.setColor(255,0,0);

   //end of >TOP


  myGLCD.print("SCREEN ON", 20,100);
  myGLCD.print("SCREEN OFF", 20,230);
  myGLCD.print("MINUTES", 20,390);
  myGLCD.print("BRIGHTNESS", 20,510);
  myGLCD.setFont(BigFont);
  myGLCD.print("to Standby", 200,400);
  myGLCD.print("in Standby", 265,520);
     myFiles.load(powLightOnHourUp[0], powLightOnHourUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnHourDown[0], powLightOnHourDown[1], 48,48, "48down.raw");
      myFiles.load(powLightOnMinuteUp[0], powLightOnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOnMinuteDown[0], powLightOnMinuteDown[1], 48,48, "48down.raw");
        myFiles.load(powLightOffHourUp[0], powLightOffHourUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffHourDown[0], powLightOffHourDown[1], 48,48, "48down.raw");
      myFiles.load(powLightOffMinuteUp[0], powLightOffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffMinuteDown[0], powLightOffMinuteDown[1], 48,48, "48down.raw");
      myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
      myFiles.load(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], 48,48, "48down.raw");

  
  
  
  
   myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
    myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
   
   
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   UpdateScreenScreen();
   
}

void UpdateScreenScreen()

{ myGLCD.setFont(UbuntuBold);
  myGLCD.setColor(255,255,255);


  
  
    myGLCD.printNumI(screenOnHour, 220,145,2,48);
  myGLCD.printNumI(screenOnMinute, 360,145,2,48);
  myGLCD.printNumI(screenOffHour, 220,275,2,48);
  myGLCD.printNumI(screenOffMinute, 360,275,2,48);

  myGLCD.printNumI(standByMinutes, 360,445,2,48);
  myGLCD.printNumI(backlightPWM, 335,575,3);  
  
  
  
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 12. Co2 Settings
void Co2SetScreen()
{  //Statusline TOP
 
   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(0,182,16);
   myGLCD.print("CO2 SETTINGS     ", 77,40);
   myFiles.load(6, 24, 60,60, "60Co2.raw");

   myGLCD.drawLine(66,75,450,75);
   //end of >TOP
   /*
  myFiles.load(ClockCord[0], ClockCord[1],74,74, "74Cloc.raw"); 
  myFiles.load(LightsCord[0], LightsCord[1],74,74, "74Ligh.raw"); 
  myFiles.load(CleanCord[0], CleanCord[1],74,74, "74Clea.raw"); 
  myFiles.load(ScheCord[0], ScheCord[1],74,74, "74Sche.raw"); 
  
  myFiles.load(ScreenCord[0], ScreenCord[1],74,74, "74Remi.raw"); 
  myFiles.load(HeatCord[0], HeatCord[1],74,74, "74Heat.raw"); 
  myFiles.load(Co2SetCord[0], Co2SetCord[1],74,74, "74Co2_N.raw"); 
  myFiles.load(DoseCord[0], DoseCord[1],74,74, "74Dose.raw"); 
   
  */ 
  
  myGLCD.print("UPPER LIMIT",  20,215);
  myGLCD.print("LOWER LIMIT", 20,385);
   myFiles.load(powLightOffMinuteUp[0], powLightOffMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powLightOffMinuteDown[0], powLightOffMinuteDown[1], 48,48, "48down.raw");
   myFiles.load(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], 48,48, "48up.raw");
   myFiles.load(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], 48,48, "48down.raw");
   myFiles.load(CancelPowerSchedCord[0], CancelPowerSchedCord[1], 168,52, "6cancel.raw");
   myFiles.load(SetPowerSchedCord[0], SetPowerSchedCord[1], 168,52, "6set.raw");
 
   //footer starts here
   myGLCD.setColor(col_white.r, col_white.g, col_white.b);
   myGLCD.drawLine(30,770,196,770); 
   myGLCD.drawLine(284,770,450,770); 
   myFiles.load(216, 746,48,48, "HomeBot.raw"); 
   
}


void updateCO2SetScreen()
{myGLCD.setFont(UbuntuBold);
 myGLCD.setColor(col_white.r, col_white.g, col_white.b);
 myGLCD.printNumF(PHUpperLimit,2, 280,275);
 myGLCD.printNumF(PHLowerLimit,2, 280,445);
}




// 13. draw TestScreen
 void TestScreen()
{ //Statusline TOP
   myGLCD.setColor(255, 255, 255);

   myGLCD.setFont(UbuntuBold);
   myGLCD.setColor(0,0,255);
   myGLCD.print("HOME       ", 77,40);
   myFiles.load(6, 24, 60,60, "1home.raw");
   myGLCD.setColor(255,255,255);
   myGLCD.drawLine(66,81,450,81);
   //end of >TOP
 myFiles.load(0, 0, 480,800, "256Col.raw");
/*draw Button 1
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (Button1Cord[0], Button1Cord[1], Button1Cord[2], Button1Cord[3]);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (Button1Cord[0], Button1Cord[1], Button1Cord[2], Button1Cord[3]);
  myGLCD.setBackColor (0, 0, 255);
  myGLCD.print("Button 1", CENTER, 740);
  myGLCD.setBackColor (0, 0, 0);
*/ //Ende Button 1
  
 /*temporär Werte anzeige
  myGLCD.setColor(0, 0, 0); //to delete the pwm field every update
  myGLCD.fillRect(330,370,375,390); //to delete the pwm field every Update
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Temperatur:", 10, 70);
  myGLCD.printNumF(Temp,2, 300, 70);
  myGLCD.print("PH:", 10, 90);
  myGLCD.printNumF(PhWert,2, 315, 90);
  myGLCD.print("Relais", 10, 130);
  myGLCD.print("pump1Pin",  10, 150);
  myGLCD.print(String(pump1Value),  365, 150);
  myGLCD.print("pump2Pin",  10, 170);
  myGLCD.print(String(pump2Value),  365, 170);
  myGLCD.print("light230Pin",  10, 190);
  myGLCD.print(String(light230Value),  365, 190);
  myGLCD.print("light2Pin",  10, 210);
  myGLCD.print(String(light2Value),  365, 210);
  myGLCD.print("co2Pin",  10, 230);
  myGLCD.print(String(co2Value),  365, 230);
  myGLCD.print("heaterPin",  10, 250);
  myGLCD.print(String(heaterValue),  365, 250);
  myGLCD.print("dPump1Pin",  10, 270);
  myGLCD.print(String(dPump1Value),  365, 270);
  myGLCD.print("dPump2Pin",  10, 290);
  myGLCD.print(String(dPump2Value),  365, 290);
  myGLCD.print("dPump3Pin",  10, 310);
  myGLCD.print(String(dPump3Value),  365, 310);  
  myGLCD.print("coolPin",  10, 330);
  myGLCD.print(String(coolValue),  365, 330); 
  myGLCD.print("lightPwmValue",  10, 370);
  myGLCD.print(String(lightPwmValue*100/255),  330, 370);
  myGLCD.print("%",  375, 370);
  */
}


void waitForIt(int x1, int y1, int x2, int y2)    // Draw a red frame while a button is touched
{/*
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
  myTouch.read();
//  x=myTouch.getX();
 //  y=myTouch.getY();
 
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  //drawScreen();
  */
}

void actOnRealease(int x1, int y1, int x2, int y2)    // Draw a red frame while a button is touched
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
  myTouch.read();
  x=myTouch.getX();
  y=myTouch.getY();
 
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  drawScreen();
  
}

