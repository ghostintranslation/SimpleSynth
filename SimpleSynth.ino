#include <Audio.h>
#include "Synth.h"

int ledPin = 13;

Synth synth;

AudioOutputI2S  i2s2;
AudioConnection patchCord1(*synth.getOutput(), 0, i2s2, 0);
AudioConnection patchCord2(*synth.getOutput(), 0, i2s2, 1);

AudioControlSGTL5000 sgtl5000_1;

/**
 * Setup
 */
void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  
  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleStop(onStop);
  usbMIDI.setHandleSystemReset(onStop);
  
  // Audio connections require memory to work.
  AudioMemory(20);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  
  while (!Serial && millis() < 2500); // wait for serial monitor
  
  Serial.println("Ready!");
  digitalWrite(ledPin, HIGH);
  delay(400);                 // Blink LED once at startup
  digitalWrite(ledPin, LOW);
}

/**
 * Loop
 */
void loop() {
  usbMIDI.read();
}

/**
 * Midi note on callback
 */
void onNoteOn(byte channel, byte note, byte velocity) {
  synth.noteOn(note);
  digitalWrite(ledPin, HIGH);
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
