//Sketch for watering station
//Version 1.0
//Author Zhuravskiy Ilya
//Mail: ilya.zhuravskiy@gmail.com

#include <LiquidCrystal.h>
#include <iarduino_RTC.h>

const int D_PIN_RTC_RST    = 2;
const int D_PIN_RTC_DAT    = 3;
const int D_PIN_RTC_CLK    = 4;
const int D_PIN_LCD_D4     = 5;
const int D_PIN_LCD_D5     = 6;
const int D_PIN_LCD_D6     = 7;
const int D_PIN_LCD_D7     = 8;
const int D_PIN_LCD_D8     = 9;
const int D_PIN_LCD_D9     = 10;
const int D_PIN_RELLAY_PWR = 11;
const int D_PIN_RELLAY_IN  = 12;
const int D_PIN_SNSR_PWR   = 13;

const int A_PIN_LCD_BTNS  = 0;
const int A_PIN_SNSR_DATA = 5;

const int LOOP_DELAY_TIME = 100;
const int MIN_HOURS_1 = 0;
const int MAX_HOURS_1 = 10;
const int MIN_HOURS_2 = 14;
const int MAX_HOURS_2 = 23;
const int MIN_DURATION_MINUTES = 0;
const int MAX_DURATION_MINUTES = 60 * 3;
const int MIN_MINUTES = 0;
const int MAX_MINUTES = 59;
const int RAIN_THRESHOLD = 1024; //0 - no rain, 1024 - max rain.
const int MINUTES_IN_HOUR = 60;

iarduino_RTC time(RTC_DS1302, D_PIN_RTC_RST, D_PIN_RTC_CLK, D_PIN_RTC_DAT);
LiquidCrystal lcd(D_PIN_LCD_D8, D_PIN_LCD_D9, D_PIN_LCD_D4, D_PIN_LCD_D5, D_PIN_LCD_D6, D_PIN_LCD_D7);

int mode; //0 - waiting mode, 1 - configuration mode.
int timeValuesArray[7] = {6, 6, 0, 30, 18, 0, 30}; //Array with preferences {0:ElemSize, 1:WaterHour1, 2:WaterMinute1, 3:Duration1, 4:WaterHour2, 5:WaterMinute2, 6:Duration2}
boolean forseWatering;
boolean skipWatering;

void setup() {
  mode = 0;
  forseWatering = false;
  skipWatering = false;
  Serial.begin(9600);
  time.begin();
  lcd.begin(16, 2);
  //time.settime(30,7,14,8,12,17,5);  //Uncomment for setup, then comment again. Sec, min, hour, day, month, year, number of week day (0-San, 1-Mon..)
  pinMode(D_PIN_RELLAY_PWR, OUTPUT);
  pinMode(D_PIN_RELLAY_IN, OUTPUT);
  pinMode(D_PIN_SNSR_PWR, OUTPUT);
  digitalWrite(D_PIN_RELLAY_PWR, HIGH);
}

void loop() {
  if (mode == 0) {
    showCurrentDateTime();
    watering();
  }
  keysEvents();

  //Delay time to adjust the sensitivity of the buttons
  delay (LOOP_DELAY_TIME);
}

//Shows current time on LCD
void showCurrentDateTime() {
  printIt(time.gettime("d.m.Y H:i"), "");
}

//Check time and do watering (the main logic block)
void watering() {
  if (forseWatering) {
    digitalWrite(D_PIN_RELLAY_IN, HIGH);
    printIt("", "*FORSE WATERING*");
  } else {
    int fromMidnight = (time.Hours * MINUTES_IN_HOUR) + time.minutes;
    int startWatering1 = (timeValuesArray[1] * MINUTES_IN_HOUR) + timeValuesArray[2];
    int startWatering2 = (timeValuesArray[4] * MINUTES_IN_HOUR) + timeValuesArray[5];

    if ((fromMidnight == (startWatering1 - MINUTES_IN_HOUR)) || (fromMidnight == (startWatering1 - 2 * MINUTES_IN_HOUR)) || (fromMidnight == (startWatering2 - MINUTES_IN_HOUR)) || (fromMidnight == (startWatering2 - 2 * MINUTES_IN_HOUR))) {
      //Time to check rain
      //If skip flag is not true but it is rain - set to skip watering
      if (!skipWatering && isItRain()) {
        skipWatering = true;
      }
    }

    if ((fromMidnight >= startWatering1) && (fromMidnight <= (startWatering1 + timeValuesArray[3])) || (fromMidnight >= startWatering2) && (fromMidnight <= (startWatering2 + timeValuesArray[6]))) {
      //Time to watering
      if (skipWatering) {
        //skip watering
        digitalWrite(D_PIN_RELLAY_IN, LOW);
        printIt("", "WATERING SKIPED!");
      } else {
        //let's watering!
        digitalWrite(D_PIN_RELLAY_IN, HIGH);
        printIt("", "WATERING: -" + getPrettyTime(getDifferenceInMinutes(calculateWateringMinutes(getNextWatering()) + timeValuesArray[(3 + 3 * getNextWatering())], ((time.Hours * MINUTES_IN_HOUR) + time.minutes))));
      }
    } else {
      digitalWrite(D_PIN_RELLAY_IN, LOW);
      if (skipWatering) {
        //watering will skip!
        printIt("", "W" + getPrettyTime(calculateWateringMinutes(getNextWatering())) + " 'LL SKIP!");
      } else {
        //to the next watering left..
        printIt("", "W" + getPrettyTime(calculateWateringMinutes(getNextWatering())) + " -> -" + getPrettyTime(getDifferenceInMinutes(calculateWateringMinutes(getNextWatering()), ((time.Hours * MINUTES_IN_HOUR) + time.minutes))));
      }
    }
  }
}

//Checks if the rain sensor shows rain
boolean isItRain() {
  digitalWrite(D_PIN_SNSR_PWR, HIGH);
  boolean result = (analogRead(A_PIN_SNSR_DATA) > RAIN_THRESHOLD);
  digitalWrite(D_PIN_SNSR_PWR, LOW);
  return result;
}

//Calculates how many minutes are from 'now' till 'purpose'
int getDifferenceInMinutes(int purpose, int now) {
  if (purpose < now) {
    return ((24 * MINUTES_IN_HOUR - now) + purpose);
  } else {
    return (purpose - now);
  }
}

//Returns current or nearest watering time number (0 - first, 1 - second).
int getNextWatering() {
  int fromMidnight = (time.Hours * MINUTES_IN_HOUR) + time.minutes;
  int endWatering1 = (timeValuesArray[1] * MINUTES_IN_HOUR) + timeValuesArray[2] + timeValuesArray[3];
  int endWatering2 = (timeValuesArray[4] * MINUTES_IN_HOUR) + timeValuesArray[5] + timeValuesArray[6];

  if ((fromMidnight > endWatering1) && (fromMidnight <= endWatering2)) {
    return 1;
  } else {
    return 0;
  }
}

//Calculates number of the minutes form the start of day
int calculateWateringMinutes(int number) {
  int indexH = (1 + (3 * number));
  int indexM = (2 + (3 * number));
  return ((timeValuesArray[indexH] * MINUTES_IN_HOUR) + timeValuesArray[indexM]);
}

//Converts value with time in minutes to string with colon
String getPrettyTime(int curTime) {
  int h = abs(curTime) / MINUTES_IN_HOUR;
  int m = abs(curTime) - (h * MINUTES_IN_HOUR);

  return getPrettyTime(h, m);
}

//Converts values with hours and minutes to string with colon
String getPrettyTime(int curHours, int curMinutes) {
  String h;
  String m;

  if (curHours <= 9) {
    h = "0" + String(curHours);
  } else {
    h = String(curHours);
  }

  if (curMinutes <= 9) {
    m = "0" + String(curMinutes);
  } else {
    m = String(curMinutes);
  }

  return (h + ":" + m);
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

