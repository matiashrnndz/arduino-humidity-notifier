# email_humidity_notifier

## Materials

- Arduino UNO R3 (x1)
- Arduino Ethernet Shield W5100 (x1)
- Buttons (x2)
- LED (x1)
- Humidity Sensor (x1)
- LCD Display 16x2 (x1)
- Resistance 1K Ohm (x2)
- Resistance 220 Ohm (x2)
- Potenciometer (x1)
- Wire M-F (x6)
- Wire M-M (x31)
- Wire F-F (x5)
- Ethernet Cable (x1)
- Small Protoboard (x1)
- Large Protoboard (x1)
- USB type B (x1)

## Functions

- 100% humidity was determined, when the sensor returns 0 as a value.
- 0% humidity was determined, when the sensor is outdoors.
- The current humidity percentage captured by the humidity sensor is displayed (every 1 second) on the LCD screen.
- Using the red button, you can turn on or off email notifications.
- Using the white button, you can rotate between being notified when the sensor determines 25% humidity (or less) or 50% humidity (or less).
- Through the LED, you can see that when it is on, it means that the notifications are disabled.
- Once a notification was sent by mail (because the sensor reached a level of humidity lower than acceptable by the customer), so that another email is sent notification, the sensor needs to have a higher level of humidity than acceptable (for example after it has been watered).
