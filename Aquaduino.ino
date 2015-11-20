//Aquaduino
// v.1.1  -  21.08.15
// v.1.2  -  05.09.15
// v.1.3  -  12.09.15 - PWM Dimmung angepasst an MOSFET Steuerung hinter dem Netzteil
// v.1.3.1 - 25.09.15 - fixed a Bug in LightCalculation (false values when now.minute == nextlight.minute)
// v.1.3.2 - 04.10.2015 - feed Mode - ScreenUpdate AFTER switching Lights
// v.1.4  -  04.10.2015 - initial TVMode implemented
// v.1.4.1 - 13.10.2015 - started to implement LightModes for RGB
// v.1.4.2 - 17.10.2015 - RGB visualisation ready
// v.1.4.3 - 18.10.2015 - RGB finalized
// v.1.5   - 04.11.2015 - added PH Curve
// v.1.5.1 - 05.11.2015 - bugfixing
// v.1.5.2 - 07.11.2015 - bugfixing | implementing TempCurve
// v.1.5.3 - 21.11.2015 - implement MoonMode



//needed Libraries
#include <SdFat.h>
#include <UTFT.h>    //TFT
#include <RCSwitch.h>
#include <UTouch.h> //Touch
#include <OneWire.h>  //needed for Temp
#include <DallasTemperature.h> //needed for Temp
#include <Wire.h>  // Wire.h library is required to use SX1509 lib
#include <sx1509_library.h>  // Include the SX1509 library
#include "RTClib.h"
#define ONE_WIRE_BUS 10 //needed for Temperature
#include <EEPROM.h>  // used to store and retrieve settings from memory
#include <tinyFAT.h> // used to acess the SD card
#include <UTFT_SdRaw.h>  // used to read .raw images from the SD card
#include <avr/wdt.h>



//bring it up - initialise
OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.
UTFT myGLCD(ITDB50, 38, 39, 40, 41); //initialize TFT
UTouch  myTouch( 6, 5, 4, 3, 2); //initialize TFT
UTFT_SdRaw myFiles(&myGLCD);
RCSwitch mySwitch = RCSwitch();


// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t UbuntuBold[];
extern uint8_t SevenSegmentFull[];
extern uint8_t OCR_A_Extended_M[];

/*  VARIABLEN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    alle globale Variablen - teilweise mit Startweten definiert
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


unsigned long currentMillis; // get current millis
unsigned long prevMillisTouch = 0;
unsigned long prevMillis5sec = 0; // track 5 seconds for refreshing clock and temp
unsigned long prevMillis1min = 0; // track 60 seconds for refreshing
unsigned long prevMillis15min = 0; //track 15 minutes
//unsigned long standbyMillis=0;

RTC_DS1307 rtc;
const byte SX1509_ADDRESS = 0x3E;
sx1509Class sx1509(SX1509_ADDRESS);
const byte interruptPin = 2;
const byte resetPin = 8;
DateTime now;
DateTime cleanEnd;
DateTime adjustTimer;
DateTime lastFert;

DateTime tankClean;
byte tankCleandDays;
DateTime co2Bottle;
byte co2BottleDays;
DateTime cleanFilter1;
byte cleanFilter1Days;
DateTime cleanFilter2;
byte cleanFilter2Days;

DateTime TVModeStart;
boolean TVModeState = false;
byte TVModeBrightness = 51; // (20%)

DateTime MoonModeStart;
boolean MoonModeState = false;
byte MoonRed;
byte MoonGreen;
byte MoonBlue;
byte MoonMinutes;





int x, y; //touched coordinates
int x2, y2;
String inputstring = "";                                                       //a string to hold incoming data from the PC
String PhWertString = "";  //a string to hold the data from the Atlas Scientific product

float PhWert = 7.01; //sting to float to calculate with it
float PHUpperLimit = 10;
float PHLowerLimit = 5;
float PHValues[96];
float TempValues[62];

boolean Co2Values[96];





byte  put_PHindex = 0;
byte  put_TempIndex = 0;
byte get_index = 1;
/**<datatype> array [DIM_0_SIZE] [DIM_1_SIZE] = {
  //as many vals as dim1
 {val,val,val},
 {val,val,val}//as many rows as dim0
};
**/


boolean input_stringcomplete = false;                                          //have we received all the data from the PC
boolean sensor_stringcomplete = false;                                         //have we received all the data from the Atlas Scientific product
float Temp = 11.11;        //sting to float to calculate with it
float TempUpperLimit = 30;
float TempLowerLimit = 25;

//Fertilizer Stuff
float FDose[] = {0, 0 , 0};
float FMax[] = {0, 0, 0};
float FLeft[] = {0, 0, 0};
float FRate[] = {0, 0, 0};
byte FSelect = 5;
unsigned long fertmillis = 0;

boolean MoF[] = {false, false, false};
boolean TuF[] = {false, false, false};
boolean WeF[] = {false, false, false};
boolean ThF[] = {false, false, false};
boolean FrF[] = {false, false, false};
boolean SaF[] = {false, false, false};
boolean SuF[] = {false, false, false};
boolean dayN[7] = {false, false, false, false, false, false, false};
boolean dayNPK[7] = {false, false, false, false, false, false, false};
boolean dayFE[7] = {false, false, false, false, false, false, false};
byte doseHour;
byte doseMinute;

//Powerschedule

byte powLightOnHour;
byte powLightOnMinute;
byte powLightOffHour;
byte powLightOffMinute;
byte powCo2OnHour;
byte powCo2OnMinute;
byte powCo2OffHour;
byte powCo2OffMinute;

byte screenOnHour;
byte screenOnMinute;
byte screenOffHour;
byte screenOffMinute;
byte standByMinutes = 10;
byte backlightPWM = 255;         // value for backlight  - 255 is full


/*
byte tankCleanMonth;
byte tankCleanDay;
byte co2BottleMonth;
byte co2BottleDay;
byte cleanFilter1Month;
byte cleanFilter1Day;
byte cleanFilter2Month;
byte cleanFilter2Day;
*/



//days and month char for displaing at the top of screen
char *dayName[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
char *monthName[] = {
  "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


byte light2Pin = 8;
byte light1Pin = 9;
byte dPump2Pin = 6;
byte dPump1Pin = 7;
byte dPump3Pin = 5;
byte redPin = 11;
byte greenPin = 12;
byte bluePin = 13;

byte heaterPin = 12; //will be RF
byte coolPin = 11; //will be RF
byte co2Pin = 10; //will be RF
byte pump1Pin = 13; //not used anymore RF now
byte pump2Pin = 14; //not used anymore RF now
byte light230Pin = 15; //not used anymore RF now
boolean pump1Value = false;
boolean pump2Value = false;
boolean light230Value = true;
boolean light1Value = true;
boolean light2Value = true;
boolean co2Value = true;
boolean heaterValue = false;
boolean dPump1Value = true;
boolean dPump2Value = true;
boolean dPump3Value = true;
boolean coolValue = true;


boolean pump1Clean = true;
boolean pump2Clean = true;
boolean light230Clean = true;
boolean light2Clean = true;
boolean co2Clean = true;
boolean heaterClean = true;
boolean coolClean = true;
byte cleanMinutes = 120;





// Used for PWM and lightstuff

TimeSpan timeSinceLastLight;
TimeSpan timeToNextLight;
TimeSpan timeSinceLastLightRGB;
TimeSpan timeToNextLightRGB;
byte currentPWM = 0; //0 - which is NO light
byte nextPWM = 0;


byte lightPwmPin = 9;            //the pin used for pwm
float calculatedPWM = 0;        //the value
float calculatedRed = 0;
float calculatedGreen = 0;
float calculatedBlue = 0;
const byte backlightPIN = 44;    // Pin 44 used for backlight





byte dispScreen = 0;       //current screen
byte lastScreen = 255;        //last screen



// screens are listed below
// 0-home, 1-cleaning, , 2-power, 3-extras, 4-lights,
// 5-clock, 6-feeding sched, 7-schedule, 8-heater
// 9-dosing, 10-pwer schedule, 11-schedule item, 13=ScreenScreen
//

//funkzeug
const unsigned long f11on = 1381717;  //Pump1 ON
const unsigned long f11off = 1381716;  //Pump1 OFF
const unsigned long f12on = 1394005;
const unsigned long f12off = 1394004;
const unsigned long f13on = 1397077; //MainLight ON
const unsigned long f13off = 1397076; //MainLight OFF
const unsigned long f14on = 1397845;
const unsigned long f14off = 1397844;
const unsigned long f21on = 4527445;   //Heater ON
const unsigned long f21off = 4527444;  //Heater FOFF
const unsigned long f22on = 4539733;   //CO2 ON
const unsigned long f22off = 4539732;  //CO2 OFF
const unsigned long f23on = 4542805;   //Coolpump ON
const unsigned long f23off = 4542804; //Coolpump OFF
const unsigned long f24on = 4543573; //Pump2 ON
const unsigned long f24off = 4543572;
const unsigned long f31on = 5313877;
const unsigned long f31off = 5313876;
const unsigned long f32on = 5326165;
const unsigned long f32off = 5326164;
const unsigned long f33on = 5329237;
const unsigned long f33off = 5329236;
const unsigned long f34on = 5330005;
const unsigned long f34off = 5330004;
const unsigned long f41on = 5510485;
const unsigned long f41off = 5510484;
const unsigned long f42on = 5522773;  //Pump2 ON
const unsigned long f42off = 5522772; //Pump2 Off
const unsigned long f43on = 5525845;
const unsigned long f43off = 5525844;
const unsigned long f44on = 5526613;
const unsigned long f44off = 5526612;


boolean cleaningInProcess = false;
boolean manualOverride = false;



//byte pwmDateAndValue[]={21,0,0,7,0,0,7,20,100,11,50,100,12,00,0,15,50,0,16,00,100,20,40,100};

typedef struct
{
  byte Hour;
  byte Minute;
  float pwmValue;
}  record_type;

record_type lightPWM[12];
byte lightScreenSet = 99;
byte RGBScreenSet = 99;


typedef struct
{
  byte Hour;
  byte Minute;
  byte red;
  byte green;
  byte blue;
}  recordRGB_type;

recordRGB_type lightRGB[12];






struct RGB {
  byte r;
  byte g;
  byte b;
};
typedef struct RGB Color;

//Farben
const Color col_white = {255, 255, 255};
const Color col_black = {255, 255, 255};
const Color col_blue = {0, 0, 255};
const Color col_red = {255, 0, 0};
const Color col_FertiN =  {45, 90, 255};
const Color col_FertiNPK =  {25, 191, 13};
const Color col_FertiFE =  {204, 17, 17};





//Buttonkoordinaten Home
const short Button1Cord[] = {0, 0 , 0, 0};
const short HomeButtonCoord[] = {21, 613 , 117, 710};
const short FeedButtonCoord[] = {133, 613 , 230, 710};
const short PowerButtonCoord[] = {249, 613 , 346, 710};
const short SettingsButtonCoord[] = {362, 613 , 458, 710};
const short LightUp[] = {60, 320, 108, 368};
const short LightDown[] = {60, 450, 108, 498};
const short BottomButtonCoord[] = {215, 746 , 263, 795}; //used @ all screens

//Powerbuttoncords
const short Filter1Cord[] = {20, 150 , 94, 224};
const short Filter2Cord[] = {250, 150 , 324, 224};
const short Ligth1Cord[] = {20, 234 , 94, 308};
const short Light2Cord[] = {250, 234 , 324, 308};
const short Co2Cord[] = {20, 318 , 94, 392};
const short HeaterCord[] = {250, 318 , 324, 392};
const short CoolingCord[] = {20, 402 , 94, 476};
const short AllOFFCord[] = {20, 524 , 94, 598};
const short ResetCord[] = {20, 608 , 94, 682};
const short CleanModeCord[] = {250, 524 , 324, 598};

//Settingbuttoncords
const short PowerSchedCord[] = {68, 150 , 142, 224};
const short LightsCord[] = {158, 150 , 232, 224};
const short CleanCord[] = {248, 150 , 322, 224};
const short ScheCord[] = {338, 150 , 412, 224};
const short ClockCord[] = {68, 240 , 142, 314};
const short Co2SetCord[] = {158, 240 , 232, 314};
const short HeatCord[] = {248, 240 , 322, 314};
const short DoseCord[] = {338, 240 , 412, 314};
const short ScreenCord[] = {68, 330 , 142, 404};
const short RGBCord[] = {158, 330 , 232, 404};


/** more buttons if needed
const short Co2Cord[] = {248, 330 ,322, 404};
const short ScreenCord[] = {248, 330 ,322, 404};
*/


//Clockbuttoncords
const short HourUp[] = {175, 220 , 223, 268};
const short HourDown[] = {175, 273 , 223, 321};
const short MinuteUp[] = {347, 220 , 395, 268};
const short MinuteDown[] = {347, 273 , 395, 321};
const short DayUp[] = {110, 391 , 158, 439};
const short DayDown[] = {110, 444 , 158, 492};
const short MonthUp[] = {220, 391 , 268, 439};
const short MonthDown[] = {220, 444 , 268, 492};
const short YearUp[] = {380, 391 , 428, 439};
const short YearDown[] = {380, 444 , 428, 492};
const short SetClockCord[] = {156, 600 , 324, 652};
//const short ResetCord[] = {20, 608 ,94, 682};

//POWERSCHEDULE BUTTONS
const short SetPowerSchedCord[] = {300, 680 , 465, 732};             //also used for Light scenes
const short CancelPowerSchedCord[] = {22, 680 , 190, 732};         //also used for light scenes
const short powLightOnHourUp[] = {275, 110 , 323  , 158};
const short powLightOnHourDown[] = {275, 163 , 323, 211};
const short powLightOnMinuteUp[] = {412, 110 , 460, 158};
const short powLightOnMinuteDown[] = {412, 163 , 460, 211};

const short powLightOffHourUp[] = {275, 240 , 323  , 288};
const short powLightOffHourDown[] = {275, 293 , 323, 341};
const short powLightOffMinuteUp[] = {412, 240 , 460, 288};
const short powLightOffMinuteDown[] = {412, 293 , 460, 341};

const short powCo2OnHourUp[] = {275, 410 , 323  , 458};
const short powCo2OnHourDown[] = {275, 463 , 323, 511};
const short powCo2OnMinuteUp[] = {412, 410 , 460, 458};
const short powCo2OnMinuteDown[] = {412, 463 , 460, 511};
const short powCo2OffHourUp[] = {275, 540 , 323  , 588};
const short powCo2OffHourDown[] = {275, 593 , 323, 641};
const short powCo2OffMinuteUp[] = {412, 540 , 460, 588};
const short powCo2OffMinuteDown[] = {412, 593 , 460, 641};


//Light Buttons (Disp 4)
const short LightMode1Cord[] = {15, 135 , 465, 195}; //also used for dosing
const short LightMode2Cord[] = {15, 225 , 465, 285}; //also used for dosing
const short LightMode3Cord[] = {15, 315 , 465, 375}; //also used for dosing
const short LightMode4Cord[] = {15, 405 , 465, 465};
const short LightMode5Cord[] = {15, 495 , 465, 555};
const short LightMode6Cord[] = {15, 585 , 465, 645};

//Dosing Button (Disp 9)
const short refillAllCord[] = {110, 580 , 370, 632};
const short calibrateCord[] = {22 , 370 , 282, 422};

//DaySched for Dosing (91,92,93)
const short MoCord[] = {18, 590 , 66, 638};
const short TuCord[] = {84, 590 , 132, 638};
const short WeCord[] = {150, 590 , 198, 638};
const short ThCord[] = {216, 590 , 264, 638};
const short FrCord[] = {282, 590 , 330, 638};
const short SaCord[] = {348, 590 , 396, 638};
const short SoCord[] = {414, 590 , 462, 638};

//RGBScreen
const short red1Up[] = {138, 240 , 186  , 288};
const short red1Down[] = {138, 293 , 186, 341};

const short red2Up[] = {138, 540 , 186  , 588};
const short red2Down[] = {138, 593 , 186, 641};


SdFat SD;
File Aquaduino;


/*EEPROM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  locations for saved settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
0 - FDose[0] in ml          0 is N
1 - FDose[1] in ml          1 is NPK
2 - FDose[2] in ml          2 is FE
3 - FMax[0] in ml
4 - FMax[1] in ml
5 - FMax[2] in ml
6 - FLeft[0] in ml
7 - NKPLeft in ml
8 - FLeft[2] in ml
 powLightOnHour=EEPROM.read(9);
 powLightOnMinute=EEPROM.read(10);
 powLightOffHour=EEPROM.read(11);
 powLightOffMinute=EEPROM.read(12);
 powCo2OnHour=EEPROM.read(13);
 powCo2OnMinute=EEPROM.read(14);
 powCo2OffHour=EEPROM.read(15);
 powCo2OffMinute=EEPROM.read(16);
 lightPWM[0].Hour=EEPROM.read(20);
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
 pump1Clean=EEPROM.read(56);
 pump2Clean=EEPROM.read(57);
 light230Clean=EEPROM.read(58);
 light2Clean=EEPROM.read(59);
 co2Clean=EEPROM.read(60);
 heaterClean=EEPROM.read(61);
 coolClean=EEPROM.read(62);
 PHUpperLimit=EEPROM.read(63);
 PHLowerLimitEEPROM.read(64);
 TempUpperLimit=(EEPROM.read(65));
 TempLowerLimit=(EEPROM.read(66));
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
  cleanMinutes=EEPROM.read(91);
  doseHour=EEPROM.read(92);
  doseMinute=EEPROM.read(93);
  //tankCleanDay=EEPROM.read(94);
 /*tankCleanMonth=EEPROM.read(95);
 co2BottleDay=EEPROM.read(96);
 co2BottleMonth=EEPROM.read(97);
 cleanFilter1Day=EEPROM.read(98);
 cleanFilter1Month=EEPROM.read(99);
 cleanFilter2Day=EEPROM.read(100);
 cleanFilter2Month=EEPROM.read(101);
  tankCleandDays=EEPROM.read(102);
 co2BottleDays=EEPROM.read(103);
 cleanFilter1Days=EEPROM.read(104);
 cleanFilter2Days=EEPROM.read(105);
screenOnHour=EEPROM.read(106);
 screenOffHour=EEPROM.read(107);
 screenOnMinute=EEPROM.read(108);
 screenOffMinute=EEPROM.read(109);
 standByMinutes=EEPROM.read(110);
 backlightPWM=EEPROM.read(111);
 lightRGB[0].Hour=EEPROM.read(112);
lightRGB[0].Minute=EEPROM.read(113);
lightRGB[0].red=EEPROM.read(114);
lightRGB[0].green=EEPROM.read(115);
lightRGB[0].blue=EEPROM.read(116);
lightRGB[1].Hour=EEPROM.read(117);
lightRGB[1].Minute=EEPROM.read(118);
lightRGB[1].red=EEPROM.read(119);
lightRGB[1].green=EEPROM.read(120);
lightRGB[1].blue=EEPROM.read(121);
lightRGB[2].Hour=EEPROM.read(122);
lightRGB[2].Minute=EEPROM.read(123);
lightRGB[2].red=EEPROM.read(124);
lightRGB[2].green=EEPROM.read(125);
lightRGB[2].blue=EEPROM.read(126);
lightRGB[3].Hour=EEPROM.read(127);
lightRGB[3].Minute=EEPROM.read(128);
lightRGB[3].red=EEPROM.read(129);
lightRGB[3].green=EEPROM.read(130);
lightRGB[3].blue=EEPROM.read(131);
lightRGB[4].Hour=EEPROM.read(132);
lightRGB[4].Minute=EEPROM.read(133);
lightRGB[4].red=EEPROM.read(134);
lightRGB[4].green=EEPROM.read(135);
lightRGB[4].blue=EEPROM.read(136);
lightRGB[5].Hour=EEPROM.read(137);
lightRGB[5].Minute=EEPROM.read(138);
lightRGB[5].red=EEPROM.read(139);
lightRGB[5].green=EEPROM.read(140);
lightRGB[5].blue=EEPROM.read(141);
lightRGB[6].Hour=EEPROM.read(142);
lightRGB[6].Minute=EEPROM.read(143);
lightRGB[6].red=EEPROM.read(144);
lightRGB[6].green=EEPROM.read(145);
lightRGB[6].blue=EEPROM.read(146);
lightRGB[7].Hour=EEPROM.read(147);
lightRGB[7].Minute=EEPROM.read(148);
lightRGB[7].red=EEPROM.read(149);
lightRGB[7].green=EEPROM.read(150);
lightRGB[7].blue=EEPROM.read(151);
lightRGB[8].Hour=EEPROM.read(152);
lightRGB[8].Minute=EEPROM.read(153);
lightRGB[8].red=EEPROM.read(154);
lightRGB[8].green=EEPROM.read(155);
lightRGB[8].blue=EEPROM.read(156);
lightRGB[9].Hour=EEPROM.read(157);
lightRGB[9].Minute=EEPROM.read(158);
lightRGB[9].red=EEPROM.read(159);
lightRGB[9].green=EEPROM.read(160);
lightRGB[9].blue=EEPROM.read(161);
lightRGB[10].Hour=EEPROM.read(162);
lightRGB[10].Minute=EEPROM.read(163);
lightRGB[10].red=EEPROM.read(164);
lightRGB[10].green=EEPROM.read(165);
lightRGB[10].blue=EEPROM.read(166);
lightRGB[11].Hour=EEPROM.read(167);
lightRGB[11].Minute=EEPROM.read(168);
lightRGB[11].red=EEPROM.read(168);
lightRGB[11].green=EEPROM.read(170);
lightRGB[11].blue=EEPROM.read(171);
MoonRed = EEPROM.read(172);
 MoonGreen = EEPROM.read(173);
  MoonBlue = EEPROM.read(174);
  MoonMinutes = EEPROM.read(175);



/*  voidSetup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void Setup - initialisierung aller Werte und Libraries
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


void setup() {
  wdt_enable(WDTO_8S);  //enable watchdog - reload after 8 seconds
  file.setSSpin(53);
  file.initFAT(SPISPEED_VERYHIGH);
  // thsoot Aquaduino
  pinMode(53, OUTPUT);
  SD.begin(53);
  pinMode(44, OUTPUT);
  analogWrite(backlightPIN, 255);
  //reset Co2Values
  for (int i = 0; i < 96; i++)
  {
    Co2Values[i] = 1;
  }
  for (int i = 0; i < 96; i++)
  {
    PHValues[i] = 7.0;
  }

  for (int i = 0; i < 62; i++)
  {
    TempValues[i] = 26;
  }








  //ph Stuff
  Serial.begin(38400);                                                      //set baud rate for the hardware serial port_0 to 38400
  Serial3.begin(38400);                                                     //set baud rate for software serial port_3 to 38400
  inputstring.reserve(5);                                                   //set aside some bytes for receiving data from the PC
  PhWertString.reserve(30);     //set aside some bytes for receiving data from Atlas Scientific product
  sensors.begin();    //temp Sensor
  //end


  //mySwtich initialisation

  mySwitch.enableTransmit(8); // Transmitter is connected to Arduino Pin #8
  mySwitch.enableReceive(4); // this is pin 19
  //mySwitch.setPulseLength(426);
  mySwitch.setRepeatTransmit(20);



  //Begin I2C - Relais und RTC
  Wire.begin();
  rtc.begin();
  DateTime now = rtc.now();
  lastFert = now.unixtime() - 82800;
  sx1509.init();  // Initialize the SX1509, does Wire.begin()
  sx1509.pinDir(pump1Pin, OUTPUT);  // Set SX1509 pin 14 as an output
  sx1509.pinDir(pump2Pin, OUTPUT);  //
  sx1509.pinDir(light230Pin, OUTPUT);  //
  sx1509.pinDir(light2Pin, OUTPUT);  //
  sx1509.pinDir(light1Pin, OUTPUT);  //
  sx1509.pinDir(co2Pin, OUTPUT);  //
  sx1509.pinDir(heaterPin, OUTPUT);  //
  sx1509.pinDir(dPump1Pin, OUTPUT);  //
  sx1509.pinDir(dPump2Pin, OUTPUT);  //
  sx1509.pinDir(dPump3Pin, OUTPUT);  //
  sx1509.pinDir(coolPin, OUTPUT);  //
  //END I2C
  //touchscreen
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.InitLCD(PORTRAIT);
  myTouch.InitTouch(PORTRAIT);
  myTouch.setPrecision(PREC_EXTREME);
  // get Values from EEPROM
  readPowerSchedule();
  readCleanSched();
  readPHValue();
  readTempValue();
  readFerti();
  readLightPWM();
  readLightRGB();
  readScreenScreen();


  timeToNextLight = now.unixtime() - (now.unixtime() - 86400 * 7); //set it to 7 days as fallback
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 0);
  drawScreen();
  processPump();
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);

  Aquaduino = SD.open("aqua.txt", FILE_WRITE);
  if (Aquaduino)
  { Aquaduino.println("Initialisation complete - Aquaduino up and running");
    Aquaduino.println("Date;Time;Temperature;PH;PWM;pump2Value;light230Value;light1Value=true;light2Value;co2Value;heaterValue;dPump1Value;dPump2Value;dPump3Value;coolValue;");
    Aquaduino.close();
  }
}



void loop() {
  wdt_reset();
  currentMillis = millis(); // get current millis
  if (myTouch.dataAvailable())//read x,y when button is touched

  { // make sure it's been .2 sec between touches
    // if (currentMillis - prevMillisTouch > 0)
    if (dispScreen != 13)
    {
      analogWrite(backlightPIN, 255);
    }
    { myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      prevMillisTouch = currentMillis;






      switch (dispScreen)
      { // Caputre Buttons @ F ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 0:  // home screen
          if (((x >= HomeButtonCoord[0]) && (x <= HomeButtonCoord[2]))  && ((y >= HomeButtonCoord[1]) && (y <= HomeButtonCoord[3]))) // homebutton
          { waitForIt(HomeButtonCoord[0], HomeButtonCoord[1], HomeButtonCoord[2], HomeButtonCoord[3]);
            // lastScreen = 1; //we fake that we switched from another screen - this will FULL reload the HomeScreen on press on the Home button
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= FeedButtonCoord[0]) && (x <= FeedButtonCoord[2]))  && ((y >= FeedButtonCoord[1]) && (y <= FeedButtonCoord[3])))
          { waitForIt(FeedButtonCoord[0], FeedButtonCoord[1], FeedButtonCoord[2], FeedButtonCoord[3]);
            light2Value = true;
            light1Value = true;
            manualOverride = true;
            analogWrite(redPin, 0);
            analogWrite(greenPin, 0);
            analogWrite(bluePin, 0);
            processRelais();
            dispScreen = 1;

            drawScreen();


          }
          else if (((x >= PowerButtonCoord[0]) && (x <= PowerButtonCoord[2]))  && ((y >= PowerButtonCoord[1]) && (y <= PowerButtonCoord[3]))) // powerbutton
          { waitForIt(PowerButtonCoord[0], PowerButtonCoord[1], PowerButtonCoord[2], PowerButtonCoord[3]);
            dispScreen = 2;
            drawScreen();
          }
          else if (((x >= SettingsButtonCoord[0]) && (x <= SettingsButtonCoord[2]))  && ((y >= SettingsButtonCoord[1]) && (y <= SettingsButtonCoord[3]))) //  settings
          { waitForIt(SettingsButtonCoord[0], SettingsButtonCoord[1], SettingsButtonCoord[2], SettingsButtonCoord[3]);
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // bottomResetButton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            cleaningInProcess = false;
            manualOverride = false;
            TVModeState = false;
            pump1Value = false;
            pump2Value = false;
            printDate(now, 5, 5);
            lightCalculator();
            drawScreen();
            AI();
            processPump();
          }
          else if (((x >= LightUp[0]) && (x <= LightUp[2]))  && ((y >= LightUp[1]) && (y <= LightUp[3]))) // pwmUp
          { //waitForIt(LightUp[0], LightUp[1], LightUp[2], LightUp[3]);
            calculatedPWM += 2.55;
            if (calculatedPWM > 255)
            { calculatedPWM = 0;
            }
            if (calculatedPWM < 0)
            { calculatedPWM = 255;
            }

            manualOverride = true;
            drawPWM();
          }
          else if (((x >= LightDown[0]) && (x <= LightDown[2]))  && ((y >= LightDown[1]) && (y <= LightDown[3]))) // pwmDown
          { //waitForIt(LightDown[0], LightDown[1], LightDown[2], LightDown[3]);
            manualOverride = true;
            calculatedPWM -= 2.55;
            if (calculatedPWM > 255)
            { calculatedPWM = 0;
            }
            if (calculatedPWM < 0)
            { calculatedPWM = 255;
            }

            drawPWM();
          }
          break;

        // Caputre Buttons @ FeedScreen ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 1:
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;

            drawScreen();

          }

          else if (((x >= ResetCord[0]) && (x <= ResetCord[2]))  && ((y >= ResetCord[1]) && (y <= ResetCord[3]))) // homebutton
          { waitForIt(ResetCord[0], ResetCord[1], ResetCord[2], ResetCord[3]);
            manualOverride = false;

            dispScreen = 0;
            drawScreen();
            //processRelais();
            lightCalculator();
            AI();

          }



          break;

        // Caputre Buttons @ PowerScreen ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 2:
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= Filter1Cord[0]) && (x <= Filter1Cord[2]))  && ((y >= Filter1Cord[1]) && (y <= Filter1Cord[3]))) // homebutton
          { waitForIt(Filter1Cord[0], Filter1Cord[1], Filter1Cord[2], Filter1Cord[3]);
            manualOverride = true;
            pump1Value = !pump1Value;
            updatePowerIcons();
            if (!pump1Value) {
              mySwitch.send(f11on, 24);
            }
            else {
              mySwitch.send(f11off, 24);
            };

          }
          else if (((x >= Filter2Cord[0]) && (x <= Filter2Cord[2]))  && ((y >= Filter2Cord[1]) && (y <= Filter2Cord[3]))) // homebutton
          { waitForIt(Filter2Cord[0], Filter2Cord[1], Filter2Cord[2], Filter2Cord[3]);
            manualOverride = true;
            pump2Value = !pump2Value;
            updatePowerIcons();
            if (!pump2Value) {
              mySwitch.send(f42on, 24);
            }
            else {
              mySwitch.send(f42off, 24);
            }
          }


          else if (((x >= Ligth1Cord[0]) && (x <= Ligth1Cord[2]))  && ((y >= Ligth1Cord[1]) && (y <= Ligth1Cord[3]))) // homebutton
          { waitForIt(Ligth1Cord[0], Ligth1Cord[1], Ligth1Cord[2], Ligth1Cord[3]);
            manualOverride = true;
            light230Value = !light230Value;
            updatePowerIcons();
            if (!light230Value) {
              mySwitch.send(f13on, 24);
            }
            else {
              mySwitch.send(f13off, 24);
            }

          }
          else if (((x >= Light2Cord[0]) && (x <= Light2Cord[2]))  && ((y >= Light2Cord[1]) && (y <= Light2Cord[3]))) // homebutton
          { waitForIt(Light2Cord[0], Light2Cord[1], Light2Cord[2], Light2Cord[3]);
            manualOverride = true;
            light1Value = !light1Value;
            light2Value = !light2Value;
            updatePowerIcons();
            sx1509.writePin(light1Pin, light1Value);
            sx1509.writePin(light2Pin, light2Value);
          }
          else if (((x >= Co2Cord[0]) && (x <= Co2Cord[2]))  && ((y >= Co2Cord[1]) && (y <= Co2Cord[3]))) // homebutton
          { waitForIt(Co2Cord[0], Co2Cord[1], Co2Cord[2], Co2Cord[3]);
            manualOverride = true;
            co2Value = !co2Value;
            updatePowerIcons();
            if (!co2Value) {
              mySwitch.send(f22on, 24);
            }
            else {
              mySwitch.send(f22off, 24);
            }
          }
          else if (((x >= HeaterCord[0]) && (x <= HeaterCord[2]))  && ((y >= HeaterCord[1]) && (y <= HeaterCord[3]))) // homebutton
          { waitForIt(HeaterCord[0], HeaterCord[1], HeaterCord[2], HeaterCord[3]);
            manualOverride = true;
            heaterValue = !heaterValue;
            updatePowerIcons();
            if (!heaterValue) {
              mySwitch.send(f21on, 24);
            }
            else {
              mySwitch.send(f21off, 24);
            }

          }
          else if (((x >= CoolingCord[0]) && (x <= CoolingCord[2]))  && ((y >= CoolingCord[1]) && (y <= CoolingCord[3]))) // homebutton
          { waitForIt(CoolingCord[0], CoolingCord[1], CoolingCord[2], CoolingCord[3]);
            manualOverride = true;
            coolValue = !coolValue;
            updatePowerIcons();
            if (!coolValue) {
              mySwitch.send(f23on, 24);
            }
            else {
              mySwitch.send(f23off, 24);
            }

          }
          else if (((x >= AllOFFCord[0]) && (x <= AllOFFCord[2]))  && ((y >= AllOFFCord[1]) && (y <= AllOFFCord[3]))) // homebutton
          { waitForIt(AllOFFCord[0], AllOFFCord[1], AllOFFCord[2], AllOFFCord[3]);
            manualOverride = false;
            pump1Value = true;
            pump2Value = true;
            light230Value = true;
            light1Value = true;
            light2Value = true;
            co2Value = true;
            heaterValue = true;
            coolValue = true;
            updatePowerIcons();
            processPump();
            processRelais();
            processPump();

          }
          else if (((x >= ResetCord[0]) && (x <= ResetCord[2]))  && ((y >= ResetCord[1]) && (y <= ResetCord[3]))) // Resetbutton
          { waitForIt(ResetCord[0], ResetCord[1], ResetCord[2], ResetCord[3]);
            manualOverride = false;
            cleaningInProcess = false;
            TVModeState = false;
            pump1Value = false;
            pump2Value = false;
            dispScreen = 0;
            drawScreen();
            //processRelais();
            lightCalculator();
            AI();
            processPump();

          }

          else if (((x >= CleanModeCord[0]) && (x <= CleanModeCord[2]))  && ((y >= CleanModeCord[1]) && (y <= CleanModeCord[3]))) // Cleanbutton
          { waitForIt(CleanModeCord[0], CleanModeCord[1], CleanModeCord[2], CleanModeCord[3]);

            CleanMode();


          }








          break;



        // Caputre Buttons @ Settingsscreen ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 3:
          if (((x >= ClockCord[0]) && (x <= ClockCord[2]))  && ((y >= ClockCord[1]) && (y <= ClockCord[3]))) // homebutton
          { waitForIt(ClockCord[0], ClockCord[1], ClockCord[2], ClockCord[3]);
            dispScreen = 5;
            adjustTimer = rtc.now();
            adjustTimer = adjustTimer.unixtime() + 60 - adjustTimer.second();
            drawScreen();
          }
          else if (((x >= LightsCord[0]) && (x <= LightsCord[2]))  && ((y >= LightsCord[1]) && (y <= LightsCord[3]))) // homebutton
          { waitForIt(LightsCord[0], LightsCord[1], LightsCord[2], LightsCord[3]);
            dispScreen = 4;
            drawScreen();
          }
          else if (((x >= CleanCord[0]) && (x <= CleanCord[2]))  && ((y >= CleanCord[1]) && (y <= CleanCord[3]))) // homebutton
          { waitForIt(CleanCord[0], CleanCord[1], CleanCord[2], CleanCord[3]);
            dispScreen = 6;
            drawScreen();
          }
          else if (((x >= ScheCord[0]) && (x <= ScheCord[2]))  && ((y >= ScheCord[1]) && (y <= ScheCord[3]))) // homebutton
          { waitForIt(ScheCord[0], ScheCord[1], ScheCord[2], ScheCord[3]);
            readReminder();
            dispScreen = 7;
            drawScreen();
          }
          else if (((x >= ScreenCord[0]) && (x <= ScreenCord[2]))  && ((y >= ScreenCord[1]) && (y <= ScreenCord[3]))) // homebutton
          { waitForIt(ScreenCord[0], ScreenCord[1], ScreenCord[2], ScreenCord[3]);
            readScreenScreen();
            dispScreen = 13;
            drawScreen();
          }

          else if (((x >= HeatCord[0]) && (x <= HeatCord[2]))  && ((y >= HeatCord[1]) && (y <= HeatCord[3]))) // homebutton
          { waitForIt(HeatCord[0], HeatCord[1], HeatCord[2], HeatCord[3]);
            dispScreen = 8;
            drawScreen();
          }
          else if (((x >= Co2SetCord[0]) && (x <= Co2SetCord[2]))  && ((y >= Co2SetCord[1]) && (y <= Co2SetCord[3]))) // homebutton
          { waitForIt(Co2SetCord[0], Co2SetCord[1], Co2SetCord[2], Co2SetCord[3]);
            dispScreen = 12;
            drawScreen();
          }
          else if (((x >= DoseCord[0]) && (x <= DoseCord[2]))  && ((y >= DoseCord[1]) && (y <= DoseCord[3]))) // homebutton
          { waitForIt(DoseCord[0], DoseCord[1], DoseCord[2], DoseCord[3]);
            dispScreen = 9;
            drawScreen();
          }
          else if (((x >= PowerSchedCord[0]) && (x <= PowerSchedCord[2]))  && ((y >= PowerSchedCord[1]) && (y <= PowerSchedCord[3]))) // homebutton
          { waitForIt(PowerSchedCord[0], PowerSchedCord[1], PowerSchedCord[2], PowerSchedCord[3]);
            dispScreen = 10;
            drawScreen();
          }
          else if (((x >= RGBCord[0]) && (x <= RGBCord[2]))  && ((y >= RGBCord[1]) && (y <= RGBCord[3]))) // homebutton
          { waitForIt(RGBCord[0], RGBCord[1], RGBCord[2], RGBCord[3]);
            dispScreen = 14;
            drawScreen();
          }




          else if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          break;


        case 4:  // Lights
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= LightMode1Cord[0]) && (x <= LightMode1Cord[2]))  && ((y >= LightMode1Cord[1]) && (y <= LightMode1Cord[3])))
          { waitForIt(LightMode1Cord[0], LightMode1Cord[1], LightMode1Cord[2], LightMode1Cord[3]);
            dispScreen = 41;
            lightScreenSet = 0;
            drawScreen();
          }
          else if (((x >= LightMode2Cord[0]) && (x <= LightMode2Cord[2]))  && ((y >= LightMode2Cord[1]) && (y <= LightMode2Cord[3])))
          { waitForIt(LightMode2Cord[0], LightMode2Cord[1], LightMode2Cord[2], LightMode2Cord[3]);
            dispScreen = 42;
            lightScreenSet = 2;
            drawScreen();
          }
          else if (((x >= LightMode3Cord[0]) && (x <= LightMode3Cord[2]))  && ((y >= LightMode3Cord[1]) && (y <= LightMode3Cord[3])))
          { waitForIt(LightMode3Cord[0], LightMode3Cord[1], LightMode3Cord[2], LightMode3Cord[3]);
            dispScreen = 43;
            lightScreenSet = 4;
            drawScreen();
          }
          else if (((x >= LightMode4Cord[0]) && (x <= LightMode4Cord[2]))  && ((y >= LightMode4Cord[1]) && (y <= LightMode4Cord[3])))
          { waitForIt(LightMode4Cord[0], LightMode4Cord[1], LightMode4Cord[2], LightMode4Cord[3]);
            dispScreen = 44;
            lightScreenSet = 6;
            drawScreen();
          }
          else if (((x >= LightMode5Cord[0]) && (x <= LightMode5Cord[2]))  && ((y >= LightMode5Cord[1]) && (y <= LightMode5Cord[3])))
          { waitForIt(LightMode5Cord[0], LightMode5Cord[1], LightMode5Cord[2], LightMode5Cord[3]);
            dispScreen = 45;
            lightScreenSet = 8;
            drawScreen();
          }
          else if (((x >= LightMode6Cord[0]) && (x <= LightMode6Cord[2]))  && ((y >= LightMode6Cord[1]) && (y <= LightMode6Cord[3])))
          { waitForIt(LightMode6Cord[0], LightMode6Cord[1], LightMode6Cord[2], LightMode6Cord[3]);
            dispScreen = 46;
            lightScreenSet = 10;
            drawScreen();
          }
          break;

        case 41:  //listen on lightscene1
        case 42:  //listen on lightscene2
        case 43:  //listen on lightscene3
        case 44:  //listen on lightscene4
        case 45:  //listen on lightscene5
        case 46:  //listen on lightscene6

          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= powLightOnHourUp[0]) && (x <= powLightOnHourUp[2]))  && ((y >= powLightOnHourUp[1]) && (y <= powLightOnHourUp[3])))
          { waitForIt(powLightOnHourUp[0], powLightOnHourUp[1], powLightOnHourUp[2], powLightOnHourUp[3]);
            if ((lightPWM[lightScreenSet].Hour >= 23) && (lightPWM[lightScreenSet].Hour <= 250)) {
              lightPWM[lightScreenSet].Hour = 0;
            }
            else {
              lightPWM[lightScreenSet].Hour++;
            }
            UpdateLightScene();
          }
          else if (((x >= powLightOnHourDown[0]) && (x <= powLightOnHourDown[2]))  && ((y >= powLightOnHourDown[1]) && (y <= powLightOnHourDown[3])))
          { waitForIt(powLightOnHourDown[0], powLightOnHourDown[1], powLightOnHourDown[2], powLightOnHourDown[3]);
            lightPWM[lightScreenSet].Hour--;
            if (lightPWM[lightScreenSet].Hour >= 230) {
              lightPWM[lightScreenSet].Hour = 23;
            }
            UpdateLightScene();
          }
          else if (((x >= powLightOnMinuteUp[0]) && (x <= powLightOnMinuteUp[2]))  && ((y >= powLightOnMinuteUp[1]) && (y <= powLightOnMinuteUp[3])))
          { waitForIt(powLightOnMinuteUp[0], powLightOnMinuteUp[1], powLightOnMinuteUp[2], powLightOnMinuteUp[3]);
            if ((lightPWM[lightScreenSet].Minute >= 59) && (lightPWM[lightScreenSet].Minute <= 250)) {
              lightPWM[lightScreenSet].Minute = 0;
            }
            else {
              lightPWM[lightScreenSet].Minute++;
            }
            UpdateLightScene();
          }
          else if (((x >= powLightOnMinuteDown[0]) && (x <= powLightOnMinuteDown[2]))  && ((y >= powLightOnMinuteDown[1]) && (y <= powLightOnMinuteDown[3])))
          { waitForIt(powLightOnMinuteDown[0], powLightOnMinuteDown[1], powLightOnMinuteDown[2], powLightOnMinuteDown[3]);
            lightPWM[lightScreenSet].Minute--;
            if (lightPWM[lightScreenSet].Minute >= 230) {
              lightPWM[lightScreenSet].Minute = 59;
            }
            UpdateLightScene();
          }
          else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3]))) // homebutton
          { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);

            lightPWM[lightScreenSet].pwmValue += 2.55;
            if (lightPWM[lightScreenSet].pwmValue > 255)
            { lightPWM[lightScreenSet].pwmValue = 0;
            }
            if (lightPWM[lightScreenSet].pwmValue < 0)
            { lightPWM[lightScreenSet].pwmValue = 255;
            }

            UpdateLightScene();
          }
          else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3]))) // homebutton
          { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
            lightPWM[lightScreenSet].pwmValue -= 2.55;
            if (lightPWM[lightScreenSet].pwmValue > 255)
            { lightPWM[lightScreenSet].pwmValue = 0;
            }
            if (lightPWM[lightScreenSet].pwmValue < 0)
            { lightPWM[lightScreenSet].pwmValue = 255;
            }
            UpdateLightScene();
          }
          else if (((x >= powCo2OnHourUp[0]) && (x <= powCo2OnHourUp[2]))  && ((y >= powCo2OnHourUp[1]) && (y <= powCo2OnHourUp[3]))) // homebutton
          { waitForIt(powCo2OnHourUp[0], powCo2OnHourUp[1], powCo2OnHourUp[2], powCo2OnHourUp[3]);
            if ((lightPWM[lightScreenSet + 1].Hour >= 23) && (lightPWM[lightScreenSet + 1].Hour <= 250)) {
              lightPWM[lightScreenSet + 1].Hour = 0;
            }
            else {
              lightPWM[lightScreenSet + 1].Hour++;
            }
            UpdateLightScene();
          }
          else if (((x >= powCo2OnHourDown[0]) && (x <= powCo2OnHourDown[2]))  && ((y >= powCo2OnHourDown[1]) && (y <= powCo2OnHourDown[3]))) // homebutton
          { waitForIt(powCo2OnHourDown[0], powCo2OnHourDown[1], powCo2OnHourDown[2], powCo2OnHourDown[3]);
            lightPWM[lightScreenSet + 1].Hour--;
            if (lightPWM[lightScreenSet + 1].Hour >= 230) {
              lightPWM[lightScreenSet + 1].Hour = 23;
            }
            UpdateLightScene();
          }
          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            if ((lightPWM[lightScreenSet + 1].Minute >= 59) && (lightPWM[lightScreenSet + 1].Minute <= 250)) {
              lightPWM[lightScreenSet + 1].Minute = 0;
            }
            else {
              lightPWM[lightScreenSet + 1].Minute++;
            }
            UpdateLightScene();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            lightPWM[lightScreenSet + 1].Minute--;
            if (lightPWM[lightScreenSet + 1].Minute >= 230) {
              lightPWM[lightScreenSet + 1].Minute = 59;
            }
            UpdateLightScene();
          }
          else if (((x >= powCo2OffMinuteUp[0]) && (x <= powCo2OffMinuteUp[2]))  && ((y >= powCo2OffMinuteUp[1]) && (y <= powCo2OffMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], powCo2OffMinuteUp[2], powCo2OffMinuteUp[3]);
            lightPWM[lightScreenSet + 1].pwmValue += 2.55;
            if (lightPWM[lightScreenSet + 1].pwmValue > 255)
            { lightPWM[lightScreenSet + 1].pwmValue = 0;
            }
            if (lightPWM[lightScreenSet + 1].pwmValue < 0)
            { lightPWM[lightScreenSet + 1].pwmValue = 255;
            }
            UpdateLightScene();
          }
          else if (((x >= powCo2OffMinuteDown[0]) && (x <= powCo2OffMinuteDown[2]))  && ((y >= powCo2OffMinuteDown[1]) && (y <= powCo2OffMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], powCo2OffMinuteDown[2], powCo2OffMinuteDown[3]);
            lightPWM[lightScreenSet + 1].pwmValue -= 2.55;
            if (lightPWM[lightScreenSet + 1].pwmValue > 255)
            { lightPWM[lightScreenSet + 1].pwmValue = 0;
            }
            if (lightPWM[lightScreenSet + 1].pwmValue < 0)
            { lightPWM[lightScreenSet + 1].pwmValue = 255;
            }
            UpdateLightScene();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            saveLightPWM();
            dispScreen = 4;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readLightPWM();
            dispScreen = 4;
            drawScreen();
          }
          break;



        // Caputre Buttons @ ClockScreen ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case 5:
          if (((x >= SetClockCord[0]) && (x <= SetClockCord[2]))  && ((y >= SetClockCord[1]) && (y <= SetClockCord[3]))) // homebutton
          { waitForIt(SetClockCord[0], SetClockCord[1], SetClockCord[2], SetClockCord[3]);
            rtc.adjust(adjustTimer);
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= HourUp[0]) && (x <= HourUp[2]))  && ((y >= HourUp[1]) && (y <= HourUp[3]))) // homebutton
          { waitForIt(HourUp[0], HourUp[1], HourUp[2], HourUp[3]);
            adjustTimer = adjustTimer.unixtime() + 3600;
            updateClockSettings();
          }
          else if (((x >= HourDown[0]) && (x <= HourDown[2]))  && ((y >= HourDown[1]) && (y <= HourDown[3]))) // homebutton
          { waitForIt(HourDown[0], HourDown[1], HourDown[2], HourDown[3]);
            adjustTimer = adjustTimer.unixtime() - 3600;
            updateClockSettings();
          }
          else if (((x >= MinuteUp[0]) && (x <= MinuteUp[2]))  && ((y >= MinuteUp[1]) && (y <= MinuteUp[3]))) // homebutton
          { waitForIt(MinuteUp[0], MinuteUp[1], MinuteUp[2], MinuteUp[3]);
            adjustTimer = adjustTimer.unixtime() + 60;
            updateClockSettings();
          }
          else if (((x >= MinuteDown[0]) && (x <= MinuteDown[2]))  && ((y >= MinuteDown[1]) && (y <= MinuteDown[3]))) // homebutton
          { waitForIt(MinuteDown[0], MinuteDown[1], MinuteDown[2], MinuteDown[3]);
            adjustTimer = adjustTimer.unixtime() - 60;
            updateClockSettings();
          }
          else if (((x >= DayUp[0]) && (x <= DayUp[2]))  && ((y >= DayUp[1]) && (y <= DayUp[3]))) // homebutton
          { waitForIt(DayUp[0], DayUp[1], DayUp[2], DayUp[3]);
            adjustTimer = adjustTimer.unixtime() + 86400;
            updateClockSettings();
          }
          else if (((x >= DayDown[0]) && (x <= DayDown[2]))  && ((y >= DayDown[1]) && (y <= DayDown[3]))) // homebutton
          { waitForIt(DayDown[0], DayDown[1], DayDown[2], DayDown[3]);
            adjustTimer = adjustTimer.unixtime() - 86400;
            updateClockSettings();
          }
          else if (((x >= MonthUp[0]) && (x <= MonthUp[2]))  && ((y >= MonthUp[1]) && (y <= MonthUp[3]))) // homebutton
          { waitForIt(MonthUp[0], MonthUp[1], MonthUp[2], MonthUp[3]);
            adjustTimer = adjustTimer.unixtime() + (30 * 86400);
            updateClockSettings();
          }
          else if (((x >= MonthDown[0]) && (x <= MonthDown[2]))  && ((y >= MonthDown[1]) && (y <= MonthDown[3]))) // homebutton
          { waitForIt(MonthDown[0], MonthDown[1], MonthDown[2], MonthDown[3]);
            adjustTimer = adjustTimer.unixtime() - (30 * 86400L);
            updateClockSettings();
          }
          else if (((x >= YearUp[0]) && (x <= YearUp[2]))  && ((y >= YearUp[1]) && (y <= YearUp[3]))) // homebutton
          { waitForIt(YearUp[0], YearUp[1], YearUp[2], YearUp[3]);
            adjustTimer = adjustTimer.unixtime() + (365 * 86400);
            updateClockSettings();
          }
          else if (((x >= YearDown[0]) && (x <= YearDown[2]))  && ((y >= YearDown[1]) && (y <= YearDown[3]))) // homebutton
          { waitForIt(YearDown[0], YearDown[1], YearDown[2], YearDown[3]);
            adjustTimer = adjustTimer.unixtime() - (365 * 86400);
            updateClockSettings();
          }
          else if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          break;



        case 6:  // Clean Schedule
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= Filter1Cord[0]) && (x <= Filter1Cord[2]))  && ((y >= Filter1Cord[1]) && (y <= Filter1Cord[3]))) // homebutton
          { waitForIt(Filter1Cord[0], Filter1Cord[1], Filter1Cord[2], Filter1Cord[3]);
            pump1Clean = !pump1Clean;
            updateCleanSchedScreen();
          }
          else if (((x >= Filter2Cord[0]) && (x <= Filter2Cord[2]))  && ((y >= Filter2Cord[1]) && (y <= Filter2Cord[3]))) // homebutton
          { waitForIt(Filter2Cord[0], Filter2Cord[1], Filter2Cord[2], Filter2Cord[3]);
            pump2Clean = !pump2Clean;
            updateCleanSchedScreen();
          }


          else if (((x >= Ligth1Cord[0]) && (x <= Ligth1Cord[2]))  && ((y >= Ligth1Cord[1]) && (y <= Ligth1Cord[3]))) // homebutton
          { waitForIt(Ligth1Cord[0], Ligth1Cord[1], Ligth1Cord[2], Ligth1Cord[3]);
            light230Clean = !light230Clean;
            updateCleanSchedScreen();
          }
          else if (((x >= Light2Cord[0]) && (x <= Light2Cord[2]))  && ((y >= Light2Cord[1]) && (y <= Light2Cord[3]))) // homebutton
          { waitForIt(Light2Cord[0], Light2Cord[1], Light2Cord[2], Light2Cord[3]);
            light2Clean = !light2Clean;
            updateCleanSchedScreen();
          }
          else if (((x >= Co2Cord[0]) && (x <= Co2Cord[2]))  && ((y >= Co2Cord[1]) && (y <= Co2Cord[3]))) // homebutton
          { waitForIt(Co2Cord[0], Co2Cord[1], Co2Cord[2], Co2Cord[3]);
            co2Clean = !co2Clean;
            updateCleanSchedScreen();
          }
          else if (((x >= HeaterCord[0]) && (x <= HeaterCord[2]))  && ((y >= HeaterCord[1]) && (y <= HeaterCord[3]))) // homebutton
          { waitForIt(HeaterCord[0], HeaterCord[1], HeaterCord[2], HeaterCord[3]);
            heaterClean = !heaterClean;
            updateCleanSchedScreen();
          }
          else if (((x >= CoolingCord[0]) && (x <= CoolingCord[2]))  && ((y >= CoolingCord[1]) && (y <= CoolingCord[3]))) // homebutton
          { waitForIt(CoolingCord[0], CoolingCord[1], CoolingCord[2], CoolingCord[3]);
            coolClean = !coolClean;
            updateCleanSchedScreen();
          }
          else if (((x >= powCo2OffMinuteUp[0]) && (x <= powCo2OffMinuteUp[2]))  && ((y >= powCo2OffMinuteUp[1]) && (y <= powCo2OffMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], powCo2OffMinuteUp[2], powCo2OffMinuteUp[3]);
            cleanMinutes++;
            quickUpdateCleanSchedScreen();
          }
          else if (((x >= powCo2OffMinuteDown[0]) && (x <= powCo2OffMinuteDown[2]))  && ((y >= powCo2OffMinuteDown[1]) && (y <= powCo2OffMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], powCo2OffMinuteDown[2], powCo2OffMinuteDown[3]);
            cleanMinutes--;
            quickUpdateCleanSchedScreen();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            saveCleanSched();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readCleanSched();
            dispScreen = 3;
            drawScreen();
          }
          break;





        case 7:  //  Reminder Screen


          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            saveReminder();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readReminder();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= Filter1Cord[0]) && (x <= Filter1Cord[2]))  && ((y >= Filter1Cord[1]) && (y <= Filter1Cord[3]))) // homebutton
          { actOnRealease(Filter1Cord[0], Filter1Cord[1], Filter1Cord[2], Filter1Cord[3]);
            tankCleandDays = (now.unixtime() - tankClean.unixtime()) / 86400;
            tankClean = now;

            updateRemindScreen();
          }
          else if (((x >= Ligth1Cord[0]) && (x <= Ligth1Cord[2]))  && ((y >= Ligth1Cord[1]) && (y <= Ligth1Cord[3]))) // homebutton
          { actOnRealease(Ligth1Cord[0], Ligth1Cord[1], Ligth1Cord[2], Ligth1Cord[3]);
            co2BottleDays = (now.unixtime() - co2Bottle.unixtime()) / 86400;
            co2Bottle = now;
            updateRemindScreen();
          }
          else if (((x >= Co2Cord[0]) && (x <= Co2Cord[2]))  && ((y >= Co2Cord[1]) && (y <= Co2Cord[3]))) // homebutton
          { actOnRealease(Co2Cord[0], Co2Cord[1], Co2Cord[2], Co2Cord[3]);
            cleanFilter1Days = (now.unixtime() - cleanFilter1.unixtime()) / 86400;
            cleanFilter1 = now;
            updateRemindScreen();
          }
          else if (((x >= CoolingCord[0]) && (x <= CoolingCord[2]))  && ((y >= CoolingCord[1]) && (y <= CoolingCord[3]))) // homebutton
          { actOnRealease(CoolingCord[0], CoolingCord[1], CoolingCord[2], CoolingCord[3]);
            cleanFilter2Days = (now.unixtime() - cleanFilter2.unixtime()) / 86400;
            cleanFilter2 = now;
            updateRemindScreen();
          }
          break;




        case 8:  //  heater
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3]))) // homebutton
          { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);
            TempUpperLimit += 1;
            updateHeaterScreen();
          }
          else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3]))) // homebutton
          { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
            TempUpperLimit -= 1;
            updateHeaterScreen();
          }
          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            TempLowerLimit += 1;
            updateHeaterScreen();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            TempLowerLimit -= 1;
            updateHeaterScreen();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            saveTempValue();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readTempValue();
            dispScreen = 3;
            drawScreen();
          }
          break;



        case 9:  //  DOSE
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= LightMode1Cord[0]) && (x <= LightMode1Cord[2]))  && ((y >= LightMode1Cord[1]) && (y <= LightMode1Cord[3])))
          { waitForIt(LightMode1Cord[0], LightMode1Cord[1], LightMode1Cord[2], LightMode1Cord[3]);
            dispScreen = 91;
            FSelect = 0;
            drawScreen();
          }
          else if (((x >= LightMode2Cord[0]) && (x <= LightMode2Cord[2]))  && ((y >= LightMode2Cord[1]) && (y <= LightMode2Cord[3])))
          { waitForIt(LightMode2Cord[0], LightMode2Cord[1], LightMode2Cord[2], LightMode2Cord[3]);
            dispScreen = 91;
            FSelect = 1;
            drawScreen();
          }
          else if (((x >= LightMode3Cord[0]) && (x <= LightMode3Cord[2]))  && ((y >= LightMode3Cord[1]) && (y <= LightMode3Cord[3])))
          { waitForIt(LightMode3Cord[0], LightMode3Cord[1], LightMode3Cord[2], LightMode3Cord[3]);
            dispScreen = 91;
            FSelect = 2;
            drawScreen();
          }


          else if (((x >= powCo2OnHourUp[0]) && (x <= powCo2OnHourUp[2]))  && ((y >= powCo2OnHourUp[1]) && (y <= powCo2OnHourUp[3]))) // homebutton
          { waitForIt(powCo2OnHourUp[0], powCo2OnHourUp[1], powCo2OnHourUp[2], powCo2OnHourUp[3]);
            if ((doseHour >= 23) && (doseHour <= 250)) {
              doseHour = 0;
            }
            else {
              doseHour++;
            }
            updateDoseScreen();
          }
          else if (((x >= powCo2OnHourDown[0]) && (x <= powCo2OnHourDown[2]))  && ((y >= powCo2OnHourDown[1]) && (y <= powCo2OnHourDown[3]))) // homebutton
          { waitForIt(powCo2OnHourDown[0], powCo2OnHourDown[1], powCo2OnHourDown[2], powCo2OnHourDown[3]);
            doseHour--;
            if (doseHour >= 230) {
              doseHour = 23;
            }
            updateDoseScreen();
          }
          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            if ((doseMinute >= 59) && (doseMinute <= 250)) {
              doseMinute = 0;
            }
            else {
              doseMinute++;
            }
            updateDoseScreen();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            doseMinute--;
            if (doseMinute >= 230) {
              doseMinute = 59;
            }
            updateDoseScreen();
          }




          else if (((x >= refillAllCord[0]) && (x <= refillAllCord[2]))  && ((y >= refillAllCord[1]) && (y <= refillAllCord[3])))
          { waitForIt(refillAllCord[0], refillAllCord[1], refillAllCord[2], refillAllCord[3]);
            FLeft[0] = FMax[0];
            FLeft[1] = FMax[1];
            FLeft[2] = FMax[2];
            saveFerti();
            drawScreen();
          }
          break;

        case 91:  //  DOSE
          { if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
            { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
              dispScreen = 0;
              drawScreen();
            }
            else if (((x >= powLightOnMinuteUp[0]) && (x <= powLightOnMinuteUp[2]))  && ((y >= powLightOnMinuteUp[1]) && (y <= powLightOnMinuteUp[3]))) // homebutton
            { waitForIt(powLightOnMinuteUp[0], powLightOnMinuteUp[1], powLightOnMinuteUp[2], powLightOnMinuteUp[3]);
              FDose[FSelect] += 1;
              quickUpdateDoseScreenN(FSelect);
            }
            else if (((x >= powLightOnMinuteDown[0]) && (x <= powLightOnMinuteDown[2]))  && ((y >= powLightOnMinuteDown[1]) && (y <= powLightOnMinuteDown[3]))) // homebutton
            { waitForIt(powLightOnMinuteDown[0], powLightOnMinuteDown[1], powLightOnMinuteDown[2], powLightOnMinuteDown[3]);
              FDose[FSelect] -= 1;
              quickUpdateDoseScreenN(FSelect);
            }
            else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3]))) // homebutton
            { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);
              FMax[FSelect] += 5;
              quickUpdateDoseScreenN(FSelect);
            }
            else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3]))) // homebutton
            { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
              FMax[FSelect] -= 5;
              quickUpdateDoseScreenN(FSelect);
            }
            else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
            { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
              FRate[FSelect] += 1;
              quickUpdateDoseScreenN(FSelect);
            }
            else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
            { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
              FRate[FSelect] -= 1;
              quickUpdateDoseScreenN(FSelect);
            }
            else if (((x >= MoCord[0]) && (x <= MoCord[2]))  && ((y >= MoCord[1]) && (y <= MoCord[3]))) // homebutton
            { waitForIt(MoCord[0], MoCord[1], MoCord[2], MoCord[3]);
              MoF[FSelect] = !MoF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= TuCord[0]) && (x <= TuCord[2]))  && ((y >= TuCord[1]) && (y <= TuCord[3]))) // homebutton
            { waitForIt(TuCord[0], TuCord[1], TuCord[2], TuCord[3]);
              TuF[FSelect] = !TuF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= WeCord[0]) && (x <= WeCord[2]))  && ((y >= WeCord[1]) && (y <= WeCord[3]))) // homebutton
            { waitForIt(WeCord[0], WeCord[1], WeCord[2], WeCord[3]);
              WeF[FSelect] = !WeF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= ThCord[0]) && (x <= ThCord[2]))  && ((y >= ThCord[1]) && (y <= ThCord[3]))) // homebutton
            { waitForIt(ThCord[0], ThCord[1], ThCord[2], ThCord[3]);
              ThF[FSelect] = !ThF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= FrCord[0]) && (x <= FrCord[2]))  && ((y >= FrCord[1]) && (y <= FrCord[3]))) // homebutton
            { waitForIt(FrCord[0], FrCord[1], FrCord[2], FrCord[3]);
              FrF[FSelect] = !FrF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= SaCord[0]) && (x <= SaCord[2]))  && ((y >= SaCord[1]) && (y <= SaCord[3]))) // homebutton
            { waitForIt(SaCord[0], SaCord[1], SaCord[2], SaCord[3]);
              SaF[FSelect] = !SaF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= SoCord[0]) && (x <= SoCord[2]))  && ((y >= SoCord[1]) && (y <= SoCord[3]))) // homebutton
            { waitForIt(SoCord[0], SoCord[1], SoCord[2], SoCord[3]);
              SuF[FSelect] = !SuF[FSelect];
              UpdateDoseScreenN(FSelect);
            }
            else if (((x >= calibrateCord[0]) && (x <= calibrateCord[2]))  && ((y >= calibrateCord[1]) && (y <= calibrateCord[3]))) // homebutton
            { waitForIt(calibrateCord[0], calibrateCord[1], calibrateCord[2], calibrateCord[3]);
              fertmillis = millis();
              switch (FSelect)
              { case 0:
                  {
                    /*dPump1Value=false;
                    processRelais();
                    wdt_disable();
                    delay(60000);
                    dPump1Value=true;
                    processRelais();
                    wdt_enable(WDTO_8S);
                    */
                    wdt_disable();
                    while (millis() - fertmillis < 1000 * FDose[0] / (FRate[0] / 60))
                    { dPump1Value = false;
                      processRelais();
                    }
                    dPump1Value = true;
                    processRelais();

                    wdt_enable(WDTO_8S);
                    break;

                  }
                case 1:
                  { wdt_disable();
                    while (millis() - fertmillis < 1000 * FDose[1] / (FRate[1] / 60))
                    { dPump2Value = false;
                      processRelais();
                    }
                    dPump2Value = true;
                    processRelais();

                    wdt_enable(WDTO_8S);
                    break;
                  }
                case 2:
                  { wdt_disable();
                    while (millis() - fertmillis < 1000 * FDose[2] / (FRate[2] / 60))
                    { dPump3Value = false;
                      processRelais();
                    }
                    dPump3Value = true;
                    processRelais();

                    wdt_enable(WDTO_8S);
                    break;
                  }
              }
            }
            else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
            { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
              saveFerti();
              dispScreen = 9;
              drawScreen();
            }
            else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
            { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
              readFerti();
              dispScreen = 9;
              drawScreen();
            }
          }
          break;


        case 10:  //  Power Sched
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            readPowerSchedule();
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= powLightOnHourUp[0]) && (x <= powLightOnHourUp[2]))  && ((y >= powLightOnHourUp[1]) && (y <= powLightOnHourUp[3]))) // homebutton
          { waitForIt(powLightOnHourUp[0], powLightOnHourUp[1], powLightOnHourUp[2], powLightOnHourUp[3]);
            if ((powLightOnHour >= 23) && (powLightOnHour <= 250)) {
              powLightOnHour = 0;
            }
            else {
              powLightOnHour++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOnHourDown[0]) && (x <= powLightOnHourDown[2]))  && ((y >= powLightOnHourDown[1]) && (y <= powLightOnHourDown[3]))) // homebutton
          { waitForIt(powLightOnHourDown[0], powLightOnHourDown[1], powLightOnHourDown[2], powLightOnHourDown[3]);
            powLightOnHour--;
            if (powLightOnHour >= 230) {
              powLightOnHour = 23;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOnMinuteUp[0]) && (x <= powLightOnMinuteUp[2]))  && ((y >= powLightOnMinuteUp[1]) && (y <= powLightOnMinuteUp[3]))) // homebutton
          { waitForIt(powLightOnMinuteUp[0], powLightOnMinuteUp[1], powLightOnMinuteUp[2], powLightOnMinuteUp[3]);
            if ((powLightOnMinute >= 59) && (powLightOnMinute <= 250)) {
              powLightOnMinute = 0;
            }
            else {
              powLightOnMinute++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOnMinuteDown[0]) && (x <= powLightOnMinuteDown[2]))  && ((y >= powLightOnMinuteDown[1]) && (y <= powLightOnMinuteDown[3]))) // homebutton
          { waitForIt(powLightOnMinuteDown[0], powLightOnMinuteDown[1], powLightOnMinuteDown[2], powLightOnMinuteDown[3]);
            powLightOnMinute--;
            if (powLightOnMinute >= 230) {
              powLightOnMinute = 59;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOffHourUp[0]) && (x <= powLightOffHourUp[2]))  && ((y >= powLightOffHourUp[1]) && (y <= powLightOffHourUp[3]))) // homebutton
          { waitForIt(powLightOffHourUp[0], powLightOffHourUp[1], powLightOffHourUp[2], powLightOffHourUp[3]);
            if ((powLightOffHour >= 23) && (powLightOffHour <= 250)) {
              powLightOffHour = 0;
            }
            else {
              powLightOffHour++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOffHourDown[0]) && (x <= powLightOffHourDown[2]))  && ((y >= powLightOffHourDown[1]) && (y <= powLightOffHourDown[3]))) // homebutton
          { waitForIt(powLightOffHourDown[0], powLightOffHourDown[1], powLightOffHourDown[2], powLightOffHourDown[3]);
            powLightOffHour--;
            if (powLightOffHour >= 230) {
              powLightOffHour = 23;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3]))) // homebutton
          { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);
            if ((powLightOffMinute >= 59) && (powLightOffMinute <= 250)) {
              powLightOffMinute = 0;
            }
            else {
              powLightOffMinute++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3]))) // homebutton
          { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
            powLightOffMinute--;
            if (powLightOffMinute >= 230) {
              powLightOffMinute = 59;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OnHourUp[0]) && (x <= powCo2OnHourUp[2]))  && ((y >= powCo2OnHourUp[1]) && (y <= powCo2OnHourUp[3]))) // homebutton
          { waitForIt(powCo2OnHourUp[0], powCo2OnHourUp[1], powCo2OnHourUp[2], powCo2OnHourUp[3]);
            if ((powCo2OnHour >= 23) && (powCo2OnHour <= 250)) {
              powCo2OnHour = 0;
            }
            else {
              powCo2OnHour++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OnHourDown[0]) && (x <= powCo2OnHourDown[2]))  && ((y >= powCo2OnHourDown[1]) && (y <= powCo2OnHourDown[3]))) // homebutton
          { waitForIt(powCo2OnHourDown[0], powCo2OnHourDown[1], powCo2OnHourDown[2], powCo2OnHourDown[3]);
            powCo2OnHour--;
            if (powCo2OnHour >= 230) {
              powCo2OnHour = 23;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            if ((powCo2OnMinute >= 59) && (powCo2OnMinute <= 250)) {
              powCo2OnMinute = 0;
            }
            else {
              powCo2OnMinute++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            powCo2OnMinute--;
            if (powCo2OnMinute >= 230) {
              powCo2OnMinute = 59;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OffHourUp[0]) && (x <= powCo2OffHourUp[2]))  && ((y >= powCo2OffHourUp[1]) && (y <= powCo2OffHourUp[3]))) // homebutton
          { waitForIt(powCo2OffHourUp[0], powCo2OffHourUp[1], powCo2OffHourUp[2], powCo2OffHourUp[3]);
            if ((powCo2OffHour >= 23) && (powCo2OffHour <= 250)) {
              powCo2OffHour = 0;
            }
            else {
              powCo2OffHour++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OffHourDown[0]) && (x <= powCo2OffHourDown[2]))  && ((y >= powCo2OffHourDown[1]) && (y <= powCo2OffHourDown[3]))) // homebutton
          { waitForIt(powCo2OffHourDown[0], powCo2OffHourDown[1], powCo2OffHourDown[2], powCo2OffHourDown[3]);
            powCo2OffHour--;
            if (powCo2OffHour >= 230) {
              powCo2OffHour = 23;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OffMinuteUp[0]) && (x <= powCo2OffMinuteUp[2]))  && ((y >= powCo2OffMinuteUp[1]) && (y <= powCo2OffMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], powCo2OffMinuteUp[2], powCo2OffMinuteUp[3]);
            if ((powCo2OffMinute >= 59) && (powCo2OffMinute <= 250)) {
              powCo2OffMinute = 0;
            }
            else {
              powCo2OffMinute++;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= powCo2OffMinuteDown[0]) && (x <= powCo2OffMinuteDown[2]))  && ((y >= powCo2OffMinuteDown[1]) && (y <= powCo2OffMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], powCo2OffMinuteDown[2], powCo2OffMinuteDown[3]);
            powCo2OffMinute--;
            if (powCo2OffMinute >= 230) {
              powCo2OffMinute = 59;
            }
            UpdatePowerSchedScreen();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            savePowerSchedule();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readPowerSchedule();
            dispScreen = 3;
            drawScreen();
          }

          break;


        case 12:  //  Co2Screen Screen
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3]))) // homebutton
          { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);
            PHUpperLimit += 0.05;
            updateCO2SetScreen();
          }
          else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3]))) // homebutton
          { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
            PHUpperLimit -= 0.05;
            updateCO2SetScreen();
          }
          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            PHLowerLimit += 0.05;
            updateCO2SetScreen();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            PHLowerLimit -= 0.05;
            updateCO2SetScreen();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            savePHValue();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readPHValue();
            dispScreen = 3;
            drawScreen();
          }
          break;

        case 13: //testscreen

          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            readScreenScreen();
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= powLightOnHourUp[0]) && (x <= powLightOnHourUp[2]))  && ((y >= powLightOnHourUp[1]) && (y <= powLightOnHourUp[3]))) // homebutton
          { waitForIt(powLightOnHourUp[0], powLightOnHourUp[1], powLightOnHourUp[2], powLightOnHourUp[3]);
            if ((screenOnHour >= 23) && (screenOnHour <= 250)) {
              screenOnHour = 0;
            }
            else {
              screenOnHour++;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOnHourDown[0]) && (x <= powLightOnHourDown[2]))  && ((y >= powLightOnHourDown[1]) && (y <= powLightOnHourDown[3]))) // homebutton
          { waitForIt(powLightOnHourDown[0], powLightOnHourDown[1], powLightOnHourDown[2], powLightOnHourDown[3]);
            screenOnHour--;
            if (screenOnHour >= 230) {
              screenOnHour = 23;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOnMinuteUp[0]) && (x <= powLightOnMinuteUp[2]))  && ((y >= powLightOnMinuteUp[1]) && (y <= powLightOnMinuteUp[3]))) // homebutton
          { waitForIt(powLightOnMinuteUp[0], powLightOnMinuteUp[1], powLightOnMinuteUp[2], powLightOnMinuteUp[3]);
            if ((screenOnMinute >= 59) && (screenOnMinute <= 250)) {
              screenOnMinute = 0;
            }
            else {
              screenOnMinute++;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOnMinuteDown[0]) && (x <= powLightOnMinuteDown[2]))  && ((y >= powLightOnMinuteDown[1]) && (y <= powLightOnMinuteDown[3]))) // homebutton
          { waitForIt(powLightOnMinuteDown[0], powLightOnMinuteDown[1], powLightOnMinuteDown[2], powLightOnMinuteDown[3]);
            screenOnMinute--;
            if (screenOnMinute >= 230) {
              screenOnMinute = 59;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOffHourUp[0]) && (x <= powLightOffHourUp[2]))  && ((y >= powLightOffHourUp[1]) && (y <= powLightOffHourUp[3]))) // homebutton
          { waitForIt(powLightOffHourUp[0], powLightOffHourUp[1], powLightOffHourUp[2], powLightOffHourUp[3]);
            if ((screenOffHour >= 23) && (screenOffHour <= 250)) {
              screenOffHour = 0;
            }
            else {
              screenOffHour++;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOffHourDown[0]) && (x <= powLightOffHourDown[2]))  && ((y >= powLightOffHourDown[1]) && (y <= powLightOffHourDown[3]))) // homebutton
          { waitForIt(powLightOffHourDown[0], powLightOffHourDown[1], powLightOffHourDown[2], powLightOffHourDown[3]);
            screenOffHour--;
            if (screenOffHour >= 230) {
              screenOffHour = 23;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3]))) // homebutton
          { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);
            if ((screenOffMinute >= 59) && (screenOffMinute <= 250)) {
              screenOffMinute = 0;
            }
            else {
              screenOffMinute++;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3]))) // homebutton
          { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
            screenOffMinute--;
            if (screenOffMinute >= 230) {
              screenOffMinute = 59;
            }
            UpdateScreenScreen();
          }

          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            if ((standByMinutes >= 59) && (standByMinutes <= 250)) {
              standByMinutes = 0;
            }
            else {
              standByMinutes++;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            standByMinutes--;
            if (standByMinutes >= 230) {
              standByMinutes = 59;
            }
            UpdateScreenScreen();
          }
          else if (((x >= powCo2OffMinuteUp[0]) && (x <= powCo2OffMinuteUp[2]))  && ((y >= powCo2OffMinuteUp[1]) && (y <= powCo2OffMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], powCo2OffMinuteUp[2], powCo2OffMinuteUp[3]);
            backlightPWM++;
            analogWrite(backlightPIN, backlightPWM);
            UpdateScreenScreen();
          }
          else if (((x >= powCo2OffMinuteDown[0]) && (x <= powCo2OffMinuteDown[2]))  && ((y >= powCo2OffMinuteDown[1]) && (y <= powCo2OffMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], powCo2OffMinuteDown[2], powCo2OffMinuteDown[3]);
            backlightPWM--;
            analogWrite(backlightPIN, backlightPWM);
            UpdateScreenScreen();
          }
          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            saveScreenScreen();
            dispScreen = 3;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readScreenScreen();
            dispScreen = 3;
            drawScreen();
          }

          break;




        case 14:  // RGBLights
          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= LightMode1Cord[0]) && (x <= LightMode1Cord[2]))  && ((y >= LightMode1Cord[1]) && (y <= LightMode1Cord[3])))
          { waitForIt(LightMode1Cord[0], LightMode1Cord[1], LightMode1Cord[2], LightMode1Cord[3]);
            dispScreen = 141;
            RGBScreenSet = 0;
            drawScreen();
          }
          else if (((x >= LightMode2Cord[0]) && (x <= LightMode2Cord[2]))  && ((y >= LightMode2Cord[1]) && (y <= LightMode2Cord[3])))
          { waitForIt(LightMode2Cord[0], LightMode2Cord[1], LightMode2Cord[2], LightMode2Cord[3]);
            dispScreen = 142;
            RGBScreenSet = 2;
            drawScreen();
          }
          else if (((x >= LightMode3Cord[0]) && (x <= LightMode3Cord[2]))  && ((y >= LightMode3Cord[1]) && (y <= LightMode3Cord[3])))
          { waitForIt(LightMode3Cord[0], LightMode3Cord[1], LightMode3Cord[2], LightMode3Cord[3]);
            dispScreen = 143;
            RGBScreenSet = 4;
            drawScreen();
          }
          else if (((x >= LightMode4Cord[0]) && (x <= LightMode4Cord[2]))  && ((y >= LightMode4Cord[1]) && (y <= LightMode4Cord[3])))
          { waitForIt(LightMode4Cord[0], LightMode4Cord[1], LightMode4Cord[2], LightMode4Cord[3]);
            dispScreen = 144;
            RGBScreenSet = 6;
            drawScreen();
          }
          else if (((x >= LightMode5Cord[0]) && (x <= LightMode5Cord[2]))  && ((y >= LightMode5Cord[1]) && (y <= LightMode5Cord[3])))
          { waitForIt(LightMode5Cord[0], LightMode5Cord[1], LightMode5Cord[2], LightMode5Cord[3]);
            dispScreen = 145;
            RGBScreenSet = 8;
            drawScreen();
          }
          else if (((x >= LightMode6Cord[0]) && (x <= LightMode6Cord[2]))  && ((y >= LightMode6Cord[1]) && (y <= LightMode6Cord[3])))
          { waitForIt(LightMode6Cord[0], LightMode6Cord[1], LightMode6Cord[2], LightMode6Cord[3]);
            dispScreen = 146;
            RGBScreenSet = 10;
            drawScreen();
          }
          break;

        case 141:  //listen on lightscene1
        case 142:  //listen on lightscene2
        case 143:  //listen on lightscene3
        case 144:  //listen on lightscene4
        case 145:  //listen on lightscene5
        case 146:  //listen on lightscene6

          if (((x >= BottomButtonCoord[0]) && (x <= BottomButtonCoord[2]))  && ((y >= BottomButtonCoord[1]) && (y <= BottomButtonCoord[3]))) // homebutton
          { waitForIt(BottomButtonCoord[0], BottomButtonCoord[1], BottomButtonCoord[2], BottomButtonCoord[3]);
            dispScreen = 0;
            drawScreen();
          }
          else if (((x >= powLightOnHourUp[0]) && (x <= powLightOnHourUp[2]))  && ((y >= powLightOnHourUp[1]) && (y <= powLightOnHourUp[3])))
          { waitForIt(powLightOnHourUp[0], powLightOnHourUp[1], powLightOnHourUp[2], powLightOnHourUp[3]);
            if ((lightRGB[RGBScreenSet].Hour >= 23) && (lightRGB[RGBScreenSet].Hour <= 250)) {
              lightRGB[RGBScreenSet].Hour = 0;
            }
            else {
              lightRGB[RGBScreenSet].Hour++;
            }
            UpdateRGBSceneTOP();
          }
          else if (((x >= powLightOnHourDown[0]) && (x <= powLightOnHourDown[2]))  && ((y >= powLightOnHourDown[1]) && (y <= powLightOnHourDown[3])))
          { waitForIt(powLightOnHourDown[0], powLightOnHourDown[1], powLightOnHourDown[2], powLightOnHourDown[3]);
            lightRGB[RGBScreenSet].Hour--;
            if (lightRGB[RGBScreenSet].Hour >= 230) {
              lightRGB[RGBScreenSet].Hour = 23;
            }
            UpdateRGBSceneTOP();
          }
          else if (((x >= powLightOnMinuteUp[0]) && (x <= powLightOnMinuteUp[2]))  && ((y >= powLightOnMinuteUp[1]) && (y <= powLightOnMinuteUp[3])))
          { waitForIt(powLightOnMinuteUp[0], powLightOnMinuteUp[1], powLightOnMinuteUp[2], powLightOnMinuteUp[3]);
            if ((lightRGB[RGBScreenSet].Minute >= 59) && (lightRGB[RGBScreenSet].Minute <= 250)) {
              lightRGB[RGBScreenSet].Minute = 0;
            }
            else {
              lightRGB[RGBScreenSet].Minute++;
            }
            UpdateRGBSceneTOP();
          }
          else if (((x >= powLightOnMinuteDown[0]) && (x <= powLightOnMinuteDown[2]))  && ((y >= powLightOnMinuteDown[1]) && (y <= powLightOnMinuteDown[3])))
          { waitForIt(powLightOnMinuteDown[0], powLightOnMinuteDown[1], powLightOnMinuteDown[2], powLightOnMinuteDown[3]);
            lightRGB[RGBScreenSet].Minute--;
            if (lightRGB[RGBScreenSet].Minute >= 230) {
              lightRGB[RGBScreenSet].Minute = 59;
            }
            UpdateRGBSceneTOP();
          }

          else if (((x >= red1Up[0]) && (x <= red1Up[2]))  && ((y >= red1Up[1]) && (y <= red1Up[3])))
          { waitForIt(red1Up[0], red1Up[1], red1Up[2], red1Up[3]);
            lightRGB[RGBScreenSet].red += 1;
            UpdateRGBSceneTOP();
            analogWrite(redPin, lightRGB[RGBScreenSet].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet].blue);


          }
          else if (((x >= red1Down[0]) && (x <= red1Down[2]))  && ((y >= red1Down[1]) && (y <= red1Down[3])))
          { waitForIt(red1Down[0], red1Down[1], red1Down[2], red1Down[3]);
            lightRGB[RGBScreenSet].red -= 1;
            analogWrite(redPin, lightRGB[RGBScreenSet].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet].blue);
            UpdateRGBSceneTOP();
          }


          else if (((x >= powLightOffHourUp[0]) && (x <= powLightOffHourUp[2]))  && ((y >= powLightOffHourUp[1]) && (y <= powLightOffHourUp[3])))
          { waitForIt(powLightOffHourUp[0], powLightOffHourUp[1], powLightOffHourUp[2], powLightOffHourUp[3]);
            lightRGB[RGBScreenSet].green += 1;
            analogWrite(redPin, lightRGB[RGBScreenSet].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet].blue);
            UpdateRGBSceneTOP();
          }
          else if (((x >= powLightOffHourDown[0]) && (x <= powLightOffHourDown[2]))  && ((y >= powLightOffHourDown[1]) && (y <= powLightOffHourDown[3])))
          { waitForIt(powLightOffHourDown[0], powLightOffHourDown[1], powLightOffHourDown[2], powLightOffHourDown[3]);
            lightRGB[RGBScreenSet].green -= 1;
            analogWrite(redPin, lightRGB[RGBScreenSet].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet].blue);
            UpdateRGBSceneTOP();
          }

          else if (((x >= powLightOffMinuteUp[0]) && (x <= powLightOffMinuteUp[2]))  && ((y >= powLightOffMinuteUp[1]) && (y <= powLightOffMinuteUp[3])))
          { waitForIt(powLightOffMinuteUp[0], powLightOffMinuteUp[1], powLightOffMinuteUp[2], powLightOffMinuteUp[3]);
            lightRGB[RGBScreenSet].blue += 1;
            analogWrite(redPin, lightRGB[RGBScreenSet].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet].blue);
            UpdateRGBSceneTOP();
          }
          else if (((x >= powLightOffMinuteDown[0]) && (x <= powLightOffMinuteDown[2]))  && ((y >= powLightOffMinuteDown[1]) && (y <= powLightOffMinuteDown[3])))
          { waitForIt(powLightOffMinuteDown[0], powLightOffMinuteDown[1], powLightOffMinuteDown[2], powLightOffMinuteDown[3]);
            lightRGB[RGBScreenSet].blue -= 1;
            analogWrite(redPin, lightRGB[RGBScreenSet].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet].blue);
            UpdateRGBSceneTOP();
          }









          else if (((x >= powCo2OnHourUp[0]) && (x <= powCo2OnHourUp[2]))  && ((y >= powCo2OnHourUp[1]) && (y <= powCo2OnHourUp[3]))) // homebutton
          { waitForIt(powCo2OnHourUp[0], powCo2OnHourUp[1], powCo2OnHourUp[2], powCo2OnHourUp[3]);
            if ((lightRGB[RGBScreenSet + 1].Hour >= 23) && (lightRGB[RGBScreenSet + 1].Hour <= 250)) {
              lightRGB[RGBScreenSet + 1].Hour = 0;
            }
            else {
              lightRGB[RGBScreenSet + 1].Hour++;
            }
            UpdateRGBSceneBOT();
          }
          else if (((x >= powCo2OnHourDown[0]) && (x <= powCo2OnHourDown[2]))  && ((y >= powCo2OnHourDown[1]) && (y <= powCo2OnHourDown[3]))) // homebutton
          { waitForIt(powCo2OnHourDown[0], powCo2OnHourDown[1], powCo2OnHourDown[2], powCo2OnHourDown[3]);
            lightRGB[RGBScreenSet + 1].Hour--;
            if (lightRGB[RGBScreenSet + 1].Hour >= 230) {
              lightRGB[RGBScreenSet + 1].Hour = 23;
            }
            UpdateRGBSceneBOT();
          }
          else if (((x >= powCo2OnMinuteUp[0]) && (x <= powCo2OnMinuteUp[2]))  && ((y >= powCo2OnMinuteUp[1]) && (y <= powCo2OnMinuteUp[3]))) // homebutton
          { waitForIt(powCo2OnMinuteUp[0], powCo2OnMinuteUp[1], powCo2OnMinuteUp[2], powCo2OnMinuteUp[3]);
            if ((lightRGB[RGBScreenSet + 1].Minute >= 59) && (lightRGB[RGBScreenSet + 1].Minute <= 250)) {
              lightRGB[RGBScreenSet + 1].Minute = 0;
            }
            else {
              lightRGB[RGBScreenSet + 1].Minute++;
            }
            UpdateRGBSceneBOT();
          }
          else if (((x >= powCo2OnMinuteDown[0]) && (x <= powCo2OnMinuteDown[2]))  && ((y >= powCo2OnMinuteDown[1]) && (y <= powCo2OnMinuteDown[3]))) // homebutton
          { waitForIt(powCo2OnMinuteDown[0], powCo2OnMinuteDown[1], powCo2OnMinuteDown[2], powCo2OnMinuteDown[3]);
            lightRGB[RGBScreenSet + 1].Minute--;
            if (lightRGB[RGBScreenSet + 1].Minute >= 230) {
              lightRGB[RGBScreenSet + 1].Minute = 59;
            }
            UpdateRGBSceneBOT();
          }

          else if (((x >= red2Up[0]) && (x <= red2Up[2]))  && ((y >= red2Up[1]) && (y <= red2Up[3])))
          { waitForIt(red2Up[0], red2Up[1], red2Up[2], red2Up[3]);
            lightRGB[RGBScreenSet + 1].red += 1;
            analogWrite(redPin, lightRGB[RGBScreenSet + 1].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet + 1].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet + 1].blue);
            UpdateRGBSceneBOT();
          }
          else if (((x >= red2Down[0]) && (x <= red2Down[2]))  && ((y >= red2Down[1]) && (y <= red2Down[3])))
          { waitForIt(red2Down[0], red2Down[1], red2Down[2], red2Down[3]);
            lightRGB[RGBScreenSet + 1].red -= 1;
            analogWrite(redPin, lightRGB[RGBScreenSet + 1].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet + 1].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet + 1].blue);
            UpdateRGBSceneBOT();
          }


          else if (((x >= powCo2OffHourUp[0]) && (x <= powCo2OffHourUp[2]))  && ((y >= powCo2OffHourUp[1]) && (y <= powCo2OffHourUp[3])))
          { waitForIt(powCo2OffHourUp[0], powCo2OffHourUp[1], powCo2OffHourUp[2], powCo2OffHourUp[3]);
            lightRGB[RGBScreenSet + 1].green += 1;
            analogWrite(redPin, lightRGB[RGBScreenSet + 1].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet + 1].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet + 1].blue);
            UpdateRGBSceneBOT();
          }
          else if (((x >= powCo2OffHourDown[0]) && (x <= powCo2OffHourDown[2]))  && ((y >= powCo2OffHourDown[1]) && (y <= powCo2OffHourDown[3])))
          { waitForIt(powCo2OffHourDown[0], powCo2OffHourDown[1], powCo2OffHourDown[2], powCo2OffHourDown[3]);
            lightRGB[RGBScreenSet + 1].green -= 1;
            analogWrite(redPin, lightRGB[RGBScreenSet + 1].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet + 1].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet + 1].blue);
            UpdateRGBSceneBOT();
          }

          else if (((x >= powCo2OffMinuteUp[0]) && (x <= powCo2OffMinuteUp[2]))  && ((y >= powCo2OffMinuteUp[1]) && (y <= powCo2OffMinuteUp[3])))
          { waitForIt(powCo2OffMinuteUp[0], powCo2OffMinuteUp[1], powCo2OffMinuteUp[2], powCo2OffMinuteUp[3]);
            lightRGB[RGBScreenSet + 1].blue += 1;
            analogWrite(redPin, lightRGB[RGBScreenSet + 1].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet + 1].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet + 1].blue);
            UpdateRGBSceneBOT();
          }
          else if (((x >= powCo2OffMinuteDown[0]) && (x <= powCo2OffMinuteDown[2]))  && ((y >= powCo2OffMinuteDown[1]) && (y <= powCo2OffMinuteDown[3])))
          { waitForIt(powCo2OffMinuteDown[0], powCo2OffMinuteDown[1], powCo2OffMinuteDown[2], powCo2OffMinuteDown[3]);
            lightRGB[RGBScreenSet + 1].blue -= 1;
            analogWrite(redPin, lightRGB[RGBScreenSet + 1].red);
            analogWrite(greenPin, lightRGB[RGBScreenSet + 1].green);
            analogWrite(bluePin, lightRGB[RGBScreenSet + 1].blue);
            UpdateRGBSceneBOT();
          }






          else if (((x >= SetPowerSchedCord[0]) && (x <= SetPowerSchedCord[2]))  && ((y >= SetPowerSchedCord[1]) && (y <= SetPowerSchedCord[3]))) // homebutton
          { waitForIt(SetPowerSchedCord[0], SetPowerSchedCord[1], SetPowerSchedCord[2], SetPowerSchedCord[3]);
            saveLightRGB();
            dispScreen = 14;
            drawScreen();
          }
          else if (((x >= CancelPowerSchedCord[0]) && (x <= CancelPowerSchedCord[2]))  && ((y >= CancelPowerSchedCord[1]) && (y <= CancelPowerSchedCord[3]))) // homebutton
          { waitForIt(CancelPowerSchedCord[0], CancelPowerSchedCord[1], CancelPowerSchedCord[2], CancelPowerSchedCord[3]);
            readLightRGB();
            dispScreen = 14;
            drawScreen();
          }
          break;










      }




    }
  }


  if (!myTouch.dataAvailable())
  { getPHValue();
    if (currentMillis - prevMillis5sec > 5000)  //if 5 seconds are over update our data
    { prevMillis5sec = millis();
      GetTemperature();
      lightCalculator();
      UpdateClockAndLight();

   



      if (dispScreen < 1)
      { drawClockPhPWM();
     
      }

    }


    if (currentMillis - prevMillis1min > 60000)  //every 60 seconds update our data
    { prevMillis1min = millis();
      printDate(now, 5, 5);



      if (currentMillis - prevMillisTouch < (standByMinutes * 60000)) //wenn die Letzte berÃ¼hrung kleiner als die eingestellten StandbyMinutes
      { analogWrite(backlightPIN, 255);  //display auf volle Helligkeit
      }
      else //wenn keine BerÃ¼hrung - AI
      { AI();
        fertilize();
        DateTime CompareScreenOnTime (now.year(), now.month(), now.day(), int(screenOnHour), int(screenOnMinute), 0);
        DateTime CompareScreenOffTime (now.year(), now.month(), now.day(), int(screenOffHour), int(screenOffMinute), 0);

        if (now.unixtime() < CompareScreenOnTime.unixtime() || now.unixtime() > CompareScreenOffTime.unixtime())
        { analogWrite(backlightPIN, 0);
        }
        else
        { analogWrite(backlightPIN, backlightPWM);




        }
      }

    }


    if (currentMillis - prevMillis15min > 900000)  //every 15 minutes write to file
    { prevMillis15min = millis();
      //writeToRingBuffer();
      writeFile();
      writeToRingBuffer();  //testing
      drawScreen();   //testing
      
    }


    if (mySwitch.available()) // get RF Data
    {
      processRFInput();
      mySwitch.resetAvailable();
    }
  }
}



