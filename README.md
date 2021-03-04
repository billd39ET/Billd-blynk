# Billd-blynk

Simple project that combines a number of widgets, sensor, display, WiFi Manager and Blynk. I’ve used it at work to monitor server room temp when we had a dodgy CRAC unit. (We now have networked monitoring/alerts, but this little beast with Blynk worked perfectly!)

Battery or USB power, Temperature, Humidity & Atmospheric Pressure sensor with variable alarm threshold and Blynk alert. WiFi Manager to allow connection to unknown networks (mobility) without having to re-flash the device.

I got to this point by learning the basics of each hardware and software component then gradually adding them together . . . for me learning and UNDERSTANDING the basics is always much more fulfilling than cut/paste chunks of code or replicating complex projects without really understanding what’s happening.

Wemos D1R2 Mini (fantastic cheap, little devices with huge capability for their size and price)
BME280 Temperature, Humidity & Atmospheric Pressure sensor (beware, most online sellers describe the BME280 for about US$1.50 . . .it’s a BMP280 that does NOT measure Humidity . . . The BME280 that DOES measure Humidity is about US$3.50 . . . the price is always the giveaway . . .)
128x64 pix SSD1306 OLED Display - I2C
Wemos Mini power supply Shield (to allow for 9V battery power)
Wemos Mini breakout to allow multiple I2C pins (BME280 & OLED)

It’s mobile so the WiFi Manager allows to easily connect to any new WiFi network.

The numeric input widget allows for a Temperature threshold, an alert is sent of temperature is above the set level.
I added the display later because I have a few on hand and if you are near by it saves opening the app if you cab see the device.

Also incorporates Arduino OTA for easy updating/changes.

Not rocket science but a nice project to utilise a number of very handy features . . . and when you show non-geeks they think you are Stephen J Hawking! :wink:
