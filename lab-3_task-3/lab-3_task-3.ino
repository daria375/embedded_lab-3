#include <Wire.h>
#include <Adafruit_BMP280.h>

// Піни
const int buttonPin = 4;
const int ledTimer = 18;
const int ledButton = 19;

// Об'єкти
Adafruit_BMP280 bmp; 
hw_timer_t * timer = NULL;

// Змінні для переривань
volatile bool timerFlag = false;
volatile unsigned long pressStartTime = 0;
volatile unsigned long duration = 0;
volatile bool buttonReleased = false;

// Обробник таймера
void IRAM_ATTR onTimer() {
  timerFlag = true;
}

// Обробник кнопки (вимірювання часу)
void IRAM_ATTR handleButton() {
  int currentState = digitalRead(buttonPin);
  if (currentState == LOW) { // Кнопку затиснули
    pressStartTime = millis();
  } else { // Кнопку відпустили
    duration = millis() - pressStartTime;
    buttonReleased = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledTimer, OUTPUT);
  pinMode(ledButton, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Ініціалізація BMP280
  if (!bmp.begin(0x76)) { // Адреса зазвичай 0x76 або 0x77
    Serial.println("Датчик BMP280 не знайдено!");
    while (1);
  }

  // Налаштування переривання по кнопці
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, CHANGE);

  // Налаштування таймера (на 3 секунди)
  timer = timerBegin(1000000); // 1 MHz
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 3000000, true, 0); 
  
  Serial.println("Система готова. Чекаю дій...");
}

void loop() {
  if (timerFlag) {
    timerFlag = false;
    
    float temp = bmp.readTemperature();
    float pres = bmp.readPressure() / 133.322;

    Serial.printf("[Таймер] Т: %.2f C, Тиск: %.2f мм рт.ст.\n", temp, pres);
    
    digitalWrite(ledTimer, HIGH);
    delay(1000);
    digitalWrite(ledTimer, LOW);
  }

  if (buttonReleased) {
    buttonReleased = false;
    if (duration > 50) { 
      Serial.printf("[Кнопка] Затиснуто на: %lu мс. Світимо діодом...\n", duration);
      
      digitalWrite(ledButton, HIGH);
      delay(duration); // Світимо стільки ж, скільки тримали
      digitalWrite(ledButton, LOW);
    }
  }
}