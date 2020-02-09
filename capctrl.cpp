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
  
  loadCalData();
}

void CapCtrl::setPos(int newPos)
{
  if (newPos == 0 || newPos > ConfigMaxPos) return;
  
  int dir = newPos > pos_ ? 1 : -1;
  
  while (pos_ != newPos) 
  {
    pos_ += dir;
    stepper_.step(-dir);
  }
  release();
}

void CapCtrl::setFreq(int freqKhz)
{
  // linearize, get position from frequency
  // go_pos()
}

void CapCtrl::park()
{
  while (digitalRead(pinBtn_) == 0) 
  {
    stepper_.step(1);
  }
  stepper_.step(-ConfigStep);
  release();
  
  pos_ = 0;
}
  
void CapCtrl::up()
{
  if (pos_ + ConfigStep <= ConfigMaxPos)
  {
    stepper_.step(-ConfigStep);
    pos_ += ConfigStep;
    release();
  }
}

void CapCtrl::down()
{
  if (pos_ - ConfigStep >= 0) 
  {
    stepper_.step(ConfigStep);
    pos_ -= ConfigStep;
    release();
  }
}
    
void CapCtrl::release()
{
  digitalWrite(pin1_, LOW);  
  digitalWrite(pin2_, LOW);
  digitalWrite(pin3_, LOW);
  digitalWrite(pin4_, LOW);
}

void CapCtrl::saveCalData() 
{
  for (int i = 0; i < ConfigCalPoints; i++)
  {
    EEPROM.put(ConfigCalAddr + i * sizeof(CalPoint), calPoints_[i]);
  }
}

void CapCtrl::loadCalData() 
{
  for (int i = 0; i < ConfigCalPoints; i++)
  {
    EEPROM.get(ConfigCalAddr + i * sizeof(CalPoint), calPoints_[i]);
  }
}

void CapCtrl::calibrate(int freqKhz, int pos)
{
  // go to position (or predefined positions)
  // ask frequency from the user
  // set frequency
  // linearize
  // store to eeprom
}
