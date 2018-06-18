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
#ifndef __RANGER_H__
#define __RANGER_H__
// Ultrasonic Ranger code

#define TRIGGER 5 // GPIO5 || D1
#define ECHO    15 // GPIO15 || D8

void setupRanger() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

int getRangeCm() {
  long duration;
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  return ((duration/2) / 29.1);
}
#endif
