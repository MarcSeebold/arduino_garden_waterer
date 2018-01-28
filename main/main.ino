/*TODOs
 * 
 *
 * 
 */

// External gClock
#include <DS3231.h>

// --------------------------------------------------------------------------------------------
// ----------------------------------------- Globals ------------------------------------------
// --------------------------------------------------------------------------------------------
#define DEBUG 1
// Init the DS3231 using the hardware interface
// PIN A4 -> SDA (D)
// PIN A5 -> SCL (C)
DS3231 gClock;
boolean gh12, gPM;
// --------------------------------------------------------------------------------------------
// ------------------------------------------ Setup -------------------------------------------
// --------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  setup_pins();
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Debug enabled.");
#endif
}

void setup_pins()
{
  Serial.println("Setting up pins");
  Serial.println("Done: Setting up pins");
}

void setup_rtc()
{
  Serial.println("Setting up rtc");
  Wire.begin(); // Initialize I2C
  gClock.setClockMode(false); // set to 24h
  // Set default time (we don't care about the date)
  gClock.setHour(03);
  gClock.setMinute(04);
  gClock.setSecond(05);
  Serial.println("Done: Setting up rtc");
}
// --------------------------------------------------------------------------------------------
// ---------------------------------------- Main logic ----------------------------------------
// --------------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println("----------------");
  Serial.println(gClock.getHour(gh12, gPM), DEC);
  Serial.println(gClock.getMinute(), DEC);
  Serial.println(gClock.getSecond(), DEC);
  Serial.println("----------------");
}


