#include <ESP32Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial btSerial;

#define BT_NAME "ESP32"

const int led_balk = 12;

// pinnen motors
const int sturen = 5;
const int vermogen = 14;

// servo's
Servo stuur;
Servo rijden;

//snelheid
//vooruit
const int forward_max_speed = 150;
const int forward_min_speed = 99;
const int uit = 98;

//snellheid
//achteruit
const int back_max_speed = 65;
const int back_min_speed = 1;
const int back_uit = 0;

//noodrem
const int noodrem = 180;

//draaicrikel
const int links = 75;
const int rechts = 146;
const int midden = 108 ;

boolean btConnected = false;
char key, previousKey;
long previousMillis = 0;
int timeout = 1000;

void Servo_Init()
{
  stuur.attach(sturen);
  rijden.attach(vermogen);

  // dit is voor de wielen recht te zetten bij het opstarten
  stuur.write(midden);
  rijden.write(uit);
}

void Stop()
{
  rijden.attach(vermogen);
  stuur.attach(sturen);

  //het stoppen van de auto door de snelheid op nul te zetten
  rijden.write(back_uit);

  stuur.write(midden);
}

void Back(int speed)
{
  rijden.attach(vermogen);
  speed = constrain(speed, back_min_speed, back_max_speed);
  rijden.write(back_max_speed);
}

void Forward(int speed)
{
  rijden.attach(vermogen);
  speed = constrain(speed, forward_min_speed, forward_max_speed);
  rijden.write(forward_max_speed);
  digitalWrite(led_balk, HIGH);   
}

void Turn_Left()
{
  stuur.attach(sturen);
  stuur.write(links);
}

void  Turn_Right()
{
  stuur.attach(sturen);
  stuur.write(rechts);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Servo_Init();

  pinMode(sturen, OUTPUT);
  pinMode(vermogen, OUTPUT);
  pinMode(led_balk, OUTPUT);

  btSerial.begin(BT_NAME);

  Serial.println("ESP32 Bluetooth RC car");
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  char _read;
  int speed = back_uit;

  if (btSerial.available())
  {
    Serial.println("Bluetooth connected.");
    _read = btSerial.read();


    if (_read >= '0' && _read <= '9') {
      if (_read == '0')
        speed = uit;
      else
        speed += (int(_read) - 48) * 10;
    }

    if (_read == 'q')
      speed = forward_max_speed;

    Serial.println(_read);

    switch (_read) {
      case 'S': {
          Stop();
          Serial.println("STOP");
          break;
        }
      case 'F': {
          Forward(speed);
          Serial.println("Forward");
          break;
        }
      case 'B': {
          Back(speed);
          Serial.println("BACK");
          break;
        }
      case 'L': {
          Turn_Left();
          Serial.println("LEFT");
          break;
        }
      case 'R': {
          Turn_Right();
          Serial.println("RIGHT");
          break;
        }
      case 'G': {
          Turn_Left();
          delay(10);
          Forward(speed);
          Serial.println("FL");
          break;
        }
      case 'I': {
          Turn_Right();
          delay(10);
          Forward(speed);
          Serial.println("FR");
          break;
        }
      case 'H': {
          Turn_Left();
          delay(10);
          Back(speed);
          Serial.println("BL");
          break;
        }
      case 'J': {
          Turn_Right();
          delay(10);
          Back(speed);
          Serial.println("BR");
          break;
        }
    }
    delay(100);
  }
}
