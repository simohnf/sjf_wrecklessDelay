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
//        std::make_unique<juce::AudioParameterFloat> ("delT", "DelT", 0.0f, 2000.0f, 500.0f),
//        std::make_unique<juce::AudioParameterFloat> ("stSpread", "StSpread", -100.0f, 100.0f, 0.0f),
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
        std::make_unique<juce::AudioParameterFloat> ("syncValLOffset", "SyncValLOffset", -33, 33, 0 ),
        std::make_unique<juce::AudioParameterFloat> ("syncValROffset", "SyncValROffset", -33, 33, 0 ),
        std::make_unique<juce::AudioParameterBool> ("fbLink", "FbLink", false),
    })
{
    delayLine.intialise( getSampleRate(), getTotalNumInputChannels(), getTotalNumOutputChannels(), getBlockSize() );
    
    dryParameter = parameters.getRawParameterValue("dry");
    wetParameter = parameters.getRawParameterValue("wet");
//    delTParameter = parameters.getRawParameterValue("delT");
//    stSpreadParameter = parameters.getRawParameterValue("stSpread");
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
    dry.reset(getSampleRate(), 0.005);
    dry.setCurrentAndTargetValue(100.0f);
    
    wet.reset(getSampleRate(), 0.005);
    wet.setCurrentAndTargetValue(80.0f);
    
    fbL.reset(getSampleRate(), 0.005);
    fbL.setCurrentAndTargetValue(40.0f);
    fbR.reset(getSampleRate(), 0.005);
    fbR.setCurrentAndTargetValue(40.0f);
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
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
    
    checkParameters();
    
    
    tempBuffer.makeCopyOf( buffer );
    tempBuffer.clear( );
    
    delayLine.writeToDelayBuffer( buffer, 1.0f ); // write dry signal to delay line
    
    delayLine.copyFromDelayBufferWithPitchShift(tempBuffer, 1.0f, detuneL/100.0f, detuneR/100.0f);
    
    // process tempBuffer here
    
    delayLine.addToDelayBuffer( tempBuffer, fbL.getNextValue() / 100.0f, fbR.getNextValue() / 100.0f ); // apply feedback level and add back into delay buffer
    
    
    buffer.applyGain(0, bufferSize, dry.getNextValue() / 100.0f); // apply dry level
    
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


void SjfRecklessDelayAudioProcessor::checkParameters()
{
    if (dry.getTargetValue() != *dryParameter){
        dry.setTargetValue( *dryParameter );
    }
    if (wet.getTargetValue() != *wetParameter){
        wet.setTargetValue( *wetParameter );
    }
    link = *linkParameter;
    sync = *syncParameter;
    delayTimeCalculations();
    fbLink = *fbLinkParameter;
    if (fbLink){
        if (fbL.getTargetValue() != *fbLParameter){
            fbL.setTargetValue( *fbLParameter );
        }
        if (fbR.getTargetValue() != *fbLParameter){
            fbR.setTargetValue( *fbLParameter );
        }
    }
    else if (!fbLink){
        if (fbL.getTargetValue() != *fbLParameter){
            fbL.setTargetValue( *fbLParameter );
        }
        if (fbR.getTargetValue() != *fbRParameter){
            fbR.setTargetValue( *fbRParameter );
        }
    }
    if (detuneL != *detuneLParameter){
        detuneL = *detuneLParameter;
    }
    if (detuneR != *detuneRParameter){
        detuneR = *detuneRParameter;
    }
} 

void SjfRecklessDelayAudioProcessor::delayTimeCalculations(){
    if (!sync && !link){
        if (delayLine.getDelTimeL() != *delTLParameter){
            delayLine.setDelTimeL(*delTLParameter);
        }
        if (delayLine.getDelTimeR() != *delTRParameter){
            delayLine.setDelTimeR(*delTRParameter);
        }
    }
    else if (!sync && link ){
        if (delayLine.getDelTimeL() != *delTLParameter){
            delayLine.setDelTimeL(*delTLParameter);
        }
        if (delayLine.getDelTimeR() != *delTLParameter){
            delayLine.setDelTimeR(*delTLParameter);
        }
    }
    else if (sync && !link ){
        auto delL = calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValLOffsetParameter);
        if (delayLine.getDelTimeL() != delL){
            delayLine.setDelTimeL(delL);
        }
        auto delR = calculateSyncedDelayTime(*syncValRParameter, *syncValRTypeParameter, *syncValROffsetParameter);
        if (delayLine.getDelTimeR() != delR){
            delayLine.setDelTimeR(delR);
        }
    }
    else if (sync && link){
        auto delL = calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValLOffsetParameter);
        if (delayLine.getDelTimeL() != delL){
            delayLine.setDelTimeL(delL);
        }
        auto delR = calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValROffsetParameter);
        if (delayLine.getDelTimeR() != delR){
            delayLine.setDelTimeR(delR);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SjfRecklessDelayAudioProcessor();
}



