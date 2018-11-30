# Hexbug Spider ESP8266 IoT

This sample demonstrates how to use the Hexbug Spider toy with Cloud IoT Core.
It is based on / incorporates parts from the [arduino_hexbug_spider project](https://github.com/xiam/arduino_hexbug_spider).

## Pre-requisites
* Google Cloud Project with Cloud IoT Core enabled
* Hexbug spider and ESP8266 hat

## Getting started
* Build the hat for controlling the robot
* Register the device on the [Google Cloud Console](https://cloud.google.com/console/iot/)
* Configure the device in `ciotc_config.h`
* Flash the device using the Arduino IDE
* Install the root certificate usign the [Arduino SPIFFS Filesystem extension](https://github.com/esp8266/arduino-esp8266fs-plugin)
* When the device connects to WiFi, it will wag its head
* At this point you can send commands from the console or can run the app in the web folder

## Support
* [Cloud IoT Core Arduino JWT library](https://github.com/googlecloudplatform/google-cloud-iot-arduino)

## License

Apache 2.0; see [LICENSE](LICENSE) for details.

## Disclaimer

This project is not an official Google project. It is not supported by Google
and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
