#include <Arduino.h>

const int button1Pin = 14; 
const int button2Pin = 12; 
const int ledPin = 25;     
const int ldrPin = 34;     

// --- ЗМІННІ ДЛЯ ПЕРЕРИВАНЬ ---
volatile bool button1Pressed = false;
volatile int pressCount = 0;
volatile bool ledState = false;

// --- ОБРОБНИКИ ПЕРЕРИВАНЬ ---
void IRAM_ATTR handleButton1() {
  button1Pressed = true; 
}

void IRAM_ATTR handleButton2() {
  pressCount++;
  ledState = !ledState; 
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(button1Pin), handleButton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(button2Pin), handleButton2, FALLING);

  Serial.println("===============================");
  Serial.println("ЗАПУЩЕНО РЕЖИМ DAC (GPIO 25)");
  Serial.println("Система готова...");
  Serial.println("===============================");
}

void loop() {
  // Логіка для 1-го переривання (Фоторезистор)
  if (button1Pressed) {
    int ldrValue = analogRead(ldrPin);
    Serial.print("Переривання 1! Дані фоторезистора: ");
    Serial.println(ldrValue);
    
    // Блимання через DAC (максимальна яскравість)
    dacWrite(ledPin, 255); 
    delay(150);
    dacWrite(ledPin, 0);
    
    button1Pressed = false; 
  }

  // Логіка для 2-го переривання (Керування яскравістю через DAC)
  if (ledState) {
    // DAC видає чисту напругу від 0 до 3.3V залежно від числа 0-255
    int dacValue = (pressCount % 6) * 51; 
    dacWrite(ledPin, dacValue);
  } else {
    dacWrite(ledPin, 0);
  }

  // Вивід лічильника
  static int lastCount = -1;
  if (lastCount != pressCount) {
    Serial.print("Кількість натискань: ");
    Serial.println(pressCount);
    lastCount = pressCount;
  }
  
  delay(10);
}