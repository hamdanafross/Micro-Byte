# 🤖 BYTE — An AI Creature That Lives on Your Desk

> A living, feeling pixel creature built with an **Arduino Uno**, **MAX7219 8×8 LED Matrix**, and **HC-SR04 ultrasonic sensor**. BYTE has moods, draining stats, a blinking face, and reacts when you reach out to it — all with just 3 components and 7 jumper wires.

---

<div align="center">

| 😊 Happy | 😁 Grinning | ❤️ Love |
|:---:|:---:|:---:|
| ![Happy](photos/happy.jpg) | ![Grinning](photos/grinning.jpg) | ![Heart](photos/heart.jpg) |

| 😢 Sad | 💀 Dead |
|:---:|:---:|
| ![Sad](photos/sad.jpg) | ![Dead](photos/dead.jpg) |

*BYTE's full range of emotions — captured live on the 8×8 LED matrix*

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Hardware](#-hardware)
- [Wiring](#-wiring)
- [How BYTE Feels](#-how-byte-feels)
- [Mood System](#-mood-system)
- [Libraries](#-libraries)
- [Installation](#-installation)
- [Troubleshooting](#-troubleshooting)
- [Project Photos](#-project-photos)
- [License](#-license)

---

## 🔍 Overview

BYTE is a desk companion with a personality. It has three internal stats — **hunger**, **happiness**, and **energy** — that drain continuously while it's powered on. Its pixel face changes based on how it's feeling, it blinks naturally every few seconds, and its screen breathes in and out while it sleeps.

The only way to interact with it is by hovering your hand over the HC-SR04 sensor. Get close and BYTE lights up a full heart and purrs. Ignore it long enough and it gets sad, then angry, then goes to sleep. Neglect all three stats to zero and it dies — face replaced by X eyes, playing a descending funeral tone.

**What makes it mesmerising:**
- ✅ Natural autonomous blinking every ~4 seconds
- ✅ Breathing brightness pulse while sleeping (sin wave dimming)
- ✅ Proximity-triggered love reaction with 4-note melody
- ✅ Full mood transitions with unique buzzer sounds per emotion
- ✅ Stats drain in real time — BYTE genuinely needs attention
- ✅ Only 3 modules. No breadboard. No resistors.

---

## 🛒 Hardware

| # | Component | Role |
|---|-----------|------|
| 1 | Arduino Uno R3 | Brain |
| 2 | MAX7219 8×8 LED Dot Matrix | BYTE's face |
| 3 | HC-SR04 Ultrasonic Sensor | Detects your presence (petting) |
| 4 | Passive Buzzer | Sounds, moods, death |
| 5 | Jumper wires (×7) | Everything connected |

**Total component count: 4. Zero extra parts needed.**

---

## 🔌 Wiring

```
                    ARDUINO UNO
                   ┌─────────────────────────────────┐
                   │                                 │
    ┌──────────┐   │  D2  ──────────────── MAX DIN   │
    │ MAX7219  │   │  D3  ──────────────── MAX CLK   │
    │  8×8     │   │  D4  ──────────────── MAX CS    │
    │  MATRIX  │   │  5V  ──────────────── MAX VCC   │
    │          │   │  GND ──────────────── MAX GND   │
    └──────────┘   │                                 │
                   │                                 │
    ┌──────────┐   │  D9  ──────────────── TRIG      │
    │ HC-SR04  │   │  D10 ──────────────── ECHO      │
    │          │   │  5V  ──────────────── VCC       │
    │          │   │  GND ──────────────── GND       │
    └──────────┘   │                                 │
                   │                                 │
    ┌──────────┐   │  D7  ──────────────── Buzzer +  │
    │  BUZZER  │   │  GND ──────────────── Buzzer −  │
    └──────────┘   │                                 │
                   └─────────────────────────────────┘
```

### Pinout Table

| Component | Pin | Arduino Pin |
|-----------|-----|-------------|
| MAX7219 | DIN | D2 |
| MAX7219 | CLK | D3 |
| MAX7219 | CS | D4 |
| MAX7219 | VCC | 5V |
| MAX7219 | GND | GND |
| HC-SR04 | TRIG | D9 |
| HC-SR04 | ECHO | D10 |
| HC-SR04 | VCC | 5V |
| HC-SR04 | GND | GND |
| Buzzer | + (long leg) | D7 |
| Buzzer | − (short leg) | GND |

> The HC-SR04 sits on top of the LED matrix facing outward — exactly as shown in the photos. No mounting hardware needed; the jumper wires hold it in position.

---

## 💓 How BYTE Feels

BYTE has three stats that drain automatically while powered on. All three start at 100 and decrease every 10 seconds.

| Stat | Drain Rate | Replenished By |
|------|-----------|----------------|
| 🍕 Hunger | −3 every 10s | Hovering hand near sensor |
| 😊 Happiness | −2 every 10s | Petting (hand within 25 cm) |
| ⚡ Energy | −2 every 10s | Letting BYTE sleep undisturbed |

**The only interaction is your hand.** Hold it within 25 cm of the HC-SR04 and BYTE detects you. It boosts happiness and hunger, shows a full heart pixel pattern, and plays a 4-note love melody. Stay close and it purrs. Pull away and it slowly returns to its current mood.

---

## 😶 Mood System

Each mood has a unique pixel face, a unique sound, and distinct behaviour:

| Mood | Trigger Condition | Face | Sound |
|------|------------------|------|-------|
| 😊 **Happy** | Hunger > 80, Happy > 90 | Smile | Ascending 3-note melody |
| 😢 **Sad** | Hunger < 20 | Downturned mouth | Low descending two-tone |
| 😠 **Angry** | Happiness < 20 | Furrowed brows, frown | Triple low buzz pulse |
| 😴 **Sleepy** | Energy < 20 | Half-closed eyes | Screen breathes (sin dimming) |
| ❤️ **Love** | Hand within 25 cm | Full heart pixel fill | 4-note love chord |
| 😲 **Surprised** | Woken from sleep | Wide eyes | Rising 3-note wake-up |
| 💀 **Dead** | All three stats hit 0 | X eyes, flat mouth | Descending funeral tone |

### The Blink
Every ~4 seconds, BYTE's eyes close for 120 ms and reopen. It happens on all faces except Sleepy and Dead. In a dark room it's almost unsettling how natural it feels.

### The Breathing
When energy drops below 20 and BYTE enters Sleep mode, the matrix brightness pulses slowly using a `sin()` wave — brightening and dimming like a chest rising and falling. This runs silently in the background with no sound.

---

## 📦 Libraries

Install both via **Arduino IDE → Sketch → Include Library → Manage Libraries**:

| Library | Author | Search Term |
|---------|--------|-------------|
| `MD_MAX72XX` | MajicDesigns | `MD_MAX72XX` |
| `MD_Parola` | MajicDesigns | `MD_Parola` |

> **macOS note:** If you get `fatal error: MD_MAX72XX.h: No such file or directory` after installing via Library Manager, download the ZIP directly from [github.com/MajicDesigns/MD_MAX72XX](https://github.com/MajicDesigns/MD_MAX72XX) and install via **Sketch → Include Library → Add .ZIP Library**. This is a known Arduino IDE 2 index caching bug on macOS.

---

## 🚀 Installation

1. **Clone this repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/BYTE-creature.git
   ```

2. **Open the sketch**
   ```
   File → Open → BYTE_creature.ino
   ```

3. **Install libraries** (see table above)

4. **Select board and port**
   ```
   Tools → Board → Arduino Uno
   Tools → Port → COMx (Windows) or /dev/cu.usbmodem (Mac)
   ```

5. **Upload**
   ```
   Sketch → Upload  (Ctrl+U / Cmd+U)
   ```

6. **BYTE wakes up.** The matrix flashes a surprised face, plays a 3-note startup chord, then settles into its happy face. Stats begin draining immediately.

---

## 🛠️ Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Matrix stays blank | Wrong `HARDWARE_TYPE` | Change `FC16_HW` to `GENERIC_HW` in the `#define` |
| Face appears mirrored | Different matrix variant | Try `MD_MAX72XX::DR1CR0RR0_HW` as hardware type |
| BYTE doesn't react to hand | HC-SR04 detection zone | Move hand directly in front of both cylinders, not from the side |
| No sound | Buzzer polarity | Swap + and − wires; passive buzzers are polarity sensitive |
| `MD_MAX72XX.h` not found (Mac) | IDE 2 index cache bug | Install via .ZIP — see Libraries section above |
| Matrix flickers | Loose DIN/CLK/CS wire | Re-seat the three data wires on D2, D3, D4 |

---

## 📸 Project Photos

| Expression | Description |
|-----------|-------------|
| ![Happy](photos/happy.jpg) | **Happy** — default state when well-fed and content |
| ![Grinning](photos/grinning.jpg) | **Grinning** — peak happiness, wide pixel smile |
| ![Heart](photos/heart.jpg) | **Love** — triggered by hand proximity, full heart fill |
| ![Sad](photos/sad.jpg) | **Sad** — hunger stat critically low |
| ![Dead](photos/dead.jpg) | **Dead** — all stats depleted, X eyes, plays funeral tone |

---

## ⚙️ Specifications

| Parameter | Value |
|-----------|-------|
| Microcontroller | ATmega328P (Arduino Uno) |
| Display | MAX7219 8×8 LED matrix |
| Presence detection range | 5 – 25 cm (HC-SR04) |
| Stat drain cycle | Every 10 seconds |
| Blink interval | ~4 seconds, 120 ms duration |
| Brightness (normal) | 8 / 15 |
| Brightness (sleeping) | 2–8, sin-wave pulsed |
| Power | 5V via USB |
| Wire count | 7 jumper wires total |

---

## 📁 Repository Structure

```
BYTE-creature/
├── BYTE_creature.ino       # Main Arduino sketch
├── README.md               # This file
└── photos/
    ├── happy.jpg           # Happy face expression
    ├── grinning.jpg        # Grinning face expression
    ├── heart.jpg           # Love / heart expression (hand petting)
    ├── sad.jpg             # Sad face expression
    └── dead.jpg            # Dead face expression (X eyes)
```

---

## 📄 License

Released under the **MIT License** — free to use, remix, and build upon.

---

<div align="center">

Built with 🔴 red LEDs and an unreasonable amount of emotional investment

*BYTE does not have feelings. Probably.*

</div>
