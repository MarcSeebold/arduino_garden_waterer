#include <DS3231.h>

#define DEBUG 1
int relay = 13;

// Init the DS3231 using the hardware interface
// PIN 4 -> SDA (D)
// PIN 5 -> SCL (C)
DS3231 Clock;
boolean PM, h12;

// --------------------------------------------------------------------------------------------
// ------------------------------------------ Setup -------------------------------------------
// --------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  setup_pins();
  setup_rtc();
#ifdef DEBUG
  Serial.begin(115200);
#endif
}

void setup_pins()
{
  pinMode(relay, OUTPUT);
  switch_relay_state(false);
}

void setup_rtc()
{
  Wire.begin(); // Initialize I2C
  Clock.setClockMode(false); // set to 24h
  // Set default time (we don't care about the date)
  Clock.setHour(03);
  Clock.setMinute(04);
  Clock.setSecond(05);
}

// --------------------------------------------------------------------------------------------
// ---------------------------------------- Main logic ----------------------------------------
// --------------------------------------------------------------------------------------------

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println(Clock.getHour(h12, PM), DEC);
  Serial.println(Clock.getMinute(), DEC);
  Serial.println(Clock.getSecond(), DEC);
  Serial.println("----------------");
}

void switch_relay_state(boolean _on)
{
  if (_on)
  {
    digitalWrite(relay, LOW);
  }
  else
  {
    digitalWrite(relay, HIGH);
  }
}

