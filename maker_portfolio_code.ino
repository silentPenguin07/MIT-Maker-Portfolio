#include "pitches.h"
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

/**
Code for maker portfolio 2024
**/

RTC_DS3231 rtc;
const int speakerPin = 10;
const int buttonPin = 5;
const int ledPin = 4;
LiquidCrystal_I2C lcd(0x27, 16, 2);

boolean pending = false; // keeps track of whether there is an activity undone

const int melody[] = { // notes put in sequential order
  NOTE_G3, NOTE_E3, NOTE_C3, NOTE_D3, NOTE_G2
};


const int noteLength[] = { // quarter note, eighth note, etc...
  4, 4, 8, 4, 4
};

struct ScheduledEvent 
{
  int hour;
  int minute;
  String message;
  int seconds;
};

// method to schedule new events
ScheduledEvent newEvent(int hr, int min, String msg)
{
  ScheduledEvent scheduled = {hr, min, msg, 0};
  return scheduled;
}

void playSound()
{
  for (int thisNote = 0; thisNote < 5; thisNote++) 
  {
    
    int noteDuration = 1000 / noteLength[thisNote];
    tone(speakerPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speakerPin);
  }
}

struct ScheduledEvent events_arr[3]; // array of events to be scheduled

// displays a string on the LCD
void displayMessage(String msg)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
}

void setup() 
{
  Serial.begin(9600);

  // pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // setting up LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  displayMessage("No Activities");

  // creating the events
  ScheduledEvent brushTeeth = newEvent(17, 32, "Brush your teeth!");
  events_arr[0] = brushTeeth;
  ScheduledEvent sleep = newEvent(15, 0, "Bedtime!");
  events_arr[1] = sleep;
  ScheduledEvent breakfast = newEvent(8, 30, "Breakfast!");
  events_arr[2] = breakfast;

  // initializing the rtc
  if(!rtc.begin()) 
  {
      Serial.println("RTC not found.");
      Serial.flush();
      while (1) delay(10);
  }

  // sync date and time with the device
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // disable unused 32K pin
  rtc.disable32K();

  // set light to default off
  digitalWrite(ledPin, LOW);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();

  // cycle through each event to see which is active
  for (int i = 0; i < 2; i++)
  {
    if (now.hour() == events_arr[i].hour && now.minute() == events_arr[i].minute && now.second() == events_arr[i].seconds)
    {
      Serial.println("Event triggered!");
      displayMessage(events_arr[i].message);
      playSound();
      digitalWrite(ledPin, HIGH);
      pending = true;
    }
  }

  // button press should mark an event as complete
  if (digitalRead(buttonPin) == LOW && pending)
  {
    digitalWrite(ledPin, LOW);
    pending = false;
    displayMessage("No Activities");
  }


}
