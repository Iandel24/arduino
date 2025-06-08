// Simple Arduino minigames using one joystick, two LEDs and a button
// Game 1: Reaction Game - move the joystick toward the LED that lights up as fast as possible
// Game 2: Simon Game - remember a sequence of LEDs and reproduce it with the joystick

const int LED_LEFT = 4;     // Digital pin for left LED
const int LED_RIGHT = 5;    // Digital pin for right LED
const int BUTTON_PIN = 2;   // Push button pin
const int JOY_X_PIN = A0;   // Joystick X-axis
const int JOY_THRESHOLD = 400; // Threshold for detecting left/right

enum GameState { WAIT_START, REACTION, SIMON };
GameState state = WAIT_START;

const int MAX_SEQUENCE = 8;
bool simonSeq[MAX_SEQUENCE];
int simonLen = 0;
int simonIndex = 0;
bool showingSimon = false;

void setup() {
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  randomSeed(analogRead(A1));
}

void loop() {
  switch (state) {
    case WAIT_START:
      if (buttonPressed()) {
        state = REACTION;
        Serial.println("Reaction game!");
        delay(500);
      }
      break;
    case REACTION:
      playReaction();
      break;
    case SIMON:
      playSimon();
      break;
  }
}

bool buttonPressed() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(20); // debounce
    while (digitalRead(BUTTON_PIN) == LOW) {}
    return true;
  }
  return false;
}

void playReaction() {
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
  delay(random(1000, 3000));
  bool left = random(2) == 0;
  unsigned long start;
  if (left) {
    digitalWrite(LED_LEFT, HIGH);
    start = millis();
    while (analogRead(JOY_X_PIN) > JOY_THRESHOLD) {
      if (buttonPressed()) { state = SIMON; return; }
    }
    Serial.print("Time: ");
    Serial.println(millis() - start);
    digitalWrite(LED_LEFT, LOW);
  } else {
    digitalWrite(LED_RIGHT, HIGH);
    start = millis();
    while (analogRead(JOY_X_PIN) < 1023 - JOY_THRESHOLD) {
      if (buttonPressed()) { state = SIMON; return; }
    }
    Serial.print("Time: ");
    Serial.println(millis() - start);
    digitalWrite(LED_RIGHT, LOW);
  }
  delay(500);
}

void startSimon() {
  simonLen = 1;
  simonSeq[0] = random(2);
  showingSimon = true;
}

void showSimon() {
  for (int i = 0; i < simonLen; i++) {
    if (simonSeq[i]) digitalWrite(LED_LEFT, HIGH);
    else digitalWrite(LED_RIGHT, HIGH);
    delay(400);
    digitalWrite(LED_LEFT, LOW);
    digitalWrite(LED_RIGHT, LOW);
    delay(150);
  }
  showingSimon = false;
  simonIndex = 0;
}

void playSimon() {
  if (showingSimon) {
    showSimon();
    return;
  }
  if (simonLen == 0) startSimon();

  int x = analogRead(JOY_X_PIN);
  if (x < JOY_THRESHOLD) {
    digitalWrite(LED_LEFT, HIGH);
    delay(200);
    digitalWrite(LED_LEFT, LOW);
    if (simonSeq[simonIndex]) {
      simonIndex++;
    } else {
      Serial.println("Wrong!");
      simonLen = 0;
      delay(500);
    }
  } else if (x > 1023 - JOY_THRESHOLD) {
    digitalWrite(LED_RIGHT, HIGH);
    delay(200);
    digitalWrite(LED_RIGHT, LOW);
    if (!simonSeq[simonIndex]) {
      simonIndex++;
    } else {
      Serial.println("Wrong!");
      simonLen = 0;
      delay(500);
    }
  }

  if (simonLen > 0 && simonIndex >= simonLen) {
    if (simonLen < MAX_SEQUENCE) {
      simonSeq[simonLen] = random(2);
      simonLen++;
      showingSimon = true;
      delay(500);
    } else {
      Serial.println("Simon complete!");
      simonLen = 0;
      showingSimon = true;
      delay(1000);
    }
  }

  if (buttonPressed()) {
    state = WAIT_START;
    simonLen = 0;
    delay(500);
  }
}

