/*TODOs



*/

// External gClock
#include <DS3231.h>
// LCD display
#include <LiquidCrystal_I2C.h>

// --------------------------------------------------------------------------------------------
// ----------------------------------------- Globals ------------------------------------------
// --------------------------------------------------------------------------------------------
#define DEBUG 0
// Init the DS3231 using the hardware interface
// PIN A4 -> SDA (D)
// PIN A5 -> SCL (C)
DS3231 gClock;
boolean gh12, gPM;

// PINS
int pump = 7;

// LCD display
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line

// --------------------------------------------------------------------------------------------
// ------------------------------------------ Setup -------------------------------------------
// --------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  // Start the I2C interface
  Wire.begin();
  setup_pins();
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Debug enabled.");
#endif
  setup_rtc();
  setup_lcd();
}

void setup_lcd()
{
  Serial.println("Setting up LCD");
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 
  Serial.println("Done: Setting up LCD");
}

void setup_pins()
{
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH); // HIGH = off!
}

// false = off, true = on
void set_pump(boolean state = false)
{
  digitalWrite(pump, state ? LOW : HIGH); // HIGH = off!
}

void setup_rtc()
{
  Serial.println("Setting up rtc");
  gClock.setClockMode(false); // set to 24h
  // Set default time (we don't care about the date)
  gClock.setHour(11);
  gClock.setMinute(12);
  gClock.setSecond(13);
  Serial.println("Done: Setting up rtc");
}

void print_temperature()
{
  int temp = gClock.getTemperature();
  lcd.clear();
  lcd.print("The temperature:");
  lcd.setCursor(0,1);
  lcd.print(String(temp) + (char)223 + "C");
}

void print_time()
{
  int hh = gClock.getHour(gh12, gPM);
  int mm = gClock.getMinute();
  int ss = gClock.getSecond();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("The time is");
  lcd.setCursor(0,1);
  lcd.print(String(hh) + ":" + String(mm) + ":" + String(ss));
}
// --------------------------------------------------------------------------------------------
// ---------------------------------------- Main logic ----------------------------------------
// --------------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
  static bool a = false;
  a = !a;
  if (a)
    print_time();
  else
    print_temperature();
  delay(5000);
}


