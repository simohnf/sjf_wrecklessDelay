/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../sjf_audio/sjf_Label.h"
#include "../../sjf_audio/sjf_numBox.h"
#include "../../sjf_audio/sjf_LookAndFeel.h" 

//==============================================================================
/**
*/
class SjfWrecklessDelayAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
    sjf_numBox testBox;
    
public:
    SjfWrecklessDelayAudioProcessorEditor (SjfWrecklessDelayAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~SjfWrecklessDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    
    
private:
    
    
    SjfWrecklessDelayAudioProcessor& audioProcessor;
    
    
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    
    sjf_lookAndFeel otherLookandFeel; 
    
    void linkOrSyncChange();
    void fbLinkChange();
    void timerCallback() override;
    
//    juce::Label dryLabel, wetLabel,fbLLabel, fbRLabel, detuneLLabel, detuneRLabel, delTLLabel, delTRLabel, syncLabelL, syncLabelR, lpCutOffLabel, hpCutOffLabel, lfoRLabel, lfoDLabel, overdriveOutLabel, overdrivePlacementLabel;
    juce::Label tooltipLabel;
    
    juce::Slider drySlider, wetSlider, fbLSlider, fbRSlider, delTLSlider, delTRSlider, lpCutOffSlider, hpCutOffSlider, overdriveGainSlider,/* overdriveOutSlider,*/ lfoRSlider, lfoDSlider;
    
    juce::ToggleButton linkButton, hostSyncButton, fbLinkButton, fbControlButton, overdriveOnButton, tooltipsToggle;
    juce::TextButton clearDelayLineButton;

    juce::String MAIN_TOOLTIP = "sjf_wrecklessDelay: \nStereo delay with independent detune for each channel, lfo modulation, and soft clipping";
    
    juce::ComboBox interpolationTypeBox, syncValLComboBox, syncValRComboBox, syncValLTypeComboBox, syncValRTypeComboBox, overdrivePlacementComboBox;
    sjf_numBox syncValLOffsetNumBox, syncValROffsetNumBox, detuneLSlider, detuneRSlider;
    
    std::unique_ptr<SliderAttachment> drySliderAttachment, wetSliderAttachment, fbLSliderAttachment, fbRSliderAttachment, detuneLSliderAttachment, detuneRSliderAttachment, delTLSliderAttachment, delTRSliderAttachment, syncValLOffsetNumBoxAttachment, syncValROffsetNumBoxAttachment, lpCutOffSliderAttachment, hpCutOffSliderAttachment, overdriveGainSliderAttachment, /*overdriveOutSliderAttachment,*/ lfoRSliderAttachment, lfoDSliderAttachment;
    std::unique_ptr<ButtonAttachment> linkButtonAttachment, hostSyncButtonAttachment, fbLinkButtonAttachment, fbControlButtonAttachment, overdriveOnButtonAttachment;
    std::unique_ptr<ComboBoxAttachment> interpolationTypeBoxAttachment, syncValLComboBoxAttachment, syncValRComboBoxAttachment, syncValLTypeComboBoxAttachment, syncValRTypeComboBoxAttachment, overdrivePlacementComboBoxAttachment;
    
    
//    juce::Image m_backgroundImage = juce::ImageCache::getFromMemory ( BinaryData::primes1_png, BinaryData::primes1_pngSize );
    
//        juce::Image m_backgroundImage = juce::ImageCache::getFromMemory (BinaryData::primes2_png, BinaryData::primes2_pngSize );
//        juce::Image m_backgroundImage = juce::ImageCache::getFromMemory (BinaryData::primes3_png, BinaryData::primes3_pngSize );
    //    juce::Image m_backgroundImage = juce::ImageCache::getFromMemory (BinaryData::primes4_png, BinaryData::primes4_pngSize );
    //    juce::Image m_backgroundImage = juce::ImageCache::getFromMemory (BinaryData::Tree1_png, BinaryData::Tree1_pngSize );
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SjfWrecklessDelayAudioProcessorEditor)
};
