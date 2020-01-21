/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ThemedComponents.h"

class GeneralOptions   : public Component
{
public:
    GeneralOptions();
    ~GeneralOptions();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    ThemedToggleButton loadPreviousSession;
    ThemedTextEditor savedDistributionLocation, distributionExportLocation, tuningExportLocation;
    Label savedDistributionLocationLabel, distributionExportLocationLabel, tuningExportLocationLabel;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralOptions)
};

class MapOptions   : public Component
{
public:
    MapOptions();
    ~MapOptions();
    
    void paint (Graphics& g) override;
    void resized() override;

    ThemedToggleButton logSteps, hearingRangePreprocessor;
    ThemedComboBox dissonanceModel;
    Label dissonanceModelLabel;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapOptions)
};

class OptimizationOptions   : public Component
{
public:
    OptimizationOptions();
    ~OptimizationOptions();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    ThemedTextEditor stepSize, stopValue, minInterval;
    Label stepSizeLabel, stopValueLabel, minIntervalLabel;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptimizationOptions)
};

class PreprocessorOptions   : public Component
{
public:
    PreprocessorOptions();
    ~PreprocessorOptions();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    ThemedTextEditor hearingRangeStart, hearingRangeEnd;
    Label hearingRangeStartLabel, hearingRangeEndLabel;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreprocessorOptions)
};

class SettingsMenu   : public Component,
                       public Button::Listener,
                       public KeyListener

{
public:
    SettingsMenu();
    ~SettingsMenu();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void setVisiblity (bool show);
    
    void buttonClicked (Button* clickedButton) override;
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    
    void init (ApplicationProperties* properties);
        
private:    
    ThemedButton generalButton, dissonanceMapsButton,
                 optimizationButton, preprocessorsButton,
                 cancelButton, saveButton;
    
    GeneralOptions general;
    MapOptions maps;
    OptimizationOptions optimizations;
    PreprocessorOptions preprocessors;
    
    Viewport view;

    PropertiesFile* settings;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsMenu)
};
