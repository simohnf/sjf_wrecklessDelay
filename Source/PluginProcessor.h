/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_pitchShifter.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_pitchShift.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_comb.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_overdrive.h"
//#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_oscillator.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_audioUtilities.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_lpf.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_wavetables.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_phasor.h"
//==============================================================================
/**
*/

class SjfWrecklessDelayAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
    static constexpr int NUM_CHANNELS = 2;
public:
    //==============================================================================
    SjfWrecklessDelayAudioProcessor();
    ~SjfWrecklessDelayAudioProcessor() override;

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
    bool getLinkState(){ return linkFlag; } 
    bool getSyncState(){ return syncFlag; }
    bool getFbLinkState(){ return fbLinkFlag; }
    void clearDelayBuffer()
    {
        for ( int i = 0; i < NUM_CHANNELS; i++)
        {
            m_pitchShifter[ i ].clearDelayline();
            m_delayLine[ i ].clearDelayline();
        }
    }
//    void filterSignal(juce::AudioBuffer<float> &buffer);
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
private:

    void initialise( int sampleRate );
    
    
//    void reset() override { lowpass.reset(); hipass.reset(); lowpass2.reset(); hipass2.reset(); };
//    juce::dsp::StateVariableTPTFilter<float> lowpass, hipass, lowpass2, hipass2;
    void checkParameters( int bufferSize );
    float calculateSyncedDelayTime(int syncVal, int syncValType, float syncValOffset);
    void delayTimeCalculations( );
    void lfoParametersAndCalculations( );
    void filterParameters();
    void feedbackParameters();
    void overdriveParameters();
    void detuneParameters();
    
//    sjf_pitchShifter delayLine;
    std::array< sjf_pitchShift< float >, NUM_CHANNELS > m_pitchShifter;
    std::array< sjf_delayLine< float >, NUM_CHANNELS > m_delayLine;
    std::array< sjf_lpf< float >, NUM_CHANNELS > m_lpf, m_hpf/*, m_fbSmoother, m_delayTimeSmoother*/;
    sjf_phasor< float > m_modPhasor;
    std::array< float, NUM_CHANNELS > m_fb, m_delayTime, m_detune;
    
    sjf_overdrive overdrive;
    juce::AudioBuffer<float> tempBuffer;
    
    
    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::PositionInfo positionInfo;
    
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<float>* dryParameter = nullptr;
    std::atomic<float>* wetParameter = nullptr;
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
    std::atomic<float>* fbControlParameter = nullptr;
    std::atomic<float>* hpCutOffParameter = nullptr;
    std::atomic<float>* lpCutOffParameter = nullptr;
    std::atomic<float>* overdriveFlagParameter = nullptr;
    std::atomic<float>* overdriveGainParameter = nullptr;
//    std::atomic<float>* overdriveOutParameter = nullptr;
    std::atomic<float>* overdrivePlacementParameter = nullptr;
    std::atomic<float>* lfoDepthParameter = nullptr;
    std::atomic<float>* lfoRateParameter = nullptr;
    std::atomic<float>* interpolationTypeParameter = nullptr;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dry, wet, /*fbL, fbR,*/ hpCutOff, lpCutOff, overdriveGain, /*overdriveOut,*/ lfoR, lfoD;
    std::array< juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, NUM_CHANNELS > m_delayTimeSmoothers, m_fbSmoothers;
    float detuneL, detuneR;
    float bpm = 120.0f, delTL, delTR;
    int syncValL, syncValR, syncValLType, syncValRType;
    float syncValLOffset, syncValROffset/*, overdrivePlacement*/;
    bool linkFlag, syncFlag, fbLinkFlag, fbControlFlag, overdriveFlag;

    
//    sjf_oscillator lfo;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SjfWrecklessDelayAudioProcessor)
};
