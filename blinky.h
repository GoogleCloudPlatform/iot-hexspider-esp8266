/******************************************************************************
 * Copyright 2018 Google LLC
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
// LED control code
#include <Adafruit_DotStar.h>

#define DATAPIN    13 // D7
#define CLOCKPIN   12 // D6

// TODO: Everybody gets one, should they?
Adafruit_DotStar strip = Adafruit_DotStar(1, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Give specific devices colors, etc...
#ifdef DEVICE1 // BLINKY!
boolean hasStrip = true;
int color = 0xFF0000;
#else
  #ifdef DEVICE4 // CLYDE!
  boolean hasStrip = true;
  int color = 0xF6BA64;
  #else
    #ifdef DEVICE2 // PINKY!
    boolean hasStrip = true;
    int color = 0xF5BCFB;
    #else
    boolean hasStrip = false;
    int color = 0;
    #endif
  #endif
#endif
void blinkySetup() {
  if (hasStrip) {
    strip.begin(); // Initialize pins for output
    strip.setPixelColor(0, color);
    strip.setBrightness(80);
    strip.show();  // Turn all LEDs off ASAP  }
  }
}


// Does things based on configuration, should be non blocking.
// This means, NO DELAYS, nerds!
unsigned long lastDraw = millis();
int refresh = 150;
byte brightness = 0;
boolean onOff = false;
void blinkyLoop(int currConfig) {
  if (millis() - lastDraw < refresh || !hasStrip) { // DFWM
    return;
  }

  // Drawn at refresh interval
  int color = 0;
  switch (currConfig) {
    case 0:
      color = strip.Color(255, 0, 0);
      break;
    case 1:
      color = strip.Color(0, 255, 0);
      break;
    case 2:
      color = strip.Color(0, 0, 255);
      break;
    case 3:
      color = strip.Color(0, 255, 255);
      break;
    case 4:
      color = strip.Color(255, 0, 255);
      break;
    case 5:
      color = strip.Color(255, 255, 0);
      break;
    default:
      color = 0;
      break;
  }

  onOff = !onOff;
  strip.setPixelColor(0, onOff ? color : 0);
  strip.show();
  // Too verbose!
  //Serial.println(String(color, HEX));

  // TODO: fade?
  /*
  byte brightnessAmt = brightness * 3 + 25;
  if (brightness > 32) {
    brightnessAmt = (64-brightness) * 3;
  }

  strip.setBrightness(brightnessAmt > 32 ? 60 : 0);

  brightness = brightness + 1;
  if (brightness == 64) {
    brightness = 0;
  }
  */
  lastDraw = millis();
}

