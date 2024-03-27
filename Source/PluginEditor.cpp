/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define potSize 80
#define textHeight 20
#define indent 10
#define alph 0.5f

#define WIDTH potSize*5 + indent*4
#define HEIGHT potSize*3 + textHeight*5 + indent
#define delayTimeBackgroundColour juce::Colour(25, 50, 25).withAlpha( alph )
#define detuneBackgroundColour juce::Colour(5, 25, 50).withAlpha( alph )
#define feedbackBackgroundColour juce::Colour(50, 5, 50).withAlpha( alph )
#define filterBackgroundColour juce::Colour(75, 50, 50).withAlpha( alph )
#define overdriveBackgroundColour juce::Colour(50, 75, 50).withAlpha( alph )
#define lfoBackgroundColour juce::Colour(50, 50, 75).withAlpha( alph )
#define dryBackgroundColour juce::Colour(1, 25, 50).withAlpha( alph )
#define wetBackgroundColour juce::Colour(50, 25, 1).withAlpha( alph )
#define clearBufferBackgroundColour juce::Colour(100, 1, 1).withAlpha( alph )
//==============================================================================
SjfWrecklessDelayAudioProcessorEditor::SjfWrecklessDelayAudioProcessorEditor (SjfWrecklessDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p),  audioProcessor (p), valueTreeState (vts)
{
    setLookAndFeel ( &otherLookandFeel );
    
//    dryLabel.setText ("Dry", juce::dontSendNotification);
//    addAndMakeVisible (&dryLabel);
//    dryLabel.setJustificationType (juce::Justification::centred);
//    dryLabel.attachToComponent(&drySlider, false);
//    dryLabel.setColour(juce::Label::outlineColourId, otherLookandFeel.outlineColour);
    
    addAndMakeVisible (&drySlider);
    drySliderAttachment.reset (new SliderAttachment (valueTreeState, "dry", drySlider));
    drySlider.setSliderStyle (juce::Slider::Rotary);
    drySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    drySlider.setNumDecimalPlacesToDisplay(3);
    drySlider.setTextValueSuffix ("%");
    drySlider.setTooltip("This sets the level of the dry input signal passed through to the output");
    drySlider.sendLookAndFeelChange();
//    wetLabel.setText ("Wet", juce::dontSendNotification);
//    addAndMakeVisible (&wetLabel);
//    wetLabel.setJustificationType (juce::Justification::centred);
//    wetLabel.attachToComponent(&wetSlider, false);
    
    addAndMakeVisible (&wetSlider);
    wetSliderAttachment.reset (new SliderAttachment (valueTreeState, "wet", wetSlider));
    wetSlider.setSliderStyle (juce::Slider::Rotary);
    wetSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    wetSlider.setNumDecimalPlacesToDisplay(3);
    wetSlider.setTextValueSuffix ("%");
    wetSlider.setTooltip("This sets the level of the delayed signal passed through to the output");
    wetSlider.sendLookAndFeelChange();

//    delTLLabel.setText ("Time L", juce::dontSendNotification);
//    addAndMakeVisible (&delTLLabel);
//    delTLLabel.setJustificationType (juce::Justification::centred);
//    delTLLabel.attachToComponent(&delTLSlider, false);
    addAndMakeVisible (&delTLSlider);
    delTLSliderAttachment.reset (new SliderAttachment (valueTreeState, "timeLeft", delTLSlider));
    delTLSlider.setSliderStyle (juce::Slider::Rotary);
    delTLSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    delTLSlider.setNumDecimalPlacesToDisplay(3);
    delTLSlider.setTextValueSuffix ("ms");
    delTLSlider.setTooltip("This sets the delay time for the left channel in milliseconds");
    delTLSlider.sendLookAndFeelChange();
//    delTRLabel.setText ("Time R", juce::dontSendNotification);
//    addAndMakeVisible (&delTRLabel);
//    delTRLabel.setJustificationType (juce::Justification::centred);
//    delTRLabel.attachToComponent(&delTRSlider, false);
    
    addAndMakeVisible (&delTRSlider);
    delTRSliderAttachment.reset (new SliderAttachment (valueTreeState, "timeRight", delTRSlider));
    delTRSlider.setSliderStyle (juce::Slider::Rotary);
    delTRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    delTRSlider.setNumDecimalPlacesToDisplay(3);
    delTRSlider.setTextValueSuffix ("ms");
    delTRSlider.setTooltip("This sets the delay time for the right channel in milliseconds");
    delTRSlider.sendLookAndFeelChange();
//    fbLLabel.setText ("Feedback L", juce::dontSendNotification);
//    addAndMakeVisible (&fbLLabel);
//    fbLLabel.setJustificationType (juce::Justification::centred);
//    fbLLabel.attachToComponent(&fbLSlider, false);
    
    addAndMakeVisible (&fbLSlider);
    fbLSliderAttachment.reset (new SliderAttachment (valueTreeState, "fbL", fbLSlider));
    fbLSlider.setSliderStyle (juce::Slider::Rotary);
    fbLSlider.setRange(0.0f, 200.0f);
    fbLSlider.setSkewFactor(0.5);
    fbLSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    fbLSlider.setNumDecimalPlacesToDisplay(3);
    fbLSlider.setTextValueSuffix ("%");
    fbLSlider.setTooltip("This sets the feedback for the left channel");
    fbLSlider.sendLookAndFeelChange();
//    fbRLabel.setText ("Feedback R", juce::dontSendNotification);
//    addAndMakeVisible (&fbRLabel);
//    fbRLabel.setJustificationType (juce::Justification::centred);
//    fbRLabel.attachToComponent(&fbRSlider, false);
    
    addAndMakeVisible (&fbRSlider);
    fbRSliderAttachment.reset (new SliderAttachment (valueTreeState, "fbR", fbRSlider));
    fbRSlider.setSliderStyle (juce::Slider::Rotary);
    fbRSlider.setRange(0.0f, 200.0f);
    fbRSlider.setSkewFactor(0.5);
    fbRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    fbRSlider.setNumDecimalPlacesToDisplay(3);
    fbRSlider.setTextValueSuffix ("%");
    fbRSlider.setTooltip("This sets the feedback for the right channel");
    fbRSlider.sendLookAndFeelChange();
    
    addAndMakeVisible(&fbLinkButton);
    fbLinkButton.onClick = [ this ] { fbLinkChange(); };
    fbLinkButtonAttachment.reset(new ButtonAttachment(valueTreeState, "fbLink", fbLinkButton));
    fbLinkButton.setButtonText("FB Link");
    fbLinkButton.setTooltip("This links the feedback of both channels");
    
    addAndMakeVisible(&fbControlButton);
    fbControlButtonAttachment.reset(new ButtonAttachment(valueTreeState, "fbControl", fbControlButton));
    fbControlButton.setButtonText("FB Control");
    fbControlButton.setTooltip("This enables a softclipper to ensure feedback never goes above 100%");
    
    
    
//    addAndMakeVisible(&detuneLLabel);
//    detuneLLabel.attachToComponent(&detuneLSlider, false);
//    detuneLLabel.setText("Detune L", juce::dontSendNotification);
    addAndMakeVisible (&detuneLSlider);
    detuneLSliderAttachment.reset (new SliderAttachment (valueTreeState, "detuneL", detuneLSlider));
    detuneLSlider.setTextValueSuffix ("cent");
    detuneLSlider.setColour( juce::Slider::textBoxOutlineColourId, otherLookandFeel.outlineColour );
    detuneLSlider.setTooltip("This detunes the left channel");
    detuneLSlider.sendLookAndFeelChange();
//    addAndMakeVisible(&detuneRLabel);
//    detuneRLabel.attachToComponent(&detuneRSlider, false);
//    detuneRLabel.setText("Detune R", juce::dontSendNotification);
    addAndMakeVisible (&detuneRSlider);
    detuneRSliderAttachment.reset (new SliderAttachment (valueTreeState, "detuneR", detuneRSlider));
    detuneRSlider.setTextValueSuffix ("cent");
    detuneRSlider.setColour( juce::Slider::textBoxOutlineColourId, otherLookandFeel.outlineColour );
    detuneRSlider.setTooltip("This detunes the right channel");
    detuneRSlider.sendLookAndFeelChange();
    
    addAndMakeVisible(&linkButton);
    linkButton.onClick = [ this ]{ linkOrSyncChange(); };
    linkButtonAttachment.reset(new ButtonAttachment(valueTreeState, "link", linkButton));
    linkButton.setButtonText("Link");
    linkButton.setTooltip("This links the delay times of the left and right channels");
    
    addAndMakeVisible(&hostSyncButton);
    hostSyncButton.onClick = [ this ]{ linkOrSyncChange(); };
    hostSyncButtonAttachment.reset(new ButtonAttachment(valueTreeState, "sync", hostSyncButton));
    hostSyncButton.setButtonText("Sync To Host");
    hostSyncButton.setTooltip("This syncs the delay times to the tempo of the host application");
    
    
//    addAndMakeVisible(&syncLabelL);
//    syncLabelL.setText ("Division L", juce::dontSendNotification);
//    syncLabelL.setJustificationType (juce::Justification::centred);
//    syncLabelL.attachToComponent(&syncValLComboBox, false);
    
    addAndMakeVisible(&syncValLComboBox);
    syncValLComboBox.addItem("1/2", 1);
    syncValLComboBox.addItem("1/4", 2);
    syncValLComboBox.addItem("1/8", 3);
    syncValLComboBox.addItem("1/16", 4);
    syncValLComboBox.addItem("1/32", 5);
    syncValLComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValL", syncValLComboBox));
    syncValLComboBox.setTooltip("This sets the delay time of the left channel in rhythmic divisions relative to the host tempo");
    
    addAndMakeVisible(syncValLTypeComboBox);
    syncValLTypeComboBox.addItem("dotted", 1);
    syncValLTypeComboBox.addItem("normal", 2);
    syncValLTypeComboBox.addItem("triplet", 3);
    syncValLTypeComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValLType", syncValLTypeComboBox));
    syncValLTypeComboBox.setTooltip("A variety of different types of rhythmic divisions for the left channel");
//    addAndMakeVisible(&syncLabelR);
//    syncLabelR.setText ("Division R", juce::dontSendNotification);
//    syncLabelR.setJustificationType (juce::Justification::centred);
//    syncLabelR.attachToComponent(&syncValRComboBox, false);
    addAndMakeVisible(syncValRComboBox);
    syncValRComboBox.addItem("1/2", 1);
    syncValRComboBox.addItem("1/4", 2);
    syncValRComboBox.addItem("1/8", 3);
    syncValRComboBox.addItem("1/16", 4);
    syncValRComboBox.addItem("1/32", 5);
    syncValRComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValR", syncValRComboBox));
    syncValRComboBox.setTooltip("This sets the delay time of the right channel in rhythmic divisions relative to the host tempo");
    addAndMakeVisible(syncValRTypeComboBox);
    syncValRTypeComboBox.addItem("dotted", 1);
    syncValRTypeComboBox.addItem("normal", 2);
    syncValRTypeComboBox.addItem("triplet", 3);
    syncValRTypeComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValRType", syncValRTypeComboBox));
    syncValRTypeComboBox.setTooltip("A variety of different types of rhythmic divisions for the right channel");
    
    addAndMakeVisible(&syncValLOffsetNumBox);
    syncValLOffsetNumBoxAttachment.reset(new SliderAttachment(valueTreeState, "syncValLOffset", syncValLOffsetNumBox));
    syncValLOffsetNumBox.setTextValueSuffix("%");
    syncValLOffsetNumBox.setTooltip("This shifts the delaytime of the left channel forwards or backwards by some percentage");
    syncValLOffsetNumBox.sendLookAndFeelChange();
    
    addAndMakeVisible(&syncValROffsetNumBox);
    syncValROffsetNumBoxAttachment.reset(new SliderAttachment(valueTreeState, "syncValROffset", syncValROffsetNumBox));
    syncValROffsetNumBox.setTextValueSuffix("%");
    syncValROffsetNumBox.setTooltip("This shifts the delaytime of the right channel forwards or backwards by some percentage");
    syncValROffsetNumBox.sendLookAndFeelChange();
    
    addAndMakeVisible(&clearDelayLineButton);
    clearDelayLineButton.setButtonText("Clear DelayLine");
    clearDelayLineButton.onClick = [this]{ audioProcessor.clearDelayBuffer() ; };
    clearDelayLineButton.setTooltip("This will completely clear all information stored in the delaylines in case of feedback explosions");
//    clearDelayLineButton.setColour(juce::TextButton::buttonColourId, clearBufferBackgroundColour);
    
    addAndMakeVisible(&lpCutOffSlider);
    lpCutOffSliderAttachment.reset(new SliderAttachment(valueTreeState, "lpCutOff", lpCutOffSlider));
    lpCutOffSlider.setTextValueSuffix("Hz");
    lpCutOffSlider.setSliderStyle (juce::Slider::Rotary);
    lpCutOffSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    lpCutOffSlider.setNumDecimalPlacesToDisplay(3);
    lpCutOffSlider.setRange(100, 20000);
    lpCutOffSlider.setSkewFactor(0.3);
    lpCutOffSlider.setTooltip("This sets the cutoff of the lowpass filter");
    lpCutOffSlider.sendLookAndFeelChange();
//    lpCutOffLabel.attachToComponent(&lpCutOffSlider, false);
//    lpCutOffLabel.setText("LPF", juce::dontSendNotification);
//    lpCutOffLabel.setJustificationType (juce::Justification::centred);
    
    addAndMakeVisible(&hpCutOffSlider);
    hpCutOffSliderAttachment.reset(new SliderAttachment(valueTreeState, "hpCutOff", hpCutOffSlider));
    hpCutOffSlider.setTextValueSuffix("Hz");
    hpCutOffSlider.setSliderStyle (juce::Slider::Rotary);
    hpCutOffSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    hpCutOffSlider.setNumDecimalPlacesToDisplay(3);
    hpCutOffSlider.setRange(20.0f, 10000.0f);
    hpCutOffSlider.setSkewFactor(0.3);
    hpCutOffSlider.setTooltip("This sets the cutoff of the highpass filter");
    hpCutOffSlider.sendLookAndFeelChange();
//    hpCutOffLabel.attachToComponent(&hpCutOffSlider, false);
//    hpCutOffLabel.setText("HPF", juce::dontSendNotification);
//    hpCutOffLabel.setJustificationType (juce::Justification::centred);
    
    addAndMakeVisible(&overdriveOnButton);
    overdriveOnButton.setButtonText("Drive");
    overdriveOnButtonAttachment.reset( new ButtonAttachment(valueTreeState, "overdriveOn", overdriveOnButton));
    overdriveOnButton.setTooltip("This turns on the softclip overdrive waveshaper");
    
    addAndMakeVisible(&overdriveGainSlider);
    overdriveGainSliderAttachment.reset( new SliderAttachment(valueTreeState, "overdriveGain", overdriveGainSlider));
    overdriveGainSlider.setSliderStyle (juce::Slider::Rotary);
    overdriveGainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    overdriveGainSlider.setNumDecimalPlacesToDisplay(3);
    overdriveGainSlider.setTooltip("This sets the gain of the signal passed into the overdrive");
    overdriveGainSlider.sendLookAndFeelChange();
    
    addAndMakeVisible(&overdrivePlacementComboBox);
    overdrivePlacementComboBox.addItem("Input", 1);
    overdrivePlacementComboBox.addItem("Pre FB", 2);
    overdrivePlacementComboBox.addItem("Output", 3);
    overdrivePlacementComboBoxAttachment.reset( new ComboBoxAttachment(valueTreeState, "overdrivePlacement", overdrivePlacementComboBox) );
    overdrivePlacementComboBox.setTooltip("This changes the position of the overdrive");
    
    addAndMakeVisible(&lfoRSlider);
    lfoRSliderAttachment.reset( new SliderAttachment(valueTreeState, "lfoRate", lfoRSlider));
    lfoRSlider.setSliderStyle (juce::Slider::Rotary);
    lfoRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    lfoRSlider.setNumDecimalPlacesToDisplay(3);
    lfoRSlider.setRange(0.0001, 20);
    lfoRSlider.setSkewFactor(0.3);
    lfoRSlider.setTextValueSuffix("Hz");
    lfoRSlider.setTooltip("This sets the rate at which the delay time is modulated");
    lfoRSlider.sendLookAndFeelChange();
//    lfoRSlider
//    lfoRLabel.attachToComponent(&lfoRSlider, false);
//    lfoRLabel.setText("LFO Rate", juce::dontSendNotification);
//    lfoRLabel.setJustificationType (juce::Justification::centred);

    
    addAndMakeVisible(&lfoDSlider);
    lfoDSliderAttachment.reset( new SliderAttachment(valueTreeState, "lfoDepth", lfoDSlider));
    lfoDSlider.setSliderStyle (juce::Slider::Rotary);
    lfoDSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    lfoDSlider.setTextValueSuffix("%");
    lfoDSlider.setTooltip("This sets the depth of the modulation applied to the delay time");
    lfoDSlider.sendLookAndFeelChange();
//    lfoDLabel.attachToComponent(&lfoDSlider, false);
//    lfoDLabel.setText("LFO Depth", juce::dontSendNotification);
//    lfoDLabel.setJustificationType (juce::Justification::centred);


    addAndMakeVisible( &interpolationTypeBox );
    interpolationTypeBox.addItem( "linear", 1 );
    interpolationTypeBox.addItem( "cubic", 2 );
    interpolationTypeBox.addItem( "PD", 3 );
    interpolationTypeBox.addItem( "4th Order", 4 );
    interpolationTypeBox.addItem( "Godot", 5 );
    interpolationTypeBox.addItem( "Hermite", 6 );
    interpolationTypeBoxAttachment.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "interpolationType", interpolationTypeBox));
    interpolationTypeBox.setTooltip("This changes between different interpolation types... linear is the least cpu intensive, cubic sounds a bit naff to me, the others are all higher quality interpolation which come at various prices with regards to cpu usage");
    
    addAndMakeVisible( &tooltipsToggle );
    tooltipsToggle.setButtonText( "Hints" );
    tooltipsToggle.onClick = [ this ]
    {
      if ( tooltipsToggle.getToggleState() )
          setSize ( WIDTH, HEIGHT + tooltipLabel.getHeight() );
      else
          setSize (WIDTH, HEIGHT);
    };
    tooltipsToggle.setTooltip( MAIN_TOOLTIP );
    
    addAndMakeVisible( &tooltipLabel );
    tooltipLabel.setColour( juce::Label::backgroundColourId, otherLookandFeel.backGroundColour.withAlpha( 0.85f ) );
    tooltipLabel.setTooltip( MAIN_TOOLTIP );
    
    startTimer(100);
    
    setSize (WIDTH, HEIGHT);
    linkOrSyncChange();
    fbLinkChange();
}

SjfWrecklessDelayAudioProcessorEditor::~SjfWrecklessDelayAudioProcessorEditor()
{
    DBG("DELETING EDITOR");
    stopTimer();
    setLookAndFeel (nullptr);
    DBG("DELETED EDITOR");
}

//==============================================================================

void SjfWrecklessDelayAudioProcessorEditor::paint (juce::Graphics& g)
{

#ifdef JUCE_DEBUG
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
#else
    juce::Rectangle<int> r = { (int)( WIDTH ), (int)(HEIGHT + tooltipLabel.getHeight()) };
    sjf_makeBackground< 40 >( g, r );
#endif
    

    auto panelColour = otherLookandFeel.panelColour.withAlpha(0.3f);
//    g.setColour (delayTimeBackgroundColour);
    g.setColour( panelColour );
    g.fillRect(delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize*2, potSize+textHeight*3);

//    g.setColour(detuneBackgroundColour);
    g.fillRect(detuneLSlider.getX(), detuneLSlider.getY() - textHeight, potSize*2, textHeight*2);
    
//    g.setColour(feedbackBackgroundColour);
    g.fillRect(fbLSlider.getX(), fbLSlider.getY() - textHeight, potSize*2, potSize+textHeight*2);

//    g.setColour(filterBackgroundColour);
    g.fillRect(lpCutOffSlider.getX(), lpCutOffSlider.getY() - textHeight, potSize*2 , potSize + textHeight);
    
//    g.setColour(overdriveBackgroundColour);
    g.fillRect(overdriveOnButton.getX(), overdriveOnButton.getY(), potSize*2 , potSize+textHeight);

//    g.setColour(lfoBackgroundColour);
    g.fillRect(lfoDSlider.getX(), lfoDSlider.getY() - textHeight, potSize*2 , potSize+textHeight);
    
//    g.setColour(wetBackgroundColour);
    g.fillRect(wetSlider.getX(), wetSlider.getY()-textHeight, potSize , potSize*1.25 + textHeight);
    
//    g.setColour(dryBackgroundColour);
    g.fillRect(drySlider.getX(), drySlider.getY()-textHeight, potSize, potSize*1.25 + textHeight);
    
    g.setColour ( juce::Colours::white );
    g.setFont ( 15.0f );
    g.drawFittedText("sjf_wrecklessDelay", 0, 0, getWidth(), textHeight, juce::Justification::centred, 1);
    
    g.drawFittedText("Wet", wetSlider.getX(), wetSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("Dry", drySlider.getX(), drySlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("LPF", lpCutOffSlider.getX(), lpCutOffSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("HPF", hpCutOffSlider.getX(), hpCutOffSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("LFO R", lfoRSlider.getX(), lfoRSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("LFO D", lfoDSlider.getX(), lfoDSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("Detune L", detuneLSlider.getX(), detuneLSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    g.drawFittedText("Detune R", detuneRSlider.getX(), detuneRSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
    
    g.setColour ( juce::Colours::white );
    if ( hostSyncButton.getToggleState() )
    {
        if ( linkButton.getToggleState() )
        {
            g.drawFittedText("Division", delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize*2, textHeight, juce::Justification::centred, 1);
            g.setColour ( otherLookandFeel.outlineColour );
            g.drawRect( delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize*2, textHeight );
        }
        else
        {
            g.drawFittedText("Division L", delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
            g.drawFittedText("Division R", delTRSlider.getX(), delTRSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
            g.setColour ( otherLookandFeel.outlineColour );
            g.drawRect( delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize, textHeight );
            g.drawRect( delTRSlider.getX(), delTRSlider.getY() - textHeight, potSize, textHeight );
        }
    }
    else
    {
        if ( linkButton.getToggleState() )
        {
            g.drawFittedText("Time", delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize*2, textHeight, juce::Justification::centred, 1);
            g.setColour ( otherLookandFeel.outlineColour );
            g.drawRect( delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize*2, textHeight );
        }
        else
        {
            g.drawFittedText("Time L", delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
            g.drawFittedText("Time R", delTRSlider.getX(), delTRSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
            g.setColour ( otherLookandFeel.outlineColour );
            g.drawRect( delTLSlider.getX(), delTLSlider.getY() - textHeight, potSize, textHeight );
            g.drawRect( delTRSlider.getX(), delTRSlider.getY() - textHeight, potSize, textHeight );
        }
    }
    
    g.setColour ( juce::Colours::white );
    if ( fbLinkButton.getToggleState() )
    {
        g.drawFittedText("Feedback", fbLSlider.getX(), fbLSlider.getY() - textHeight, potSize*2, textHeight, juce::Justification::centred, 1);
        g.setColour ( otherLookandFeel.outlineColour );
        g.drawRect( fbLSlider.getX(), fbLSlider.getY() - textHeight, potSize*2, textHeight );
    }
    else
    {
        g.drawFittedText("Feedback L", fbLSlider.getX(), fbLSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
        g.drawFittedText("Feedback R", fbRSlider.getX(), fbRSlider.getY() - textHeight, potSize, textHeight, juce::Justification::centred, 1);
        g.setColour ( otherLookandFeel.outlineColour );
        g.drawRect( fbLSlider.getX(), fbLSlider.getY() - textHeight, potSize, textHeight );
        g.drawRect( fbRSlider.getX(), fbRSlider.getY() - textHeight, potSize, textHeight );
    }
    
    g.setColour ( otherLookandFeel.outlineColour );
    g.drawRect( wetSlider.getX(), wetSlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( drySlider.getX(), drySlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( lpCutOffSlider.getX(), lpCutOffSlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( hpCutOffSlider.getX(), hpCutOffSlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( lfoRSlider.getX(), lfoRSlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( lfoDSlider.getX(), lfoDSlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( detuneLSlider.getX(), detuneLSlider.getY() - textHeight, potSize, textHeight );
    g.drawRect( detuneRSlider.getX(), detuneRSlider.getY() - textHeight, potSize, textHeight );

}

void SjfWrecklessDelayAudioProcessorEditor::resized()
{
    delTLSlider.setBounds(indent, textHeight*2, potSize, potSize);
    delTRSlider.setBounds(delTLSlider.getX()+potSize, delTLSlider.getY(), potSize, potSize);
    
    
    syncValLComboBox.setBounds(delTLSlider.getX(), delTLSlider.getY(), potSize, textHeight*2);
    syncValLTypeComboBox.setBounds(syncValLComboBox.getX(), syncValLComboBox.getY()+syncValLComboBox.getHeight(), potSize, textHeight);
    syncValLOffsetNumBox.setBounds(syncValLTypeComboBox.getX(), syncValLTypeComboBox.getY()+syncValLTypeComboBox.getHeight(), potSize, textHeight);
    syncValRComboBox.setBounds(syncValLComboBox.getX()+syncValLComboBox.getWidth(), syncValLComboBox.getY(), potSize, textHeight*2);
    syncValRTypeComboBox.setBounds(syncValRComboBox.getX(), syncValRComboBox.getY()+syncValRComboBox.getHeight(), potSize, textHeight);
    syncValROffsetNumBox.setBounds(syncValRTypeComboBox.getX(), syncValRTypeComboBox.getY()+syncValRTypeComboBox.getHeight(), potSize, textHeight);
    
    linkButton.setBounds(delTLSlider.getX(), delTLSlider.getY()+delTLSlider.getHeight(), potSize*2, textHeight);
    hostSyncButton.setBounds(linkButton.getX(), linkButton.getY()+linkButton.getHeight(), potSize*2, textHeight);
    
    
    detuneLSlider.setBounds(delTLSlider.getX(), indent + hostSyncButton.getY()+hostSyncButton.getHeight()+textHeight, potSize, textHeight);
    detuneRSlider.setBounds(detuneLSlider.getX()+detuneLSlider.getWidth(), detuneLSlider.getY(), detuneLSlider.getWidth(), detuneLSlider.getHeight());
    
    fbLSlider.setBounds(delTLSlider.getX(), indent +detuneLSlider.getY()+detuneLSlider.getHeight()+textHeight, potSize, potSize);
    fbRSlider.setBounds(fbLSlider.getX()+fbLSlider.getWidth(), fbLSlider.getY(), potSize, potSize);
    fbLinkButton.setBounds(fbLSlider.getX(), fbLSlider.getY()+fbLSlider.getHeight(), potSize, textHeight);
    fbControlButton.setBounds(fbLinkButton.getX()+fbLinkButton.getWidth(), fbLinkButton.getY(), potSize, textHeight);
    
    //==============================================================================
    
    lpCutOffSlider.setBounds(delTRSlider.getRight() + indent, delTRSlider.getY(), potSize, potSize);
    hpCutOffSlider.setBounds(lpCutOffSlider.getX()+lpCutOffSlider.getWidth(), lpCutOffSlider.getY(), potSize, potSize);
    
    overdriveOnButton.setBounds(lpCutOffSlider.getX(), indent + lpCutOffSlider.getY()+ lpCutOffSlider.getHeight(), potSize, textHeight);
    overdrivePlacementComboBox.setBounds(overdriveOnButton.getX() + overdriveOnButton.getWidth(), overdriveOnButton.getY(), potSize, textHeight);
    overdriveGainSlider.setBounds(overdriveOnButton.getX(), overdriveOnButton.getBottom(), potSize*2, potSize);
//    overdriveOutSlider.setBounds(overdriveGainSlider.getX()  + overdriveGainSlider.getWidth(), overdriveGainSlider.getY(), potSize, potSize);
    
    lfoDSlider.setBounds(overdriveGainSlider.getX(), indent*3 + overdriveGainSlider.getBottom(), potSize, potSize);
    lfoRSlider.setBounds(lfoDSlider.getX()+lfoDSlider.getWidth(), lfoDSlider.getY(), potSize, potSize);
    
    //==============================================================================
    
    drySlider.setBounds(hpCutOffSlider.getRight() + indent, textHeight*2, potSize, potSize*1.25);
    wetSlider.setBounds(drySlider.getX(), drySlider.getBottom()+textHeight + indent/2, potSize, drySlider.getHeight());
    interpolationTypeBox.setBounds(drySlider.getX(), wetSlider.getBottom()+indent/2, potSize, textHeight);
    clearDelayLineButton.setBounds(interpolationTypeBox.getX(), interpolationTypeBox.getBottom()+indent/2, potSize, textHeight);
    tooltipsToggle.setBounds( clearDelayLineButton.getX(), clearDelayLineButton.getBottom()+indent/2, potSize, textHeight );
    
    tooltipLabel.setBounds( 0, HEIGHT, WIDTH, textHeight*3 );
    fbLinkChange();
    linkOrSyncChange();
    
}

void SjfWrecklessDelayAudioProcessorEditor::fbLinkChange()
{
    //    if (audioProcessor.getFbLinkState())
    if ( fbLinkButton.getToggleState() )
    {
        //        DBG("FB LINKED");
        fbRSlider.setVisible(false);
        fbLSlider.setBounds(fbLSlider.getBounds().getX(), fbLSlider.getBounds().getY(), potSize*2, fbLSlider.getBounds().getHeight());
        fbLSlider.setTooltip("This sets the feedback for the both channels");
        fbLSlider.sendLookAndFeelChange();
//        fbLLabel.setText ("Feedback", juce::dontSendNotification);
    }
    else
    {
        //        DBG("FB NOT LINKED");
        fbRSlider.setVisible(true);
        fbLSlider.setBounds(fbLSlider.getBounds().getX(), fbLSlider.getBounds().getY(), potSize, fbLSlider.getBounds().getHeight());
        fbLSlider.setTooltip("This sets the feedback for the left channel");
        fbLSlider.sendLookAndFeelChange();
//        fbLLabel.setText ("Feedback L", juce::dontSendNotification);
    }
    repaint();
}

void SjfWrecklessDelayAudioProcessorEditor::linkOrSyncChange()
{
    //    if( audioProcessor.getSyncState() )
    if ( hostSyncButton.getToggleState() )
    {
        //        DBG("SYNCED");
        delTLSlider.setVisible(false);
        delTRSlider.setVisible(false);
        syncValLComboBox.setVisible(true);
        syncValLTypeComboBox.setVisible(true);
        syncValLOffsetNumBox.setVisible(true);
        //        if(!audioProcessor.getLinkState())
        if ( !linkButton.getToggleState() )
        {
            //            DBG("NOT LINKED");
            syncValRComboBox.setVisible(true);
            syncValRTypeComboBox.setVisible(true);
//            syncLabelL.setText ("Division L", juce::dontSendNotification);
            syncValLComboBox.setBounds(syncValLComboBox.getBounds().getX(), syncValLComboBox.getBounds().getY(), potSize, textHeight*2);
            syncValLComboBox.setTooltip("This sets the delay time of the left channel in rhythmic divisions relative to the host tempo");
            syncValLTypeComboBox.setBounds(syncValLTypeComboBox.getBounds().getX(), syncValLTypeComboBox.getBounds().getY(), potSize, textHeight);
            syncValLTypeComboBox.setTooltip("A variety of different types of rhythmic divisions for the left channel");
        }
        else
        {
            //            DBG("LINKED");
            syncValRComboBox.setVisible(false);
            syncValRTypeComboBox.setVisible(false);
//            syncLabelL.setText ("Division", juce::dontSendNotification);
            syncValLComboBox.setBounds(syncValLComboBox.getBounds().getX(), syncValLComboBox.getBounds().getY(), potSize*2, textHeight*2);
            syncValLComboBox.setTooltip("This sets the delay time of both channels in rhythmic divisions relative to the host tempo");
            syncValLTypeComboBox.setBounds(syncValLTypeComboBox.getBounds().getX(), syncValLTypeComboBox.getBounds().getY(), potSize*2, textHeight);
            syncValLTypeComboBox.setTooltip("A variety of different types of rhythmic divisions for both channels");
        }
        syncValROffsetNumBox.setVisible(true);
        
    }
    else
    {
        //        DBG("NOT SYNCED");
        delTLSlider.setVisible(true);
        //        if (!audioProcessor.getLinkState())
        if( !linkButton.getToggleState() )
        {
            //            DBG("NOT LINKED");
            delTRSlider.setVisible(true);
            delTLSlider.setBounds(delTLSlider.getBounds().getX(), delTLSlider.getBounds().getY(), potSize, delTLSlider.getBounds().getHeight());
            delTLSlider.setTooltip("This sets the delay time for the left channel in milliseconds");
            delTLSlider.sendLookAndFeelChange();
//            delTLLabel.setText ("Time L", juce::dontSendNotification);
        }
        else
        {
            //            DBG("LINKED");
            delTRSlider.setVisible(false);
            delTLSlider.setBounds(delTLSlider.getBounds().getX(), delTLSlider.getBounds().getY(), potSize*2, delTLSlider.getBounds().getHeight());
            delTLSlider.setTooltip("This sets the delay time for both channels in milliseconds");
            delTLSlider.sendLookAndFeelChange();
//            delTLLabel.setText ("Time", juce::dontSendNotification);
        }
        syncValLComboBox.setVisible(false);
        syncValLTypeComboBox.setVisible(false);
        syncValLOffsetNumBox.setVisible(false);
        syncValRComboBox.setVisible(false);
        syncValRTypeComboBox.setVisible(false);
        syncValROffsetNumBox.setVisible(false);
    }
    repaint();
}

void SjfWrecklessDelayAudioProcessorEditor::timerCallback()
{
    sjf_setTooltipLabel( this, MAIN_TOOLTIP, tooltipLabel );
}
