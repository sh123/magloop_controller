#ifndef CAPCTRL_H
#define CAPCTRL_H

#include <Arduino.h>
#include <Stepper.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

typedef void (*t_print)(const String &s);

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
  void calPrint(t_print print);
  
  void park();
  
  void up5kHz();
  void down5kHz();
  void up50kHz();
  void down50kHz();
  void up500kHz();
  void down500kHz();

private:
  bool setPos(int newPos);
  void releaseMotor();
  void compensate(int dir);

  void up(int step, int speed);
  void down(int step, int speed);
 
  void calLoad();
  bool calMove(int index);
  bool calStore(int index, int freqKhz);

private:
  const int ConfigSpeed = 64;
  const int ConfigStep5kHz = 4;
  const int ConfigStep50kHz = 17;
  const int ConfigStep500kHz = 90;
  const int ConfigStepCompensate = 14;
  const int ConfigMaxPos = 5200;
  const int ConfigBtnDisabled = 1;

  const int ConfigCalPoints = 23;
  const int ConfigCalAddr = 0x0;

  const int ConfigDirUp = 1;
  const int ConfigDirNone = 0;
  const int ConfigDirDn = -1;

private:
  int prevDir_;
  int pos_;
  Stepper stepper_;
  int pin1_, pin2_, pin3_, pin4_, pinBtn_;

  static CalPoint calPoints_[];
};

#endif // CAPCTRL_H
