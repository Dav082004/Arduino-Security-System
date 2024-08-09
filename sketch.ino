#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definición de pines
#define TRIG_PIN 9
#define ECHO_PIN 8
#define PIR_PIN 7
#define BUZZER_PIN 6
#define BUTTON_PIN 4
#define LDR_PIN A0  // Pin analógico para la fotoresistencia
#define LED1_PIN 11 // Pin para el primer LED
#define LED2_PIN 12 // Pin para el segundo LED

// Código correcto de seguridad
const char* correctCode = "1234"; // Código de seguridad correcto
char enteredCode[5]; // Código ingresado por el usuario
int attempts = 0; // Número de intentos incorrectos
bool systemActive = true; // Estado del sistema (activo/inactivo)
unsigned long lastIncorrectTime = 0; // Tiempo del último intento incorrecto
unsigned long motionDetectedTime = 0;  // Tiempo de detección del movimiento
bool motionDetected = false;  // Estado de detección de movimiento

// Crear una instancia de la pantalla LCD con la dirección I2C correcta
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C 0x27 y tamaño de la pantalla 16x2

void setup() {
  // Configuración de pines
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  // Inicialización de la pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA INACTIVO");

  // Inicialización del puerto serie para entrada de código
  Serial.begin(9600);
}

void loop() {
  // Leer el valor de la fotoresistencia
  int ldrValue = analogRead(LDR_PIN);

  // Verificar si el sistema debe estar activo basado en el valor de la fotoresistencia
  if (ldrValue < 500) {  // Ajustar el umbral de luz según sea necesario (500 es un ejemplo)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SISTEMA ACTIVO");

    digitalWrite(LED1_PIN, HIGH);  // Encender LED 1
    digitalWrite(LED2_PIN, HIGH);  // Encender LED 2
    systemActive = true;

    if (systemActive) {
      // Verificar detección de movimiento por PIR o ultrasonidos
      if (digitalRead(PIR_PIN) == HIGH || detectMotionWithUltrasonic()) {
        motionDetected = true;
        motionDetectedTime = millis();  // Registrar el tiempo de detección de movimiento
      }

      // Si se ha detectado movimiento recientemente, continuar con la lógica
      if (motionDetected) {
        if (millis() - motionDetectedTime < 10000) {  // 10 segundos
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("INGRESE CODIGO:");
          Serial.println("Movimiento detectado. Ingrese el código de seguridad:");

          // Esperar a que se ingrese el código
          if (getCodeFromSerial()) {
            // Verificar si el código ingresado es correcto
            if (strcmp(enteredCode, correctCode) == 0) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("CLAVE CORRECTA");
              Serial.println("Clave correcta.");
              playWelcomeSound(); // Reproducir sonido de bienvenida
              delay(500);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("SISTEMA DESACTIVO");
              systemActive = false;
              motionDetected = false;  // Restablecer el estado de detección de movimiento
              waitForButtonPress();
            } else {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("CODIGO INCORRECTO");
              Serial.println("Código incorrecto.");
              attempts++;
              playErrorSound(); // Reproducir sonido de error
              delay(500);

              if (attempts >= 3) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("SISTEMA ACTIVO");
                Serial.println("Demasiados intentos incorrectos. Sistema bloqueado.");
                tone(BUZZER_PIN, 1000, 2000); // Sonido de error largo
                lastIncorrectTime = millis(); // Marcar el tiempo del último intento incorrecto
                while (millis() - lastIncorrectTime < 10000) {} // Esperar 10 segundos
                attempts = 0; // Reiniciar intentos después de esperar
                motionDetected = false;  // Restablecer el estado de detección de movimiento
              }
            }
          }
        } else {
          motionDetected = false;  // Restablecer el estado de detección de movimiento después de 10 segundos
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("SISTEMA ACTIVO");
        }
      }
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SISTEMA INACTIVO");
    digitalWrite(LED1_PIN, LOW);  // Apagar LED 1
    digitalWrite(LED2_PIN, LOW);  // Apagar LED 2
    systemActive = false;
  }
  // Esperar un poco antes de la siguiente iteración
  delay(100);
}

// Función para detectar movimiento con sensor de ultrasonidos
bool detectMotionWithUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  if (distance < 250) { // Ajustar el umbral de distancia a 250 cm
    return true; // Si la distancia es menor a 250 cm, se considera como movimiento detectado
  } else {
    return false; // Si la distancia es mayor a 250 cm, no se considera como movimiento
  }
}

// Función para recibir el código desde el puerto serie
bool getCodeFromSerial() {
  if (Serial.available() > 0) {
    int i = 0;
    while (Serial.available() > 0 && i < 4) {
      char c = Serial.read();
      if (c >= '0' && c <= '9') {  // Asegurar que sólo se lean dígitos
        enteredCode[i] = c;
        i++;
      }
      delay(50);
    }
    enteredCode[i] = '\0';
    return true;
  }
  return false;
}

// Función para esperar a que el usuario presione el botón para reactivar el sistema
void waitForButtonPress() {
  while (digitalRead(BUTTON_PIN) == HIGH) {
    delay(50); // Esperar hasta que el botón sea presionado
  }
  while (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Esperar hasta que el botón sea liberado
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA ACTIVO");
  systemActive = true; // Restablecer el sistema a activo
}

// Función para reproducir sonido de bienvenida
void playWelcomeSound() {
  tone(BUZZER_PIN, 523, 200); // Nota Do
  delay(250);
  tone(BUZZER_PIN, 587, 200); // Nota Re
  delay(250);
  tone(BUZZER_PIN, 659, 200); // Nota Mi
  delay(250);
  tone(BUZZER_PIN, 784, 200); // Nota Sol
  delay(250);
  noTone(BUZZER_PIN); // Detener el sonido
}

// Función para reproducir sonido de error
void playErrorSound() {
  tone(BUZZER_PIN, 440, 200); // Nota La
  delay(250);
  tone(BUZZER_PIN, 349, 200); // Nota Fa
  delay(250);
  tone(BUZZER_PIN, 392, 200); // Nota Sol
  delay(250);
  tone(BUZZER_PIN, 262, 200); // Nota Do
  delay(250);
  noTone(BUZZER_PIN); // Detener el sonido
}
