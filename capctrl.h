#ifndef CAPCTRL_H
#define CAPCTRL_H

#include <Arduino.h>
#include <Stepper.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

class CapCtrl 
{
  struct CalPoint 
  {
    int pos;
    int freqKhz; 
  } CalPoint_t;
  
public:
  CapCtrl(int steps, int pin1, int pin2, int pin3, int pin4, int pinBtn);
  ~CapCtrl();
  
  bool setFreq(long freqKhz);
  int getPos() const { return pos_; }

  bool calStore(int freqKhz);
  void calSave();
  void calPrint(SoftwareSerial serial);
  
  void park();
  void up();
  void upLarge();
  void down();
  void downLarge();

private:
  bool setPos(int newPos);
  void releaseMotor();
  
  void calLoad();
  bool calMove(int index);
  bool calStore(int index, int freqKhz);

private:
  const int ConfigSpeed = 64;
  const int ConfigStep = 10;
  const int ConfigStepLarge = 100;
  const int ConfigMaxPos = 5300;

  const int ConfigCalPoints = 23;
  const int ConfigCalAddr = 0x0;

private:
  int pos_;
  Stepper stepper_;
  int pin1_, pin2_, pin3_, pin4_, pinBtn_;

  static CalPoint calPoints_[];
};

#endif // CAPCTRL_H
