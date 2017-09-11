# ESP32_AP_MY9231
This is a demo using ESP32 to control MY9231 RGB LED.

# Development Environment
* ESP32
* PlatformIO
* Arduino
* Visual Studio code

# Libraries
* my9291 for ESP8266

# Test Tools
* Smart phone
  * iOS - NetAssist
  * Android - any TCP test tool
* Test step
  * Use your smart phone connect to AP 'MyESP***', default key is '12345678'.
  * open NetAssist or other TCP test tools
  * connect to IP "192.168.4.1", port is "80"
  * Send color string "@r,g,b#" ('xx' is between 0 to 255 )
  * Send "@clear#" or "@CLEAR#" to set LED black out
  * Send "@rainbow#" or "@RAINBOW#" to enter rainbow effects.
