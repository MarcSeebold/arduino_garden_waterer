// Timer Interrupts
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
// External gClock
#include <DS3231.h>

// --------------------------------------------------------------------------------------------
// ----------------------------------------- Globals ------------------------------------------
// --------------------------------------------------------------------------------------------
#define DEBUG 1
int relay = 13;
int battery_keep_awake = 7;
// Init the DS3231 using the hardware interface
// PIN 4 -> SDA (D)
// PIN 5 -> SCL (C)
DS3231 gClock;
boolean gPM, gh12;
Thread gBatteryAwakeTimer;
// --------------------------------------------------------------------------------------------
// ------------------------------------------ Setup -------------------------------------------
// --------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  setup_pins();
  setup_rtc();
  setup_battery_awake();
#ifdef DEBUG
  Serial.begin(115200);
#endif
}

void setup_battery_awake()
{
  gBatteryAwakeTimer = Thread();
  gBatteryAwakeTimer.setInterval(4500);
  gBatteryAwakeTimer.enabled = true;
  gBatteryAwakeTimer.onRun(keep_battery_awake);
}

void setup_pins()
{
  pinMode(relay, OUTPUT);
  pinMode(battery_keep_awake, OUTPUT);
  digitalWrite(battery_keep_awake, HIGH);
  switch_relay_state(false);
}

void setup_rtc()
{
  Wire.begin(); // Initialize I2C
  gClock.setClockMode(false); // set to 24h
  // Set default time (we don't care about the date)
  gClock.setHour(03);
  gClock.setMinute(04);
  gClock.setSecond(05);
}
// --------------------------------------------------------------------------------------------
// ---------------------------------------- Main logic ----------------------------------------
// --------------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println(gClock.getHour(gh12, gPM), DEC);
  Serial.println(gClock.getMinute(), DEC);
  Serial.println(gClock.getSecond(), DEC);
  Serial.println("----------------");

  // First check if our Thread "should" be runned
  if(gBatteryAwakeTimer.shouldRun()){
  // Yes, the Thread should be runned, let's run it
    gBatteryAwakeTimer.run();
  }
}

void keep_battery_awake()
{
  digitalWrite(battery_keep_awake, LOW);
  delay(50);
  digitalWrite(battery_keep_awake, HIGH);
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

