// C++ code

#include <Adafruit_LiquidCrystal.h>
#include <string.h>

Adafruit_LiquidCrystal lcd(0);

const int ledPin = 4;
const int tapButton = 7;
const int blinkButton = 9;

String morse = "";
String text = "";
unsigned long lastPress = 0;
bool lastTapState = HIGH;
bool lastBlinkState = HIGH;

const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ";
String morseCode[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",     // A-I
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",   // J-R
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",          // S-Z
  ".----", "..---", "...--", "....-", ".....", "-....", "--...",     // 1-7
  "---..", "----.", "-----", " "                                     // 8-0 and space
};

void setup() {
  pinMode(tapButton, INPUT_PULLUP);
  pinMode(blinkButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);

  lcd.print("Morse Translator");
  delay(2000);
  showMenu();
}

void loop() {
  if (Serial.available()) {
    char option = Serial.read();
    lcd.clear();
    if (option == '1') {
      textToMorse();
    } else if (option == '2') {
      tappingMode();
    } else if (option == '3') {
      eyeBlinkMode();
    } else if (option == '4') {
      lcd.print("Thank you!");
    } else {
      lcd.print("Invalid Option");
    }
    delay(2000);
    showMenu();
  }
}

void showMenu() {
  lcd.clear();
  lcd.print("1.Txt-Morse");
  lcd.setCursor(0, 1);
  lcd.print("2.Tap 3.Blink");
  Serial.println("Choose Mode:");
  Serial.println("1. Text to Morse");
  Serial.println("2. Tapping Input");
  Serial.println("3. Eye Blink Input");
  Serial.println("4. Exit");
}

String getMorseCode(char c) {
  c = toupper(c);
  for (int i = 0; i < strlen(alphabet); i++) {
    if (alphabet[i] == c) return morseCode[i];
  }
  return "";
}

char morseToChar(String code) {
  for (int i = 0; i < sizeof(morseCode)/sizeof(String); i++) {
    if (morseCode[i] == code) return alphabet[i];
  }
  return '?';
}

void textToMorse() {
  Serial.println("Enter text:");
  while (!Serial.available());
  String input = Serial.readStringUntil('\n');
  input.trim();
  input.toUpperCase();
  
  String result = "";
  for (int i = 0; i < input.length(); i++) {
    result += getMorseCode(input[i]) + " ";
  }
  lcd.clear();
  lcd.print("Morse:");
  lcd.setCursor(0, 1);
  lcd.print(result.substring(0, 16));
  delay(3000);
  lcd.clear();
  lcd.print("Text:");
  lcd.setCursor(0, 1);
  lcd.print(input.substring(0, 16));
  Serial.println("Morse: " + result);
}

void tappingMode() {
  lcd.clear();
  lcd.print("Tapping...");
  Serial.println("Tap: '.' <300ms | '-' >300ms | wait=space");

  morse = "";
  text = "";
  unsigned long pressTime = 0;
  while (true) {
    bool tapState = digitalRead(tapButton) == LOW;
    if (tapState && !lastTapState) {
      pressTime = millis();
    } else if (!tapState && lastTapState) {
      unsigned long duration = millis() - pressTime;
      if (duration > 50) {
        morse += (duration < 300) ? "." : "-";
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
      }
      lastPress = millis();
    }

    if (millis() - lastPress > 1000 && morse.length() > 0) {
      char c = morseToChar(morse);
      text += c;
      lcd.clear();
      lcd.print("Morse: " + morse);
      lcd.setCursor(0, 1);
      lcd.print("Text: " + text);
      Serial.println("Input: " + morse + " -> " + c);
      morse = "";
      delay(1000);
    }

    lastTapState = tapState;
    if (Serial.available() && Serial.read() == 'x') break;  // Exit on 'x'
  }
}

void eyeBlinkMode() {
  lcd.clear();
  lcd.print("Blinking...");
  Serial.println("Blink '.' short press | '-' long press");

  morse = "";
  text = "";
  unsigned long pressTime = 0;

  while (true) {
    bool blinkState = digitalRead(blinkButton) == LOW;
    if (blinkState && !lastBlinkState) {
      pressTime = millis();
    } else if (!blinkState && lastBlinkState) {
      unsigned long duration = millis() - pressTime;
      if (duration > 50) {
        morse += (duration < 300) ? "." : "-";
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
      }
      lastPress = millis();
    }

    if (millis() - lastPress > 1000 && morse.length() > 0) {
      char c = morseToChar(morse);
      text += c;
      lcd.clear();
      lcd.print("Morse: " + morse);
      lcd.setCursor(0, 1);
      lcd.print("Text: " + text);
      Serial.println("Blink: " + morse + " -> " + c);
      morse = "";
      delay(1000);
    }

    lastBlinkState = blinkState;
    if (Serial.available() && Serial.read() == 'x') break;  // Exit on 'x'
  }
}