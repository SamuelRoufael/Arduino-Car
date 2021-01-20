
#include <Adafruit_GFX.h>   
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Fonts/FreeSansOblique18pt7b.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1 
#define LCD_RD A0 

#define LCD_RESET A4 
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;


char playStatus = '0' ;
int iV = 15;

#define YP A3 
#define XM A2 
#define YM 9   
#define XP 8   

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button buttons[5];

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define BLACK    0x0000
#define RED      0xF800
#define WHITE    0xFFFF
#define VOLCOLOR 0x479F


void setup(void) {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) { 
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); 

  
  myDFPlayer.volume(15);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  tft.begin(0x9341);
  tft.setRotation(0);
  tft.fillScreen(BLACK);

  tft.setCursor(10,10);
  tft.setTextSize(2);
  tft.print("MP3");
  
  
  tft.drawLine(5,30, tft.width()-5,30,VOLCOLOR);

  tft.fillRect (75, 210, 90, 10,WHITE);
  tft.fillRect (75, 210, 45, 10,VOLCOLOR);

  tft.drawLine(5,tft.height()-35, tft.width()-5,tft.height()-35,VOLCOLOR);

  displayError("NinjaTurtles!");
  

  createButtons();
  drawPlayButton();
  drawNextButton();
  drawPreviousButton();
  drawVolumeDown();
  drawVolumeUp();


  delay(1000);
  Serial.println(myDFPlayer.readFileCounts());

  
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop(void) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
   }
   
   for (uint8_t b=0; b<5; b++) {
    if (buttons[b].contains(p.x, p.y)) {
     
      buttons[b].press(true); 
    } else {
      buttons[b].press(false);  
    }
  }

  for (uint8_t b=0; b<5; b++) {
    if (buttons[b].justReleased()) {
      buttons[b].drawButton(true);
      int current = myDFPlayer.readCurrentFileNumber();
      if(b==0){
        if(playStatus == '0' || playStatus == '1'){
          drawPlayButton();
          displayError("Paused");
        }else {
          drawPauseButton();
          displaySongName(current);
          }
          }if(b==1){
        drawNextButton();
        if(playStatus=='2')
        displaySongName(current);
      }if(b==2){
        drawPreviousButton();
        if(playStatus=='2')
        displaySongName(current);
      }if(b==3){
        drawVolumeDown();
        displayVolume();
        delay(500);
        if(playStatus == '1')
          displayError("Paused");
        else if(playStatus == '2')
          displaySongName(current); 
         else 
         displayError("NinjaTurtles!"); 
      }if(b==4){
        drawVolumeUp();
        displayVolume();
        
        delay(500);
        if(playStatus == '1')
          displayError("Paused");
        else if(playStatus == '2')
          displaySongName(current); 
         else 
         displayError("NinjaTurtles!");
          
      }
    }

    
    //on press
    if (buttons[b].justPressed()) {
        buttons[b].drawButton();
        if(b==0){
        if(playStatus == '0'){
          Serial.println(myDFPlayer.readFileCounts());
          drawPauseButton();
          playStatus = '2';
          myDFPlayer.play(1);  //Play the first mp3
          delay(1000);
        }else if(playStatus == '1'){
          drawPauseButton();
          playStatus = '2';
          myDFPlayer.start(); 
          delay(1000);
        }else if(playStatus == '2'){
          drawPlayButton();
          playStatus ='1';
          myDFPlayer.pause(); 
          delay(1000);
        }
        
      }if(b==1){
        drawNextButton();
          if(playStatus=='2'){
          myDFPlayer.next();  
          delay(1000);
          }
      }if(b==2){
        drawPreviousButton();
        if(playStatus=='2'){
          myDFPlayer.previous();  
          delay(1000);
        }
      }if(b==3){
        drawVolumeDown();
        if (iV > 0 ) {
          iV--;
          drawVolume(iV);
        }
        myDFPlayer.volumeDown();
      }if(b==4){
        drawVolumeUp();
        if ( iV < 30) {
          iV++;
          drawVolume(iV);
        }
        myDFPlayer.volumeUp();
      }
    }
  }
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(100); // UI debouncing
  }



void createButtons() {
  buttons[0].initButton(&tft, tft.width()/2, 
                 141,83, 83, VOLCOLOR, VOLCOLOR, BLACK,
                 "P/P",0);
  buttons[0].drawButton(true);
  
   buttons[1].initButton(&tft, tft.width()/2+83, 
                 141,50, 50, VOLCOLOR, VOLCOLOR, BLACK,
                  "N",0);
  buttons[1].drawButton(true);
  
   buttons[2].initButton(&tft, tft.width()/2-83, 
                 141,50, 50, VOLCOLOR, VOLCOLOR, BLACK,
                  "P",0);
  buttons[2].drawButton(true);
  
   buttons[3].initButton(&tft, 40, 
                 215,40, 40, VOLCOLOR, VOLCOLOR, BLACK,
                  "D",0);
  buttons[3].drawButton(true);
  
   buttons[4].initButton(&tft, tft.width()-40, 
                 215,40, 40, VOLCOLOR, VOLCOLOR, BLACK,
                  "U",0);
  buttons[4].drawButton(true);
}


void drawPlayButton() {
  
  tft.fillCircle(tft.width()/2, 141, 83/2, WHITE);
  tft.fillTriangle(tft.width()/2-10, 156, tft.width()/2-10, 126, tft.width()/2+15, 141, BLACK);
  
}
void drawPauseButton() {
  tft.fillCircle(tft.width()/2, 141, 83/2, WHITE);
  tft.fillRect (tft.width()/2-12, 141-15,10,33,BLACK);
  tft.fillRect (tft.width()/2+3, 141-15,10,33,BLACK);
}
void drawNextButton() {
  tft.fillCircle(tft.width()/2+83, 141, 25, WHITE);
  tft.fillRect (tft.width()/2+87, 132,4,20,BLACK);
  tft.fillTriangle(tft.width()/2+75, 131, tft.width()/2+75, 151, tft.width()/2+87, 141, BLACK);
  
}
void drawPreviousButton() {
  tft.fillCircle(tft.width()/2-83, 141, 25, WHITE);
  tft.fillRect (tft.width()/2-90, 132,4,20,BLACK);
  tft.fillTriangle(tft.width()/2-74, 131, tft.width()/2-74, 151, tft.width()/2-87, 141, BLACK);
}
void drawVolumeDown() {
  tft.fillCircle(40, 215, 20, WHITE);
  tft.fillRect (30, 212,20,6,BLACK);
}
void drawVolumeUp() {
  tft.fillCircle(tft.width()-40, 215, 20, WHITE);
  tft.fillRect (tft.width()-50, 212,20,6,BLACK);
  tft.fillRect (tft.width()-43, 205,6,20,BLACK);
}

void displaySongName(int song){
  tft.fillRect (0, tft.height()-30,tft.width(),30,BLACK);
  tft.setCursor(10,tft.height()-20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Song #");
  tft.print(song);
}
void displayVolume(){
  tft.fillRect (0, tft.height()-30,tft.width(),30,BLACK);
  tft.setCursor(10,tft.height()-20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Volume : ");
  tft.print(myDFPlayer.readVolume());
}

void displayError(String error){
  tft.fillRect (0, tft.height()-30,tft.width(),30,BLACK);
  tft.setCursor(10,tft.height()-20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(error);
}

void drawVolume(int x) {
  tft.fillRect (75, 210, 90, 10,WHITE);
  tft.fillRect (75, 210, 3*x, 10,VOLCOLOR);
}
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      displayError("Time Out!");
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      displayError("Stack Wrong!");
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      displayError("Card Inserted!");
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      displayError("Card Removed!");
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      displayError("Card Online!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      displayError("Song Finished!");
      drawPlayButton();
      playStatus = '1';
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
