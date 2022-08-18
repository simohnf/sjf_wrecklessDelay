/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SjfRecklessDelayAudioProcessorEditor::SjfRecklessDelayAudioProcessorEditor (SjfRecklessDelayAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p),  audioProcessor (p), valueTreeState (vts)
{
    setLookAndFeel (&otherLookandFeel);
    
    dryLabel.setText ("Dry", juce::dontSendNotification);
    addAndMakeVisible (&dryLabel);
    dryLabel.setJustificationType (juce::Justification::centred);
    dryLabel.attachToComponent(&drySlider, false);
    
    addAndMakeVisible (&drySlider);
    drySliderAttachment.reset (new SliderAttachment (valueTreeState, "dry", drySlider));
    drySlider.setSliderStyle (juce::Slider::Rotary);
    drySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    drySlider.setNumDecimalPlacesToDisplay(3);
    drySlider.setTextValueSuffix ("%");
    
    wetLabel.setText ("Wet", juce::dontSendNotification);
    addAndMakeVisible (&wetLabel);
    wetLabel.setJustificationType (juce::Justification::centred);
    wetLabel.attachToComponent(&wetSlider, false);
    
    addAndMakeVisible (&wetSlider);
    wetSliderAttachment.reset (new SliderAttachment (valueTreeState, "wet", wetSlider));
    wetSlider.setSliderStyle (juce::Slider::Rotary);
    wetSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    wetSlider.setNumDecimalPlacesToDisplay(3);
    wetSlider.setTextValueSuffix ("%");
    

    delTLLabel.setText ("Time L", juce::dontSendNotification);
    addAndMakeVisible (&delTLLabel);
    delTLLabel.setJustificationType (juce::Justification::centred);
    delTLLabel.attachToComponent(&delTLSlider, false);
    
    addAndMakeVisible (&delTLSlider);
    delTLSliderAttachment.reset (new SliderAttachment (valueTreeState, "timeLeft", delTLSlider));
    delTLSlider.setSliderStyle (juce::Slider::Rotary);
    delTLSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    delTLSlider.setNumDecimalPlacesToDisplay(3);
    delTLSlider.setTextValueSuffix ("ms");
    
    delTRLabel.setText ("Time R", juce::dontSendNotification);
    addAndMakeVisible (&delTRLabel);
    delTRLabel.setJustificationType (juce::Justification::centred);
    delTRLabel.attachToComponent(&delTRSlider, false);
    
    addAndMakeVisible (&delTRSlider);
    delTRSliderAttachment.reset (new SliderAttachment (valueTreeState, "timeRight", delTRSlider));
    delTRSlider.setSliderStyle (juce::Slider::Rotary);
    delTRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
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
    fbLSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
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
    fbRSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    fbRSlider.setNumDecimalPlacesToDisplay(3);
    fbRSlider.setTextValueSuffix ("%");
    
    addAndMakeVisible(&fbLinkButton);
    fbLinkButtonAttachment.reset(new ButtonAttachment(valueTreeState, "fbLink", fbLinkButton));
    fbLinkButton.setButtonText("Feedback Link");
    
    addAndMakeVisible(&detuneLLabel);
    detuneLLabel.attachToComponent(&detuneLSlider, false);
    detuneLLabel.setText("detune L", juce::dontSendNotification);
    addAndMakeVisible (&detuneLSlider);
    detuneLSliderAttachment.reset (new SliderAttachment (valueTreeState, "detuneL", detuneLSlider));
    detuneLSlider.setTextValueSuffix ("cent");

    addAndMakeVisible(&detuneRLabel);
    detuneRLabel.attachToComponent(&detuneRSlider, false);
    detuneRLabel.setText("detune R", juce::dontSendNotification);
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
    
    startTimer(100);
    
    setSize (400, 300);
}

SjfRecklessDelayAudioProcessorEditor::~SjfRecklessDelayAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void SjfRecklessDelayAudioProcessorEditor::timerCallback(){
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
            syncValLComboBox.setBounds(0, 20, 80, 40);
            syncValLTypeComboBox.setBounds(0, 60, 80, 20);
        }
        else{
            syncValRComboBox.setVisible(false);
            syncValRTypeComboBox.setVisible(false);
            syncLabelL.setText ("Division", juce::dontSendNotification);
            syncValLComboBox.setBounds(0, 20, 160, 40);
            syncValLTypeComboBox.setBounds(0, 60, 160, 20);
        }
        syncValROffsetNumBox.setVisible(true);
        
    }
    else{
        delTLSlider.setVisible(true);
        if (!audioProcessor.getLinkState()){
            delTRSlider.setVisible(true);
            delTLSlider.setBounds(0, 20, 80, 80);
            delTLLabel.setText ("Time L", juce::dontSendNotification);
        }
        else{
            delTRSlider.setVisible(false);
            delTLSlider.setBounds(0, 20, 160, 80);
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
        fbLSlider.setBounds(0, 200, 160, 80);
        fbLLabel.setText ("Feedback", juce::dontSendNotification);
    }
    else if (!audioProcessor.getFbLinkState()){
        fbRSlider.setVisible(true);
        fbLSlider.setBounds(0, 200, 80, 80);
        fbLLabel.setText ("Feedback L", juce::dontSendNotification);
    }
}


void SjfRecklessDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("sjf-V-reckless_delay!", getLocalBounds(), juce::Justification::centred, 1);
}

void SjfRecklessDelayAudioProcessorEditor::resized()
{
    drySlider.setBounds(160, 20, 80, 80);
    wetSlider.setBounds(240, 20, 80, 80);
    
    delTLSlider.setBounds(0, 20, 80, 80);
    delTRSlider.setBounds(80, 20, 80, 80);
    
    syncValLComboBox.setBounds(0, 20, 80, 40);
    syncValLTypeComboBox.setBounds(0, 60, 80, 20);
    syncValLOffsetNumBox.setBounds(0, 80, 80, 20);
    
    syncValRComboBox.setBounds(80, 20, 80, 40);
    syncValRTypeComboBox.setBounds(80, 60, 80, 20);
    syncValROffsetNumBox.setBounds(80, 80, 80, 20);

    linkButton.setBounds(0, 100, 160, 20);
    hostSyncButton.setBounds(0, 120, 160, 20);
    
    
    detuneLSlider.setBounds(0, 160, 80, 20);
    detuneRSlider.setBounds(80, 160, 80, 20);
    
    fbLSlider.setBounds(0, 200, 80, 80);
    fbRSlider.setBounds(80, 200, 80, 80);
    fbLinkButton.setBounds(0, 280, 160, 20);
}
