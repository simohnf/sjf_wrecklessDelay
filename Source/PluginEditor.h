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
    SjfRecklessDelayAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    sjf_LookAndFeel otherLookandFeel;
    
    void timerCallback() override;
    
    juce::Slider drySlider, wetSlider, fbLSlider, fbRSlider, delTLSlider, delTRSlider, lpCutOffSlider, hpCutOffSlider, overdriveGainSlider, overdriveOutSlider, lfoRSlider, lfoDSlider;
    sjf_Label dryLabel, wetLabel,fbLLabel, fbRLabel, detuneLLabel, detuneRLabel, delTLLabel, delTRLabel, syncLabelL, syncLabelR, lpCutOffLabel, hpCutOffLabel, overdriveGainLabel, overdriveOutLabel, overdrivePlacementLabel;
    
    juce::ToggleButton linkButton, hostSyncButton, fbLinkButton, fbControlButton, overdriveOnButton;
    juce::TextButton clearDelayLineButton;
    
    juce::ComboBox syncValLComboBox, syncValRComboBox, syncValLTypeComboBox, syncValRTypeComboBox, overdrivePlacementComboBox;
    sjf_numBox syncValLOffsetNumBox, syncValROffsetNumBox, detuneLSlider, detuneRSlider;
    
    std::unique_ptr<SliderAttachment> drySliderAttachment, wetSliderAttachment, fbLSliderAttachment, fbRSliderAttachment, detuneLSliderAttachment, detuneRSliderAttachment, delTLSliderAttachment, delTRSliderAttachment, syncValLOffsetNumBoxAttachment, syncValROffsetNumBoxAttachment, lpCutOffSliderAttachment, hpCutOffSliderAttachment, overdriveGainSliderAttachment, overdriveOutSliderAttachment, lfoRSliderAttachment, lfoDSliderAttachment;
    std::unique_ptr<ButtonAttachment> linkButtonAttachment, hostSyncButtonAttachment, fbLinkButtonAttachment, fbControlButtonAttachment, overdriveOnButtonAttachment;
    std::unique_ptr<ComboBoxAttachment> syncValLComboBoxAttachment, syncValRComboBoxAttachment, syncValLTypeComboBoxAttachment, syncValRTypeComboBoxAttachment, overdrivePlacementComboBoxAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SjfRecklessDelayAudioProcessorEditor)
};
