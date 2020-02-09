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

  capCtrl.park();
}

String readLine() 
{
  String result;

  while (1) 
  {
    while (Serial.available()) 
    {
      char newChar = Serial.read(); 
      if (newChar == '\n') 
        return result;
      else 
        result += newChar;  
    }
    delay(100);
  }
}

void runCommand(const String &cmd) 
{
  // down
  if (cmd == "d") 
  {
    capCtrl.down();
  }
  // up
  else if (cmd == "u") 
  {
    capCtrl.up();
  }
  // park
  else if (cmd == "p") 
  {
    capCtrl.park();
  }
  // calibrate
  else if (cmd == "c") 
  {
    runCalibration();
  }
  // go to frequency
  else 
  {
    long freqKhz = cmd.toInt();
    
    if (!capCtrl.setFreq(freqKhz))
      Serial.println("err");
  }
}

void loop()
{  
  if (Serial.available()) 
  {
    char cmd = Serial.read(); 
    if (cmd == '\n') 
    {
      runCommand(currentCommand);
      currentCommand = "";
    }
    else 
    {
      currentCommand += cmd;  
    }
  }
}

void runCalibration()
{
  int i = 0;

  Serial.println("cal:");
  
  while (capCtrl.calMove(i++))
  {
    Serial.println("freq?");
    
    int freqKhz = readLine().toInt();

    if (capCtrl.calStore(i, freqKhz))
      Serial.println("ok");
    else
      Serial.println("err");
  }
  
  capCtrl.calSave();
  Serial.println("done");
  
  capCtrl.park();
}
