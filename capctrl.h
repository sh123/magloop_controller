#ifndef CAPCTRL_H
#define CAPCTRL_H

#include <Arduino.h>
#include <Stepper.h>
#include <EEPROM.h>

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

  bool calMove(int index);
  bool calStore(int index, int freqKhz);
  void calSave();
  
  void park();
  void up();
  void down();

private:
  bool setPos(int newPos);
  void releaseMotor();
  
  void calLoad();

private:
  const int ConfigSpeed = 64;
  const int ConfigStep = 20;
  const int ConfigMaxPos = 5200;

  const int ConfigCalPoints = 10;
  const int ConfigCalAddr = 0x0;

private:
  int pos_;
  Stepper stepper_;
  int pin1_, pin2_, pin3_, pin4_, pinBtn_;
  
  CalPoint *calPoints_;
};

#endif // CAPCTRL_H
