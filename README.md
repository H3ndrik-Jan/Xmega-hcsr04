# Xmega-hcsr04
Xmega library for hc-sr04 ultrasonic sensor. Uses xmega inputcapture

## Description:
This code can be used to measure distance from an object
using an HC-SR04 ultrasonic sensor. It works by sending a
11us pulse to the trigger pin. This will start a measurement,
and when the measurement is done, the echo pin will turn high
for a time that can be mapped to a distance in centimeters.

Code is derived from example code found in this great book: https://www.dolman-wim.nl/xmega/index.php

