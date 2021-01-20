#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>
#include <Wire.h>

TickType_t getTick;

// Light Sensor
#define lightSensorInput A2
#define analogOutputLED 8

// Joy Stick
#define analogSignalX A0
#define analogSignalY A1
#define joyStickPullUp 7
int joyStickX = 0;
int joyStickY = 0;

// LCD
const int rs = 40, en = 41, d4 = 42, d5 = 43, d6 = 44, d7 = 45;

// Line tracking Sensor sensor (LEFT)
#define L1s0 30
#define L1s1 28
#define L1s2 24
#define L1s3 26
#define L1out 22

// Line tracking Sensor sensor (RIGHT)
#define L2s0 31
#define L2s1 29
#define L2s2 25
#define L2s3 27
#define L2out 23

#define lineDetectLED 47

// Back Rear 
#define enAB 12

// Left
#define inA1 32
#define inA2 33

// Right
#define inB3 34
#define inB4 35

// Front Rear
#define enCD 11

// Left 
#define inC1 36
#define inC2 37

// Right
#define inD3 38
#define inD4 39

int RedL1=0, BlueL1=0, GreenL1=0, RedL2=0, BlueL2=0, GreenL2=0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
TaskHandle_t IdleTasks_Handler;
TaskHandle_t LineDetection_Handler;

void setup() {
// Line tracking Sensor sensor (LEFT)
  pinMode(L1s0,OUTPUT);    
  pinMode(L1s1,OUTPUT);
  pinMode(L1s2,OUTPUT);
  pinMode(L1s3,OUTPUT);
  pinMode(L1out,INPUT);

// Line tracking Sensor sensor (RIGHT)
  pinMode(L2s0,OUTPUT);    
  pinMode(L2s1,OUTPUT);
  pinMode(L2s2,OUTPUT);
  pinMode(L2s3,OUTPUT);
  pinMode(L2out,INPUT);

// Back Rear
  pinMode(enAB, OUTPUT);
  // Left
  pinMode(inA1, OUTPUT);
  pinMode(inA2, OUTPUT);
  // Right
  pinMode(inB3, OUTPUT);
  pinMode(inB4, OUTPUT);
 
 // Front Rear
  pinMode(enCD, OUTPUT);
  // Left
  pinMode(inC1, OUTPUT);
  pinMode(inC2, OUTPUT);
  // Right
  pinMode(inD3, OUTPUT);
  pinMode(inD4, OUTPUT);

  analogWrite(enAB, 70); // Send PWM signal to motor A (Speed)
  analogWrite(enCD, 70); // Send PWM signal to motor C (Speed)
  
// Line tracking Sensor sensor (LEFT)
  digitalWrite(L1s0,HIGH); //Putting S0/S1 on HIGH/HIGH levels means the output frequency scalling is at 100% (recommended)
  digitalWrite(L1s1,HIGH); //LOW/LOW is off HIGH/LOW is 20% and LOW/HIGH is  2%
// Line tracking Sensor sensor ( RIGHT)  
  digitalWrite(L2s0,HIGH); //Putting S0/S1 on HIGH/HIGH levels means the output frequency scalling is at 100% (recommended)
  digitalWrite(L2s1,HIGH); //LOW/LOW is off HIGH/LOW is 20% and LOW/HIGH is  2%

  pinMode(lineDetectLED,OUTPUT);
  digitalWrite(lineDetectLED,LOW);
  
  // Light Sensor
  pinMode(analogOutputLED,OUTPUT);
  pinMode(lightSensorInput,INPUT);
  
  // Joy Stick
  pinMode(analogSignalX, INPUT);
  pinMode(analogSignalY, INPUT);
  pinMode(joyStickPullUp, INPUT_PULLUP);

  // LCD
  lcd.begin(16, 2);
  lcd.print("Ninja Tutrtles");
  lcd.setCursor(0, 1);

  xTaskCreate(LineDetection, "Line Tracking", 100, NULL,5, &LineDetection_Handler);  
  xTaskCreate(IdleTasks, "Light Sensor", 100, NULL,0, &IdleTasks_Handler);
  
  Serial.begin(9600);
}

void loop() {}

void IdleTasks(void *param){
  (void) param;
  while(1){
    Serial.print("Task Starts \n");
    LightIntensity();
    JoyStickLCD();
    getTick = xTaskGetTickCount();
    xTaskDelayUntil(&getTick,1000 / portTICK_PERIOD_MS);
  }
}

void LineDetection(void *param){
  (void) param;
  while(1){
    LeftSensorLineDetector();
    RightSensorLineDetector();
    if (RedL1 >= 15 && RedL1 <= 60 && GreenL1 >= 30 && GreenL1 <= 55 && BlueL1 >= 13 && BlueL1 <= 45){
      analogWrite(enAB, 50);
      analogWrite(enAB, 50);
      // Dircetion of A
      digitalWrite(inA1, LOW);
      digitalWrite(inA2, HIGH);
      // Dircetion of B
      digitalWrite(inB3, LOW);
      digitalWrite(inB4, HIGH);
      // Dircetion of C
      digitalWrite(inC1, HIGH);
      digitalWrite(inC2, LOW);
      // Dircetion of D
      digitalWrite(inD3, HIGH);
      digitalWrite(inD4, LOW);
       digitalWrite(lineDetectLED,HIGH);
      Serial.print("1 \n");     
    
    }
    
    else if (RedL2 >= 15 && RedL2 <= 60 && GreenL2 >= 30 && GreenL2 <= 55 && BlueL2 >= 13 && BlueL2 <= 45){
      // Dircetion of A
      digitalWrite(inA1, HIGH);
      digitalWrite(inA2, LOW);
      // Dircetion of B
      digitalWrite(inB3, HIGH);
      digitalWrite(inB4, LOW);
      // Dircetion of C
      digitalWrite(inC1, LOW);
      digitalWrite(inC2, HIGH);
      // Dircetion of D
      digitalWrite(inD3, LOW);
      digitalWrite(inD4, HIGH);
       digitalWrite(lineDetectLED,HIGH);
      Serial.print("2 \n");
    }
    else {
      // Dircetion of A
      digitalWrite(inA1, HIGH);
      digitalWrite(inA2, LOW);
      // Dircetion of B
      digitalWrite(inB3, LOW);
      digitalWrite(inB4, HIGH);
      // Dircetion of C
      digitalWrite(inC1, LOW);
      digitalWrite(inC2, HIGH);
      // Dircetion of D
      digitalWrite(inD3, HIGH);
      digitalWrite(inD4, LOW);
       digitalWrite(lineDetectLED,LOW);
      Serial.print("3 \n");
    }
    getTick = xTaskGetTickCount();
    xTaskDelayUntil(&getTick,100 / portTICK_PERIOD_MS);
  }
}

void LightIntensity(){
  int analogValue = analogRead(lightSensorInput);
  Serial.print(analogValue);
  Serial.print("\n");
    if (analogValue < 400) {
      analogWrite(analogOutputLED,50);
    } 
    else if (analogValue < 800) {
      analogWrite(analogOutputLED,125);
    } 
    else {
      analogWrite(analogOutputLED,255);
    }
}

void JoyStickLCD(){
    joyStickX = analogRead(A0); // read X axis value [0..1023]
    joyStickY = analogRead(A1); // read Y axis value [0..1023]
    
    lcd.setCursor(0, 1);
    if ( joyStickX >= 900){
      lcd.print("State : D");  
      Serial.print("D \n");
    }
    else if (joyStickX <= 100){
      lcd.print("State : R");
      Serial.print("R \n");
    }
    else if (joyStickY >= 900){
      lcd.print("State : N");
      Serial.print("N \n");
    }
    else if (joyStickY <= 100){
      lcd.print("State : P");
      Serial.print("P \n");
    }
    else {
      lcd.print("State :    ");
      Serial.print("idle \n");
    }
}

void LeftSensorLineDetector(){
  digitalWrite(L1s2, LOW);                                            
  digitalWrite(L1s3, LOW);                                           
  RedL1 = pulseIn(L1out, digitalRead(L1out) == HIGH ? LOW : HIGH);  
  digitalWrite(L1s3, HIGH);                                         
  BlueL1 = pulseIn(L1out, digitalRead(L1out) == HIGH ? LOW : HIGH);
  digitalWrite(L1s2, HIGH);  
  GreenL1 = pulseIn(L1out, digitalRead(L1out) == HIGH ? LOW : HIGH);
}

void RightSensorLineDetector(){
  digitalWrite(L2s2, LOW); 
  digitalWrite(L2s3, LOW);                                           
  RedL2 = pulseIn(L2out, digitalRead(L2out) == HIGH ? LOW : HIGH);         
  digitalWrite(L2s3, HIGH);                                         
  BlueL2 = pulseIn(L2out, digitalRead(L2out) == HIGH ? LOW : HIGH); 
  digitalWrite(L2s2, HIGH);  
  GreenL2 = pulseIn(L2out, digitalRead(L2out) == HIGH ? LOW : HIGH);
}
