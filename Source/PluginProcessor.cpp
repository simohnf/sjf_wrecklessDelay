/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SjfWrecklessDelayAudioProcessor::SjfWrecklessDelayAudioProcessor()
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
, parameters(*this, nullptr, juce::Identifier("SjfWrecklessDelay"), createParameterLayout() )
{
    DBG( "inside construtor");
//    delayLine.intialise( getSampleRate(), getTotalNumInputChannels(), getTotalNumOutputChannels(), getBlockSize() );
    
    
    
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
//    overdriveOutParameter = parameters.getRawParameterValue("overdriveOut");
    overdrivePlacementParameter = parameters.getRawParameterValue("overdrivePlacement");
    lfoRateParameter = parameters.getRawParameterValue("lfoRate");
    lfoDepthParameter = parameters.getRawParameterValue("lfoDepth");
    interpolationTypeParameter = parameters.getRawParameterValue("interpolationType");

    initialise( getSampleRate() );
}

SjfWrecklessDelayAudioProcessor::~SjfWrecklessDelayAudioProcessor()
{
}

void SjfWrecklessDelayAudioProcessor::initialise( int sampleRate )
{
    if (sampleRate == 0){ sampleRate = 44100; }
    DBG("INITIALISING");
    static constexpr float RAMP_LENGTH = 0.005f;
    auto skewVal = calculateLPFCoefficient< float > ( 1, sampleRate );
    //--------------------------------------------------
    static constexpr float WINDOW_SIZE = 50;
    for ( int i = 0; i < NUM_CHANNELS; i++ )
    {
        m_pitchShifter[ i ].initialise( sampleRate, WINDOW_SIZE );
        m_delayLine[ i ].initialise( sampleRate * 4 );
        
//        m_fbSmoother[ i ].setCutoff( skewVal );
//        m_delayTimeSmoother[ i ].setCutoff( skewVal );
        
        DBG("SET DELAY SMOOTHER");
        m_delayTimeSmoothers[ i ].reset( sampleRate, RAMP_LENGTH*20.0f );
        
        m_fbSmoothers[ i ].reset( sampleRate, RAMP_LENGTH );
    }
    
    m_modPhasor.initialise( getSampleRate(), 1 );
    //--------------------------------------------------
    
    overdriveGain.reset(sampleRate, RAMP_LENGTH);
    overdriveGain.setCurrentAndTargetValue(*overdriveGainParameter);
//    overdriveOut.reset(sampleRate, RAMP_LENGTH);
//    overdriveOut.setCurrentAndTargetValue(*overdriveOutParameter);
    
    
    lfoD.reset(sampleRate, RAMP_LENGTH);
    lfoD.setCurrentAndTargetValue(*lfoDepthParameter * 0.01f );
    lfoR.reset(sampleRate, RAMP_LENGTH);
    lfoR.setCurrentAndTargetValue(*lfoRateParameter);
    
    dry.reset(sampleRate, RAMP_LENGTH);
    dry.setCurrentAndTargetValue(*dryParameter);
    
    wet.reset(sampleRate, RAMP_LENGTH);
    wet.setCurrentAndTargetValue(*wetParameter);
    
//    fbL.reset(sampleRate, RAMP_LENGTH);
//    fbL.setCurrentAndTargetValue(*fbLParameter);
//    fbR.reset(sampleRate, RAMP_LENGTH);
//    fbR.setCurrentAndTargetValue(*fbRParameter);
    
    lpCutOff.reset(sampleRate, RAMP_LENGTH);
    lpCutOff.setCurrentAndTargetValue(*lpCutOffParameter);
    hpCutOff.reset(sampleRate, RAMP_LENGTH);
    hpCutOff.setCurrentAndTargetValue(*hpCutOffParameter);
    
    DBG("INITIALISED");
}
//==============================================================================
const juce::String SjfWrecklessDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SjfWrecklessDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SjfWrecklessDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SjfWrecklessDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SjfWrecklessDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SjfWrecklessDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SjfWrecklessDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SjfWrecklessDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SjfWrecklessDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void SjfWrecklessDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SjfWrecklessDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    initialise( sampleRate );
}

void SjfWrecklessDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SjfWrecklessDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (/* layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && */ layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void SjfWrecklessDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    static constexpr int TABLE_SIZE = 4096;
    static constexpr int MOD_OFFSET = TABLE_SIZE / NUM_CHANNELS;
    static constexpr auto sinWavetable = sinArray< float, TABLE_SIZE >();
    float delayed = 0, inSamp, drySmoothed, wetSmoothed, postFBval, dt, hpfCutoffSmoothed, lpfCutoffSmoothed, modFactor, driveFactor, phasorOut;
    for ( int indexThroughBuffer = 0; indexThroughBuffer < bufferSize; indexThroughBuffer++ )
    {
        drySmoothed = dry.getNextValue();
        wetSmoothed = wet.getNextValue();
        hpfCutoffSmoothed = hpCutOff.getNextValue();
        lpfCutoffSmoothed = lpCutOff.getNextValue();
        
        float d = lfoD.getNextValue();
        m_modPhasor.setFrequency( lfoR.getNextValue() );
        phasorOut = m_modPhasor.output() * TABLE_SIZE;
        
        
        driveFactor = overdriveGain.getNextValue();
        
        for ( int channel = 0; channel < NUM_CHANNELS; channel++ )
        {
            inSamp = buffer.getSample( channel, indexThroughBuffer );
            // set delay time
            dt = m_delayTimeSmoothers[ channel ].getNextValue();
            phasorOut += channel * MOD_OFFSET;
            fastMod3< float >( phasorOut, TABLE_SIZE );
            modFactor = sinWavetable.getValue( phasorOut ) * d;
//            DBG( modFactor );
            dt += ( dt * modFactor );
            m_delayLine[ channel ].setDelayTimeSamps( dt );
            // read from delay with pitchShift
            delayed = m_delayLine[ channel ].getSample2();
            if ( m_detune[ channel ] != 1 )
            {
                m_pitchShifter[ channel ].setSample( delayed );
                delayed = m_pitchShifter[ channel ].pitchShiftOutput( m_detune[ channel ] );
            }
            // apply overdrive
            // filter
            if( overdriveFlag && *overdrivePlacementParameter == 2)
            { // drive before filters
                sjf_drive< float >::driveInPlace( delayed, driveFactor );
            }
            m_lpf[ channel ].setCutoff( lpfCutoffSmoothed );
            m_lpf[ channel ].filterInPlace( delayed );
            m_hpf[ channel ].setCutoff( hpfCutoffSmoothed );
            m_hpf[ channel ].filterInPlaceHP( delayed );
            postFBval = delayed * m_fbSmoothers[ channel ].getNextValue();
//            postFBval = delayed * m_fbSmoother[ channel ].filterInput( m_fb[ channel ] );
            if (fbControlFlag)
            { postFBval = juce::dsp::FastMathApproximations::tanh( postFBval ); }
            // add delayed output (with feedback) to dry input and write to delay
            if ( !overdriveFlag || *overdrivePlacementParameter != 1 )
            { m_delayLine[ channel ].setSample2( postFBval + inSamp ); }
            else
            { // drive input into delayline
                m_delayLine[ channel ].setSample2( sjf_drive< float >::driveInput( inSamp, driveFactor) + postFBval );
            }

            if( overdriveFlag && *overdrivePlacementParameter == 3)
            { // drive before output
                sjf_drive< float >::driveInPlace( delayed, driveFactor );
            }
            // copy delayed signal to buffer
            delayed *= wetSmoothed;
            inSamp *= drySmoothed;
            buffer.setSample( channel, indexThroughBuffer, delayed + inSamp );
        }
    }
}

//==============================================================================
bool SjfWrecklessDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SjfWrecklessDelayAudioProcessor::createEditor()
{
    return new SjfWrecklessDelayAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SjfWrecklessDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SjfWrecklessDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType())){
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
}


void SjfWrecklessDelayAudioProcessor::checkParameters( int bufferSize )
{
    dry.setTargetValue( *dryParameter * 0.01f );
    wet.setTargetValue( *wetParameter * 0.01f );

    filterParameters();
    overdriveParameters();

    lfoParametersAndCalculations( );
    delayTimeCalculations( );
    feedbackParameters();
    detuneParameters();
    
    for ( int i = 0; i < NUM_CHANNELS; i++ )
    {
        m_delayLine[ i ].setInterpolationType( *interpolationTypeParameter );
        m_pitchShifter[ i ].setInterpolationType( *interpolationTypeParameter );
    }
}

void SjfWrecklessDelayAudioProcessor::detuneParameters()
{
    static constexpr float transpositionConversion = 0.01f/12.0f;
    m_detune[ 0 ] = std::pow(2, *detuneLParameter * transpositionConversion );
    m_detune[ 1 ] = std::pow(2, *detuneRParameter * transpositionConversion );
}

void SjfWrecklessDelayAudioProcessor::overdriveParameters()
{
    overdriveFlag = *overdriveFlagParameter;

    overdriveGain.setTargetValue( *overdriveGainParameter );
//    overdriveOut.setTargetValue( *overdriveOutParameter) ;
}

void SjfWrecklessDelayAudioProcessor::feedbackParameters()
{
    fbControlFlag = *fbControlParameter;
    fbLinkFlag = *fbLinkParameter;
    if (fbLinkFlag)
    {
        for ( int i = 0; i < NUM_CHANNELS; i++ )
        { m_fbSmoothers[ i ].setTargetValue( *fbLParameter / 100.0f ); }
    }
    else if (!fbLinkFlag)
    {
        m_fbSmoothers[ 0 ].setTargetValue( *fbLParameter / 100.0f );
        m_fbSmoothers[ 1 ].setTargetValue( *fbRParameter / 100.0f );
    }
}

void SjfWrecklessDelayAudioProcessor::filterParameters()
{
    hpCutOff.setTargetValue( calculateLPFCoefficient< float >( *hpCutOffParameter, getSampleRate() ) );
    lpCutOff.setTargetValue( calculateLPFCoefficient< float >( *lpCutOffParameter, getSampleRate() ) );
}

void SjfWrecklessDelayAudioProcessor::lfoParametersAndCalculations( )
{
    lfoD.setTargetValue(*lfoDepthParameter * 0.01f );
    lfoR.setTargetValue( *lfoRateParameter );
}

void SjfWrecklessDelayAudioProcessor::delayTimeCalculations( )
{
    linkFlag = *linkParameter;
    syncFlag = *syncParameter;
    
    if (!syncFlag && !linkFlag)
    {
        m_delayTimeSmoothers[ 0 ].setTargetValue( *delTLParameter );
        m_delayTimeSmoothers[ 1 ].setTargetValue( *delTRParameter );
    }
    else if (!syncFlag && linkFlag )
    {
        m_delayTimeSmoothers[ 0 ].setTargetValue( *delTLParameter );
        m_delayTimeSmoothers[ 1 ].setTargetValue( *delTLParameter );
    }
    else if (syncFlag && !linkFlag )
    {
        m_delayTimeSmoothers[ 0 ].setTargetValue( calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValLOffsetParameter) );
        m_delayTimeSmoothers[ 1 ].setTargetValue( calculateSyncedDelayTime(*syncValRParameter, *syncValRTypeParameter, *syncValROffsetParameter) );
    }
    else if (syncFlag && linkFlag)
    {
        m_delayTimeSmoothers[ 0 ].setTargetValue( calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValLOffsetParameter) );
        m_delayTimeSmoothers[ 1 ].setTargetValue( calculateSyncedDelayTime(*syncValLParameter, *syncValLTypeParameter, *syncValROffsetParameter) );
    }
//    DBG( "m_delayTime[ 0 ]" << m_delayTime[ 0 ] );
    const float scale = getSampleRate() * 0.001f;
    m_delayTimeSmoothers[ 0 ].setTargetValue( m_delayTimeSmoothers[ 0 ].getTargetValue() * scale );
    m_delayTimeSmoothers[ 1 ].setTargetValue( m_delayTimeSmoothers[ 1 ].getTargetValue() * scale );
//    m_delayTime[ 0 ] *= getSampleRate() * 0.001f;
//    m_delayTime[ 1 ] *= getSampleRate() * 0.001f;
//    DBG( "m_delayTime[ 0 ]" << m_delayTime[ 0 ] );
}

float SjfWrecklessDelayAudioProcessor::calculateSyncedDelayTime(int syncVal, int syncValType, float syncValOffset)
{
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

juce::AudioProcessorValueTreeState::ParameterLayout SjfWrecklessDelayAudioProcessor::createParameterLayout()
{
    DBG("creating parameter layout");
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    static constexpr int pIDVersionNumber = 1;
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "dry", pIDVersionNumber }, "Dry", 0.0f, 100.0f, 100.0f) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "wet", pIDVersionNumber }, "Wet", 0.0f, 100.0f, 80.0f) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "timeLeft", pIDVersionNumber }, "TimeLeft", 0.0f, 2000.0f, 500.0f) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "timeRight", pIDVersionNumber }, "TimeRight", 0.0f, 2000.0f, 500.0f) );
    juce::NormalisableRange < float > fbRange( 00.0f, 200.0f, 0.001f );
    fbRange.setSkewForCentre(50);
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "fbL", pIDVersionNumber }, "FbL", fbRange, 40.0f) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "fbR", pIDVersionNumber }, "FbR", fbRange, 40.0f) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "detuneL", pIDVersionNumber }, "DetuneL", -100.0f, 100.0f, 0.0f) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "detuneR", pIDVersionNumber }, "DetuneL", -100.0f, 100.0f, 0.0f) );
    params.add( std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ "link", pIDVersionNumber }, "Link", false) );
    params.add( std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ "sync", pIDVersionNumber }, "Sync", false) );
    params.add( std::make_unique<juce::AudioParameterInt> (juce::ParameterID{ "syncValL", pIDVersionNumber }, "SyncValL", 1, 5, 2 ) );
    params.add( std::make_unique<juce::AudioParameterInt> (juce::ParameterID{ "syncValR", pIDVersionNumber }, "SyncValR", 1, 5, 2 ) );
    params.add( std::make_unique<juce::AudioParameterInt> (juce::ParameterID{ "syncValLType", pIDVersionNumber }, "SyncValLType", 1, 3, 2 ) );
    params.add( std::make_unique<juce::AudioParameterInt> (juce::ParameterID{ "syncValRType", pIDVersionNumber }, "SyncValRType", 1, 3, 2 ) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "syncValLOffset", pIDVersionNumber }, "SyncValLOffset", -33.33f, 33.33f, 0.0f ) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "syncValROffset", pIDVersionNumber }, "SyncValROffset", -33.33f, 33.33f, 0.0f ) );
    params.add( std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ "fbLink", pIDVersionNumber }, "FbLink", false) );
    params.add( std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ "fbControl", pIDVersionNumber }, "FbControl", false) );
    juce::NormalisableRange< float > lpCutoffRange( 100.0f, 20000.0f, 0.001f);
    lpCutoffRange.setSkewForCentre(1000);
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "lpCutOff", pIDVersionNumber }, "LpCutOf", lpCutoffRange, 20000.0f ) );
    juce::NormalisableRange< float > hpCutoffRange( 20.0f, 10000.0f, 0.001f);
    hpCutoffRange.setSkewForCentre(500);
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "hpCutOff", pIDVersionNumber }, "HpCutOf", hpCutoffRange, 20.0f ) );
    params.add( std::make_unique<juce::AudioParameterBool> (juce::ParameterID{ "overdriveOn", pIDVersionNumber }, "OverdriveOn", false) );
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "overdriveGain", pIDVersionNumber }, "OverdriveGain", 0.00001f, 5.0f, 0.5f ) );
//    params.add( std::make_unique<juce::AudioParameterFloat> ("overdriveOut", "OverdriveOut", 0.0f, 1.0f, 1.0f ) );
    params.add( std::make_unique<juce::AudioParameterInt> (juce::ParameterID{ "overdrivePlacement", pIDVersionNumber }, "OverdrivePlacement", 1, 3, 1 ) );
    juce::NormalisableRange< float > lfoRateRange( 0.0001f, 20.0f, 0.001f);
    lfoRateRange.setSkewForCentre(0.7f);
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "lfoRate", pIDVersionNumber }, "LfoRate", lfoRateRange, 1.0f ) );
    juce::NormalisableRange< float > lfoDepthRange( 0.0f, 10.0f, 0.001f);
    lfoDepthRange.setSkewForCentre(0.5f);
    params.add( std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{ "lfoDepth", pIDVersionNumber }, "lfoDepth", lfoDepthRange, 0.0f ) );
    
    
    params.add( std::make_unique<juce::AudioParameterInt> (juce::ParameterID{ "interpolationType", pIDVersionNumber }, "InterpolationType", 1, 6, 1) );
    
    return params;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SjfWrecklessDelayAudioProcessor();
}



