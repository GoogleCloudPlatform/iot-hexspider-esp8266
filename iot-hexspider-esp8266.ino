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
#define HEXBUG_FULL_ROTATION 18

#include "hexbug_spider.h"
#include "backoff.h"
#include "cli.h"
#define DEVICE1
//#define DEVICE2
//#define DEVICE3
//#define DEVICE4
//#define HWTEST
#include "blinky.h"
// Forward declare callback
void spiderMessage(String &topic, String &payload);
#include "esp8266_mqtt.h"
#include "ranger.h"

#define IR_LED 4 // ESP8266 GPIO pin to use. Recommended: 4 (D2).

bool blinkOn = false;
void waitForEcho() {
  int firstRead;
  unsigned long lastTime = millis();
  firstRead = getRangeCm();
  //Serial.println(firstRead);

  delay(250);
  boolean keepGoing = true;
  while (keepGoing) {
    int lastRead = getRangeCm();
    //Serial.println(lastRead);
    if (millis() - lastTime > 1000){
      blinkOn = blinkOn ? false : true;
      if(hasStrip) { // defined in blinky
        if (blinkOn) {
          strip.setPixelColor(0, color);
        } else {
          strip.setPixelColor(0, 0);
        }
        strip.show();
      }
      digitalWrite(LED_BUILTIN, blinkOn ? HIGH : LOW);
      lastTime = millis();
    }
    if (abs(lastRead - firstRead) > 30){
      Serial.println(String(abs(lastRead - firstRead)));
      keepGoing = false;
    }
    delay(0);
  }
}

// NOTE: Uses waits, only to be used in boot animations, etc.
void blinkTimes(int times) {
  for (int i=0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    if(hasStrip) { // defined in blinky
      strip.setPixelColor(0, color);
      strip.show();
    }
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    if(hasStrip) {
      strip.setPixelColor(0, 0);
      strip.show();
    }
    delay(250);
  }
}

boolean rangeWait = false; // block config refresh until range changes
boolean booted = false;
void setup() {
  Serial.begin(115200);

  delay(1000);
  Serial.println("Hello, waiting...");
  delay(5000);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize hardware
  if (hasStrip) { // used for blinken
    blinkySetup();
  }
  blinkTimes(3);
  delay(500);
  setupRanger();
  blinkTimes(2);
  delay(500);
  hexbug_spider_setup_pin(IR_LED);

  #ifdef HWTEST
  while(true) {
    testHardware();
    delay(1000);
  }
  return;
  #endif

  // Setting up WIFI
  blinkTimes(1);
  setupCloudIoT(); // Creates globals for MQTT
  pinMode(LED_BUILTIN, OUTPUT);

  startMQTT();

  // Donion rings
  blinkTimes(4);
}

bool configChange = false; // Changes on version increments
int lastSeen = 0;
int hexConfig = -1;
int getHexConfig () {
  return hexConfig;
}
void spiderMessage(String& topic, String& payload){
  Serial.println("YOU HAVE A MESSage! - " + topic + ": " + payload );
  configChange = true;

  // TODO: Enum
  hexConfig = payload.toInt();
}


// Send a reading from the ranger to the cloud
int scanId = 1;
void postRange() {
  String data = "{\"deviceId\": \"" + String(device_id)
      + "\", \"time\": " + String(time(nullptr)) + ", \"range\": \"" +
      String(getRangeCm(), DEC) + " cm\", \"scanId\": \""+ String(scanId) +
      "\", \"scanAngle\": \"" + String(HEXBUG_ROT % 360) + " deg\"}";
  publishTelemetry(data);
  Serial.println(data);
}

// Test surroundings
bool scanDone = true;
void scan() {
  Serial.println("scan");
  if (!scanDone) {
    for (int i=0; i < 6; i++) {
      hexbug_spider_spin(20);
      postRange();
      delay(200);
      mqttClient->loop(); // Keep MQTT connection alive
    }
  }
  scanDone = true; // Only do one scan per config change
  scanId++;
}

int moveConfig = -1;
void bootLoop(){
  // Get initial config/version
  moveConfig = getHexConfig();
  Serial.println("Boot config: " + String(moveConfig));
  hexbug_spider_spin(-20);
  hexbug_spider_spin(20);

  configChange = false; // explicitly disable handling of initial command
  booted = true;
}

// TODO: Enum for configuration / mode
//       Would like dance / swoop modes
void processConfig(int conf) {
  switch (conf) {
    case 1:
      scanDone = false;
      scan();
      break;
    case 2: // FW
      hexbug_spider_advance(10);
      break;
    case 3: // Right
      hexbug_spider_spin(20);
      break;
    case 4: // Back
      hexbug_spider_advance(-5);
      break;
    case 5: // Left
      hexbug_spider_spin(-20);
      break;
    default:
      // STOP!
      break;
  }
}

int configFreq = 1000;
int telemFreq = 100;
unsigned long lastTime = millis();
unsigned long lastConfig = millis();
unsigned long lastTelemetry = millis();
void loop() {
  unsigned long currTime = millis();

  // UI First
  if (!booted){
    bootLoop();
  }
  if (hasStrip){
    blinkyLoop(moveConfig);
  }
  if (!mqttClient->connected()) {
    connect();
  }
  mqttClient->loop();

  // Serial command interface
  //cliLoop();

  // Update configuration
  // TODO: Fix logic to backoff, currently will let connection fail
  int newConfig = getHexConfig();
  if (newConfig != moveConfig) {
    configChange = true; // TODO: tihs moves to version
  }
  if (newConfig != 0){
    // TODO: resetBackoff(); // Done in MQTT?
    moveConfig = newConfig;
    // Too verbose!
    // Serial.println("Device config now: " + String(newConfig));
    blinkyLoop(newConfig);
  }
  lastConfig = currTime;

  // Update position
  if (configChange) {
    processConfig(moveConfig);
    configChange = false; // remove to continuously process config
  } else {
    // Wait for sensor input to change
    if (rangeWait) {
      waitForEcho();
    }
  }
}

// TRIGGER the things
void testHardware() {
  Serial.println("tick");
  hexbug_spider_spin_right();
  Serial.println(getRangeCm());
  if (hasStrip) {
    blinkyLoop(0);
  }
  delay(100);
}
