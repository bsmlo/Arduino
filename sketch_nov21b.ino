#include <DFRobotHighTemperatureSensor.h>
#include <TM1637Display.h>
#include <DS18B20.h>
#include <OneWire.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>


//  *** Pins define ***

// Display
#define CLK 6 // Clock
#define DIO 5 // Data in

// Fan gear control relays
#define rel1 A0 // Gear 1 and 3
#define rel2 A1 // Gear 2 and 3
#define rel3 7  // Gear 4
#define rel4 8  // Manual control relay
#define rel5 10 // Power relay I 
#define rel6 11  // Power relay II 

// Temperature and humitidy sensor
#define sensor 3 //Pin
#define sensorType DHT11 //Type

#define button 9 // Manual control pin

/*
// RGB Diod
#define PIN 2
#define LICZBADIOD 1
*/


//Define Object
DHT dht(sensor, sensorType); 

// Temperature sonde
const float voltageRef = 5.000; //Reference Voltage
int HighTemperaturePin = A3; //Pin
DFRobotHighTemperature PT100 = DFRobotHighTemperature(voltageRef); //Define Object


// Display object define
TM1637Display display(CLK, DIO);

// #define ONEWIRE_PIN 4 //temp 2

// Adres czujnika 0x28, 0xFF, 0x36, 0x85, 0x23, 0x17, 0x3, 0x18
// byte address[8] = {0x28, 0xFF, 0x36, 0x85, 0x23, 0x17, 0x3, 0x18};


// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LICZBADIOD, PIN, NEO_GRB + NEO_KHZ800);

// OneWire onewire(ONEWIRE_PIN);
// DS18B20 sensors(&onewire);


//  *** Variables define ***

// Table for average temperature
float tempTable[10] = {20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0};
float aveTemp = 0;

// Actual mode of relays 
int actualMode = 0;

// Current temperature and humitidy from DHT11
int curHumidity = 0;
int curTemp = 0;

// High temperature value
float temp2 = 0.0;

// Manual control
int manCtr = 0;

// Control relays gear and histeresis
int d1 = 0;
int d2 = 0;
int d3 = 0;
int d4 = 0;

// Temperature histeresis value
int histeresisValue = 15;

// Display letters matrixs
// Display M
uint8_t M[]={
  SEG_B|SEG_F|SEG_A|SEG_G|SEG_E,
  SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,
  SEG_F|SEG_G|SEG_E|SEG_D,
  SEG_D,};

// Display T
uint8_t T[]={
  SEG_F|SEG_G|SEG_E|SEG_D,
  SEG_B|SEG_F|SEG_A|SEG_G|SEG_E,
  SEG_D,
  SEG_D,};

// Display Ti
uint8_t Ti[]={
  SEG_F|SEG_G|SEG_E|SEG_D,
  SEG_B|SEG_C,
  SEG_D,
  SEG_D,};

// Display H
uint8_t H[]={
  SEG_F|SEG_E|SEG_G|SEG_C|SEG_B,
  SEG_D,
  SEG_D,
  SEG_D,};

// Display G
uint8_t G[]={
  SEG_A|SEG_F|SEG_E|SEG_D|SEG_C,
  SEG_D,
  SEG_D,
  SEG_D,}; 


// *** Setup 
void setup() 
{
  Serial.begin(9600);
  pinMode(13, OUTPUT); // Board LED
  pinMode(rel1, OUTPUT); // Relay 1
  pinMode(rel2, OUTPUT); // Relay 2
  pinMode(rel3, OUTPUT); // Relay 3
  pinMode(rel4, OUTPUT); // Relay 4
  pinMode(rel5, OUTPUT); // Relay 5
  pinMode(rel6, OUTPUT); // Relay 6
  pinMode(button, INPUT);
  display.setBrightness(0x0A);
  dht.begin();
  
/*
  sensors.begin();
  sensors.request(address);
  //  pixels.begin();
  //pinMode(A3, OUTPUT);
*/

}


/*
int r = 0;
int g = 0;
int b = 0;
*/


void loop() 
{ 
  //led();  
  temp();
  averageTemp();
  readtemp2(); 
  
  manCtr = digitalRead(9);
  relay();
  
  //printer();
  onDisp2(1);
  delay(1000);
  
  manCtr = digitalRead(9);
  relay();
  
  onDisp(curTemp);
  delay(1000);
  
  manCtr = digitalRead(9);
  relay();
  
  onDisp2(2);
  delay(1000);
  
  manCtr = digitalRead(9);
  relay();
  
  onDisp(temp2);
  delay(1000);
  
  manCtr = digitalRead(9);
  relay();
  
  onDisp2(3);
  delay(1000);
  manCtr = digitalRead(9);
  onDisp(curHumidity);
  delay(1000);
  manCtr = digitalRead(9);
  onDisp2(4);
  delay(1000);
  
  manCtr = digitalRead(9);
  relay();
  
  if(manCtr == 1)
  {
    onDisp2(0);
    delay(1000);
    manCtr = digitalRead(9);
  }
  else
  {
    onDisp(actualMode);
    delay(1000);
    manCtr = digitalRead(9);
  }
  
}

int temp()
{
  curTemp = dht.readTemperature();
  curHumidity = dht.readHumidity();
}

/*
void led()
{
  digitalWrite(13, HIGH);
  delay(150);
  digitalWrite(13, LOW);
  delay(150);
}
*/

float readtemp2()
{
  temp2 = PT100.readTemperature(HighTemperaturePin);;
}


// Control relays
void relay()
{

  // Histeresis control
  if(manCtr == 0)
  {
  
      if(actualMode == 0)
      {
        d1 = 0;
        d2 = 0;
        d3 = 0;
        d4 = 0;
      }
      
      if(actualMode == 1)
      {
        d1 = histeresisValue;
        d2 = 0;
        d3 = 0;
        d4 = 0;
      }
    
        if(actualMode == 2)
      {
        d1 = 0;
        d2 = histeresisValue;
        d3 = 0;
        d4 = 0;
      }
    
        if(actualMode == 3)
      {
        d1 = 0;
        d2 = 0;
        d3 = histeresisValue;
        d4 = 0;
      }

      if(actualMode == 4)
      {
        d1 = 0;
        d2 = 0;
        d3 = 0;
        d4 = histeresisValue;
      }

      // Fans gears control
      if(temp2 < aveTemp + 25 - d1)
      {
        actualMode = 0;
        digitalWrite(rel1, HIGH); // Fans Relays
        digitalWrite(rel2, HIGH);
        
        digitalWrite(rel3, HIGH);
        digitalWrite(rel4, HIGH);

        digitalWrite(rel5, HIGH); // PWR Relays
        digitalWrite(rel6, HIGH);
      }
      
      else  if(temp2 >= aveTemp + 25 - d1 && temp2 < aveTemp + 50 - d2)
      {
        actualMode = 1;
        digitalWrite(rel1, LOW); // Fans Relays
        digitalWrite(rel2, HIGH);
        
        digitalWrite(rel3, HIGH);
        digitalWrite(rel4, HIGH);

        digitalWrite(rel5, LOW); // PWR Relays
        digitalWrite(rel6, LOW);
      }
    
      else  if(temp2 >= aveTemp + 50 - d2 && temp2 < aveTemp + 80 - d3)
      {
        actualMode = 2;
        digitalWrite(rel1, HIGH); // Fans Relays
        digitalWrite(rel2, LOW);
        
        digitalWrite(rel3, HIGH);
        digitalWrite(rel4, HIGH);

        digitalWrite(rel5, LOW); // PWR Relays
        digitalWrite(rel6, LOW);
      }

      else  if(temp2 >= aveTemp + 80 - d3 && temp2 < aveTemp + 110 - d4)
      {
        actualMode = 3;
        digitalWrite(rel1, LOW); // Fans Relays
        digitalWrite(rel2, LOW);
        
        digitalWrite(rel3, HIGH);
        digitalWrite(rel4, HIGH);

        digitalWrite(rel5, LOW); // PWR Relays
        digitalWrite(rel6, LOW);
      }
      
      else if (temp2 >= aveTemp + 110 - d4)
      {
        actualMode = 4;
        digitalWrite(rel1, HIGH); // Fans Relays
        digitalWrite(rel2, HIGH);
        
        digitalWrite(rel3, LOW);
        digitalWrite(rel4, HIGH);

        digitalWrite(rel5, LOW); // PWR Relays
        digitalWrite(rel6, LOW);
      }
  }
  else if(manCtr == 1)// manual control
  {
    digitalWrite(rel1, HIGH); // Fans Relays
    digitalWrite(rel2, HIGH);
    digitalWrite(rel3, HIGH);
    digitalWrite(rel4, LOW);

    digitalWrite(rel5, LOW); // PWR Relays
    digitalWrite(rel6, LOW);
  }
  
}


// Function calculate average temperature 
float averageTemp()
{
  float sumTemp = 0;
  
  for(int i = 1; i <10; i++)
  {
    tempTable[i-1] = tempTable[i];
  }
  tempTable[9] = curTemp;
  for(int i = 0; i <10; i++)
  {
    sumTemp = sumTemp + tempTable[i];
  }
  aveTemp = sumTemp/10.0;
}


// Display control function for values
void onDisp(int a)
{
  display.showNumberDec(a ,false, 4, 0);
}


// Display control function for informations
void onDisp2(int let)
{
  if (let == 0)
  {
   display.setSegments(M);
  } 
  if (let == 1)
  {
    display.setSegments(T);
  }
  else if (let == 2)
  {
    display.setSegments(Ti);
  }
  else if (let == 3)
  {
    display.setSegments(H);
  }
  else if (let == 4)
  {
    display.setSegments(G);
  }
  else
  {
    
  }
}



/*
void printer()
{
  Serial.print("Temp: ");
  Serial.print(curTemp);
  Serial.print(" °C / Humidity:  ");
  Serial.print(curHumidity);
  Serial.print("% ");
  Serial.print("Temp2: ");
  Serial.print(temp2);
  Serial.print(" °C   ");
  Serial.print("AVerage Temp ");
  Serial.print(aveTemp);
  Serial.print(" °C   ");
  Serial.print("Actual mode: ");
  Serial.print(actualMode);
  Serial.print("  ");
  Serial.print(d1);
  Serial.print(d2);
  Serial.print(d3);
  Serial.print(d4);
  Serial.print("  button");
  Serial.print(manCtr);
  Serial.println();     
}
*/

