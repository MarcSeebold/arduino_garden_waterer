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
  attachInterrupt(digitalPinToInterrupt(pin_button_red), button_red_pressed, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_button_green), button_green_pressed, RISING);
}

void button_red_pressed()
{
  button_red_was_pressed = true;
}

void button_green_pressed()
{
  button_green_was_pressed = true;
}

void setup_rtc()
{
  Serial.println("Setting up rtc");
  gClock.setClockMode(false); // set to 24h
  // Set default time (we don't care about the date)
  gClock.setHour(0);
  gClock.setMinute(0);
  gClock.setSecond(0);
  Serial.println("Done: Setting up rtc");
}
// --------------------------------------------------------------------------------------------
// ---------------------------------------- Main logic ----------------------------------------
// --------------------------------------------------------------------------------------------
boolean in_menu = false;
enum menu_pos_t
{
  HOUR,
  MINUTE,
  SECOND
};
menu_pos_t menu_pos = menu_pos_t::HOUR;
// -------------------------------------Set time buttons---------------------------------------
void draw_menu()
{
  // Draw time
  lcd.clear();
  lcd.setCursor(0,0);
  int hh = gClock.getHour(gh12, gPM);
  int mm = gClock.getMinute();
  int ss = gClock.getSecond();
  lcd.print(String(hh) + ":" + String(mm) + ":" + String(ss));
  // Draw "cursor"
  switch(menu_pos)
  {
    case menu_pos_t::HOUR:    lcd.setCursor(0,1); break;
    case menu_pos_t::MINUTE:  lcd.setCursor(3,1); break;
    case menu_pos_t::SECOND:  lcd.setCursor(6,1); break;
  }
  lcd.print("^^");
}

void time_increase_hour()
{
  int old = gClock.getHour(gh12, gPM);
  gClock.setHour((old+1)%24);
}

void time_increase_minute()
{
  int old = gClock.getMinute();
  gClock.setMinute((old+1)%60);
}

void time_increase_second()
{
  int old = gClock.getSecond();
  gClock.setSecond((old+1)%60);
}

void handle_buttons()
{
  if (button_red_was_pressed || button_green_was_pressed)
  {
    // Open menu (any button)
    if (!in_menu)
    {
      in_menu = true;
    }
    // red button: Select next field
    else if (button_red_was_pressed)
    {
      switch(menu_pos)
      {
        case menu_pos_t::HOUR:    menu_pos = menu_pos_t::MINUTE;                   break;
        case menu_pos_t::MINUTE:  menu_pos = menu_pos_t::SECOND;                   break;
        case menu_pos_t::SECOND:  menu_pos = menu_pos_t::HOUR;    in_menu = false; break;
      }
    }
    // green button: Increase field
    else if (button_green_was_pressed)
    {
      switch(menu_pos)
      {
        case menu_pos_t::HOUR:    time_increase_hour();   break;
        case menu_pos_t::MINUTE:  time_increase_minute(); break;
        case menu_pos_t::SECOND:  time_increase_second(); break;
      }
    }
  }
  button_red_was_pressed = false;
  button_green_was_pressed = false;
}
// --------------------------------------------------------------------------------------------

// false = off, true = on
void set_pump(boolean state = false)
{
  digitalWrite(pin_pump, state ? LOW : HIGH); // HIGH = off!
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

// MAIN LOOP
void loop() {
  handle_buttons();
  if (in_menu)
  {
    draw_menu();
    delay(200);
    return;
  }
  // put your main code here, to run repeatedly:
  static bool a = false;
  a = !a;
  if (a)
    print_time();
  else
    print_temperature();

  delay(2000);
  Serial.println("ping");
}


