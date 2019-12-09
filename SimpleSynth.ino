#include <Audio.h>

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE(); // MIDI library init

#include "Synth.h"

int ledPin = 13;
bool controllerIsLaunchpad = true;

const int interval_time = 50;
elapsedMillis clock_count;

Synth synth(0, 8, 7, 1, 2, 5, 3, 4, 5);
//
AudioOutputI2S  i2s2;
AudioConnection patchCord1(*synth.getOutput(), 0, i2s2, 0);
AudioConnection patchCord2(*synth.getOutput(), 0, i2s2, 1);
AudioOutputUSB           usb1;
AudioConnection          patchCord3(*synth.getOutput(), 0, usb1, 0);
AudioConnection          patchCord4(*synth.getOutput(), 0, usb1, 1);

AudioControlSGTL5000 sgtl5000_1;

/**
 * Setup
 */
void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  
  MIDI.setHandleNoteOn(onNoteOn);
  MIDI.setHandleNoteOff(onNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleStop(onStop);
  usbMIDI.setHandleSystemReset(onStop);

  
  // Audio connections require memory to work.
  AudioMemory(30);

  sgtl5000_1.enable();
  sgtl5000_1.volume(2);
  
  while (!Serial && millis() < 2500); // wait for serial monitor

  synth.init();
  
  // Starting sequence
  Serial.println("Ready!");
  
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);

  if(controllerIsLaunchpad){
    MIDI.sendControlChange(0,2,1);
    MIDI.sendControlChange(0,2,1);
    MIDI.sendControlChange(0,2,1);
  }
}

/**
 * Loop
 */
void loop() {
  MIDI.read();
  usbMIDI.read();

  // Leveraging the overload
  if (clock_count >= interval_time) {
    // Do things every 50ms

    // Synth update
    synth.update();
    
    // Launchpad lights
    if(controllerIsLaunchpad){
      for (int i = 0; i < voiceCount ; i++) {
        if(synth.getVoices()[i]->isNotePlayed() && !synth.getVoices()[i]->isActive()){
          MIDI.sendNoteOn(synth.getVoices()[i]->currentNote, 0, 1);
          synth.getVoices()[i]->setNotePlayed(false);
        }else if(synth.getVoices()[i]->isNotePlayed() && synth.getVoices()[i]->isActive()){
          MIDI.sendNoteOn(synth.getVoices()[i]->currentNote, 51, 1);
        }
      }
    }
    
    clock_count = 0;
  }

}

/**
 * Midi note on callback
 */
void onNoteOn(byte channel, byte note, byte velocity) {
  synth.noteOn(note);
  digitalWrite(ledPin, HIGH);

  if(controllerIsLaunchpad){
    MIDI.sendNoteOn(note, 51, 1);
  }
}

/**
 * Midi note off callback
 */
void onNoteOff(byte channel, byte note, byte velocity) {
  synth.noteOff(note);
  digitalWrite(ledPin, LOW);
}

/**
 * Midi stop callback
 */
void onStop() {
  synth.stop();
}


void myControlChange(byte channel, byte control, byte value){
  Serial.print("myControlChange ");
  Serial.print(channel);
  Serial.print(" ");
  Serial.print(control);
  Serial.print(" ");
  Serial.print(value);
}


byte noteToLaunchpadNote(byte note){
  //byte launchpadNote = this->currentNote%8 + (this->currentNote/8 * 16);
  return note/16*8 + note%8;
}
