#include <Arduino.h>

const int ldrPin = 34;

hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;

volatile bool timer0Flag = false;
volatile bool timer1Flag = false;

void IRAM_ATTR onTimer0() {
  timer0Flag = true;
}

void IRAM_ATTR onTimer1() {
  timer1Flag = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Налаштування піна як входу
  pinMode(ldrPin, INPUT);

  // --- Налаштування Таймера 0 (2 секунди) ---
  timer0 = timerBegin(1000000); 
  timerAttachInterrupt(timer0, &onTimer0);
  timerAlarm(timer0, 2000000, true, 0); 

  // --- Налаштування Таймера 1 (5 секунд) ---
  timer1 = timerBegin(1000000);
  timerAttachInterrupt(timer1, &onTimer1);
  // 5 000 000 мікросекунд = 5 секунд
  timerAlarm(timer1, 5000000, true, 0);

  Serial.println("=====================================");
  Serial.println("СИСТЕМА ТАЙМЕРІВ ЗАПУЩЕНА (v3.0)");
  Serial.println("Очікування зчитування з піна D34...");
  Serial.println("=====================================");
}

void loop() {
  // Кожні 2 секунди
  if (timer0Flag) {
    timer0Flag = false;
    int val0 = analogRead(ldrPin);
    Serial.print("[2 сек] Значення LDR: ");
    Serial.println(val0);
  }

  // Кожні 5 секунд
  if (timer1Flag) {
    timer1Flag = false;
    int val1 = analogRead(ldrPin);
    Serial.print("[5 сек] Значення LDR: ");
    Serial.println(val1);
  }
}