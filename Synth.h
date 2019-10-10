#ifndef Synth_h
#define Synth_h

#include <Audio.h>

#include "Voice.h"


// Number of voices
const byte voiceCount = 8; // max = 16

/*
 * Synth
 */
class Synth{
  
  private:
    Voice *voices[voiceCount];

    unsigned int attack;
    unsigned int decay;
    unsigned int release;
    int modulatorFrequency;
    float modulatorAmplitude;

    AudioConnection* patchCords[voiceCount/4 + voiceCount]; 
    AudioMixer4 *mixers[voiceCount/2];
    AudioMixer4 *output;
    
  public:
    Synth();

    void noteOn(byte midiNote);
    void noteOff(byte midiNote);
    void stop();
    void update();
    AudioMixer4 * getOutput();

};

/**
 * Constructor
 */
inline Synth::Synth(){
  
  this->attack = 10;
  this->decay = 35;
  this->release = 2000;

  this->output = new AudioMixer4();
  this->output->gain(0, 0.6 );
  this->output->gain(1, 0.6 );
  this->output->gain(2, 0.6 );
  this->output->gain(3, 0.6 );

  for (int i = 0; i < voiceCount/4; i++) {
    this->mixers[i] = new AudioMixer4();
    this->mixers[i]->gain(0, 0.6 );
    this->mixers[i]->gain(1, 0.6 );
    this->mixers[i]->gain(2, 0.6 );
    this->mixers[i]->gain(3, 0.6 );
    
    this->patchCords[i] = new AudioConnection(*this->mixers[i], 0, *this->output, i%4);
  }
  
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i] = new Voice();
    this->voices[i]->setADR(this->attack, this->decay, this->release);
    this->patchCords[i] = new AudioConnection(*this->voices[i]->getOutput(), 0, *this->mixers[i/4], i%4);
  }
}

/**
 * Note on
 */
inline void Synth::noteOn(byte note){
  bool foundOne = false;
  int oldestVoice = 0;
  unsigned long oldestVoiceTime = 0;
  unsigned long currentTime = millis();
  
  for (int i = 0; i < voiceCount; i++) {
    // Search for the oldest voice
    if(this->voices[i]->last_played > oldestVoiceTime){
      oldestVoiceTime = this->voices[i]->last_played;
      oldestVoice = i;
    }
    
    // Search for an inactive voice
    if(!this->voices[i]->isActive()){
      this->voices[i]->noteOn(note);
      foundOne = true;
      break;
    }
  }

  // No inactive voice then will take over the oldest note
  if(!foundOne){
    this->voices[oldestVoice]->noteOn(note);
  }
}

/**
 * Note off
 */
inline void Synth::noteOff(byte note){
  for (int i = 0; i < voiceCount ; i++) {
    if(this->voices[i]->currentNote == note){
      this->voices[i]->noteOff();
    }
  }
}

/**
 * Stop all the voices
 */
inline void Synth::stop(){
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i]->noteOff();
  }
}

/**
 * Return the audio output
 */
inline AudioMixer4 * Synth::getOutput(){
  return this->output;
}


inline void Synth::update(){
  int modulatorFrequency = map(analogRead(0), 0, 1023, 100, 2000);
  float modulatorAmplitude = (float)analogRead(1)/1000;

  if(this->modulatorFrequency != modulatorFrequency){
    this->modulatorFrequency = modulatorFrequency;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setModulatorFrequency(modulatorFrequency);
    }
  }
  
  if(this->modulatorAmplitude != modulatorAmplitude){
    Serial.println(modulatorAmplitude);
    this->modulatorAmplitude = modulatorAmplitude;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setModulatorAmplitude(modulatorAmplitude);
    }
  }
  
}

#endif
