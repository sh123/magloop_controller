#include <SoftwareSerial.h>
#include <Stepper.h>
#include "capctrl.h"

#define STEPS     200
#define BTN_STOP  11

String currentCommand;

SoftwareSerial btCtrl(9, 10); // our RX, TX, hc06 TX, RX
CapCtrl capCtrl(STEPS, A2, A3, A4, A5, BTN_STOP);

void setup() 
{
  Serial.begin(9600);
  btCtrl.begin(9600);
  
  println("CapCtrl v1.0 RDY");

  capCtrl.park();
}

void println(const String &s) 
{
  Serial.println(s);
  btCtrl.println(s);  
}

bool readChar(char &c)
{
  bool found = true;
  
  if (Serial.available()) 
    c = Serial.read(); 
  else if (btCtrl.available())
    c = btCtrl.read();
  else
    found = false;
    
  return found;
}

String readLine() 
{
  String result;

  while (1) 
  {
    char newChar;
      
    if (readChar(newChar)) 
    {
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
    {
      println("err");
      return;
    }
  }
  println("ok");
}

void loop()
{ 
  char cmd;
  
  if (readChar(cmd)) 
  {
    if (cmd == '\n')
    {
      runCommand(currentCommand);
      currentCommand = String();
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

  println("cal:");
  
  while (capCtrl.calMove(i++))
  {
    println("freq?");
    
    int freqKhz = readLine().toInt();

    if (capCtrl.calStore(i, freqKhz))
      println("ok");
    else
      println("err");
  }
  
  capCtrl.calSave();
  println("done");
  
  capCtrl.park();
}
