#ifndef Input_h
#define Input_h

/*
 * Input
 */
class Input{
  
  private:
    byte analogInput;
    byte midiChannel;
    byte midiControl;
    byte value;
    
  public:
    Input();
    Input(byte analogInput);
    Input(byte midiChannel, byte midiControl);
    Input(byte analogInput, byte midiChannel, byte midiControl);
    byte getAnalogInput();
    byte getMidiChannel();
    byte getMidiControl();
    byte getValue();
    void setValue(byte value);
    void handleMidi(byte channel, byte control, byte value);
    void handleAnalog();
};

/**
 * Constructor
 */
inline Input::Input(){
}

inline Input::Input(byte analogInput){
  this->analogInput = analogInput;
}

inline Input::Input(byte midiChannel, byte midiControl){
  this->analogInput = 255;
  this->midiChannel = midiChannel;
  this->midiControl = midiControl;
}

inline Input::Input(byte analogInput, byte midiChannel, byte midiControl){
  this->analogInput = analogInput;
  this->midiChannel = midiChannel;
  this->midiControl = midiControl;
}

inline byte Input::getAnalogInput(){
  return this->analogInput;
}

inline byte Input::getMidiChannel(){
  return this->midiChannel;
}

inline byte Input::getMidiControl(){
  return this->midiControl;
}

inline byte Input::getValue(){
  return this->value;
}

inline void Input::setValue(byte value){
  this->value = value;
}

inline void Input::handleMidi(byte channel, byte control, byte value){
  if(channel == this->midiChannel && control == this->midiControl){
    byte byteValue = map(value, 0, 127, 0, 255);
    this->value = byteValue;
  }
}

inline void Input::handleAnalog(){
  if(this->analogInput < 255){
    byte value = map(analogRead(this->analogInput), 0, 1023, 0, 255);
  
    if(this->value != value){
      this->value = value;
    }
  }
}

#endif
