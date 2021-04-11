unsigned long interval[2] = {1}; //1};//{200, 400};
unsigned long tab_z[2] = {0};//, 0};

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int newFlashInterval1 = 0;
int newFlashInterval2 = 0;
int newFlashInterval3 = 0;
int rows = 0;
int columns = 0;

int midi_tab[] = {};
int midi_row[] = {};

int midi_play[][10] = {{}};

int midi_t[50][6] = {{}};

unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;

//--------------------------------------------------------------------- MIDI 
#include <stdlib.h>
#define ATMEGA328 1
#ifdef ATMEGA328
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#elif ATMEGA32U4
#include "MIDIUSB.h"

#endif


// BUTTONS
const int N_BUTTONS = 2;                    
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {2, 3};
int buttonCState[N_BUTTONS] = {};
int buttonPState[N_BUTTONS] = {};

#define pin13 1
byte pin13index = 12;

// DEBOUNCE
unsigned long lastDebounceTime[N_BUTTONS] = {0};
unsigned long debounceDelay = 5;


// MIDI
byte midiCh = 1; //* MIDI channel to be used
byte note = 0; //* Lowest note to be used
byte cc = 1; //* Lowest MIDI CC to be used

// -------------------------------------------------------------- MIDI







int count_flag = 0; 
int count_loop = 0;
int midi_cnt = 0;

void setup() {
  Serial.begin(115200);
  
  delay(500); // delay() is OK in setup as it only happens once

  Serial.println("<Arduino is ready>");
  for (int i = 0; i < N_BUTTONS; i++) {
    pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
  }

#ifdef pin13
pinMode(BUTTON_ARDUINO_PIN[pin13index], INPUT);
#endif

}



void loop() {
  curMillis = millis();
  getDataFromPC();
  replyToPC();
  testf();
  buttons(); 
}

//==========================================================

void getDataFromPC() {
  if(Serial.available() > 0) {

    char x = Serial.read();    
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      //Serial.print(inputBuffer);
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

//=========================================================
void clearTab() {
  for(int i = 0; i < sizeof(midi_tab, DEC); i++) {
    midi_tab[i] = 0; 
  }
}


void parseData() {                                           
  char * strtokIndx;                          
  
  if(count_flag == 0) {
    clearTab();
    strtokIndx = strtok(inputBuffer, " ");    
    strcpy(messageFromPC, strtokIndx);       
    
    rows = atoi(strtokIndx);                  
    
    strtokIndx = strtok(NULL, " ");          
    columns = atoi(strtokIndx); 
    //int midi_t[6][3]={{}};          
  }
  
  if(count_flag > 0){
    int b = 0;
    strtokIndx = strtok(inputBuffer, " ");
    strcpy(messageFromPC, strtokIndx);      
    b = atoi(strtokIndx);
    midi_tab[0] = b;
    

    for(int i = 1; i<columns; i++) {
      int a = 0;
      strtokIndx = strtok(NULL, " ");
      a = atoi(strtokIndx);
      midi_tab[i] = a;
    } 
    getMidiData();
    count_loop = count_loop + 1; 
  }
  count_flag = count_flag + 1;
}

void getMidiData () {
  for(int i = 0; i <= columns; i++) {
    midi_play[count_flag][i] = midi_tab[i];
  }
}


void setMidi_t(int i, int j, int miditab) {
  midi_t[i][j] = miditab;
}

int getMidi_t(int i, int j) {
  return midi_t[i][j];
}

void replyToPC() {
  newFlashInterval2 = midi_tab[0];
  if (newDataFromPC) {
    //midi_t[midi_cnt][0] = midi_tab[0];
    setMidi_t(midi_cnt, 0, midi_tab[0]);
    midi_t[midi_cnt][1] = midi_tab[1];
    midi_t[midi_cnt][2] = midi_tab[2];
    midi_t[midi_cnt][3] = midi_tab[3];
    midi_t[midi_cnt][4] = midi_tab[4];
    midi_t[midi_cnt][5] = midi_tab[5];
    
    newDataFromPC = false;
    
    Serial.print("<rows ");
    Serial.print(rows);
    Serial.print(" columns ");
    Serial.print(columns);
    Serial.print(" ----- ");
    Serial.print(getMidi_t(midi_cnt, 0));
    Serial.print(" , ");
    Serial.print(midi_t[midi_cnt][1]);
    Serial.print(" , ");
    Serial.print(midi_t[midi_cnt][2]);
    Serial.print(" , ");
    Serial.print(midi_t[midi_cnt][3]);
    Serial.print(" , ");
    Serial.print(midi_t[midi_cnt][4]);
    Serial.print(" , ");
    Serial.print(midi_t[midi_cnt][5]);
    Serial.print(" MIDI_TAB --- ");
    Serial.print(curMillis);
    Serial.print(" : ");
    Serial.println(">");
    midi_cnt++;
    }
}


// ----------------------------------------------------------------------- MIDI

void buttons() 
{
  for (int i = 0; i < N_BUTTONS; i++) {
    buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]);

#ifdef pin13
if (i == pin13index) {
buttonCState[i] = !buttonCState[i];
}
#endif

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();
        if (buttonCState[i] == LOW) {






#ifdef ATMEGA328  
int x = 6;
int y = 4;
int time_resolution = 1000;
int midi_notation[x][y] = {{60, 64, 67, 71}, {0, 67, 0, 0}, {60, 64, 67, 0}, {64, 67, 71, 0}, {0, 0, 62, 0}, {0, 0, 60, 0}};

//MIDI.sendNoteOn(note + midi_t[1][0], 127, midiCh);
//delay(500);
//MIDI.sendNoteOn(note + midi_t[1][0], 0, midiCh);



 //PLAY MIDI
for(int i = 0; i <= x-1; i++) 
{
  for(int j = 0; j <= y-1; j++)
  {
    if (midi_notation[i][j] == 0) 
    {
      MIDI.sendNoteOn(note + midi_notation[i][j], 0, midiCh);
    }
    else if (midi_notation[i][j] > 0)
    {
      MIDI.sendNoteOn(note + midi_notation[i][j], 127, midiCh);
    }
  }
  delay(time_resolution);
  for(int k = 0; k <=y-1; k++)
    {
      MIDI.sendNoteOn(note + midi_notation[i][k], 0, midiCh);
    }
}

#endif


        }
        buttonPState[i] = buttonCState[i];
      }
    }
  }
}





// ----------------------------------------------------------------------- MIDI

void testf() {
  tab_z[0] += interval[0];
}
