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
      else if (newChar == '\r') {}
      else 
        result += newChar;  
    }
    delay(100);
  }
}

void runCommand(const String &cmd) 
{
  // down
  if (cmd == "h") 
  {
    println(F("u/U/d/D/p/c<freq>/s/C/<freq>"));
  }
  else if (cmd == "d") 
  {
    capCtrl.down();
  }
  else if (cmd == "D") 
  {
    capCtrl.downLarge();
  }
  // up
  else if (cmd == "u") 
  {
    capCtrl.up();
  }
  // up large
  else if (cmd == "U") 
  {
    capCtrl.upLarge();
  }
  // park
  else if (cmd == "p") 
  {
    capCtrl.park();
  }
  // calibrate
  else if (cmd[0] == 'c') 
  {
    String freq = cmd.substring(1);
    if (!capCtrl.calStore(freq.toInt())) 
    {
      println(F("unknown freq"));
      return;
    }
    capCtrl.calSave();
  }
  // print calibration table
  else if (cmd == "C") 
  {
    capCtrl.calPrint(btCtrl);
  }
  // print calibration table
  else if (cmd == "s") 
  {
    capCtrl.calSave();
  }
  // go to frequency
  else 
  {
    long freqKhz = cmd.toInt();
    
    if (!capCtrl.setFreq(freqKhz)) 
    {
      println(F("wrong freq"));
      return;
    }
  }
  println("ok, " + String(capCtrl.getPos()));
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
    else if (cmd == '\r') {}
    else 
    {
      currentCommand += cmd;  
    }
  }
}
