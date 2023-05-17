// Songs using tone library

#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976

#define REST      1


#define SCORE_SECTION 0
#define SCORE_END -1

// notes of the melody followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!


// tempo 108
int16_t  starwars [] { 
  NOTE_AS4,8, NOTE_AS4,8, NOTE_AS4,8,//1
  NOTE_F5,2, NOTE_C6,2,
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4,  
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4,  
  NOTE_AS5,8, NOTE_A5,8, NOTE_AS5,8, NOTE_G5,2, NOTE_C5,8, NOTE_C5,8, NOTE_C5,8,
  NOTE_F5,2, NOTE_C6,2,
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4, SCORE_SECTION,0, 
  
  NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F6,2, NOTE_C6,4, //8  
  NOTE_AS5,8, NOTE_A5,8, NOTE_AS5,8, NOTE_G5,2, NOTE_C5,-8, NOTE_C5,16, 
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  NOTE_F5,8, NOTE_G5,8, NOTE_A5,8, NOTE_G5,4, NOTE_D5,8, NOTE_E5,4,NOTE_C5,-8, NOTE_C5,16,
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8, SCORE_SECTION,0,
  
  NOTE_C6,-8, NOTE_G5,16, NOTE_G5,2, REST,8, NOTE_C5,8,//13
  NOTE_D5,-4, NOTE_D5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  NOTE_F5,8, NOTE_G5,8, NOTE_A5,8, NOTE_G5,4, NOTE_D5,8, NOTE_E5,4,NOTE_C6,-8, NOTE_C6,16,
  NOTE_F6,4, NOTE_DS6,8, NOTE_CS6,4, NOTE_C6,8, NOTE_AS5,4, NOTE_GS5,8, NOTE_G5,4, NOTE_F5,8,
  NOTE_C6,1,  SCORE_END,0
};

//tempo = 160 (114)
int16_t ode_to_joy[] = {
  NOTE_E4, 4,  NOTE_E4, 4,  NOTE_F4, 4,  NOTE_G4, 4, //1
  NOTE_G4, 4,  NOTE_F4, 4,  NOTE_E4, 4,  NOTE_D4, 4,
  NOTE_C4, 4,  NOTE_C4, 4,  NOTE_D4, 4,  NOTE_E4, 4,
  NOTE_E4, -4, NOTE_D4, 8,  NOTE_D4, 2,  SCORE_SECTION,0,

  NOTE_E4, 4,  NOTE_E4, 4,  NOTE_F4, 4,  NOTE_G4, 4, //4
  NOTE_G4, 4,  NOTE_F4, 4,  NOTE_E4, 4,  NOTE_D4, 4,
  NOTE_C4, 4,  NOTE_C4, 4,  NOTE_D4, 4,  NOTE_E4, 4,
  NOTE_D4, -4,  NOTE_C4, 8,  NOTE_C4, 2, SCORE_SECTION,0,

  NOTE_D4, 4,  NOTE_D4, 4,  NOTE_E4, 4,  NOTE_C4, 4, //8
  NOTE_D4, 4,  NOTE_E4, 8,  NOTE_F4, 8,  NOTE_E4, 4, NOTE_C4, 4,
  NOTE_D4, 4,  NOTE_E4, 8,  NOTE_F4, 8,  NOTE_E4, 4, NOTE_D4, 4,
  NOTE_C4, 4,  NOTE_D4, 4,  NOTE_G3, 2,  SCORE_SECTION,0, 

  NOTE_E4, 4,  NOTE_E4, 4,  NOTE_F4, 4,  NOTE_G4, 4, //12
  NOTE_G4, 4,  NOTE_F4, 4,  NOTE_E4, 4,  NOTE_D4, 4,
  NOTE_C4, 4,  NOTE_C4, 4,  NOTE_D4, 4,  NOTE_E4, 4,
  NOTE_D4, -4,  NOTE_C4, 8,  NOTE_C4, 2,  SCORE_END,0
};

// tempo = 76;
int16_t lullaby[] = {

  NOTE_G4, 4, NOTE_G4, 4, //1
  NOTE_AS4, -4, NOTE_G4, 8, NOTE_G4, 4,
  NOTE_AS4, 4, REST, 4, NOTE_G4, 8, NOTE_AS4, 8,
  NOTE_DS5, 4, NOTE_D5, -4, NOTE_C5, 8,
  NOTE_C5, 4, NOTE_AS4, 4, NOTE_F4, 8, NOTE_G4, 8,
  NOTE_GS4, 4, NOTE_F4, 4, NOTE_F4, 8, NOTE_G4, 8,
  NOTE_GS4, 4, REST, 4, NOTE_F4, 8, NOTE_GS4, 8,
  NOTE_D5, 8, NOTE_C5, 8, NOTE_AS4, 4, NOTE_D5, 4,
  SCORE_SECTION,0, 

  NOTE_DS5, 4, REST, 4, NOTE_DS4, 8, NOTE_DS4, 8, //8
  NOTE_DS5, 2, NOTE_C5, 8, NOTE_GS4, 8,
  NOTE_AS4, 2, NOTE_G4, 8, NOTE_DS4, 8,
  NOTE_GS4, 4, NOTE_AS4, 4, NOTE_C5, 4,
  NOTE_AS4, 2, NOTE_DS4, 8, NOTE_DS4, 8,
  NOTE_DS5, 2, NOTE_C5, 8, NOTE_GS4, 8,
  NOTE_AS4, 2, NOTE_G4, 8, NOTE_DS4, 8,
  NOTE_AS4, 4, NOTE_G4, 4, NOTE_DS4, 4,
  NOTE_DS4, 2, SCORE_END,0, 
};

// tempo = 200;
int16_t mario_bros[] = {
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,8, //1
  NOTE_G5,4, REST,4, NOTE_G4,8, REST,4, 
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 3
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4,NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,SCORE_SECTION,0,

  
  REST,4, NOTE_G5,8, NOTE_FS5,8, NOTE_F5,8, NOTE_DS5,4, NOTE_E5,8,//7
  REST,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C4,8, REST,8, NOTE_A4,8, NOTE_C5,8, NOTE_D5,8,
  REST,4, NOTE_DS5,4, REST,8, NOTE_D5,-4,
  NOTE_C5,2, REST,2,SCORE_SECTION,0,

  REST,4, NOTE_G5,8, NOTE_FS5,8, NOTE_F5,8, NOTE_DS5,4, NOTE_E5,8,//repeats from 7
  REST,8, NOTE_GS4,8, NOTE_A4,8, NOTE_C4,8, REST,8, NOTE_A4,8, NOTE_C5,8, NOTE_D5,8,
  REST,4, NOTE_DS5,4, REST,8, NOTE_D5,-4,
  NOTE_C5,2, REST,2, SCORE_SECTION,0,

  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4,//11
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,

  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,8, NOTE_E5,8,//13
  REST,1, 
  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4,
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,4,
  NOTE_G5,4, REST,4, NOTE_G4,4, REST,4, 
  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, SCORE_SECTION,0,
  
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4,

  NOTE_C5,-4, NOTE_G4,8, REST,4, NOTE_E4,-4, // repeats from 19
  NOTE_A4,4, NOTE_B4,4, NOTE_AS4,8, NOTE_A4,4,
  NOTE_G4,-8, NOTE_E5,-8, NOTE_G5,-8, NOTE_A5,4, NOTE_F5,8, NOTE_G5,8,
  REST,8, NOTE_E5,4, NOTE_C5,8, NOTE_D5,8, NOTE_B4,-4, SCORE_SECTION,0,

  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,//23
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
  
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //26
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
  NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2, SCORE_SECTION,0,

  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,//repeats from 23
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
  
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //26
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
  NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2,
  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,8, NOTE_E5,8,
  REST,1, SCORE_SECTION,0,

  NOTE_C5,8, NOTE_C5,4, NOTE_C5,8, REST,8, NOTE_C5,8, NOTE_D5,4, //33
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2,
  NOTE_E5,8, NOTE_E5,8, REST,8, NOTE_E5,8, REST,8, NOTE_C5,8, NOTE_E5,4,
  NOTE_G5,4, REST,4, NOTE_G4,4, REST,4, 
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_D5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_A5,-8, NOTE_G5,-8, NOTE_F5,-8,
  SCORE_SECTION,0,
  
  NOTE_E5,8, NOTE_C5,4, NOTE_A4,8, NOTE_G4,2, //40
  NOTE_E5,8, NOTE_C5,4, NOTE_G4,8, REST,4, NOTE_GS4,4,
  NOTE_A4,8, NOTE_F5,4, NOTE_F5,8, NOTE_A4,2,
  NOTE_B4,8, NOTE_F5,4, NOTE_F5,8, NOTE_F5,-8, NOTE_E5,-8, NOTE_D5,-8,
  NOTE_C5,8, NOTE_E4,4, NOTE_E4,8, NOTE_C4,2, SCORE_END,0,
};
  
int16_t game_over[] = {
  NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4, //45
  NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
  NOTE_G4,8, NOTE_D4,8, NOTE_E4,-2, SCORE_END,0,
};

//tempo = 110;
int16_t prince_igor[] = {  
  // Prince Igor - Polovtsian Dances, Borodin 
  NOTE_G4, 4, NOTE_G4, 4, NOTE_D5, -2,
  NOTE_C5, 8, NOTE_D5, 8, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8,
  NOTE_A4, 8, NOTE_AS4, 8, NOTE_C5, 1, SCORE_SECTION,0,
  
  NOTE_D5, 4, NOTE_A4, 4, NOTE_G4, 8, NOTE_F4, 8,
  NOTE_D4, 4, NOTE_D4, 4, NOTE_G4, -2,
  NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, 8, NOTE_E4, 8, SCORE_SECTION,0,
  
  NOTE_F4, 4, NOTE_E4, 4, NOTE_D4, 1,
  NOTE_E4, 4, NOTE_F4, 4, NOTE_A4, 4,
  NOTE_G4, 4, NOTE_G4, 4, NOTE_AS4, -2, SCORE_SECTION,0,

  NOTE_C5, 4, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8,
  NOTE_A4, 4, NOTE_AS4, 4, NOTE_C5, -2,
  NOTE_CS5, 4, NOTE_C5, 4, NOTE_A4, 4, SCORE_SECTION,0,
  
  NOTE_CS5, 4, NOTE_CS4, 4, NOTE_F5, -2,
  NOTE_G5, 4, NOTE_F5, 4, NOTE_DS4, 8, NOTE_CS4, 8,
  NOTE_F5, 2, NOTE_C5, -2,  SCORE_SECTION,0,
  
  NOTE_AS4, 4, NOTE_C5, 4, NOTE_AS4, 8, NOTE_A4, 8,
  NOTE_G4, 4, NOTE_G4, 4, NOTE_AS4, 1,
  NOTE_C5, 4, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8,   SCORE_SECTION,0,

  NOTE_F4, 4, NOTE_G4, 4, NOTE_A4, 1,
  NOTE_AS4, 4, NOTE_A4, 4, NOTE_F4, 4,
  NOTE_G4, 4, NOTE_G4, 4, NOTE_D5, -2, SCORE_SECTION,0,

  NOTE_C5, 4, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8,
  NOTE_A4,-1, NOTE_A4,-1, REST,2,
  NOTE_G4, 4, NOTE_G4, 4, NOTE_D5, -2, SCORE_SECTION,0,
  
  NOTE_C5, 8, NOTE_D5, 8, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8,
  NOTE_A4, 8, NOTE_AS4, 8, NOTE_C5, 1,
  NOTE_C5, 8, NOTE_D5, 8, NOTE_C5, 4, NOTE_AS4, 8, NOTE_A4, 8,
  SCORE_SECTION,0,
  
  NOTE_D4, 4, NOTE_D4, 4, NOTE_G4, -2,
  NOTE_G4, 8, NOTE_A4, 8, NOTE_G4, 4, NOTE_F4, 8, NOTE_E4, 8,
  NOTE_F4, 8, NOTE_E4, 8, NOTE_D4, -2,//1
  
  REST,4, NOTE_C5, 8, NOTE_D5, 8, NOTE_C5, 4, NOTE_AS4, 8, NOTE_A4, 8,
  NOTE_G4, 4, NOTE_G4, 4, NOTE_B4, -2,
  NOTE_C5, 4, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8,  SCORE_SECTION,0, 

  NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -1, REST,4,
  NOTE_G4, 4, NOTE_G4, 4, NOTE_D5, -2,
  NOTE_C5, 8, NOTE_D5, 8, NOTE_AS4, 4, NOTE_A4, 8, NOTE_G4, 8, SCORE_SECTION,0,
  
  NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -2,
  NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -2,
  NOTE_A4, 4, NOTE_G4, 4, NOTE_F4, -1, SCORE_END,0,
};

class PlayMelody {
  private:
    int tonePin;
    int16_t *_melody;
    int _tempo = 160; // 114;
    // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
    // there are two values per note (pitch and duration), so for each note there are four bytes
    int nbrNotes = 0;
    int wholenote = 0;
    int divider = 0;
    int noteDuration = 0;
    int nextNoteIndex = 0;
    uint32_t startMillis = 0;
  public:
    PlayMelody(int pin) {
      tonePin = pin;
    }

    void play(int16_t *melody, int tempo) {
      _tempo = tempo;
      play(melody);
    }
    void play(int16_t *melody) {
      _melody = melody;
      wholenote = (60000 * 4) / _tempo;
      nextNoteIndex = 0;
      startMillis = millis();
      service(SCORE_SECTION); // start playing (service must be called to repeatidly)
    }

    bool service(int16_t terminator) { // returns false if no more notes to play
      if ( _melody[nextNoteIndex] != terminator) {
        if(_melody[nextNoteIndex] == SCORE_SECTION) {
          // skip score section tag
          nextNoteIndex += 2; 
        }
        // calculates the duration of each note
        divider = _melody[nextNoteIndex + 1];
        if (divider > 0) {
          // regular note, just proceed
          noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
          // dotted notes are represented with negative durations!!
          noteDuration = (wholenote) / abs(divider);
          noteDuration *= 1.5; // increases the duration in half for dotted notes
        }
        tone(tonePin, _melody[nextNoteIndex], noteDuration * 0.9); // allow 10% pause between notes
        delay(noteDuration);      // Wait for the specief duration before playing the next note.
        noTone(tonePin);       // stop the waveform generation before the next note.
        nextNoteIndex += 2;
      }
      return (_melody[nextNoteIndex] != terminator) ;
    }
};
