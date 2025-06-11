#include "capctrl.h"

struct CapCtrl::CalPoint CapCtrl::calPoints_[] = 
{
  // 30m
  { 1620, 10100 },
  { 1670, 10120 },
  { 1720, 10140 },
  // 20m
  { 2990, 14000 },
  { 3070, 14170 },
  { 3150, 14350 },
  // 17m
  { 3640, 18068 },
  { 3680, 18110 },
  { 3700, 18168 },
  // 15m
  { 3860, 21000 },
  { 3880, 21225 },
  { 3900, 21450 },
  // 12m
  { 4360, 24890 },
  { 4360, 24940 },
  { 4360, 24990 },
  // 11m
  { 4640, 27000 },
  { 4700, 27100 },
  { 4800, 27300 },
  { 4900, 27500 },
  { 4980, 27600 },
  // 10m
  { 5080, 28000 },
  { 5100, 28200 },
  { 5200, 28500 },
};

CapCtrl::CapCtrl(int steps, int pin1, int pin2, int pin3, int pin4, int pinBtn)
  : prevDir_(ConfigDirNone)
  , pos_(0)
  , stepper_(steps, pin1, pin2, pin3, pin4)
  , pin1_(pin1)
  , pin2_(pin2)
  , pin3_(pin3)
  , pin4_(pin4)
  , pinBtn_(pinBtn)
{
  // set default speed and button mode
  stepper_.setSpeed(ConfigSpeed);
  pinMode(pinBtn, INPUT_PULLUP);
  
  // store default table on first time run
  int probe = 0;
  EEPROM.get(ConfigCalAddr, probe);
  if (probe == -1)
    calSave();
  else
    calLoad();
}

CapCtrl::~CapCtrl() 
{
}

bool CapCtrl::setPos(int newPos)
{
  if (newPos < 0 || newPos > ConfigMaxPos)
    return false;

  stepper_.setSpeed(ConfigSpeed/2);
  int dir = newPos > pos_ ? ConfigDirUp : ConfigDirDn;
  
  while (pos_ != newPos) 
  {
    pos_ += dir;
    stepper_.step(-dir);
  }
  compensate(dir);
  releaseMotor();
  return true;
}

bool CapCtrl::setFreq(long freqKhz)
{
  if (freqKhz == 0)
    return false;

  for (int i = 0; i < ConfigCalPoints - 1; i++) 
  {
    long freqA = calPoints_[i].freqKhz;
    long freqB = calPoints_[i + 1].freqKhz;

    if (freqKhz >= freqA && freqKhz < freqB) 
    {
        long posA = calPoints_[i].pos;
        long posB = calPoints_[i + 1].pos;

        long pos = (posA * (freqB - freqKhz) + posB * (freqKhz - freqA)) / (freqB - freqA);

        return setPos(pos);
    }
  }
  return false;
}

void CapCtrl::park()
{
  stepper_.setSpeed(ConfigSpeed);

  while (digitalRead(pinBtn_) == ConfigBtnDisabled)
    stepper_.step(1);

  releaseMotor();
  
  pos_ = 0;
}

void CapCtrl::compensate(int dir) 
{   
  if (prevDir_ != ConfigDirNone && prevDir_ != dir) 
  {
    stepper_.step(-dir * ConfigStepCompensate);
  }
  prevDir_ = dir;
}

void CapCtrl::up5kHz()
{
  up(ConfigStep5kHz, ConfigSpeed / 4);
}

void CapCtrl::down5kHz()
{
  down(ConfigStep5kHz, ConfigSpeed / 4);
}

void CapCtrl::up50kHz()
{
  up(ConfigStep50kHz, ConfigSpeed / 2);
}

void CapCtrl::down50kHz()
{
  down(ConfigStep50kHz, ConfigSpeed / 2);
}

void CapCtrl::up500kHz()
{
  up(ConfigStep500kHz, ConfigSpeed);
}

void CapCtrl::down500kHz()
{
  down(ConfigStep500kHz, ConfigSpeed);
}

void CapCtrl::up(int step, int speed)
{
  if (pos_ + step <= ConfigMaxPos)
  {
    stepper_.setSpeed(speed);
    stepper_.step(-step);
    compensate(ConfigDirUp);
    stepper_.setSpeed(speed);
    pos_ += step;
    releaseMotor();
  }
}

void CapCtrl::down(int step, int speed)
{
  if (digitalRead(pinBtn_) == ConfigBtnDisabled && pos_ - step >= 0) 
  {
    stepper_.setSpeed(speed);
    stepper_.step(step);
    compensate(ConfigDirDn);
    stepper_.setSpeed(speed);
    pos_ -= step;
    releaseMotor();
  }
}

void CapCtrl::releaseMotor()
{
  digitalWrite(pin1_, LOW);  
  digitalWrite(pin2_, LOW);
  digitalWrite(pin3_, LOW);
  digitalWrite(pin4_, LOW);
}

void CapCtrl::calSave() 
{
  for (int i = 0; i < ConfigCalPoints; i++)
    EEPROM.put(ConfigCalAddr + i * sizeof(CalPoint), calPoints_[i]);
}

void CapCtrl::calLoad() 
{
  for (int i = 0; i < ConfigCalPoints; i++)
    EEPROM.get(ConfigCalAddr + i * sizeof(CalPoint), calPoints_[i]);
}

bool CapCtrl::calMove(int index)
{ 
  if (index >= 0 && index < ConfigCalPoints)
    return setPos((long)index * (long)ConfigMaxPos / ConfigCalPoints);
    
  return false;
}

bool CapCtrl::calStore(int freqKhz)
{
  for (int i = 0; i < ConfigCalPoints; i++)
  {
    if (calPoints_[i].freqKhz == freqKhz) 
    {
      calPoints_[i].pos = pos_;
      return true;
    }
  }
  return false;
}

void CapCtrl::calPrint(t_print print) 
{
  for (int i = 0; i < ConfigCalPoints; i++)
  {
    print(String(calPoints_[i].pos) + ":" + String(calPoints_[i].freqKhz));
  }
}

bool CapCtrl::calStore(int index, int freqKhz)
{
  if (index >= 0 && index < ConfigMaxPos)
  {
    calPoints_[index].pos = (long)index * (long)ConfigMaxPos / ConfigCalPoints;
    calPoints_[index].freqKhz = freqKhz;
    
    return true;
  }
  return false;
}
