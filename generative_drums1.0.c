#include <i2c_t3.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>
#include <SD.h>
#include <SerialFlash.h>
#include <synth_simple_drum.h>

AudioSynthSimpleDrum     drum2;          //xy=399,244
AudioSynthSimpleDrum     drum3;          //xy=424,310
AudioSynthSimpleDrum     drum1;          //xy=431,197
AudioSynthSimpleDrum     drum4;          //xy=464,374
AudioMixer4              mixer1;         //xy=737,265
AudioOutputAnalogStereo           i2s1;           //xy=979,214
AudioConnection          patchCord1(drum2, 0, mixer1, 1);
AudioConnection          patchCord2(drum3, 0, mixer1, 2);
AudioConnection          patchCord3(drum1, 0, mixer1, 0);
AudioConnection          patchCord4(drum4, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=930,518

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

#define NUMTRACKS 4 // number of tracks
#define NUMSTEPS 16 // number of steps in sequence
#define NUMPARAMS 15 // number of parameters
/*
 * ----- Parameters -----
 *  0 = ON/OFF
 *  1 = OSC PITCH
 *  2 = 
 */
int track[NUMTRACKS][NUMSTEPS][NUMPARAMS];
static uint32_t next;
int tempo = 75;
int step = 0;
int bars = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  randomSeed(analogRead(0));
  AudioMemory(15);
  pinMode(33,INPUT_PULLUP);
  
  next = millis() + 1000;
  
//  initialize the OLED
  display.begin();
  display.clearDisplay();
  
  display.fillScreen(WHITE);
  display.display();
  delay(1000);
  display.setTextColor(BLACK);
  display.setCursor( 35, 14); // position to start text
  display.println("loading...");
  display.display();
  delay(1000);
    
  AudioNoInterrupts();

  for(int i = 0; i<=4; i++){
    mixer1.gain(i, 0.2);
  }
  randomize();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.2);
  
  AudioInterrupts();
  next = millis();
}

void loop() {
  
  static uint32_t num = 0;
  //  sequencer init. display
  
  if (step > NUMSTEPS)
  { 
    step = 0; 
//    bars++;

//      bars = 0;
  }
  if(digitalRead(33) == LOW){
  randomize();
  }
  if(millis() == next)
  {
     next = millis() + tempo;
     displaySequence(track, step);
     for(int i=0; i<=NUMTRACKS; i++)
     {
        playDrums(i, step);
     }
     step++;
  }
}

void playDrums(int drum, int pos){
  if(track[drum][pos][0] == 1){
    switch(drum)
    {
      case 0: drum1.noteOn();
        break;
      case 1: drum2.noteOn();
        break;
      case 2: drum3.noteOn();
        break;
      case 3: drum4.noteOn();
        break;
      default:
        break;
    }
  }
}

void randomize(){
//  for(int i=0; i<=32; i++){
  for(int i=0; i<=4; i++){
    track[random(4)][random(15)][0] = 1;
    track[random(4)][random(15)][0] = 0;
  }

    drum1.frequency(random(35,100));
  drum1.length(random(100));
  drum1.secondMix(random(0.1));
  drum1.pitchMod(random(0.75));
  
  drum2.frequency(random(700));
  drum2.length(random(200));
  drum2.secondMix(random(1.0));
  drum2.pitchMod(random(0.55));
  
  drum3.frequency(random(3000));
  drum3.length(random(200));
  drum3.secondMix(random(1.0));
  drum3.pitchMod(random(0.5));

  drum4.frequency(random(15000));
  drum4.length(random(100));
  drum4.secondMix(random(1.0));
  drum4.pitchMod(random(0.5));
}


void displaySequence(int track[][16][15], int step) {
  
  int i;
  int j;
  int k;
  uint16_t off = 8; //offset
  uint16_t horizSpacing = 7;
  uint16_t vertSpacing = 7;
  display.clearDisplay();
  display.setTextColor(WHITE);
  for(j = 0; j<NUMTRACKS; j++) //for each track 
  {
    display.setCursor(0, j*vertSpacing);
    switch(j)
    {
      case 0: display.println("o");
      break;
      case 1: display.println("");
      break;
      case 2: display.println("");
      break;
      case 3: display.println("");
      break;
      default: 
      break; 
    }
    for(i=0; i<=NUMSTEPS; i++) // for each step in the sequence
    {
      if(track[j][i][0] == 1){ //create boxes for sequence & fill them if note == on/1
          display.fillRect( (i*horizSpacing) + off, j*vertSpacing, horizSpacing+1, vertSpacing+1, WHITE);
      }
      else{ //empty if not not on
          display.drawRect( (i*horizSpacing) + off, j*vertSpacing, horizSpacing+1, vertSpacing+1, WHITE);
      }
    }
  }
//  step indicator
// if looks bad consider a rectangle
  display.drawRect(off + ((step)*horizSpacing), 0, horizSpacing+1, 29, BLACK);
//  display.drawFastVLine( off + ((step)*horizSpacing),0,33,BLACK);
//  display.drawFastVLine( off + ((step+1)*horizSpacing),0,33,BLACK);
  display.drawFastVLine( off + ((step+1)*horizSpacing),0,29,WHITE);
  display.drawFastVLine( off + ((step)*horizSpacing),0,29,WHITE);
  display.display();
}

