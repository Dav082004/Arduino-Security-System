# Arduino-Security-System

This project is an Arduino-based security system that uses ultrasonic sensors, a PIR (passive infrared) sensor, a buzzer, an LCD screen, and other electronic components to monitor and secure a specific area.

## Features

- **Motion Detection:** Utilizes a PIR sensor and an ultrasonic sensor to detect movement in the monitored area.
- **Security Code System:** Requires the user to input a 4-digit security code when movement is detected.
- **Alarm:** If the incorrect code is entered, the system sounds an alert and locks out new attempts for 10 seconds after 3 failed tries.
- **LCD Interface:** Displays system status and user instructions on an LCD screen.
- **LDR for Light Detection:** Activates the system only when the ambient light level is low.
- **LED Indicators:** LEDs show whether the system is active or inactive.

## Components

- Arduino UNO or similar
- HC-SR04 Ultrasonic Sensor
- PIR Sensor
- Buzzer
- I2C-compatible LCD Screen
- Light Dependent Resistor (LDR)
- Two LEDs
- Reset Button

## Setup

1. Connect the components to the Arduino according to the pin definitions in the code.
2. Upload the code to the Arduino using the Arduino IDE.
3. Adjust the values in the code as needed (e.g., the LDR threshold).
4. Power your Arduino and observe the system's behavior.

## Usage

1. The system activates when it detects low light levels.
2. If motion is detected, the system prompts the user for a security code.
3. If the correct code is entered, the system deactivates.
4. If the wrong code is entered, an alarm will sound, and after 3 failed attempts, the system will lock new entries for 10 seconds.
5. Use the reset button to reactivate the system.

## Notes

- Make sure to adjust the I2C address for your LCD screen in the code if necessary.
- This project is a basic implementation and can be enhanced with additional features such as wireless connectivity or logging capabilities.

## Contributions

Contributions are welcome! Please fork the repository and submit pull requests for any enhancements or bug fixes.
