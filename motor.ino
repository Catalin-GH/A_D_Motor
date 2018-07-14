#include <LiquidCrystal.h>
#include <DFR_LCD_Keypad.h>

#define trigPin 13
#define echoPin 12
#define motorPin 11
#define speedStep 25
#define MAX_SPEED 250

const size_t distanceMin = 10;
const size_t distanceMax = 25;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
DFR_LCD_Keypad keypad(A0, &lcd);

bool selectionModes;    //false - manual; true - automatic;
size_t motorSpeed;
int lastKey;
int key;

void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motorPin, OUTPUT);

  lcd.begin(16, 2);
  selectionModes = true;
  DisplayMode((selectionModes) ? "AUTOMATIC" : "MANUAL");
  delay(2000);
  lcd.clear();
  DisplayDistance(0);
  DisplaySpeed(8, 0);

  delay(1000);
}

void DisplayMode(const char * mode)
{
  lcd.setCursor(0, 0);
  lcd.print(mode);
}

void DisplaySpeed(uint8_t pos, int speedVal)
{
  lcd.setCursor(0, 1);
  lcd.print("Speed:");
  lcd.setCursor(pos, 1);
  lcd.print(speedVal);
  lcd.setCursor(10, 1);
  lcd.print('%');
}

void DisplayDistance(const int & distance)
{
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.setCursor(10, 0);
  lcd.print(distance); 
  lcd.setCursor(12, 0);
  lcd.print("cm");
}

void loop()
{
  lastKey = keypad.get_last_key();
  key = keypad.read_key();

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 30;
  uint8_t infoDistance = (distance >= distanceMin && distanceMax >= distance) ? distance : 0 ;
  
  if (key != lastKey)
    switch (key)
    {
      case KEY_UP:
        {
          selectionModes = true;
          motorSpeed = 0;
          lcd.clear();
          DisplayMode("AUTOMATIC");
          delay(2000);
          lcd.clear();
          DisplayDistance(0);
          DisplaySpeed(8, 0);
        } break;
      case KEY_DOWN:
        {
          selectionModes = false;
          motorSpeed = 0;
          lcd.clear();
          DisplayMode("MANUAL");
          DisplaySpeed(8, 0);
        } break;
      case KEY_LEFT:
        {
          if (!selectionModes)
          {
            if (motorSpeed == MAX_SPEED || motorSpeed == speedStep)
            {
              lcd.clear();
              DisplayMode("MANUAL");
            }
            motorSpeed -= (motorSpeed) ? speedStep : 0;
            if (motorSpeed <= MAX_SPEED - speedStep)
            {
              DisplaySpeed(8, (motorSpeed / speedStep) * 10);
            }
          }
        } break;
      case KEY_RIGHT:
        {
          if (!selectionModes)
          {
            motorSpeed += (motorSpeed < MAX_SPEED) ? speedStep : 0;
            if (motorSpeed <= MAX_SPEED - speedStep)
            {
              DisplaySpeed(8, (motorSpeed / speedStep) * 10);
            }
            else
            {
              DisplaySpeed(7, (motorSpeed / speedStep) * 10);
            }
          }
        } break;
    }

  if (selectionModes)
  {
    static bool bBlinkDisplay = true;
    DisplayDistance(infoDistance);
    if (distance >= distanceMin && distance <= distanceMax )
    {
      bBlinkDisplay = true;
      motorSpeed = (float(distance - distanceMin) / (distanceMax - distanceMin)) * MAX_SPEED;

      int motorSpeedPerc = motorSpeed / speedStep * 10;
      if (motorSpeedPerc < 100)
      {
        lcd.setCursor(7, 1);
        lcd.print(' ');
        DisplaySpeed(8, motorSpeedPerc);
      }
      else
      {
        DisplaySpeed(7, motorSpeedPerc);
      }
      analogWrite(motorPin, motorSpeed);
    }
    else
    {
      analogWrite(motorPin, LOW);
      if (bBlinkDisplay)
      {
        lcd.setCursor(10, 0);
        lcd.print("  ");
        lcd.setCursor(7, 1);
        lcd.print("   ");
        DisplaySpeed(8, 0);
        bBlinkDisplay = false;
      }
    }
  }
  else
  {
    analogWrite(motorPin, motorSpeed);
  }

  delay(50);
}
