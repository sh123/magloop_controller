# Magnetic loop antenna capacitor controller
Magnetic loop antenna capacitor KISS (keep it simple stupid) controller based on 
 - Arduino Nano
 - L298N stepper driver
 - EM-290 stepper from Epson printer
 - 3d printed slide capacitor printed with PETG plastic
   - Plate sizes are 50x60 mm
   - Plate spacing 5 mm
   - 11 plates
   - Worm gear is M3
   - Can handle 100W CW without arcing (tested with 0.9m loop)
   - Capacitance range ~3-95 pF
   - Can tune from ~9 up to 29 MHz with 0.9m loop

![alt text](images/magloop.png)

# Serial commands
 - *p*: park to initial 0 position, automatically performed when powered on
 - *c*: start calibration procedure, each time capacitor goes into new position you need to type frequency where resonance occurs, calibration is done across 10 points and then linear interpolation is used to find position based on input frequency, calibration points are stored in EEPROM after completion and loaded when powered on
 - *u*: go/tune up to higher frequency
 - *d*: go/tune down to lower frequency
 - *number*, frequency in kHz where to move capacitor, should be used after calibration procedure completion
