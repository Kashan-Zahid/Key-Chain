#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Initialize U8g2 for a 128x64 or 128x32 I2C OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Pet States
enum PetState {
  IDLE,
  WALKING,
  SLEEPING
};

PetState currentPetState = IDLE;
unsigned long lastStateChange = 0;
const long stateInterval = 5000; // Switch states every 5 seconds

// Simple pixel-art bitmap arrays for your pet
const uint8_t pet_idle_bits[] = { 
  0x3c, 0x7e, 0xff, 0xdb, 0xff, 0xff, 0x5a, 0xa5 
};

const uint8_t pet_walk_bits[] = { 
  0x3c, 0x7e, 0xff, 0xdb, 0xff, 0xff, 0x66, 0x33 
};

void setup() {
  // Explicitly map I2C to your schematic pins: SDA = D4 (GPIO 6), SCL = D5 (GPIO 7)
  Wire.begin(6, 7); 
  
  u8g2.begin();
  u8g2.setContrast(255);
}

void loop() {
  unsigned long currentMillis = millis();

  // Autonomous state machine switching behavior over time
  if (currentMillis - lastStateChange > stateInterval) {
    lastStateChange = currentMillis;
    if (currentPetState == IDLE) {
      currentPetState = WALKING;
    } else if (currentPetState == WALKING) {
      currentPetState = SLEEPING;
    } else {
      currentPetState = IDLE;
    }
  }

  // Render graphics frame
  u8g2.clearBuffer();

  // Draw UI Header
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.setCursor(2, 10);
  u8g2.print("PET: ZOT");

  // Draw pet based on current state
  switch (currentPetState) {
    case IDLE:
      u8g2.drawXBMP(56, 24, 8, 8, pet_idle_bits);
      u8g2.setCursor(40, 50);
      u8g2.print("Chill");
      break;

    case WALKING:
      {
        int xPos = 50 + (int)((currentMillis / 200) % 15);
        u8g2.drawXBMP(xPos, 24, 8, 8, pet_walk_bits);
        u8g2.setCursor(40, 50);
        u8g2.print("Walking");
      }
      break;

    case SLEEPING:
      u8g2.setCursor(50, 30);
      u8g2.print("Zzz...");
      break;
  }

  u8g2.sendBuffer();
  delay(50);
}