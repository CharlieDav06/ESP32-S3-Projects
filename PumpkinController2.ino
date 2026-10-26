#include <USB.h>
#include <USBHIDMouse.h>
#include <USBHIDKeyboard.h>


USBHIDMouse pumpkinMouse;
USBHIDKeyboard pumpkinKeyboard;




constexpr uint16_t pumpkinPin {1};



//Things for calculating baseline using first 10 readings untouched
uint64_t averageUntouchedReading {};
uint64_t readingSum {};
uint64_t untouchedReadings[10] = {0,0,0,0,0,0,0,0,0,0};
bool touched {false};

//Variables for threshold of no click
constexpr double clickCapacitanceThreshold {1.5};
constexpr double unclickValue {0.3};
char pumpkinLetter {'R'};





void setup() {

  pumpkinMouse.begin();
  pumpkinKeyboard.begin();
  USB.begin();

  Serial.begin(115200);

  //Do a read at the start just to give sensor time to calibrate cuz first few readings are off
  touchRead(pumpkinPin);
  delay(500);

  //Calculates first 10 readings to get a baseline
  for (int i = 0; i <= 9; i++)
  {

    untouchedReadings[i] = touchRead(pumpkinPin);
    readingSum += untouchedReadings[i];
    if (i == 9){
      averageUntouchedReading = readingSum / 10;
      readingSum = 0;
    }
    delay(5);

  }


}

void loop() {


  uint32_t pumpkinReading {touchRead(pumpkinPin)}; //Stores charge of the pumpkin pin

  

  
  //Checks if the reading is higher than expected. If so, click
  if ((pumpkinReading > (averageUntouchedReading * clickCapacitanceThreshold) && touched == false))
  {
    Serial.println("CLICKED!");
    pumpkinMouse.click(MOUSE_RIGHT);
    touched = true;
  }

  //If not clicking, set touch to false
  else if (pumpkinReading > averageUntouchedReading * (1 - unclickValue) && pumpkinReading < averageUntouchedReading * (1 + unclickValue))
  {
    touched = false;
  }



  


}
