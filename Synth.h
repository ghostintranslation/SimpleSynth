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
    
    byte synthesis;
    byte mode;
    unsigned int attack;
    unsigned int decay;
    unsigned int release;
    int modulatorFrequency;
    float modulatorAmplitude;
    byte synthPin;
    byte modePin;
    byte functionPin;
    byte modulatorFrequencyPin;
    byte modulatorAmplitudePin;
    byte attackPin;
    byte decayPin;
    byte releasePin;

    AudioConnection* patchCords[voiceCount/4 + voiceCount]; 
    AudioMixer4 *mixers[voiceCount/2];
    AudioMixer4 *output;
    
  public:
    Synth();
    Synth(byte synthPin, byte modePin, byte functionPin, byte modulatorFrequencyPin, byte modulatorAmplitudePin, byte mixPin, byte attackPin, byte decayPin, byte releasePin);

    void init();
    void noteOn(byte midiNote);
    void noteOff(byte midiNote);
    void stop();
    void update();
    Voice **getVoices();
    AudioMixer4 * getOutput();

};

/**
 * Constructor
 */
inline Synth::Synth(){
  this->synthesis = 0;
  this->attack = 10;
  this->decay = 35;
  this->release = 2000;

  this->output = new AudioMixer4();
  this->output->gain(0, 1 );
  this->output->gain(1, 1 );
  this->output->gain(2, 1 );
  this->output->gain(3, 1 );

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
 * Constructor that sets the potentiometer pins
 */
inline Synth::Synth(byte synthPin, byte modePin, byte functionPin, byte modulatorFrequencyPin, byte modulatorAmplitudePin, byte mixPin, byte attackPin, byte decayPin, byte releasePin): Synth(){
  this->synthPin = synthPin;
  this->modePin = modePin;
  this->functionPin = functionPin;
  this->modulatorFrequencyPin = modulatorFrequencyPin;
  this->modulatorAmplitudePin = modulatorAmplitudePin;
  this->attackPin = attackPin;
  this->decayPin = decayPin;
  this->releasePin = releasePin;
}

inline void Synth::init(){
  
//  Serial.println("init");
//  for (int i = 0; i < voiceCount; i++) {
//  Serial.println("new Voice");
//    this->voices[i] = new Voice();
//  Serial.println("setADR");
//    this->voices[i]->setADR(this->attack, this->decay, this->release);
//  Serial.println("Voice AudioConnection");
//    this->patchCords[i] = new AudioConnection(*this->voices[i]->getOutput(), 0, *this->mixers[i/4], i%4);
//  }
}

/**
 * Note on
 */
inline void Synth::noteOn(byte note){
  bool foundOne = false;
  int oldestVoice = 0;
  unsigned long oldestVoiceTime = 0;
  unsigned long currentTime = millis();

  // In Drone mode, only one voice playing at a time
  if(this->mode == 2){ // TODO: Maybe have the enums outside the Voice to use it here too
    this->voices[0]->noteOn(note);
  }else{
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


/**
 * Update
 */
inline void Synth::update(){

  // Synthesis
  byte synthesis = (byte)map(analogRead(this->synthPin), 0, 1023, 0, 4);
  if(this->synthesis != synthesis){
    this->synthesis = synthesis;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setSynth(synthesis);
    }
  }

  // Mode
  byte mode = (byte)map(analogRead(this->modePin), 0, 1023, 0, 2);
  if(this->mode != mode){
    this->mode = mode;
    Serial.println(mode);
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setMode(mode);
    }

    if(this->mode == 2){
      this->mixers[0]->gain(0, 0.6 );
      this->mixers[0]->gain(1, 0 );
      this->mixers[0]->gain(2, 0 );
      this->mixers[0]->gain(3, 0 );
      this->output->gain(0, 1 );
      this->output->gain(1, 0 );
      this->output->gain(2, 0 );
      this->output->gain(3, 0 );
    }else{
      this->mixers[0]->gain(0, 0.6 );
      this->mixers[0]->gain(1, 0.6 );
      this->mixers[0]->gain(2, 0.6 );
      this->mixers[0]->gain(3, 0.6 );
      this->output->gain(0, 1 );
      this->output->gain(1, 1 );
      this->output->gain(2, 1 );
      this->output->gain(3, 1 );
    }
  }
  
  // Attack
  // TODO This should send a standardized value from 0 to 1023
  int attack = map(analogRead(this->attackPin), 0, 1023, 0, 2000);
  if(this->attack != attack){
    this->attack = attack;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setAttack(attack);
    }
  }

  // Decay
  // TODO This should send a standardized value from 0 to 1023
  int decay = map(analogRead(this->decayPin), 0, 1023, 0, 2000);
  if(this->decay != decay){
    this->decay = decay;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setDecay(decay);
    }
  }

  // Release
  // TODO This should send a standardized value from 0 to 1023
  int release = map(analogRead(this->releasePin), 0, 1023, 0, 2000);
  if(this->release != release){
    this->release = release;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setRelease(release);
    }
  }
  
  // Modulator frequency
  // TODO This should send a standardized value from 0 to 1023
  int modulatorFrequency = map(analogRead(this->modulatorFrequencyPin), 0, 1023, 1, 50);
  if(this->modulatorFrequency != modulatorFrequency){
    this->modulatorFrequency = modulatorFrequency;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setModulatorFrequency(modulatorFrequency);
    }
  }
  
  // Modulator amplitude
  // TODO This should send a standardized value from 0 to 1023
  float modulatorAmplitude = (float)analogRead(this->modulatorAmplitudePin)/(float)4096;
  if(this->modulatorAmplitude != modulatorAmplitude){
    this->modulatorAmplitude = modulatorAmplitude;
    for (int i = 0; i < voiceCount ; i++) {
      this->voices[i]->setModulatorAmplitude(modulatorAmplitude);
    }
  }
}

inline Voice** Synth::getVoices(){
  return this->voices;
}

#endif
