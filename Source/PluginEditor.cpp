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
    setLookAndFeel (&otherLookandFeel);
    
    dryLabel.setText ("Dry", juce::dontSendNotification);
    addAndMakeVisible (&dryLabel);
    dryLabel.setJustificationType (juce::Justification::centred);
    dryLabel.attachToComponent(&drySlider, false);
//    dryLabel.setColour(juce::Label::outlineColourId, otherLookandFeel.outlineColour);
    
    addAndMakeVisible (&drySlider);
    drySliderAttachment.reset (new SliderAttachment (valueTreeState, "dry", drySlider));
    drySlider.setSliderStyle (juce::Slider::Rotary);
    drySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    drySlider.setNumDecimalPlacesToDisplay(3);
    drySlider.setTextValueSuffix ("%");
    
    wetLabel.setText ("Wet", juce::dontSendNotification);
    addAndMakeVisible (&wetLabel);
    wetLabel.setJustificationType (juce::Justification::centred);
    wetLabel.attachToComponent(&wetSlider, false);
    
    addAndMakeVisible (&wetSlider);
    wetSliderAttachment.reset (new SliderAttachment (valueTreeState, "wet", wetSlider));
    wetSlider.setSliderStyle (juce::Slider::Rotary);
    wetSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    wetSlider.setNumDecimalPlacesToDisplay(3);
    wetSlider.setTextValueSuffix ("%");
    

    delTLLabel.setText ("Time L", juce::dontSendNotification);
    addAndMakeVisible (&delTLLabel);
    delTLLabel.setJustificationType (juce::Justification::centred);
    delTLLabel.attachToComponent(&delTLSlider, false);
    addAndMakeVisible (&delTLSlider);
    delTLSliderAttachment.reset (new SliderAttachment (valueTreeState, "timeLeft", delTLSlider));
    delTLSlider.setSliderStyle (juce::Slider::Rotary);
    delTLSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    delTLSlider.setNumDecimalPlacesToDisplay(3);
    delTLSlider.setTextValueSuffix ("ms");
    
    delTRLabel.setText ("Time R", juce::dontSendNotification);
    addAndMakeVisible (&delTRLabel);
    delTRLabel.setJustificationType (juce::Justification::centred);
    delTRLabel.attachToComponent(&delTRSlider, false);
    
    addAndMakeVisible (&delTRSlider);
    delTRSliderAttachment.reset (new SliderAttachment (valueTreeState, "timeRight", delTRSlider));
    delTRSlider.setSliderStyle (juce::Slider::Rotary);
    delTRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    delTRSlider.setNumDecimalPlacesToDisplay(3);
    delTRSlider.setTextValueSuffix ("ms");
    
    fbLLabel.setText ("Feedback L", juce::dontSendNotification);
    addAndMakeVisible (&fbLLabel);
    fbLLabel.setJustificationType (juce::Justification::centred);
    fbLLabel.attachToComponent(&fbLSlider, false);
    
    addAndMakeVisible (&fbLSlider);
    fbLSliderAttachment.reset (new SliderAttachment (valueTreeState, "fbL", fbLSlider));
    fbLSlider.setSliderStyle (juce::Slider::Rotary);
    fbLSlider.setRange(0.0f, 200.0f);
    fbLSlider.setSkewFactor(0.5);
    fbLSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    fbLSlider.setNumDecimalPlacesToDisplay(3);
    fbLSlider.setTextValueSuffix ("%");
    
    fbRLabel.setText ("Feedback R", juce::dontSendNotification);
    addAndMakeVisible (&fbRLabel);
    fbRLabel.setJustificationType (juce::Justification::centred);
    fbRLabel.attachToComponent(&fbRSlider, false);
    
    addAndMakeVisible (&fbRSlider);
    fbRSliderAttachment.reset (new SliderAttachment (valueTreeState, "fbR", fbRSlider));
    fbRSlider.setSliderStyle (juce::Slider::Rotary);
    fbRSlider.setRange(0.0f, 200.0f);
    fbRSlider.setSkewFactor(0.5);
    fbRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    fbRSlider.setNumDecimalPlacesToDisplay(3);
    fbRSlider.setTextValueSuffix ("%");
    
    addAndMakeVisible(&fbLinkButton);
    fbLinkButtonAttachment.reset(new ButtonAttachment(valueTreeState, "fbLink", fbLinkButton));
    fbLinkButton.setButtonText("FB Link");
    addAndMakeVisible(&fbControlButton);
    fbControlButtonAttachment.reset(new ButtonAttachment(valueTreeState, "fbControl", fbControlButton));
    fbControlButton.setButtonText("FB Control");
    
    addAndMakeVisible(&detuneLLabel);
    detuneLLabel.attachToComponent(&detuneLSlider, false);
    detuneLLabel.setText("Detune L", juce::dontSendNotification);
    addAndMakeVisible (&detuneLSlider);
    detuneLSliderAttachment.reset (new SliderAttachment (valueTreeState, "detuneL", detuneLSlider));
    detuneLSlider.setTextValueSuffix ("cent");

    addAndMakeVisible(&detuneRLabel);
    detuneRLabel.attachToComponent(&detuneRSlider, false);
    detuneRLabel.setText("Detune R", juce::dontSendNotification);
    addAndMakeVisible (&detuneRSlider);
    detuneRSliderAttachment.reset (new SliderAttachment (valueTreeState, "detuneR", detuneRSlider));
    detuneRSlider.setTextValueSuffix ("cent");
    
    addAndMakeVisible(&linkButton);
    linkButtonAttachment.reset(new ButtonAttachment(valueTreeState, "link", linkButton));
    linkButton.setButtonText("Link");
    
    
    addAndMakeVisible(&hostSyncButton);
    hostSyncButtonAttachment.reset(new ButtonAttachment(valueTreeState, "sync", hostSyncButton));
    hostSyncButton.setButtonText("Sync To Host");
    
    addAndMakeVisible(&syncLabelL);
    syncLabelL.setText ("Division L", juce::dontSendNotification);
    syncLabelL.setJustificationType (juce::Justification::centred);
    syncLabelL.attachToComponent(&syncValLComboBox, false);
    
    addAndMakeVisible(&syncValLComboBox);
    syncValLComboBox.addItem("1/2", 1);
    syncValLComboBox.addItem("1/4", 2);
    syncValLComboBox.addItem("1/8", 3);
    syncValLComboBox.addItem("1/16", 4);
    syncValLComboBox.addItem("1/32", 5);
    syncValLComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValL", syncValLComboBox));
    addAndMakeVisible(syncValLTypeComboBox);
    syncValLTypeComboBox.addItem("dotted", 1);
    syncValLTypeComboBox.addItem("normal", 2);
    syncValLTypeComboBox.addItem("triplet", 3);
    syncValLTypeComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValLType", syncValLTypeComboBox));
    
    addAndMakeVisible(&syncLabelR);
    syncLabelR.setText ("Division R", juce::dontSendNotification);
    syncLabelR.setJustificationType (juce::Justification::centred);
    syncLabelR.attachToComponent(&syncValRComboBox, false);
    addAndMakeVisible(syncValRComboBox);
    syncValRComboBox.addItem("1/2", 1);
    syncValRComboBox.addItem("1/4", 2);
    syncValRComboBox.addItem("1/8", 3);
    syncValRComboBox.addItem("1/16", 4);
    syncValRComboBox.addItem("1/32", 5);
    syncValRComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValR", syncValRComboBox));
    addAndMakeVisible(syncValRTypeComboBox);
    syncValRTypeComboBox.addItem("dotted", 1);
    syncValRTypeComboBox.addItem("normal", 2);
    syncValRTypeComboBox.addItem("triplet", 3);
    syncValRTypeComboBoxAttachment.reset(new ComboBoxAttachment(valueTreeState, "syncValRType", syncValRTypeComboBox));
    
    addAndMakeVisible(&syncValLOffsetNumBox);
    syncValLOffsetNumBoxAttachment.reset(new SliderAttachment(valueTreeState, "syncValLOffset", syncValLOffsetNumBox));
    syncValLOffsetNumBox.setTextValueSuffix("%");
    
    addAndMakeVisible(&syncValROffsetNumBox);
    syncValROffsetNumBoxAttachment.reset(new SliderAttachment(valueTreeState, "syncValROffset", syncValROffsetNumBox));
    syncValROffsetNumBox.setTextValueSuffix("%");
    
    addAndMakeVisible(&clearDelayLineButton);
    clearDelayLineButton.setButtonText("Clear DelayLine");
    clearDelayLineButton.onClick = [this]{ audioProcessor.clearDelayBuffer() ; };
    clearDelayLineButton.setColour(juce::TextButton::buttonColourId, clearBufferBackgroundColour);
    
    addAndMakeVisible(&lpCutOffSlider);
    lpCutOffSliderAttachment.reset(new SliderAttachment(valueTreeState, "lpCutOff", lpCutOffSlider));
    lpCutOffSlider.setTextValueSuffix("Hz");
    lpCutOffSlider.setSliderStyle (juce::Slider::Rotary);
    lpCutOffSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    lpCutOffSlider.setNumDecimalPlacesToDisplay(3);
    lpCutOffSlider.setRange(100, 20000);
    lpCutOffSlider.setSkewFactor(0.3);
    lpCutOffLabel.attachToComponent(&lpCutOffSlider, false);
    lpCutOffLabel.setText("LPF", juce::dontSendNotification);
    lpCutOffLabel.setJustificationType (juce::Justification::centred);
    
    addAndMakeVisible(&hpCutOffSlider);
    hpCutOffSliderAttachment.reset(new SliderAttachment(valueTreeState, "hpCutOff", hpCutOffSlider));
    hpCutOffSlider.setTextValueSuffix("Hz");
    hpCutOffSlider.setSliderStyle (juce::Slider::Rotary);
    hpCutOffSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    hpCutOffSlider.setNumDecimalPlacesToDisplay(3);
    hpCutOffSlider.setRange(20.0f, 10000.0f);
    hpCutOffSlider.setSkewFactor(0.3);
    hpCutOffLabel.attachToComponent(&hpCutOffSlider, false);
    hpCutOffLabel.setText("HPF", juce::dontSendNotification);
    hpCutOffLabel.setJustificationType (juce::Justification::centred);
    
    addAndMakeVisible(&overdriveOnButton);
    overdriveOnButton.setButtonText("overdrive");
    overdriveOnButtonAttachment.reset( new ButtonAttachment(valueTreeState, "overdriveOn", overdriveOnButton));
    
    addAndMakeVisible(&overdriveGainSlider);
    overdriveGainSliderAttachment.reset( new SliderAttachment(valueTreeState, "overdriveGain", overdriveGainSlider));
    overdriveGainSlider.setSliderStyle (juce::Slider::Rotary);
    overdriveGainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    overdriveGainSlider.setNumDecimalPlacesToDisplay(3);
//    overdriveGainLabel.attachToComponent(&overdriveGainSlider, false);
//    overdriveGainLabel.setText("Gain", juce::dontSendNotification);
//    overdriveGainLabel.setJustificationType (juce::Justification::centred);
//
//    addAndMakeVisible(&overdriveOutSlider);
//    overdriveOutSliderAttachment.reset( new SliderAttachment(valueTreeState, "overdriveOut", overdriveOutSlider));
//    overdriveOutSlider.setSliderStyle (juce::Slider::Rotary);
//    overdriveOutSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
//    overdriveOutSlider.setNumDecimalPlacesToDisplay(3);
//    overdriveOutLabel.attachToComponent(&overdriveOutSlider, false);
//    overdriveOutLabel.setText("Out Level", juce::dontSendNotification);
//    overdriveOutLabel.setJustificationType (juce::Justification::centred);
    
    addAndMakeVisible(&overdrivePlacementComboBox);
    overdrivePlacementComboBox.addItem("Input", 1);
    overdrivePlacementComboBox.addItem("Pre FB", 2);
    overdrivePlacementComboBox.addItem("Output", 3);
    overdrivePlacementComboBoxAttachment.reset( new ComboBoxAttachment(valueTreeState, "overdrivePlacement", overdrivePlacementComboBox) );
    
    
    addAndMakeVisible(&lfoRSlider);
    lfoRSliderAttachment.reset( new SliderAttachment(valueTreeState, "lfoRate", lfoRSlider));
    lfoRSlider.setSliderStyle (juce::Slider::Rotary);
    lfoRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    lfoRSlider.setNumDecimalPlacesToDisplay(3);
    lfoRSlider.setRange(0.0001, 20);
    lfoRSlider.setSkewFactor(0.3);
    lfoRSlider.setTextValueSuffix("Hz");
    modulationLabel1.attachToComponent(&lfoRSlider, false);
    modulationLabel1.setText("LFO Rate", juce::dontSendNotification);
    modulationLabel1.setJustificationType (juce::Justification::centred);

    
    addAndMakeVisible(&lfoDSlider);
    lfoDSliderAttachment.reset( new SliderAttachment(valueTreeState, "lfoDepth", lfoDSlider));
    lfoDSlider.setSliderStyle (juce::Slider::Rotary);
    lfoDSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, potSize, textHeight);
    lfoDSlider.setTextValueSuffix("%");
    modulationLabel2.attachToComponent(&lfoDSlider, false);
    modulationLabel2.setText("LFO Depth", juce::dontSendNotification);
    modulationLabel2.setJustificationType (juce::Justification::centred);


    addAndMakeVisible( &interpolationTypeBox );
    interpolationTypeBox.addItem( "linear", 1 );
    interpolationTypeBox.addItem( "cubic", 2 );
    interpolationTypeBox.addItem( "PD", 3 );
    interpolationTypeBox.addItem( "4th Order", 4 );
    interpolationTypeBox.addItem( "Godot", 5 );
    interpolationTypeBox.addItem( "Hermite", 6 );
    interpolationTypeBoxAttachment.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "interpolationType", interpolationTypeBox));
    
    startTimer(100);
    
    setSize (potSize*5 + indent*4, potSize*3 + textHeight*5 + indent);
}

SjfWrecklessDelayAudioProcessorEditor::~SjfWrecklessDelayAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================

void SjfWrecklessDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
//    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    auto backColour = juce::Colours::darkgrey;
//    g.fillAll ( backColour );
//
//    static constexpr int NUM_RAND_BOXES = 70;
//    static constexpr auto randomColours = sjf_matrixOfRandomFloats< float, NUM_RAND_BOXES, 4 >();
//    static constexpr auto randomCoordinates = sjf_matrixOfRandomFloats< float, NUM_RAND_BOXES, 5 >();
//    static constexpr auto CORNER_SIZE = 5;
//
//    for ( int i = 0; i < NUM_RAND_BOXES; i++)
//    {
//        int red = randomColours.getValue(i, 0) * 255;
//        int green = randomColours.getValue(i, 1)  * 255;
//        int blue = randomColours.getValue(i, 2) * 255;
//        float alpha = randomColours.getValue(i, 3) * 0.3;
//        auto randColour = juce::Colour( red, green, blue );
//        randColour = randColour.withAlpha(alpha);
//        g.setColour( randColour );
//
//        auto x = randomCoordinates.getValue( i, 0 ) * getWidth()*1.5;// - getWidth();
//        auto y = randomCoordinates.getValue( i, 1 ) * getHeight()*1.5;// - getHeight();
//        auto w = randomCoordinates.getValue( i, 2 ) * getWidth();
//        auto h = randomCoordinates.getValue( i, 3 ) * getHeight();
//        auto rectRand = juce::Rectangle<float>( x, y, w, h );
//        auto rotate = randomCoordinates.getValue( i, 4 ) * M_PI*2;
//        auto shearX = randomCoordinates.getValue( i, 5 );
//        auto shearY = randomCoordinates.getValue( i, 6 );
//        DBG(" rotate " << rotate << " " << shearX << " " << shearY );
//        auto rotation = juce::AffineTransform::rotation( rotate, rectRand.getX(), rectRand.getY() );
//        auto shear = juce::AffineTransform::shear(shearX, shearY);
//        juce::Path path;
//        path.addRoundedRectangle( rectRand, CORNER_SIZE );
//        path.applyTransform( shear );
//        g.fillPath(path, rotation );
//    }

    juce::Rectangle<int> r = getLocalBounds();
    sjf_makeBackground< 20 >( g, r );
    
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("sjf_wrecklessDelay", getWidth()/2-potSize, 0, potSize*2, textHeight, juce::Justification::centred, 1);
    g.setColour (delayTimeBackgroundColour);
    g.fillRect(delTLLabel.getX(), delTLLabel.getY(), delTRLabel.getRight()-delTLLabel.getX(), hostSyncButton.getBottom()- delTLLabel.getY());

    g.setColour(detuneBackgroundColour);
    g.fillRect(detuneLLabel.getX(), detuneLLabel.getY(), detuneRLabel.getRight()-detuneLLabel.getX(), detuneRSlider.getBottom()- detuneLLabel.getY());
    
    g.setColour(feedbackBackgroundColour);
    g.fillRect(fbLLabel.getX(), fbLLabel.getY(), fbControlButton.getRight() - fbLLabel.getX(),fbControlButton.getBottom() - fbLLabel.getY());

    g.setColour(filterBackgroundColour);
    g.fillRect(lpCutOffLabel.getX(), lpCutOffLabel.getY(), hpCutOffSlider.getRight() - lpCutOffLabel.getX() , hpCutOffSlider.getBottom() - lpCutOffLabel.getY());
    
    g.setColour(overdriveBackgroundColour);
    g.fillRect(overdriveOnButton.getX(), overdriveOnButton.getY(), overdrivePlacementComboBox.getRight() - overdriveOnButton.getX() , overdriveGainSlider.getBottom() - overdriveOnButton.getY());

    g.setColour(lfoBackgroundColour);
    g.fillRect(lfoDSlider.getX(), lfoDSlider.getY(), lfoRSlider.getRight() - lfoDSlider.getX() , lfoRSlider.getBottom() - lfoDSlider.getY());
    
    g.setColour(wetBackgroundColour);
    g.fillRect(wetLabel.getX(), wetLabel.getY(), wetLabel.getRight() - wetLabel.getX() , wetSlider.getBottom() - wetLabel.getY());
    
    g.setColour(dryBackgroundColour);
    g.fillRect(dryLabel.getX(), dryLabel.getY(), dryLabel.getRight() - dryLabel.getX() , drySlider.getBottom() - dryLabel.getY());
    
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
    
    drySlider.setBounds(hpCutOffSlider.getX() + hpCutOffSlider.getWidth() + indent , textHeight*2, potSize, potSize*1.25);
    wetSlider.setBounds(drySlider.getX(), drySlider.getY()+drySlider.getHeight()+textHeight + indent, potSize, drySlider.getHeight());
    interpolationTypeBox.setBounds(drySlider.getX(), wetSlider.getBottom()+indent, potSize, textHeight);
    clearDelayLineButton.setBounds(interpolationTypeBox.getX(), interpolationTypeBox.getBottom()+indent, potSize, textHeight);
}



void SjfWrecklessDelayAudioProcessorEditor::timerCallback(){
    if(audioProcessor.getSyncState()){
        delTLSlider.setVisible(false);
        delTRSlider.setVisible(false);
        syncValLComboBox.setVisible(true);
        syncValLTypeComboBox.setVisible(true);
        syncValLOffsetNumBox.setVisible(true);
        if(!audioProcessor.getLinkState()){
            syncValRComboBox.setVisible(true);
            syncValRTypeComboBox.setVisible(true);
            syncLabelL.setText ("Division L", juce::dontSendNotification);
            syncValLComboBox.setBounds(syncValLComboBox.getBounds().getX(), syncValLComboBox.getBounds().getY(), potSize, textHeight*2);
            syncValLTypeComboBox.setBounds(syncValLTypeComboBox.getBounds().getX(), syncValLTypeComboBox.getBounds().getY(), potSize, textHeight);
        }
        else{
            syncValRComboBox.setVisible(false);
            syncValRTypeComboBox.setVisible(false);
            syncLabelL.setText ("Division", juce::dontSendNotification);
            syncValLComboBox.setBounds(syncValLComboBox.getBounds().getX(), syncValLComboBox.getBounds().getY(), potSize*2, textHeight*2);
            syncValLTypeComboBox.setBounds(syncValLTypeComboBox.getBounds().getX(), syncValLTypeComboBox.getBounds().getY(), potSize*2, textHeight);
        }
        syncValROffsetNumBox.setVisible(true);
        
    }
    else{
        delTLSlider.setVisible(true);
        if (!audioProcessor.getLinkState()){
            delTRSlider.setVisible(true);
            delTLSlider.setBounds(delTLSlider.getBounds().getX(), delTLSlider.getBounds().getY(), potSize, delTLSlider.getBounds().getHeight());
            delTLLabel.setText ("Time L", juce::dontSendNotification);
        }
        else{
            delTRSlider.setVisible(false);
            delTLSlider.setBounds(delTLSlider.getBounds().getX(), delTLSlider.getBounds().getY(), potSize*2, delTLSlider.getBounds().getHeight());
            delTLLabel.setText ("Time", juce::dontSendNotification);
        }
        syncValLComboBox.setVisible(false);
        syncValLTypeComboBox.setVisible(false);
        syncValLOffsetNumBox.setVisible(false);
        syncValRComboBox.setVisible(false);
        syncValRTypeComboBox.setVisible(false);
        syncValROffsetNumBox.setVisible(false);
    }
    
    if (audioProcessor.getFbLinkState()){
        fbRSlider.setVisible(false);
        fbLSlider.setBounds(fbLSlider.getBounds().getX(), fbLSlider.getBounds().getY(), potSize*2, fbLSlider.getBounds().getHeight());
        fbLLabel.setText ("Feedback", juce::dontSendNotification);
    }
    else if (!audioProcessor.getFbLinkState()){
        fbRSlider.setVisible(true);
        fbLSlider.setBounds(fbLSlider.getBounds().getX(), fbLSlider.getBounds().getY(), potSize, fbLSlider.getBounds().getHeight());
        fbLLabel.setText ("Feedback L", juce::dontSendNotification);
    }
}
