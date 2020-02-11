#include "capctrl.h"

CapCtrl::CapCtrl(int steps, int pin1, int pin2, int pin3, int pin4, int pinBtn)
  : pin1_(pin1)
  , pin2_(pin2)
  , pin3_(pin3)
  , pin4_(pin4)
  , pinBtn_(pinBtn)
  , pos_(0)
  , stepper_(steps, pin1, pin2, pin3, pin4)
  , calPoints_(new CalPoint[ConfigCalPoints])
{
  stepper_.setSpeed(ConfigSpeed);
  pinMode(pinBtn, INPUT_PULLUP);
  
  calLoad();
}

CapCtrl::~CapCtrl() 
{
  delete[] calPoints_;
}

bool CapCtrl::setPos(int newPos)
{
  if (newPos < 0 || newPos > ConfigMaxPos)
    return false;
  
  int dir = newPos > pos_ ? 1 : -1;
  
  while (pos_ != newPos) 
  {
    pos_ += dir;
    stepper_.step(-dir);
  }
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
  while (digitalRead(pinBtn_) == 0) 
    stepper_.step(1);

  stepper_.step(-ConfigStep);
  releaseMotor();
  
  pos_ = 0;
}
  
void CapCtrl::up()
{
  if (pos_ + ConfigStep <= ConfigMaxPos)
  {
    stepper_.step(-ConfigStep);
    pos_ += ConfigStep;
    releaseMotor();
  }
}

void CapCtrl::down()
{
  if (pos_ - ConfigStep >= 0) 
  {
    stepper_.step(ConfigStep);
    pos_ -= ConfigStep;
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
  if (index >= 0 && index <= ConfigCalPoints)
    return setPos((long)index * (long)ConfigMaxPos / ConfigCalPoints);
    
  return false;
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
