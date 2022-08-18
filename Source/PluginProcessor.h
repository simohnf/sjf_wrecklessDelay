/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_audio.hpp"
//==============================================================================
/**
*/
class SjfRecklessDelayAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SjfRecklessDelayAudioProcessor();
    ~SjfRecklessDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    bool getLinkState(){ return link; }
    bool getSyncState(){ return sync; }
    bool getFbLinkState(){ return fbLink; }
    
    
    
private:
    
    void checkParameters();
    float calculateSyncedDelayTime(int syncVal, int syncValType, float syncValOffset);
    void delayTimeCalculations();
    
    sjf_pitchShifter delayLine;
    juce::AudioBuffer<float> tempBuffer;
    
    
    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::PositionInfo positionInfo;
    
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* dryParameter = nullptr;
    std::atomic<float>* wetParameter = nullptr;
//    std::atomic<float>* delTParameter = nullptr;
//    std::atomic<float>* stSpreadParameter = nullptr;
    std::atomic<float>* delTLParameter = nullptr;
    std::atomic<float>* delTRParameter = nullptr;
    std::atomic<float>* fbLParameter = nullptr;
    std::atomic<float>* fbRParameter = nullptr;
    std::atomic<float>* detuneLParameter = nullptr;
    std::atomic<float>* detuneRParameter = nullptr;
    std::atomic<float>* linkParameter = nullptr;
    std::atomic<float>* syncParameter = nullptr;
    std::atomic<float>* syncValLParameter = nullptr;
    std::atomic<float>* syncValRParameter = nullptr;
    std::atomic<float>* syncValLTypeParameter = nullptr;
    std::atomic<float>* syncValRTypeParameter = nullptr;
    std::atomic<float>* syncValLOffsetParameter = nullptr;
    std::atomic<float>* syncValROffsetParameter = nullptr;
    std::atomic<float>* fbLinkParameter = nullptr;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dry, wet, fbL, fbR;
    //    float dry = 100.0f;
    //    float wet = 80.0f;
    //    float fb = 40.0f;
    //    float delT = 500.0f;
    //    float stSpread = 0.0f;
    float detuneL, detuneR;
    float bpm = 120.0f;
    int syncValL, syncValR, syncValLType, syncValRType;
    float syncValLOffset, syncValROffset;
    bool link, sync, fbLink;

    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SjfRecklessDelayAudioProcessor)
};
