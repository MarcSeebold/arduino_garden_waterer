/*TODOs



*/

// External gClock
#include <DS3231.h>
// LCD display
#include <LiquidCrystal_I2C.h>

// --------------------------------------------------------------------------------------------
// ----------------------------------------- Globals ------------------------------------------
// --------------------------------------------------------------------------------------------
#define DEBUG 1
// Init the DS3231 using the hardware interface
// PIN A4 -> SDA (D)
// PIN A5 -> SCL (C)
DS3231 gClock;
boolean gh12, gPM;

// PINS
int pin_pump = 7;
int pin_button_green = 3;
int pin_button_red = 2;

// Buttons
volatile int button_red_was_pressed = false;
volatile int button_green_was_pressed = false;

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
  pinMode(pin_pump, OUTPUT);
  digitalWrite(pin_pump, HIGH); // HIGH = off!
  pinMode(pin_button_red, INPUT_PULLUP);
  pinMode(pin_button_green, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_button_red), button_red_pressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pin_button_green), button_green_pressed, CHANGE);
}

void button_red_pressed()
{
  button_red_was_pressed = true;
}

void button_green_pressed()
{
  button_green_was_pressed = true;
}

// false = off, true = on
void set_pump(boolean state = false)
{
  digitalWrite(pin_pump, state ? LOW : HIGH); // HIGH = off!
}

void setup_rtc()
{
  Serial.println("Setting up rtc");
  gClock.setClockMode(false); // set to 24h
  // Set default time (we don't care about the date)
  gClock.setHour(10);
  gClock.setMinute(21);
  gClock.setSecond(50);
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

  if (button_red_was_pressed)
  {
    button_red_was_pressed = false;
    lcd.clear();
    lcd.print("red!");
  }
  if (button_green_was_pressed)
  {
    button_green_was_pressed = false;
    lcd.clear();
    lcd.print("green!");    
  }

  delay(2000);
  Serial.println("ping");
}


