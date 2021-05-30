#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>


char auth[] = "tP0PSVx_xCPM6SCXVaE8wCaqHKxlcfIR";
char ssid[] = "netwerk";
char pass[] = "password";

BlynkTimer timer;

// servo's
Servo stuur;
Servo rijden;


// leds
const int groen_led = 25;
const int geel_led = 4;
const int rood_led = 19 ;
const int led_balk = 32;

// pinnen motors
const int sturen = 12;
const int vermogen = 13;

//snelheid
//vooruit
const int forward_max_speed = 150;
const int forward_min_speed = 101;
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

//IR sensor
const int IR_midden = 33;
int sensorValue = 0;
const int IR_links = 14;
int sensorValue1 = 0;
const int IR_rechts = 27;
int sensorValue2 = 0;

// ultrasonic sensor
const int trigPin = 25;
const int echoPin = 26;
int distance;
long duration;

//milles timer
int period = 20000;
unsigned long tijd = 0;
static int sec1 = 0;
static int sec10 = 0;

int buttonState;

void Servo_Init()
{
  // dit is voor de wielen recht te zetten bij het opstarten
  stuur.write(midden);
  rijden.write(uit);
}

void Stop()
{
  //het stoppen van de auto door de snelheid op nul te zetten
  rijden.write(back_uit);
}

// achteruit wordt nornmaal niet gebeukt maar het is wel mogelijk
void Back(int speed)
{
  speed = constrain(speed, back_min_speed, back_max_speed);
  rijden.write(back_max_speed);
}

//vooruit
void Forward(int speed)
{
  speed = constrain(speed, forward_min_speed, forward_max_speed);
  rijden.write(forward_min_speed);
}


//links draaien
void Turn_Left()
{
  stuur.write(links);
}

//rechts draaien
void  Turn_Right()
{
  stuur.write(rechts);
}

//wielen midden
void Center()
{
  stuur.write(midden);
}

//groene licht alleen
void groen_licht()
{
  digitalWrite(led_balk, LOW);
  digitalWrite(rood_led, LOW);
  digitalWrite(geel_led, LOW);
  digitalWrite(groen_led, HIGH);
}

//geel licht alleen
void geel_licht()
{
  digitalWrite(led_balk, LOW);
  digitalWrite(rood_led, LOW);
  digitalWrite(geel_led, HIGH);
  digitalWrite(groen_led, LOW);
}

//roodlicht alleen
void rood_licht()
{
  digitalWrite(led_balk, LOW);
  digitalWrite(rood_led, HIGH);
  digitalWrite(geel_led, LOW);
  digitalWrite(groen_led, LOW);
}

//led balk vanachter
void LED_licht()
{
  digitalWrite(led_balk, HIGH);
  digitalWrite(rood_led, LOW);
  digitalWrite(geel_led, LOW);
  digitalWrite(groen_led, LOW);
}

//geel led + Led balk
void object_licht()
{
  digitalWrite(led_balk, HIGH);
  digitalWrite(rood_led, LOW);
  digitalWrite(geel_led, HIGH);
  digitalWrite(groen_led, LOW);
}

void Blynk_groen()
{
  WidgetLED led_balk (V4);
  led_balk.off();
  WidgetLED rood_led (V3);
  rood_led.off();
  WidgetLED geel_led (V2);
  geel_led.off();
  WidgetLED groen_led (V1);
  groen_led.on();
}

void Blynk_geel()
{
  WidgetLED led_balk (V4);
  led_balk.off();
  WidgetLED rood_led (V3);
  rood_led.off();
  WidgetLED geel_led (V2);
  geel_led.on();
  WidgetLED groen_led (V1);
  groen_led.off();
}

void Blynk_rood()
{
  WidgetLED led_balk (V4);
  led_balk.off();
  WidgetLED rood_led (V3);
  rood_led.on();
  WidgetLED geel_led (V2);
  geel_led.off();
  WidgetLED groen_led (V1);
  groen_led.off();
}

void Blynk_balk()
{
  WidgetLED led_balk (V4);
  led_balk.on();
  WidgetLED rood_led (V3);
  rood_led.off();
  WidgetLED geel_led (V2);
  geel_led.off();
  WidgetLED groen_led (V1);
  groen_led.off();
}

void Blynk_object()
{
  WidgetLED led_balk (V4);
  led_balk.on();
  WidgetLED rood_led (V3);
  rood_led.off();
  WidgetLED geel_led (V2);
  geel_led.on();
  WidgetLED groen_led (V1);
  groen_led.off();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

  stuur.setPeriodHertz(50);
  stuur.attach(sturen, 500, 2400);
  rijden.setPeriodHertz(50);
  rijden.attach(vermogen, 500, 2400);

  Servo_Init();

  pinMode(sturen, OUTPUT);
  pinMode(vermogen, OUTPUT);
  pinMode(led_balk, OUTPUT);
  pinMode(IR_midden, INPUT);
  pinMode(IR_links, INPUT);
  pinMode(IR_rechts, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(rood_led, OUTPUT);
  pinMode(geel_led, OUTPUT);
  pinMode(groen_led, OUTPUT);

  timer.setInterval(1000L, loop);
}

BLYNK_WRITE(V0)
{
  buttonState = param.asInt();
}

void loop() {
  // put your main code here, to run repeatedly:

  Blynk.run();
  timer.run();

  // de snelheid voor in de voids
  int speed = uit;

  //switch case
  int stap = 0;

  //clear command voor de ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //afstand meeting ultrasonic sensor
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  //printen van afstand in cm
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");

  // IR sensoren digitaal
  sensorValue = digitalRead (IR_midden);
  sensorValue1 = digitalRead (IR_links);
  sensorValue2 = digitalRead (IR_rechts);

  if (distance <= 40)
  {
    stap = 6;
  }
  else
  {

    if (sensorValue1 == 1 && sensorValue2 == 1)
    {
      stap = 5;
      Stop();
      tijd = millis();
      if (tijd >= 1000) {
        sec1 = sec1 + 1;
        tijd = 0;
      }
      if (sec1 == 10) {
        sec10 = sec10 + 1 ;
        sec1 = 0;
      }
      Serial.print(sec10);
      Serial.println(sec1);

      if ( sec10 == 50)
      {
        stap = 1;
        sec1 = 0;
        sec10 = 0;
      }

      if (buttonState == 1)
      {
        Forward(speed);
        groen_licht();
        Blynk_groen();
        Serial.println("vooruit");
      }
    }
    else
    {

      if (sensorValue == 1)
      {
        Forward(speed);
        groen_licht();
        Blynk_groen();
        Serial.println("vooruit");

        if (sensorValue1 == 1 && sensorValue2 == 0)
        {
          stap = 3;
        }
        else if (sensorValue1 == 0 && sensorValue2 == 1)
        {
          stap = 2;
        }
        else if (sensorValue1 == 0 && sensorValue2 == 0)
        {
          stap = 4;
        }
      }
      else
      {
        stap = 8;
      }
    }
  }

  switch (stap) {
    case 1:
      {
        Forward(speed);
        groen_licht();
        Blynk_groen();
        Serial.println("vooruit");
        break;
      }
    case 2:
      {
        Forward(speed);
        groen_licht();
        Blynk_groen();
        Turn_Left();
        Serial.println("links");
        break;
      }
    case 3:
      {
        Forward(speed);
        groen_licht();
        Blynk_groen();
        Turn_Right();
        Serial.println("rechts");
        break;
      }
    case 4:
      {
        Center();
        groen_licht();
        Blynk_groen();
        Serial.println("midden");
        break;
      }
    case 5:
      {
        geel_licht();
        Blynk_geel();
        Serial.println("zwarte lijn");
        break;
      }
    case 6:
      {
        Stop();
        object_licht();
        Blynk_object();
        Serial.println("auto in de weg");
        break;
      }
    case 7:
      {
        Back(speed);
        groen_licht();
        Blynk_groen();
        Serial.print("achteruit");
        break;
      }
    case 8:
      {
        Stop();
        rood_licht();
        Blynk_rood  ();
        Serial.println("lijn kwijt");
        break;
      }
  }
}
