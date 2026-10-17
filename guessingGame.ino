#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



//Pins
constexpr int buzzer1Pin {3};
constexpr int button1 {2};
constexpr int button2 {42};
constexpr int OLED_SDA {9};
constexpr int OLED_SCL {8};



//Game state
int target {random(0, 2)};
int score {0};
bool guessed {};
bool running {true};



//Buzzer state
bool buzzerPlaying {false};
unsigned long buzzerEndTime {0};



//Display setup
constexpr int screenWidth {128};
constexpr int screenHeight {64};
constexpr uint8_t oledAddress {0x3C};



//Game Settings
constexpr int gameDurationsec {30};
constexpr int buzzerDurationMs {200};
constexpr int buzzerFreqHz {2000};
constexpr int buzzerResolutionBits {8};
constexpr int correctToneHz {700};
constexpr int wrongToneHz {200};


//Display on screen
Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, -1);



void buzzerStart(int frequency){
  ledcWriteTone(buzzer1Pin, frequency);
  buzzerEndTime = millis() + buzzerDurationMs;
  buzzerPlaying = true;
}



void setup() {

  Serial.begin(115200);

  //Setting up pins
  ledcAttach(buzzer1Pin, buzzerFreqHz, buzzerResolutionBits);
  pinMode(1, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  //Setting up screen
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, oledAddress)) {
    Serial.println("Display not found!");
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(SSD1306_WHITE);
}



void loop() {

  long currentTime = gameDurationsec - (millis()/1000);
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  String text = String(target);


  display.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);

  //Calculate the centre position
  int y = (screenHeight - textHeight) / 2;

  //Constantly clear display to replace with new values
  display.clearDisplay();

  //Answer that isnt 0 or 1
  int answer = 2;

  int button1Reading = digitalRead(button1);
  int button2Reading = digitalRead(button2);



  if (currentTime <=0){
    running = false;
  }

  //Running condition, so it knows when to display end screen
  if (running){

    //If button not pressed yet, and button press detected, set answer to 0
    if (!guessed) {
    if (button1Reading == LOW) {
      answer = 0;
      guessed = true;
    }


    //If button not pressed yet, and button press detected, set answer to 1
    else if (button2Reading == LOW) {
      answer = 1;
      guessed = true;
    }

    //Only score when a new guess is made
    if (guessed) {
      if (answer == target) {
        score++;
        buzzerStart(correctToneHz);
      }
      else{
        score --;
        buzzerStart(wrongToneHz);
      }
    }
  }

  // Wait until both buttons have been released
  if (guessed && button1Reading == HIGH && button2Reading == HIGH) {
    target = random(0, 2);
    guessed = false;
  }


  //Timer Display
  display.setCursor(0, y-10);
  display.setTextSize(1);
  display.println("Timer: " + String(currentTime));


  //0 or 1 Display
  display.setTextSize(5);
  display.setCursor(0, y+10);
  display.println(text);


  //Score display
  display.setCursor(screenWidth - (textWidth * 2), y+10);
  display.println(score);
  display.display();


  }

  //If not running, display score
  else{
  display.setTextSize(1);
  display.setCursor(0, y+10);
  display.println("You scored: " + String(score));
  display.display();

  }
  if (buzzerPlaying && millis() >= buzzerEndTime) {
  ledcWriteTone(buzzer1Pin, 0);
  buzzerPlaying = false;
}


}