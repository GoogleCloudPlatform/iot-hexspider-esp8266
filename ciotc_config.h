/******************************************************************************
 * Copyright 2018 Google
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
// This file contains your configuration used to connect to Cloud IoT Core

// WIFI
const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-pass";

// Cloud iot details.
const char* project_id = "your-project-id";
const char* location = "us-central1"; // replace with your region
const char* registry_id = "your-registry-id";

// Originally used four device types / IDs, replace as appropriate.
#ifdef DEVICE1
const char* device_id = "blinky";
#endif

#ifdef DEVICE2
const char* device_id = "pinky";
#endif

#ifdef DEVICE3
const char* device_id = "inky";
#endif

#ifdef DEVICE4
const char* device_id = "clyde";
#endif


// To get the private key run (where private-key.pem is the ec private key
// used to create the certificate uploaded to google cloud iot):
// openssl ec -in <private-key.pem> -noout -text
// and copy priv: part.
const char* private_key_str =
    "b9:2c:01:5a:bd:cf:db:2d:7c:80:72:e8:89:53:e3:"
    "3f:b4:9c:27:ac:ae:65:4c:50:fa:5d:a8:b8:30:76:"
    "ac:89";

// TODO: Use root certificate to verify tls connection rather than using a
// fingerprint.
// To get the fingerprint run
// openssl s_client -connect cloudiotdevice.googleapis.com:443 -cipher <cipher>
// Copy the certificate (all lines between and including ---BEGIN CERTIFICATE---
// and --END CERTIFICATE--) to a.cert. Then to get the fingerprint run
// openssl x509 -noout -fingerprint -sha1 -inform pem -in a.cert
// <cipher> is probably ECDHE-RSA-AES128-GCM-SHA256, but if that doesn't work
// try it with other ciphers obtained by sslscan cloudiotdevice.googleapis.com.
const char* fingerprint =
    "7C:D4:99:11:FE:FC:0D:78:C0:A9:C1:18:52:1D:3F:0B:8B:38:C9:90";
