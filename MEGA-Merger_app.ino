#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#define DETECT 2  //zero cross detect
#define GATE 13    //TRIAC gate
#define PULSE 4   //trigger pulse width (counts)
void fanctrl(int a);
void zeroCrossingInterrupt();
//int i=300;
/////////////////////////////////////////////////////////////////////
int tempfun();
void settemp();
void tilt();
void gas_leak();
int tilt_val = 0, gas_val = 0;
int counter = 0;
int temp = 35;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tf;
int  Tc;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int s;
////////////////////////////////////////////////////////////////////
int a1;
int bl;
void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  while (digitalRead(26) == 0);
  pinMode(DETECT, INPUT);     //zero cross detect
  digitalWrite(DETECT, HIGH); //enable pull-up resistor
  pinMode(GATE, OUTPUT);      //TRIAC gate control
  OCR1A = 100;      //initialize the comparator
  TIMSK1 = 0x03;    //enable comparator A and overflow interrupts
  TCCR1A = 0x00;    //timer control registers set for
  TCCR1B = 0x00;    //normal operation, timer disabled
  attachInterrupt(0, zeroCrossingInterrupt, RISING);
  /////////////////////////////////////////////////////
  lcd.begin(16, 2);
  pinMode(22, INPUT);
  pinMode(24, INPUT);
  pinMode(26, INPUT);
  ////////////////////////////////////////////////////
  pinMode(32, OUTPUT); //ignitorrelay
  pinMode(30, OUTPUT); //gasassembly
  pinMode(35, INPUT); //tilt
  pinMode(38, OUTPUT); //tilt

  fanctrl(380);
  analogWrite(10, 255);
  digitalWrite(30, HIGH);
  digitalWrite(32, HIGH);
  delay(3000);
  digitalWrite(32, LOW);
  digitalWrite(38, LOW);
  //digitalWrite(30,LOW);
}

void loop() {
    tilt();
  gas_leak();
  if (tilt_val == 1)
  {
    digitalWrite(30, LOW); //GAS_shutdown
    analogWrite(10, 0); //modulating_valve shutdown
    fanctrl(600);//fan_shutdown
    lcd.setCursor(0,0);
    lcd.print("Tilted ");
    lcd.setCursor(0,1);
    lcd.print("       ");
    while (1);
  }
    if (gas_val >= 450)
  {
    analogWrite(10, 0); //modulating_valve shutdown
    fanctrl(600);//fan_shutdown
    digitalWrite(38,HIGH);
    lcd.setCursor(0,0);
    lcd.print("Gas Leak");
    lcd.setCursor(0,1);
    lcd.print("       ");
    while (1);
  }
  //fanctrl(350);
  if (digitalRead(26) >= 1)
  {
    delay(300);
    counter++;
  }
  if (counter >= 1)
  {
    while (Serial2.available())
    {
      delay(200);
      s = Serial2.read();
      Serial.write(s);
    }
  }
  if (s == 40)
  {
    temp = 40;
  }
  if (s == 22)
  {
    temp = 22;
  }
  a1 = tempfun();
  settemp();
  //Serial.println("loop");
  if (a1 <= temp)
  {
    //Serial.println("equal1");
    fanctrl(350);
    analogWrite(10, 100);
    //Serial.println("equal");
  }
  else if (a1 > temp)
  {
    //Serial.println("less1");
    fanctrl(450);
    analogWrite(10, 255);
    //  Serial.println("less");
  }
  //   else if(a1>temp)
  //  {
  //    fanctrl(400);
  //    analogWrite(10,220);
  //  }
  Serial.print("Target Temp= ");
  Serial.println(temp);
  Serial.print("Room Temp= ");
  Serial.println(a1);
  //Serial.println(s);
  Serial.println(counter);
}
void zeroCrossingInterrupt()
{ //zero cross detect
  TCCR1B = 0x04;
  TCNT1 = 0;
}

ISR(TIMER1_COMPA_vect) { //comparator match
  digitalWrite(GATE, HIGH); //set TRIAC gate to high
  TCNT1 = 65536 - PULSE;    //trigger pulse width
}

ISR(TIMER1_OVF_vect) { //timer1 overflow
  digitalWrite(GATE, LOW); //turn off TRIAC gate
  TCCR1B = 0x00;          //disable timer stopd unintended triggers
}
void fanctrl(int a)
{
  OCR1A = a;
}
////////////////////////////////////////////////////
void settemp()
{
  {
    if (digitalRead(22) == 1)
    {
      delay(300);
      temp = temp + 1;
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temp);
    }

    else if (digitalRead(24) == 1)
    {
      delay(300);
      temp = temp - 1;
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temp);
    }
    //    else if (digitalRead(26) == 0)
    //    {
    //      temp = temp;
    //      lcd.setCursor(0, 0);
    //      lcd.print("Tar Temp:");
    //      lcd.print(temp);
    //    }
    else
    {
      tempfun();
    }
  }
}
int tempfun()
{

  Vo = analogRead(1);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  Tc=Tc+7;
  Tf = (Tc * 9.0) / 5.0 + 32.0;
  //lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.print("Room:");
  lcd.print(Tc);
  return Tc;
}
void tilt()
{
  if (digitalRead(35) == HIGH)
  {
    tilt_val = 1;
  }
  else
    tilt_val = 0;
}

void gas_leak()
{
  gas_val = analogRead(2);
}
