#include <Servo.h>

//Numpad
int row1 = 2;
int row2 = 3;
int row3 = 4;
int row4 = 5;
int col1 = 6;
int col2 = 7;
int col3 = 8;
int col4 = 9;

char setPassCode[4] = { '1', '2', '3', '4' };

int rows[4] = {
  row1,
  row2,
  row3,
  row4,
};
int cols[4] = { col1, col2, col3, col4 };
char passCode[4];
int charTracker = 0;

int attemptTracker = 0;

//LED
int red = 10;
int green = 12;
int blue = 11;

//Buzzer
int buzzer = A0;

//Servo
int servo = 13;
Servo myServo;


void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH);  // Set rows HIGH initially
    pinMode(cols[i], INPUT_PULLUP);
  }

  //;ed pins
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  pinMode(buzzer, OUTPUT);

  myServo.attach(servo);
  myServo.write(0);

  Serial.begin(9600);
}

void scanForMatrixInput() {
  unsigned long startTime = millis();  // Record the start time
  int row;
  int col;

  while (true) {
    signalIdleLed();
    for (int i = 0; i < 4; i++) {
      digitalWrite(rows[i], LOW);

      for (int j = 0; j < 4; j++) {
        if (digitalRead(cols[j]) == LOW) {
          tone(buzzer, 1000, 5);
          Serial.print("Row ");
          Serial.print(i + 1);
          row = i + 1;
          Serial.print(", Column ");
          Serial.println(j + 1);
          col = j + 1;
          int cords[2] = { row, col };
          recordCode(cordsToChar(cords));
          delay(200);  // Simple debounce
        }
      }
      digitalWrite(rows[i], HIGH);  // Set the row back to HIGH
    }
  }
}

void recordCode(char codeKey) {
  passCode[charTracker] = codeKey;

  if (charTracker == 3) {
    Serial.println("Checking Code...");
    if (validateCode(passCode)) {
      Serial.println("Correct!");
      signalCorrectPassword();
    } else {
      Serial.println("Nop!!");
      signalWrongPassword();
      resetPassCodeValues();
    }
  }
  charTracker++;
}

//use this function to keep track of resets.. tries..
void resetPassCodeValues() {
  charTracker = 0;
  scanForInit();
}

bool validateCode(char inputArray[4]) {
  for (int i = 0; i < 4; i++) {
    if (inputArray[i] != setPassCode[i]) {
      return false;
    }
  }

  return true;
}

char cordsToChar(int cords[2]) {
  if (cords[0] == 1 && cords[1] == 1) {
    Serial.println("1");
    return '1';
  } else if (cords[0] == 1 && cords[1] == 2) {
    Serial.println("2");
    return '2';
  } else if (cords[0] == 1 && cords[1] == 3) {
    Serial.println("3");
    return '3';
  } else if (cords[0] == 1 && cords[1] == 4) {
    Serial.println("A");
    return 'A';
  } else if (cords[0] == 2 && cords[1] == 1) {
    Serial.println("4");
    return '4';
  } else if (cords[0] == 2 && cords[1] == 2) {
    Serial.println("5");
    return '5';
  } else if (cords[0] == 2 && cords[1] == 3) {
    Serial.println("6");
    return '6';
  } else if (cords[0] == 2 && cords[1] == 4) {
    Serial.println("B");
    return 'B';
  } else if (cords[0] == 3 && cords[1] == 1) {
    Serial.println("7");
    return '7';
  } else if (cords[0] == 3 && cords[1] == 2) {
    Serial.println("8");
    return '8';
  } else if (cords[0] == 3 && cords[1] == 3) {
    Serial.println("9");
    return '9';
  } else if (cords[0] == 3 && cords[1] == 4) {
    Serial.println("C");
    return 'C';
  } else if (cords[0] == 4 && cords[1] == 1) {
    Serial.println("*");
    return '*';
  } else if (cords[0] == 4 && cords[1] == 2) {
    Serial.println("0");
    return '0';
  } else if (cords[0] == 4 && cords[1] == 3) {
    Serial.println("#");
    return '#';
  } else if (cords[0] == 4 && cords[1] == 4) {
    Serial.println("D");
    return 'D';
  }
  // Default case if no match is found
  return '?';
}


bool scanForInit() {
  int targetRow = 1;
  int targetCol = 4;
  int row;
  int col;

  Serial.print("Keypad is ready. Attempt: ");
  Serial.print(attemptTracker);
  Serial.println("/3");
  Serial.println("Press A to Enter Code");

  if (attemptTracker == 3) {
    while (true) {
      Serial.println("Blocked!!");
      blockedLedPattern();
    }
  }

  attemptTracker++;

  while (true) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(rows[i], LOW);
      for (int j = 0; j < 4; j++) {
        if (digitalRead(cols[j]) == LOW) {
          row = i + 1;
          col = j + 1;
          delay(200);  // Simple debounce
          if (row == targetRow && col == targetCol) {
            Serial.println("Enter Code:");
            scanForMatrixInput();
            row = 0;
            col = 0;
          }
        }
      }
      digitalWrite(rows[i], HIGH);  // Set the row back to HIGH
    }
  }
}

// LED STATES //

//Blocked
void blockedLedPattern() {
  digitalWrite(red, 255);
  analogWrite(buzzer, 255);
  tone(buzzer, 400, 300);
  delay(200);
  digitalWrite(red, 0);
  delay(500);
}

//Wrong Passcode
void signalWrongPassword() {
  digitalWrite(blue, 0);
  tone(buzzer, 300, 500);
  for (int i = 0; i < 3; i++) {
    digitalWrite(red, 255);
    delay(200);
    digitalWrite(red, 0);
    delay(200);
  }
}

//Correct
void signalCorrectPassword() {
  digitalWrite(blue, 0);
  tone(buzzer, 1500, 150);

  for (int i = 0; i < 3; i++) {
    digitalWrite(green, 225);
    delay(200);
    digitalWrite(green, 0);
    delay(200);
  }

  attemptTracker = 0;
  openDoor();
  resetPassCodeValues();
}

//Idle
void signalIdleLed() {
  digitalWrite(blue, 255);
}

void openDoor() {

  Serial.println("Opening...!");

  for (int i = 0; i < 90; i++) {
    myServo.write(i);
    digitalWrite(blue, 255);
    delay(100);
    digitalWrite(blue, 0);
    delay(100);
  }

  digitalWrite(green, 255);
  delay(5000);
  digitalWrite(green, 0);
  Serial.println("Closing...");

  for (int i = 90; i > 0; i--) {
    myServo.write(i);
    digitalWrite(red, 255);
    delay(100);
    digitalWrite(red, 0);
    delay(100);
  }
}


void loop() {
  scanForInit();
}

