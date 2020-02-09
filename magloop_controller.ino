#include <Stepper.h>
#include "capctrl.h"

#define STEPS     200
#define BTN_STOP  11

String currentCommand;

CapCtrl capCtrl(STEPS, A2, A3, A4, A5, BTN_STOP);
  
void setup() 
{
  Serial.begin(9600);
  Serial.println("CapCtrl v1.0 RDY");
}

void run_command(const String &cmd) 
{
  if (cmd == "d") 
  {
    capCtrl.down();
  }
  else if (cmd == "u") 
  {
    capCtrl.up();
  }
  else if (cmd == "p") 
  {
    capCtrl.park();
  }
  else 
  {
    int pos = cmd.toInt();

    if (pos != 0) 
    {
      capCtrl.setPos(pos); 
    }
  }
}

void loop()
{  
  if (Serial.available()) 
  {
    char cmd = Serial.read(); 
    if (cmd == '\n') 
    {
      run_command(currentCommand);
      currentCommand = "";
    }
    else 
    {
      currentCommand += cmd;  
    }
  }
}
