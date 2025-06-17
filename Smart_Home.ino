#include <Keypad.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <DHT_U.h>
#include <DHT.h>
#include <Servo.h>

//************************** Music Keys Frequencies *****************************
// Define the melody notes:
#define C1 131
#define Db1 140
#define D1 147
#define Eb1 156
#define E1 165
#define F1 175
#define Gb1 185
#define G1 196
#define Ab1 208
#define Key_A1  220
#define Bb1 223
#define B1 247
#define C2 262
#define Db2 277
#define D2 294
#define Eb2 311
#define E2 330
#define F2 349
#define Gb2 370
#define G2 392
#define Ab2 415
#define Key_A2 440
#define Bb2 466
#define B2 494
#define C3 529
#define Db3 554
#define D3 587
#define Eb3 622
#define E3 659
#define F3 698
#define Gb3 740
#define G3 784
#define Ab3 831
#define Key_A3 880
#define Bb3 932
#define B3 988
#define C4 1047


int melody[] = {
  C3, C3, G3, G3, Key_A3, Key_A3, G3,
  F3, F3, E3, E3, D3, D3, C3,
  G3, G3, F3, F3, E3, E3, D3,
  G3, G3, F3, F3, E3, E3, D3,
  C3, C3, G3, G3, Key_A3, Key_A3, G3,
  F3, F3, E3, E3, D3, D3, C3
};

// Define the note durations (in ms):
int noteDurations[] = {
  1000, 1000, 1000, 1000, 1000, 1000, 2000,
  1000, 1000, 1000, 1000, 1000, 1000, 4000,
  1000, 1000, 1000, 1000, 1000, 1000, 2000,
  1000, 1000, 1000, 1000, 1000, 1000, 4000,
  1000, 1000, 1000, 1000, 1000, 1000, 4000,
  1000, 1000, 1000, 1000, 1000, 1000, 4000
};

// State variables for non-blocking music:
bool isPlayingMusic = false;
int currentNoteIndex = 0;
unsigned long noteStartTime = 0;

//******************************** Key Frequencies Ends *************************

LiquidCrystal_I2C lcd(0x27,20,4);

// ********************** Keypad Begins ******************************
String password = "1234";
String input = "";

int failedAttempts = 0;
const int maxAttempts = 4;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] =  {31,33,35,37}; // R1, R2, R3, R4
byte colPins[COLS] =  {30,32,34,36}; // C1, C2, C3, C4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ********************** Keypad Ends ******************************
#define SS_PIN 53
#define RST_PIN 5
MFRC522 rfid(SS_PIN, RST_PIN);
byte user1UID[] = {0x93, 0xD1, 0x3A, 0xE3}; //You can cange the UID codes to your own UID
byte user2UID[] = {0xF3, 0x5A, 0x1C, 0xE3};
//byte user3UID[] = {0x03, 0xF6, 0x40, 0x10};

// ********************* RFID Begins *****************************

bool inPasswordEntry = false;
bool accessGranted = false;

const int motion = 22;
const int parlour = 4;
const int parlourLight = 2;
const int redLed = 6;
const int blueLed = 7; 
const int fan = 9;
const int securityLight = 10;
const int flameSensor = 38;
const int alarmLed = 39;
const int buzzer = 40;


Servo door;
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String data;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  lcd.begin();
  lcd.backlight();
  dht.begin();
  SPI.begin();
  rfid.PCD_Init();
  door.attach(11);
  door.write(90);

  pinMode(redLed,OUTPUT);
  pinMode(blueLed,OUTPUT);
  pinMode(parlourLight,OUTPUT);
  pinMode(parlour,OUTPUT);
  pinMode(motion,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(fan ,OUTPUT);
  pinMode(securityLight,OUTPUT);
  pinMode(flameSensor,INPUT); 


  frontPage();
  displayMenu();
}

void loop() {
  checkBluetoothCommand();
  automation();
  keyPress();
  fireDetection();

  // If music is playing, keep updating it
  if (isPlayingMusic) {
    music();
  }
}
void frontPage(){
  lcd.setCursor(4,0);
  lcd.print("Roseville's");
  lcd.setCursor(5,1);
  lcd.print("Smart Home");
  lcd.setCursor(8,2);
  lcd.print(" By");
  lcd.setCursor(2,3);
  lcd.print("Robotics Club ");
  delay(3000);
}
void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose a method.");
  
  lcd.setCursor(0, 1);
  lcd.print("A: Password");
  lcd.setCursor(0, 2);
  lcd.print("B: Card Scan");
  lcd.setCursor(0, 3);
  lcd.print("C: Temperature");
  delay(2000);
}
void keyPress(){
  char key = keypad.getKey();
  if (key) handleKeyPress(key);
  delay(200);
}

void exit() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Returning to");
  lcd.setCursor(0,1);
  lcd.print("main menu...");
  delay(1500);
  
  // Reset all security-related variables
  input = "";
  failedAttempts = 0;
  
  // Return to initial state
  displayMenu();
}
void handleKeyPress(char key) {
  if (inPasswordEntry) return;  // Don't handle keys globally when in password mode
  switch (key) {
    case 'A':
      passwordEntry();
      break;
    case 'B':
      checkRFIDCard();
      break;
    case 'C':
      displayTemperature();
      break;

    case '*':
      displayMenu(); // Back to main menu
      break;
    case '#':
      exit(); // Back to main menu
      break;
    
    default:
      break;
  }
}

void passwordEntry(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter Password: ");
  input = "";

  while (true) { 
    inPasswordEntry = true;  // Entering password mode
    char key = keypad.getKey();

    if (key) {
      // If the '#' key is pressed, clear the last character of the input
      if (key == '#') {
        if (input.length() > 0) {
          input.remove(input.length()-1); // remove the last character
          lcd.setCursor(0,1);
          lcd.print("                "); // Clear row
          lcd.setCursor(0,1);
          lcd.print(input);
        }
        continue;
      }
      // If the '*' key is pressed, check the password
      else if (key == '*') {
        checkPassword();
        inPasswordEntry = false;  // Exit password mode
        return;
      }
      // For other keys, add to input and display
      else {
        input += key;
        lcd.setCursor(0,1);
        lcd.print("                "); // clear second row
        lcd.setCursor(0,1);
        lcd.print(input);
      }
    }
  }
}

void checkPassword() {
  int time;
  lcd.clear();
  lcd.setCursor(0,0);
  // Check if the input matches the password
  if (input == password) {
    lcd.print("Access Granted");
    digitalWrite(blueLed,HIGH);
    digitalWrite(redLed,LOW);
    accessGranted = true;
    openDoor();
    delay(3000);

    digitalWrite(blueLed,LOW);
    closeDoor();
    failedAttempts = 0; // Reset on successful attempt
    displayMenu();
  } else {
    failedAttempts++;
    lcd.print("Access Denied");
    lcd.setCursor(0,1);
    lcd.print("Attempts left: ");
    lcd.print(maxAttempts - failedAttempts);
    digitalWrite(blueLed,LOW);
    digitalWrite(redLed,HIGH);
    accessGranted = false;
    delay(2000);
    digitalWrite(redLed,LOW);
    

    if (failedAttempts >= maxAttempts) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Too many tries!");
      lcd.setCursor(0,1);
      lcd.print("Press # to exit");
      digitalWrite(blueLed,LOW);
      digitalWrite(redLed,HIGH);
      
    
      lcd.setCursor(0,2);
      lcd.print("Try again later");
      for(time = 10; time >= 0; time --) {
        lcd.setCursor(0, 3);
        lcd.print("Time: ");
        lcd.print(time);
        lcd.print(" Seconds");

        //beeping section
        digitalWrite(redLed,HIGH);
        delay(1000);
        digitalWrite(redLed,LOW);
        delay(1000);
        if (time == 0) {
          displayMenu();
        }
      }
      keyPress();
    }
    
  }
  delay(1500); // Show result for 2 seconds

  input = "";  // Reset for next attempt
}

void displayTemperature() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (isnan(humidity) || isnan(temperature)) {
    lcd.print("Sensor Error");
    return;
  }
  
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");

  // if (temperature > 34) {
  //   digitalWrite(FAN, HIGH); // Turn the fan on
  // } else {
  //   digitalWrite(FAN, LOW);  // Turn the fan off
  // }
  delay(3000); // Display for 5 seconds
  displayMenu();
}

void checkRFIDCard() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Present your card...");
  delay(4000);

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card read error");
    lcd.setCursor(0, 1);
    lcd.print("Try again or press *");
    return;
  }

  //For the print statements, you can change the user tou your names
  if (compareUID(rfid.uid.uidByte, rfid.uid.size, user1UID)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome Zimife");
    digitalWrite(blueLed,HIGH);
    digitalWrite(redLed,LOW);
    accessGranted = true;
    failedAttempts = 0;

    openDoor();
    delay(3000);
    digitalWrite(blueLed,LOW);
    closeDoor();
    displayMenu();
  } 
  else if (compareUID(rfid.uid.uidByte, rfid.uid.size, user2UID)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome Chizzy");
    digitalWrite(blueLed,HIGH);
    digitalWrite(redLed,LOW);
    accessGranted = true;
    failedAttempts = 0;
    openDoor();
    delay(3000);
    digitalWrite(blueLed,LOW);
    closeDoor();
    displayMenu();
  } 

  // else if (compareUID(rfid.uid.uidByte, rfid.uid.size, macUID)) {
  //   lcd.clear();
  //   lcd.setCursor(0, 1);
  //   lcd.print("Welcome user3");
  //   openDoor();
  //   delay(1000);
  // }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Access");
    lcd.setCursor(0, 1);
    lcd.print("Try another method ");
    lcd.setCursor(0, 3);
    lcd.print("Please");
    digitalWrite(redLed, HIGH);
    digitalWrite(blueLed,LOW);
    accessGranted = false;
    delay(2000);
    digitalWrite(redLed, LOW);
    displayMenu();
  }
  rfid.PICC_HaltA();
}

bool compareUID(byte *uid, byte uidSize, byte *knownUID) {
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != knownUID[i]) return false;
  }
  return true;
}

void checkBluetoothCommand() {
  while (Serial1.available()) {
    char c = Serial1.read();

    // Optional: debug print each received char
    Serial.print(c);

    if (c == '#') {
      break;
    }
    else if (c == '\n' || c == '\r') {  // End of line
      data.trim();  // Remove whitespace

      if (data == "X" || data == "parlor light on") {
        digitalWrite(parlourLight, HIGH);
      } 
      else if (data == "V" || data == "parlor light off") {
        digitalWrite(parlourLight, LOW);
      } 
      else if (data == "C" || data == "fan on") {
        digitalWrite(fan, HIGH);
      } 
      else if (data == "D" || data == "fan off") {
        digitalWrite(fan, LOW);
      } 
      else if (data == "E" || data == "play music") {
        isPlayingMusic = true;
      } 
      else if (data == "M" || data == "stop music") {
        isPlayingMusic = false;  // Stop music
        noTone(buzzer); 
      } 
      else if (data == "G" || data == "security light on") {
        digitalWrite(securityLight, HIGH);
      } 
      else if (data == "H" || data == "security light off") {
        digitalWrite(securityLight, LOW);
      }
      else if (data == "I" || data == "open the door") {
        openDoor();
      } 
      else if (data == "J" || data == "close the door") {
        closeDoor();
      }

      data = "";  // Reset after processing
    }
    else {
      data += c;
    }
  }
}


void automation(){
  int value = digitalRead(motion);

  if (value == HIGH){
    digitalWrite(parlour,HIGH);
  }
  else{
    digitalWrite(parlour,LOW);
  }
}

void fireDetection(){
  static unsigned long lastToggle = 0;
  static bool ledState = false;

  int flameValue = digitalRead(flameSensor);

  if(flameValue == LOW){
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Attention !!!!");
    lcd.setCursor(0, 1);
    lcd.print("There's a fire");
    lcd.setCursor(0, 2);
    lcd.print("Outbreak");
    lcd.setCursor(0, 3);
    lcd.print("in The Building");

    // Non-blocking alarm toggle every 500ms
    if (millis() - lastToggle >= 500) {
      lastToggle = millis();
      ledState = !ledState;
      digitalWrite(alarmLed, ledState ? HIGH : LOW);
      if (ledState) {
        tone(buzzer, 500);
      } else {
        noTone(buzzer);
      }
    }
    displayMenu();
  }
  else {
    digitalWrite(alarmLed, LOW);
    noTone(buzzer);
  }
}


void alarm(){
  digitalWrite(alarmLed, HIGH);
  tone(buzzer,500);
  delay(1000);
  digitalWrite(alarmLed, HIGH);
  noTone(buzzer);
  delay(1000);
}

void music() {
  if (!isPlayingMusic) {
    // Start playing
    isPlayingMusic = true;
    currentNoteIndex = 0;
    noteStartTime = millis();
    tone(buzzer, melody[currentNoteIndex]);
  } else {
    // Check if time to change note
    if (millis() - noteStartTime >= noteDurations[currentNoteIndex]) {
      currentNoteIndex++;

      if (currentNoteIndex < sizeof(melody) / sizeof(int)) {
        // Play next note
        noteStartTime = millis();
        tone(buzzer, melody[currentNoteIndex]);
      } else {
        // Finished playing
        noTone(buzzer);
        isPlayingMusic = false;
      }
    }
  }
}

void openDoor(){
  door.write(0);
  delay(500);
  door.write(90);
  delay(2000);
}
void closeDoor(){
  door.write(180);
  delay(500);
  door.write(90);
  delay(2000);
}

