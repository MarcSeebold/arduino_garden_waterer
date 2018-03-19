

/*
Plant Water System
2017-2018 by Marc@Seebold.eu

ToDos:
- Create setting for when to water plants and how often
- Add HW to sensor if there is water available (don't pump air)
- Add setting for how long to pump
*/

// External gClock
#include <DS3231.h>
// LCD display
#include <LiquidCrystal_I2C.h>
// Timer
#include <TaskScheduler.h>

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
volatile bool isr_button_red = false;
volatile bool isr_button_green = false;

// LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line

// Scheduler
Scheduler runner;
void callback_lcd();
Task task_lcd(100, TASK_FOREVER, &callback_lcd);

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
  setup_scheduler();
}

void setup_scheduler()
{
  Serial.println("Setting up Scheduler");
  runner.init();
  runner.addTask(task_lcd);
  task_lcd.enable();
  Serial.println("Done: Setting up Scheduler");
}

void setup_lcd()
{
  Serial.println("Setting up LCD");
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight
  lcd.clear();
  Serial.println("Done: Setting up LCD");
}

void setup_pins()
{
  pinMode(pin_pump, OUTPUT);
  digitalWrite(pin_pump, HIGH); // HIGH = off!
  pinMode(pin_button_red, INPUT_PULLUP);
  pinMode(pin_button_green, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_button_red), button_red_pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(pin_button_green), button_green_pressed, FALLING);
}

void button_red_pressed()
{
  isr_button_red = true;
}

void button_green_pressed()
{
  isr_button_green = true;
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

String get_time_as_string()
{
  int hh = gClock.getHour(gh12, gPM);
  int mm = gClock.getMinute();
  int ss = gClock.getSecond();
  String s_hh = time_to_str(hh);
  String s_mm = time_to_str(mm);
  String s_ss = time_to_str(ss);
  return String(s_hh + ":" + s_mm + ":" + s_ss);
  
}

// Draw the settings menu
void draw_menu()
{
  // Draw time
  lcd.setCursor(0, 0);
  lcd.print(get_time_as_string());
  // Draw "cursor"
  switch (menu_pos)
  {
    case menu_pos_t::HOUR:    lcd.setCursor(0, 1); break;
    case menu_pos_t::MINUTE:  lcd.setCursor(3, 1); break;
    case menu_pos_t::SECOND:  lcd.setCursor(6, 1); break;
  }
  lcd.print("^^");
}

// Increase time on clock chip by 1 hour
void time_increase_hour()
{
  int old = gClock.getHour(gh12, gPM);
  gClock.setHour((old + 1) % 24);
}

// Increase time on clock chip by 1 minute
void time_increase_minute()
{
  int old = gClock.getMinute();
  gClock.setMinute((old + 1) % 60);
}

// Increase time on clock chip by 1 second
void time_increase_second()
{
  int old = gClock.getSecond();
  gClock.setSecond((old + 1) % 60);
}

// Handle input via red_pressed and green_pressed variables that are set via ISR
void handle_buttons()
{
  // Pass to menu
  handle_button_menu(isr_button_red, isr_button_green);
  isr_button_red = false;
  isr_button_green = false;
}

// Handle menu input
void handle_button_menu(bool red_pressed, bool green_pressed)
{
  if (!red_pressed && !green_pressed)
    return;

  // Open menu (any button)
  if (!in_menu)
  {
    lcd.clear();
    in_menu = true;
  }
  // red button: Select next field
  else if (red_pressed)
  {
    switch (menu_pos)
    {
      case menu_pos_t::HOUR:    menu_pos = menu_pos_t::MINUTE;                                break;
      case menu_pos_t::MINUTE:  menu_pos = menu_pos_t::SECOND;                                break;
      case menu_pos_t::SECOND:  menu_pos = menu_pos_t::HOUR;    in_menu = false; lcd.clear(); break;
    }
  }
  // green button: Increase field
  else if (green_pressed)
  {
    switch (menu_pos)
    {
      case menu_pos_t::HOUR:    time_increase_hour();   break;
      case menu_pos_t::MINUTE:  time_increase_minute(); break;
      case menu_pos_t::SECOND:  time_increase_second(); break;
    }
  }
}

// Convert int to string. Adds a leading zero for single digits.
String time_to_str(int num)
{
  return (num < 10 ? "0" : "") + String(num);
}
// --------------------------------------------------------------------------------------------

// Start or stop pumping water
// false = off, true = on
void set_pump(boolean state = false)
{
  digitalWrite(pin_pump, state ? LOW : HIGH); // HIGH = off!
}

void print_time_and_temp()
{
  // Get data
  int temp = gClock.getTemperature();
  // Prepare LCD
  lcd.setCursor(0, 0);
  // First line: Some info
  lcd.print("Plant Waterer");
  // Second line: time
  lcd.setCursor(0, 1);
  lcd.print(String(temp) + (char)223 + "C - " + get_time_as_string());
}

// Callback for task_lcd
// Render LCD and handle buttons
void callback_lcd() {
  handle_buttons();
  if (in_menu)
  {
    draw_menu();
    return;
  }
  print_time_and_temp();
}

// MAIN LOOP
void loop() {
  // Everything managed by scheduler
  runner.execute();
}


