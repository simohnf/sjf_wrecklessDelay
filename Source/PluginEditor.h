/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SjfRecklessDelayAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SjfRecklessDelayAudioProcessorEditor (SjfRecklessDelayAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~SjfRecklessDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SjfRecklessDelayAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    sjf_LookAndFeel otherLookandFeel;
    
    void timerCallback() override;
    
    juce::Slider drySlider, wetSlider, fbLSlider, fbRSlider, delTLSlider, delTRSlider;
    juce::Label dryLabel, wetLabel,fbLLabel, fbRLabel, detuneLLabel, detuneRLabel, delTLLabel, delTRLabel, syncLabelL, syncLabelR;
    
    juce::ToggleButton linkButton, hostSyncButton, fbLinkButton;
    
    juce::ComboBox syncValLComboBox, syncValRComboBox, syncValLTypeComboBox, syncValRTypeComboBox;
    sjf_numBox syncValLOffsetNumBox, syncValROffsetNumBox, detuneLSlider, detuneRSlider;
    
    std::unique_ptr<SliderAttachment> drySliderAttachment, wetSliderAttachment, /*delTSliderAttachment, stSpreadSliderAttachment,*/ fbLSliderAttachment, fbRSliderAttachment, detuneLSliderAttachment, detuneRSliderAttachment, delTLSliderAttachment, delTRSliderAttachment, syncValLOffsetNumBoxAttachment, syncValROffsetNumBoxAttachment;
    std::unique_ptr<ButtonAttachment> linkButtonAttachment, hostSyncButtonAttachment, fbLinkButtonAttachment;
    std::unique_ptr<ComboBoxAttachment> syncValLComboBoxAttachment, syncValRComboBoxAttachment, syncValLTypeComboBoxAttachment, syncValRTypeComboBoxAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SjfRecklessDelayAudioProcessorEditor)
};
