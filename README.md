# Magnetic loop antenna capacitor controller
Magnetic loop antenna capacitor controller based on 
 - OpenSCAD project to generate custom sliding variable high voltage capacitors with two predefined configurations
 - Arduino Nano
 - L298N stepper driver
 - EM-290 stepper from Epson printer
 - HC-06 bluetooth serial controller for remote control using Bluetooth console application from the phone or PC
 - Generated high voltage 3d printed slide capacitor printed with PETG plastic (up to ~15kV and 130pF)
   - Plate sizes are ~70x55 mm
   - Capacitor dimensions are 60x70x80 mm
   - Plate spacing 5 mm
   - 14 plates
   - Worm gear is M4
   - Can handle 100W CW without arcing (tested with 0.9m loop) and continous 50W operation in FT8
   - Capacitance range ~5-130 pF
   - Can tune ~9-29 MHz with 0.9m loop
 - Second test generated medium voltage 3d printed slide capacitor up to ~6 kV and higher 400pF capacitance

# Serial commands
On successful command execution "ok" is returned when operation was successfully completed or "err" if command was unsuccessful. Next commands are supported:
 - Movements
   - *p*: park to initial 0 position, automatically performed when powered on
   - *u*, *uu*, *uuu*: go/tune up to higher frequency (5/50/500kHz steps)
   - *d*, *dd*, *ddd*: go/tune down to lower frequency (5/50/500kHz steps)
   - *freqkhz*, frequency in kHz where to move capacitor, should be used after calibration procedure completion
 - Calibration
   - *t*: print calibration table
   - *c[freqkhz]*: store current minimum swr position into given calibration frequency cell
   - *s*: save calibration table

# Configuration parameters (capctrl.h)
 - *ConfigSpeed*: stepper speed, default is 64
 - *ConfigStep(5|50|500)kHz*: number of steps for tune up/down commands for 5/50/500kHz steps
 - *ConfigMaxPos*: end position of capacitor, maximum number of steps, default is 5200
 - *ConfigCalPoints*: number of calibration points, default is 23
 - *ConfigCalAddr*: EEPROM start address where calibration data is stored, default is 0x0
 - *ConfigStepCompensate*: number of steps for reverse compensation when changing direction

# Photos

- Large capacitor 15kV/130pF

![alt text](extras/images/magloop.png)

-  Medium size capacitor with lower maximum voltage, but larger capacitance 6kV/400pF

![alt text](extras/images/magloop2.png)

- Test magnetic loop antenna, 1m loop diameter, 40mm tube diameter, fed using inductive coupling from smaller loop and 1:1 balun

![alt text](extras/images/magloop_test.png)

