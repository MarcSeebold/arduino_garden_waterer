/*TODOs
 * 
 *
 * 
 */



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
int battery_keep_awake1 = 7;
int battery_keep_awake2 = 8;
Thread gBatteryAwakeTimer1;
Thread gBatteryAwakeTimer2;
// Init the DS3231 using the hardware interface
// PIN 4 -> SDA (D)
// PIN 5 -> SCL (C)
DS3231 gClock;
boolean gPM, gh12;
// --------------------------------------------------------------------------------------------
// ------------------------------------------ Setup -------------------------------------------
// --------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  setup_pins();
  //setup_rtc();
  setup_battery_awake();
#ifdef DEBUG
  Serial.begin(115200);
#endif
}

void setup_battery_awake()
{
  // Module 1
  gBatteryAwakeTimer1 = Thread();
  gBatteryAwakeTimer1.setInterval(3000);
  gBatteryAwakeTimer1.enabled = true;
  gBatteryAwakeTimer1.onRun(keep_battery_awake1);
  // Module 2
  gBatteryAwakeTimer2 = Thread();
  gBatteryAwakeTimer2.setInterval(3000);
  gBatteryAwakeTimer2.enabled = true;
  gBatteryAwakeTimer2._cached_next_run = 1500;
  gBatteryAwakeTimer2.onRun(keep_battery_awake2);
}

void setup_pins()
{
  pinMode(relay, OUTPUT);
  pinMode(battery_keep_awake1, OUTPUT);
  pinMode(battery_keep_awake2, OUTPUT);
  digitalWrite(battery_keep_awake1, LOW);
  digitalWrite(battery_keep_awake2, LOW);
  switch_relay_state(false);
}

void setup_rtc()
{
  Serial.println(".");
  Wire.begin(); // Initialize I2C
  Serial.println("a");
  gClock.setClockMode(false); // set to 24h
  Serial.println("b");
  // Set default time (we don't care about the date)
  gClock.setHour(03);
  
  gClock.setMinute(04);
  gClock.setSecond(05);
  Serial.println("c");
}
// --------------------------------------------------------------------------------------------
// ---------------------------------------- Main logic ----------------------------------------
// --------------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  //Serial.println(gClock.getHour(gh12, gPM), DEC);
  //Serial.println(gClock.getMinute(), DEC);
  //Serial.println(gClock.getSecond(), DEC);
  //Serial.println("----------------");

  // First check if our Thread "should" be runned
  if(gBatteryAwakeTimer1.shouldRun()){
  // Yes, the Thread should be runned, let's run it
    gBatteryAwakeTimer1.run();
  }
  // First check if our Thread "should" be runned
  if(gBatteryAwakeTimer2.shouldRun()){
  // Yes, the Thread should be runned, let's run it
    gBatteryAwakeTimer2.run();
  }
  //delay(5000);
  //switch_relay_state(true);
  //delay(2000);
  //switch_relay_state(false);
}

void keep_battery_awake1()
{
  Serial.println("ping1");
  digitalWrite(battery_keep_awake1, HIGH);
  delay(300);
  digitalWrite(battery_keep_awake1, LOW);
}

void keep_battery_awake2()
{
  Serial.println("ping2");
  digitalWrite(battery_keep_awake2, HIGH);
  delay(500);
  digitalWrite(battery_keep_awake2, LOW);
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


