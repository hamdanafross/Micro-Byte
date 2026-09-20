// ============================================================
//  BYTE — an AI creature that lives on your desk
//  MAX7219 8x8 matrix face + HC-SR04 presence + Buzzer
//
//  Wiring:
//    MAX7219  DIN → D2 | CLK → D3 | CS  → D4
//    HC-SR04  TRIG→ D9 | ECHO→ D10
//    Buzzer   +  → D7 | −   → GND
// ============================================================

#include <SPI.h>
#include <MD_MAX72xx.h>

// ── Hardware config ─────────────────────────────────────────
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES   1
#define CLK_PIN       3
#define DATA_PIN      2
#define CS_PIN        4
#define TRIG_PIN      9
#define ECHO_PIN      10
#define BUZZER_PIN    7

MD_MAX72XX mx(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ── Stats (0–100) ────────────────────────────────────────────
int hunger = 100;
int happy  = 100;
int energy = 100;

// ── Mood states ──────────────────────────────────────────────
enum Mood { HAPPY, SAD, ANGRY, SLEEPY, LOVE, SURPRISED, DEAD };
Mood currentMood = HAPPY;
Mood lastMood    = HAPPY;

// ── Timing ───────────────────────────────────────────────────
unsigned long lastStatDrain = 0;
unsigned long lastBlink     = 0;
unsigned long lastPresence  = 0;
unsigned long moodStartTime = 0;
bool blinking               = false;
bool wasPresent             = false;

// ── Pixel faces — 8 bytes, each byte = one row, MSB = left ──
//    Read each row left→right: 1=LED on, 0=LED off
const uint8_t FACE_HAPPY[8] PROGMEM = {
  0b00000000,
  0b00100100,
  0b00100100,
  0b00000000,
  0b01000010,
  0b00100100,
  0b00011000,
  0b00000000
};
const uint8_t FACE_SAD[8] PROGMEM = {
  0b00000000,
  0b00100100,
  0b00100100,
  0b00000000,
  0b00011000,
  0b00100100,
  0b01000010,
  0b00000000
};
const uint8_t FACE_ANGRY[8] PROGMEM = {
  0b01000010,
  0b00100100,
  0b00100100,
  0b00000000,
  0b00000000,
  0b00011000,
  0b00100100,
  0b01000010
};
const uint8_t FACE_SLEEPY[8] PROGMEM = {
  0b00000000,
  0b00000000,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00011000,
  0b00100100,
  0b00000000
};
const uint8_t FACE_LOVE[8] PROGMEM = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};
const uint8_t FACE_SURPRISED[8] PROGMEM = {
  0b00100100,
  0b01100110,
  0b00100100,
  0b00000000,
  0b00011000,
  0b00100100,
  0b00100100,
  0b00011000
};
const uint8_t FACE_DEAD[8] PROGMEM = {
  0b01000010,
  0b00100100,
  0b01000010,
  0b00000000,
  0b00000000,
  0b01111110,
  0b00000000,
  0b00000000
};
const uint8_t FACE_BLINK[8] PROGMEM = {
  0b00000000,
  0b00000000,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00011000,
  0b00100100,
  0b00000000
};

// ── Helper: display a PROGMEM face ───────────────────────────
void showFace(const uint8_t* face) {
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t r = 0; r < 8; r++) {
    mx.setRow(0, r, pgm_read_byte(&face[r]));
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

// ── HC-SR04: get distance in cm ─────────────────────────────
uint16_t getDistance() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH, 20000);
  if (dur == 0) return 999;
  return dur / 58;
}

// ── Sounds ───────────────────────────────────────────────────
void soundHappy() {
  tone(BUZZER_PIN, 880, 80); delay(100);
  tone(BUZZER_PIN, 1046, 80); delay(100);
  tone(BUZZER_PIN, 1318, 150); delay(200);
  noTone(BUZZER_PIN);
}
void soundSad() {
  tone(BUZZER_PIN, 440, 200); delay(220);
  tone(BUZZER_PIN, 330, 300); delay(320);
  noTone(BUZZER_PIN);
}
void soundAngry() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 200, 60); delay(80);
  }
  noTone(BUZZER_PIN);
}
void soundPurr() {
  for (int i = 0; i < 6; i++) {
    tone(BUZZER_PIN, 180 + (i % 2) * 20, 40); delay(50);
  }
  noTone(BUZZER_PIN);
}
void soundWakeUp() {
  tone(BUZZER_PIN, 600, 100); delay(120);
  tone(BUZZER_PIN, 800, 100); delay(120);
  tone(BUZZER_PIN, 1000, 150); delay(200);
  noTone(BUZZER_PIN);
}
void soundDead() {
  tone(BUZZER_PIN, 400, 500); delay(520);
  tone(BUZZER_PIN, 300, 500); delay(520);
  tone(BUZZER_PIN, 200, 800); delay(900);
  noTone(BUZZER_PIN);
}
void soundLove() {
  tone(BUZZER_PIN, 523, 100); delay(110);
  tone(BUZZER_PIN, 659, 100); delay(110);
  tone(BUZZER_PIN, 784, 100); delay(110);
  tone(BUZZER_PIN, 1047, 200); delay(250);
  noTone(BUZZER_PIN);
}

// ── Pick mood from current stats ─────────────────────────────
Mood computeMood() {
  if (hunger <= 0 && happy <= 0 && energy <= 0) return DEAD;
  if (energy < 20)  return SLEEPY;
  if (hunger < 20)  return SAD;
  if (happy < 20)   return ANGRY;
  if (happy > 90 && hunger > 80) return HAPPY;
  return HAPPY;
}

// ── Render current mood face, handle blinking ────────────────
void renderFace() {
  const uint8_t* face;
  switch (currentMood) {
    case HAPPY:     face = FACE_HAPPY;     break;
    case SAD:       face = FACE_SAD;       break;
    case ANGRY:     face = FACE_ANGRY;     break;
    case SLEEPY:    face = FACE_SLEEPY;    break;
    case LOVE:      face = FACE_LOVE;      break;
    case SURPRISED: face = FACE_SURPRISED; break;
    default:        face = FACE_DEAD;      break;
  }
  if (blinking && currentMood != SLEEPY && currentMood != DEAD) {
    showFace(FACE_BLINK);
  } else {
    showFace(face);
  }
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 8);  // brightness 0–15
  mx.clear();

  // Wake-up animation: scroll face in
  soundWakeUp();
  showFace(FACE_SURPRISED);
  delay(600);
  showFace(FACE_HAPPY);
  delay(400);

  lastStatDrain = millis();
  moodStartTime = millis();
}

// ── Main loop ─────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // ── 1. Drain stats every 10 seconds ───────────────────────
  if (now - lastStatDrain > 10000) {
    lastStatDrain = now;
    hunger = max(0, hunger - 3);
    happy  = max(0, happy  - 2);
    energy = max(0, energy - 2);

    Serial.print("Hunger:"); Serial.print(hunger);
    Serial.print(" Happy:"); Serial.print(happy);
    Serial.print(" Energy:"); Serial.println(energy);
  }

  // ── 2. Presence detection ─────────────────────────────────
  uint16_t dist = getDistance();
  bool present  = (dist < 25);   // hand within 25 cm = petting

  if (present && !wasPresent) {
    // Someone just approached — react!
    lastPresence = now;
    if (currentMood == DEAD) {
      // Can't be revived
    } else if (currentMood == SLEEPY) {
      // Woken up
      energy = min(100, energy + 15);
      currentMood = SURPRISED;
      showFace(FACE_SURPRISED);
      soundWakeUp();
      delay(800);
    } else {
      // Being petted — boost happiness
      happy  = min(100, happy  + 20);
      hunger = min(100, hunger + 5);
      currentMood = LOVE;
      showFace(FACE_LOVE);
      soundLove();
      delay(1000);
    }
  }

  // ── 3. Continuous petting boost ───────────────────────────
  if (present && (now - lastPresence > 3000)) {
    happy = min(100, happy + 1);
    soundPurr();
  }

  wasPresent = present;

  // ── 4. Recompute mood from stats (unless in LOVE state) ───
  Mood newMood = computeMood();
  if (currentMood != LOVE || (now - moodStartTime > 2000)) {
    if (newMood != currentMood && currentMood != DEAD) {
      currentMood = newMood;
      moodStartTime = now;
      // Play mood transition sound
      switch (currentMood) {
        case HAPPY:  soundHappy(); break;
        case SAD:    soundSad();   break;
        case ANGRY:  soundAngry(); break;
        case DEAD:   soundDead();  break;
        default: break;
      }
    }
  }

  // ── 5. Blink every ~4 seconds for 120ms ───────────────────
  if (!blinking && (now - lastBlink > 4000)) {
    blinking = true;
    lastBlink = now;
  }
  if (blinking && (now - lastBlink > 120)) {
    blinking = false;
  }

  // ── 6. Draw the face ──────────────────────────────────────
  renderFace();

  // ── 7. Idle breathing — gentle brightness pulse ───────────
  if (currentMood == SLEEPY) {
    int brightness = 2 + (int)(6.0 * abs(sin(now / 1500.0)));
    mx.control(MD_MAX72XX::INTENSITY, brightness);
  } else {
    mx.control(MD_MAX72XX::INTENSITY, 8);
  }

  delay(50);
}
