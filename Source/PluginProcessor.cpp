/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SjfRecklessDelayAudioProcessor::SjfRecklessDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
, parameters(*this, nullptr, juce::Identifier("sjfRecklessDelay"),
    {
        std::make_unique<juce::AudioParameterFloat> ("dry", "Dry", 0.0f, 100.0f, 100.0f),
        std::make_unique<juce::AudioParameterFloat> ("wet", "Wet", 0.0f, 100.0f, 80.0f),
        std::make_unique<juce::AudioParameterFloat> ("timeLeft", "TimeLeft", 0.0f, 2000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat> ("timeRight", "TimeRight", 0.0f, 2000.0f, 500.0f),
        std::make_unique<juce::AudioParameterFloat> ("fbL", "FbL", 0.0f, 200.0f, 40.0f),
        std::make_unique<juce::AudioParameterFloat> ("fbR", "FbR", 0.0f, 200.0f, 40.0f),
        std::make_unique<juce::AudioParameterFloat> ("detuneL", "DetuneL", -100.0f, 100.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat> ("detuneR", "DetuneL", -100.0f, 100.0f, 0.0f),
        std::make_unique<juce::AudioParameterBool> ("link", "Link", false),
        std::make_unique<juce::AudioParameterBool> ("sync", "Sync", false),
        std::make_unique<juce::AudioParameterInt> ("syncValL", "SyncValL", 1, 5, 2 ),
        std::make_unique<juce::AudioParameterInt> ("syncValR", "SyncValR", 1, 5, 2 ),
        std::make_unique<juce::AudioParameterInt> ("syncValLType", "SyncValLType", 1, 3, 2 ),
        std::make_unique<juce::AudioParameterInt> ("syncValRType", "SyncValRType", 1, 3, 2 ),
        std::make_unique<juce::AudioParameterFloat> ("syncValLOffset", "SyncValLOffset", -33.33f, 33.33f, 0.0f ),
        std::make_unique<juce::AudioParameterFloat> ("syncValROffset", "SyncValROffset", -33.33f, 33.33f, 0.0f ),
        std::make_unique<juce::AudioParameterBool> ("fbLink", "FbLink", false),
        std::make_unique<juce::AudioParameterBool> ("fbControl", "FbControl", false),
        std::make_unique<juce::AudioParameterFloat> ("lpCutOff", "LpCutOf", 100.0f, 20000.0f, 20000.0f ),
        std::make_unique<juce::AudioParameterFloat> ("hpCutOff", "HpCutOf", 20.0f, 10000.0f, 20.0f ),
        std::make_unique<juce::AudioParameterBool> ("overdriveOn", "OverdriveOn", false),
        std::make_unique<juce::AudioParameterFloat> ("overdriveGain", "OverdriveGain", 1.0f, 10.0f, 1.0f ),
        std::make_unique<juce::AudioParameterFloat> ("overdriveOut", "OverdriveOut", 0.0f, 1.0f, 1.0f ),
        std::make_unique<juce::AudioParameterInt> ("overdrivePlacement", "OverdrivePlacement", 1, 3, 1 ),
        std::make_unique<juce::AudioParameterFloat> ("lfoRate", "LfoRate", 0.001f, 20.0f, 1.0f ),
        std::make_unique<juce::AudioParameterFloat> ("lfoDepth", "lfoDepth", 0.0f, 10.0f, 0.0f ),
    })
{
    delayLine.intialise( getSampleRate(), getTotalNumInputChannels(), getTotalNumOutputChannels(), getBlockSize() );
    
    dryParameter = parameters.getRawParameterValue("dry");
    wetParameter = parameters.getRawParameterValue("wet");
    delTLParameter = parameters.getRawParameterValue("timeLeft");
    delTRParameter = parameters.getRawParameterValue("timeRight");
    fbLParameter = parameters.getRawParameterValue("fbL");
    fbRParameter = parameters.getRawParameterValue("fbR");
    detuneLParameter = parameters.getRawParameterValue("detuneL");
    detuneRParameter = parameters.getRawParameterValue("detuneR");
    linkParameter = parameters.getRawParameterValue("link");
    syncParameter = parameters.getRawParameterValue("sync");
    syncValLParameter = parameters.getRawParameterValue("syncValL");
    syncValRParameter = parameters.getRawParameterValue("syncValR");
    syncValLTypeParameter = parameters.getRawParameterValue("syncValLType");
    syncValRTypeParameter = parameters.getRawParameterValue("syncValRType");
    syncValLOffsetParameter = parameters.getRawParameterValue("syncValLOffset");
    syncValROffsetParameter = parameters.getRawParameterValue("syncValROffset");
    fbLinkParameter = parameters.getRawParameterValue("fbLink");
    fbControlParameter = parameters.getRawParameterValue("fbControl");
    lpCutOffParameter = parameters.getRawParameterValue("lpCutOff");
    hpCutOffParameter = parameters.getRawParameterValue("hpCutOff");
    overdriveFlagParameter = parameters.getRawParameterValue("overdriveOn");
    overdriveGainParameter = parameters.getRawParameterValue("overdriveGain");
    overdriveOutParameter = parameters.getRawParameterValue("overdriveOut");
    overdrivePlacementParameter = parameters.getRawParameterValue("overdrivePlacement");
    lfoRateParameter = parameters.getRawParameterValue("lfoRate");
    lfoDepthParameter = parameters.getRawParameterValue("lfoDepth");

}

SjfRecklessDelayAudioProcessor::~SjfRecklessDelayAudioProcessor()
{
}

//==============================================================================
const juce::String SjfRecklessDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SjfRecklessDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SjfRecklessDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SjfRecklessDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SjfRecklessDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SjfRecklessDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SjfRecklessDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SjfRecklessDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SjfRecklessDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void SjfRecklessDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SjfRecklessDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dry.reset(sampleRate, 0.005);
    dry.setCurrentAndTargetValue(*dryParameter);
    
    wet.reset(sampleRate, 0.005);
    wet.setCurrentAndTargetValue(*wetParameter);
    
    fbL.reset(sampleRate, 0.005);
    fbL.setCurrentAndTargetValue(*fbLParameter);
    fbR.reset(sampleRate, 0.005);
    fbR.setCurrentAndTargetValue(*fbRParameter);
    
    lpCutOff.reset(sampleRate, 0.005);
    lpCutOff.setCurrentAndTargetValue(*lpCutOffParameter);
    hpCutOff.reset(sampleRate, 0.005);
    hpCutOff.setCurrentAndTargetValue(*hpCutOffParameter);
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2; // only stereo
    spec.sampleRate = sampleRate;
    
    lowpass.prepare(spec);
    lowpass.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    hipass.prepare(spec);
    hipass.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    
    lowpass2.prepare(spec);
    lowpass2.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    hipass2.prepare(spec);
    hipass2.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    
    overdriveGain.reset(sampleRate, 0.005);
    overdriveGain.setCurrentAndTargetValue(*overdriveGainParameter);
    overdriveOut.reset(sampleRate, 0.005);
    overdriveOut.setCurrentAndTargetValue(*overdriveOutParameter);
    
    
    lfoD.reset(sampleRate, 0.005);
    lfoD.setCurrentAndTargetValue(*lfoDepthParameter);
    lfoR.reset(sampleRate, 0.005);
    lfoR.setCurrentAndTargetValue(*lfoRateParameter);
    
    lfo.initialise(sampleRate, lfoR.getCurrentValue());
    
    reset();
}

void SjfRecklessDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SjfRecklessDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SjfRecklessDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto bufferSize = buffer.getNumSamples( );
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear ( i, 0, bufferSize );
    
    playHead = this->getPlayHead();
    if (playHead != nullptr)
    {
        positionInfo = *playHead->getPosition();
        if( positionInfo.getBpm() ){
            bpm = *positionInfo.getBpm();
        }
    }
    
    
    checkParameters( bufferSize );
    

    tempBuffer.makeCopyOf( buffer );

    // this is just so I can prevent any noise in input
    auto block = juce::dsp::AudioBlock<float> (tempBuffer);
    auto contextIn = juce::dsp::ProcessContextReplacing<float> (block);
    lowpass.setCutoffFrequency(20000.0f);
    lowpass.process( contextIn );
    hipass.setCutoffFrequency(20.0f);
    hipass.process( contextIn );

    if (!overdriveFlag)
    {
        overdriveGain.setCurrentAndTargetValue( overdriveGain.getTargetValue() ) ;
        overdriveOut.setCurrentAndTargetValue( overdriveOut.getTargetValue() );
    }

    if(*overdrivePlacementParameter == 1 && overdriveFlag)
    {
        overdrive.drive( tempBuffer, overdriveGain.getNextValue() );
        for (int channel = 0; channel < tempBuffer.getNumChannels(); channel++)
        {
            tempBuffer.applyGain( channel, 0, tempBuffer.getNumSamples(), overdriveOut.getNextValue() );
        }
    }
    delayLine.writeToDelayBuffer( tempBuffer, 1.0f ); // write dry signal to delay line
    
    tempBuffer.clear( );

    delayLine.copyFromDelayBufferWithPitchShift(tempBuffer, 1.0f, detuneL/100.0f, detuneR/100.0f);
    
    if(*overdrivePlacementParameter == 2 && overdriveFlag)
    {
        overdrive.drive( tempBuffer, overdriveGain.getNextValue() );
        for (int channel = 0; channel < tempBuffer.getNumChannels(); channel++)
        {
            tempBuffer.applyGain( channel, 0, tempBuffer.getNumSamples(), overdriveOut.getNextValue() );
        }
    }
    // process delayLineOutput(tempBuffer) here
    if(fbControlFlag){ overdrive.drive(tempBuffer, 1.0f); }
    filterSignal(tempBuffer);
    
    delayLine.addToDelayBuffer( tempBuffer, fbL.getNextValue() / 100.0f, fbR.getNextValue() / 100.0f ); // apply feedback level and add back into delay buffer
    
    buffer.applyGain(0, bufferSize, dry.getNextValue() / 100.0f); // apply dry level
    
    if(*overdrivePlacementParameter == 3 && overdriveFlag)
    {
        overdrive.drive( tempBuffer, overdriveGain.getNextValue() );
        for (int channel = 0; channel < tempBuffer.getNumChannels(); channel++)
        {
            tempBuffer.applyGain( channel, 0, tempBuffer.getNumSamples(), overdriveOut.getNextValue() );
        }
    }
    tempBuffer.applyGain( 0, bufferSize, wet.getNextValue() / 100.0f ); // apply wet level here
    
    for (int channel = 0; channel < buffer.getNumChannels(); channel ++)
    { buffer.addFrom( channel, 0, tempBuffer, channel, 0, bufferSize ) ; } // add delay output to buffer
    
    delayLine.updateBufferPositions( buffer.getNumSamples() );
}

//==============================================================================
bool SjfRecklessDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SjfRecklessDelayAudioProcessor::createEditor()
{
    return new SjfRecklessDelayAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SjfRecklessDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SjfRecklessDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType())){
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
}


void SjfRecklessDelayAudioProcessor::checkParameters( int bufferSize )
{

    if (dry.getTargetValue() != *dryParameter){
        dry.setTargetValue( *dryParameter );
    }
    if (wet.getTargetValue() != *wetParameter){
        wet.setTargetValue( *wetParameter );
    }

    filterParameters();
    overdriveParameters();

    auto delTimeMod = lfoParametersAndCalculations( bufferSize );
    delayTimeCalculations( delTimeMod );
    feedbackParameters();
    detuneParameters();
}

void SjfRecklessDelayAudioProcessor::detuneParameters(){
    detuneL = *detuneLParameter;
    detuneR = *detuneRParameter;
}

void SjfRecklessDelayAudioProcessor::overdriveParameters()
{
    overdriveFlag = *overdriveFlagParameter;
    if (overdriveGain.getTargetValue() != *overdriveGainParameter){
        overdriveGain.setTargetValue( *overdriveGainParameter );
    }
    if (overdriveOut.getTargetValue() != *overdriveOutParameter){
        overdriveOut.setTargetValue( *overdriveOutParameter) ;
    }
}

void SjfRecklessDelayAudioProcessor::feedbackParameters()
{
    fbControlFlag = *fbControlParameter;
    fbLinkFlag = *fbLinkParameter;
    if (fbLinkFlag){
        if (fbL.getTargetValue() != *fbLParameter){
            fbL.setTargetValue( *fbLParameter );
        }
        if (fbR.getTargetValue() != *fbLParameter){
            fbR.setTargetValue( *fbLParameter );
        }
    }
    else if (!fbLinkFlag){
        if (fbL.getTargetValue() != *fbLParameter){
            fbL.setTargetValue( *fbLParameter );
        }
        if (fbR.getTargetValue() != *fbRParameter){
            fbR.setTargetValue( *fbRParameter );
        }
    }
}

void SjfRecklessDelayAudioProcessor::filterParameters()
{
    if (lpCutOff.getTargetValue() != *lpCutOffParameter){
        lpCutOff.setTargetValue(*lpCutOffParameter);
    }
    if (hpCutOff.getTargetValue() != *hpCutOffParameter){
        hpCutOff.setTargetValue(*hpCutOffParameter);
    }
}

float SjfRecklessDelayAudioProcessor::lfoParametersAndCalculations(int bufferSize)
{
    if(lfoD.getTargetValue() != *lfoDepthParameter){
        lfoD.setTargetValue(*lfoDepthParameter);
    }
    if(lfoR.getTargetValue() != *lfoRateParameter){
        lfoR.setTargetValue(*lfoRateParameter);
    }
    lfo.setFrequency( lfoR.getNextValue() );
    return lfo.outputSample( bufferSize ) * lfoD.getNextValue()/100.0f;
}

void SjfRecklessDelayAudioProcessor::delayTimeCalculations(float delTimeMod)
{
    linkFlag = *linkParameter;
    syncFlag = *syncParameter;
    
    if (!syncFlag && !linkFlag){
        float delT = *delTLParameter;
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeL() != delT){
            delayLine.setDelTimeL(delT);
        }
        delT = *delTRParameter;
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeR() != delT){
            delayLine.setDelTimeR( delT );
        }
    }
    else if (!syncFlag && linkFlag ){
        float delT = *delTLParameter;
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeL() != delT){
            delayLine.setDelTimeL(delT);
        }
        if (delayLine.getDelTimeR() != delT){
            delayLine.setDelTimeR(delT);
        }
    }
    else if (syncFlag && !linkFlag ){
        auto delT = calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValLOffsetParameter);
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeL() != delT){
            delayLine.setDelTimeL(delT);
        }
        delT = calculateSyncedDelayTime(*syncValRParameter, *syncValRTypeParameter, *syncValROffsetParameter);
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeR() != delT){
            delayLine.setDelTimeR(delT);
        }
    }
    else if (syncFlag && linkFlag){
        auto delT = calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValLOffsetParameter);
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeL() != delT){
            delayLine.setDelTimeL(delT);
        }
        delT = calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValROffsetParameter);
        delT += delT * delTimeMod;
        if (delayLine.getDelTimeR() != delT){
            delayLine.setDelTimeR(delT);
        }
    }
}

float SjfRecklessDelayAudioProcessor::calculateSyncedDelayTime(int syncVal, int syncValType, float syncValOffset){
    auto del = (2*60000.0f/bpm) * pow(2, -1*(syncVal-1));
    switch(syncValType){
        case 1:
            del *= 1.5f;
            break;
        case 2:
            break;
        case 3:
            del *= 2.0f;
            del /= 3.0f;
            break;
    }
    del += del*(syncValOffset/100.f);
    return del;
}

void SjfRecklessDelayAudioProcessor::filterSignal(juce::AudioBuffer<float> &buffer){
    auto block = juce::dsp::AudioBlock<float> (buffer);
    auto context = juce::dsp::ProcessContextReplacing<float> (block);
    auto cof = 20000*pow(2, -1 * ( std::fmax(detuneL, detuneR) / 1200.0f ) );
    cof = std::fmin(lpCutOff.getNextValue(), cof);
    if (cof > 20000.0f) { cof = 20000.0f; }
    lowpass2.setCutoffFrequency(cof);
    lowpass2.process( context );
    cof = 20*pow(2, -1 * ( std::fmin(detuneL, detuneR) / 1200.0f ) );
    cof = std::fmax(hpCutOff.getNextValue(), cof);
    if (cof < 20.0f) { cof = 20.0f; }
    hipass2.setCutoffFrequency(cof);
    hipass2.process( context );
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SjfRecklessDelayAudioProcessor();
}



