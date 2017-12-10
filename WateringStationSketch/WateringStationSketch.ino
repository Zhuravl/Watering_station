//Scetch for watering station
//Version 1.0
//Author Zhuravskiy Ilya
//Mail: ilya.zhuravskiy@gmail.com

#include <LiquidCrystal.h>
#include <iarduino_RTC.h>

const int D_PIN_RTC_CLK = 4;
const int D_PIN_RTC_DAT = 3;
const int D_PIN_RTC_RST = 2;
const int D_PIN_LCD_D4 = 5;
const int D_PIN_LCD_D5 = 6;
const int D_PIN_LCD_D6 = 7;
const int D_PIN_LCD_D7 = 8;
const int D_PIN_LCD_D8 = 9;
const int D_PIN_LCD_D9 = 10;
const int A_PIN_LCD_BTNS = 0;

const int LOOP_DELAY_TIME = 100;
const int MIN_HOURS_1 = 0;
const int MAX_HOURS_1 = 10;
const int MIN_HOURS_2 = 14;
const int MAX_HOURS_2 = 23;
const int MIN_DURATION_MINUTES = 0;
const int MAX_DURATION_MINUTES = 60 * 3;
const int MIN_MINUTES = 0;
const int MAX_MINUTES = 59;

iarduino_RTC time(RTC_DS1302, D_PIN_RTC_RST, D_PIN_RTC_CLK, D_PIN_RTC_DAT);
LiquidCrystal lcd(D_PIN_LCD_D8, D_PIN_LCD_D9, D_PIN_LCD_D4, D_PIN_LCD_D5, D_PIN_LCD_D6, D_PIN_LCD_D7);

int mode; //0 - waiting mode, 1 - configuration mode.
int timeValuesArray[7] = {6, 6, 0, 30, 18, 0, 30}; //Array with preferences {0:ElemSize, 1:WaterHour1, 2:WaterMinute1, 3:Duration1, 4:WaterHour2, 5:WaterMinute2, 6:Duration2}
boolean forseWatering = false;
boolean skipWatering = false;

void setup() {
  mode = 0;
  Serial.begin(9600);
  time.begin();
  lcd.begin(16, 2);
  //time.settime(30,7,14,8,12,17,5);  //Uncomment for setup, then comment again. Sec, min, hour, day, month, year, number of week day (0-San, 1-Mon..)
}

void loop() {
  showCurrentDateTime();
  watering();
  keysEvents();

  //Delay time to adjust the sensitivity of the buttons
  delay (LOOP_DELAY_TIME);
}

//Shows current time on LCD
void showCurrentDateTime() {
  if (mode == 0) {
    printIt(time.gettime("d.m.Y H:i"), "                ");
  }
}

//Check time and do watering (the main logic block)
void watering() {

}

//Check and react if buttons have been pressed
void keysEvents() {
  static int arrNumber;
  int buttonValue = analogRead (A_PIN_LCD_BTNS);
  //For unpressed buttons will return ~1023
  if (buttonValue < 1000) {
    //Button "Right"
    if (buttonValue < 60) {
      if (mode == 1) {
        if (arrNumber < timeValuesArray[0]) {
          arrNumber += 1;
        } else {
          arrNumber = 1;
        }
        showValueWithId(arrNumber);
      } else {
        forseWatering = !forseWatering;
      }
    }
    //Button "Up"
    else if (buttonValue < 200) {
      int curValue = timeValuesArray[arrNumber];
      curValue += 1;
      //case for wattering 1 hours
      if ((arrNumber == 1) && (curValue > MAX_HOURS_1)) {
        curValue = MIN_HOURS_1;
        //case for wattering 2 hours
      } else if ((arrNumber == 4) && (curValue > MAX_HOURS_2)) {
        curValue = MIN_HOURS_2;
        //case for wattering minutes
      } else if ((arrNumber == 2 || arrNumber == 5) && (curValue > MAX_MINUTES)) {
        curValue = MIN_MINUTES;
        //case for wattering duration
      } else if ((arrNumber == 3 || arrNumber == 6) && (curValue > (MAX_DURATION_MINUTES))) {
        curValue = MIN_DURATION_MINUTES;
      }
      //save and show result
      timeValuesArray[arrNumber] = curValue;
      showValueWithId(arrNumber);
    }
    //Button "Down"
    else if (buttonValue < 400) {
      int curValue = timeValuesArray[arrNumber];
      curValue -= 1;
      if (curValue < 0 || (arrNumber == 4 && curValue < MIN_HOURS_2)) {
        //case for wattering 1 hours
        if (arrNumber == 1) {
          curValue = MAX_HOURS_1;
          //case for wattering 2 hours
        } else if (arrNumber == 4) {
          curValue = MAX_HOURS_2;
          //case for wattering minutes
        } else if (arrNumber == 2 || arrNumber == 5) {
          curValue = MAX_MINUTES;
          //case for wattering duration
        } else if (arrNumber == 3 || arrNumber == 6) {
          curValue = MAX_DURATION_MINUTES;
        }
      }
      //save and show result
      timeValuesArray[arrNumber] = curValue;
      showValueWithId(arrNumber);
    }
    //Button "Left"
    else if (buttonValue < 600) {
      if (mode == 1) {
        if (arrNumber <= 1) {
          arrNumber = timeValuesArray[0];
        } else {
          arrNumber -= 1;
        }
        showValueWithId(arrNumber);
      } else {
        skipWatering = !skipWatering;
      }
    }
    //Button "Select"
    else if (buttonValue < 800) {
      mode += 1;
      if (mode == 2) {
        mode = 0;
      }
      if (mode == 1) {
        printIt("    SETTINGS:   ", "press  <- or ->");
        arrNumber = 0;
      }
    }
  }
}

//Prints first argument at first line and second at second line on LCD
void printIt(String s1, String s2) {
  if (s1.length() > 0) {
    lcd.setCursor(0, 0);
    for (int i = s1.length(); i < 16; i++) {
      s1 += " ";
    }
    lcd.print(s1);
  }
  if (s2.length() > 0) {
    lcd.setCursor(0, 1);
    for (int i = s2.length(); i < 16; i++) {
      s2 += " ";
    }
    lcd.print(s2);
  }
}

//Show preference by id with full name
void showValueWithId(int id) {
  switch (id) {
    case 1:
      printIt("<  W. HOURS 1  >", "       " + String(timeValuesArray[id]));
      break;
    case 2:
      printIt("< W. MINUTES 1 >", "       " + String(timeValuesArray[id]));
      break;
    case 3:
      printIt("< W.DURATION 1 >", "       " + String(timeValuesArray[id]));
      break;
    case 4:
      printIt("<  W. HOURS 2  >", "       " + String(timeValuesArray[id]));
      break;
    case 5:
      printIt("< W. MINUTES 2 >", "       " + String(timeValuesArray[id]));
      break;
    case 6:
      printIt("< W.DURATION 2 >", "       " + String(timeValuesArray[id]));
      break;
    default:
      printIt("   SOMETHING    ", "     WRONG!     ");
  }
}

