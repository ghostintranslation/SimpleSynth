#ifndef Voice_h
#define Voice_h

#include <Audio.h>

/*
 * Voice
 */
class Voice{

  private:
    AudioSynthWaveformSine *osc;
    AudioSynthWaveformSineModulated *oscFm;
    AudioSynthWaveformSine *osc2;
    AudioSynthWaveformModulated *oscFm2;
    AudioEffectEnvelope *env;
    AudioEffectEnvelope *env2;
    AudioConnection* patchCords[7];
    AudioMixer4 *output;
    unsigned int attack;
    unsigned int decay;
    unsigned int release;

  public:
    Voice();

    byte currentNote; // The midi note currently being played.
    unsigned long last_played;
    
    AudioMixer4 * getOutput();
    void noteOn(byte midiNote = 0);
    void noteOff();
    void setADR(unsigned int attack, unsigned int decay, unsigned int release);
    bool isActive();
    void setModulatorFrequency(int freq);
    void setModulatorAmplitude(float amp);
};

/**
 * Constructor
 */
inline Voice::Voice(){

  this->osc = new AudioSynthWaveformSine();
  this->osc->amplitude(0.1);
  this->osc->frequency(1200);
  
  this->oscFm = new AudioSynthWaveformSineModulated();
  this->oscFm->amplitude(1);
  
  this->osc2 = new AudioSynthWaveformSine();
  this->osc2->amplitude(0.005);
  this->osc2->frequency(1200);
  
  this->oscFm2 = new AudioSynthWaveformModulated();
  this->oscFm2->begin(WAVEFORM_SAWTOOTH);
  this->oscFm2->amplitude(1);

  this->attack = 10;
  this->decay = 35;
  this->release = 2000;
  this->env = new AudioEffectEnvelope();
  this->env->attack(this->attack); // TODO Link to a potentiometer
  this->env->decay(this->decay); // TODO Link to a potentiometer
  this->env->release(this->release); // TODO Link to a potentiometer
  this->env->releaseNoteOn(4);
  this->env2 = new AudioEffectEnvelope();
  this->env2->attack(this->attack); // TODO Link to a potentiometer
  this->env2->decay(this->decay); // TODO Link to a potentiometer
  this->env2->release(this->release); // TODO Link to a potentiometer
  this->env2->releaseNoteOn(4);

  this->output = new AudioMixer4();
  this->output->gain(0, 0.3 ); // TODO Link to a potentiometer
  this->output->gain(1, 0 ); // TODO Link to a potentiometer
  
  this->patchCords[0] = new AudioConnection(*this->osc, 0, *this->oscFm, 0);
  this->patchCords[1] = new AudioConnection(*this->oscFm, 0, *this->env, 0);
  this->patchCords[2] = new AudioConnection(*this->env, 0, *this->output, 0);
  this->patchCords[3] = new AudioConnection(*this->osc2, 0, *this->oscFm2, 0);
  this->patchCords[4] = new AudioConnection(*this->oscFm2, 0, *this->env2, 0);
  this->patchCords[5] = new AudioConnection(*this->env2, 0, *this->output, 1);

}

/**
 * Set Attack Decay Release
 */
inline void Voice::setADR(unsigned int attack, unsigned int decay, unsigned int release){
  this->attack = attack;
  this->decay = decay;
  this->release = release;
  
  this->env->attack(attack);
  this->env->decay(decay);
  this->env->release(release);
  this->env2->attack(attack);
  this->env2->decay(decay);
  this->env2->release(release);
}

/**
 * Return the audio output
 */
inline AudioMixer4 * Voice::getOutput(){
  return this->output;
}

/**
 * Note one
 */
inline void Voice::noteOn(byte midiNote) {
  float freq = 440.0 * powf(2.0, (float)(midiNote - 69) * 0.08333333);
  this->oscFm->frequency(freq);
  this->oscFm2->frequency(freq);
  this->currentNote = midiNote;
  this->env->noteOn();
  this->env2->noteOn();
  this->last_played = millis();
}

/**
 * Note off
 */
inline void Voice::noteOff() {
  this->env->noteOff();
  this->env2->noteOff();
}

/**
 * Is the voice active
 */
inline bool Voice::isActive(){
  return this->env->isActive();
}


inline void Voice::setModulatorFrequency(int freq){
  this->osc->frequency(freq);
  this->osc2->frequency(freq);
}
inline void Voice::setModulatorAmplitude(float amp){
  this->osc->amplitude(amp);
  this->osc2->amplitude(amp);
}
#endif
