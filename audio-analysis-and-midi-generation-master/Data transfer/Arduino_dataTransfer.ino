unsigned long LEDinterval[2] = {1}; //1};//{200, 400};
unsigned long prevLEDmillis[2] = {0};//, 0};

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

unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;




int count_flag = 0; 
int count_loop = 0;

void setup() {
  Serial.begin(115200);
  
  delay(500); // delay() is OK in setup as it only happens once

  Serial.println("<Arduino is ready>");
}



void loop() {
  curMillis = millis();
  getDataFromPC();
  replyToPC();
  flashLEDs();
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
    midi_play[count_loop][i] = midi_tab[i];
  }
}

void replyToPC() {
  newFlashInterval2 = midi_tab[0];
  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<rows ");
    Serial.print(rows);
    Serial.print(" columns ");
    Serial.print(columns);
    Serial.print(" ----- ");
    Serial.print(midi_tab[0]);
    Serial.print(" , ");
    Serial.print(midi_tab[1]);
    Serial.print(" , ");
    Serial.print(midi_tab[2]);
    Serial.print(" , ");
    Serial.print(midi_tab[3]);
    Serial.print(" , ");
    Serial.print(midi_tab[4]);
    Serial.print(" , ");
    Serial.print(midi_tab[5]);
    Serial.print(" MIDI_TAB --- ");
    Serial.print(curMillis);
    Serial.print(" : ");
    Serial.println(">"); 
    }
}





void flashLEDs() {
  prevLEDmillis[0] += LEDinterval[0];
}
