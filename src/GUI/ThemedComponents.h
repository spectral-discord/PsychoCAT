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

#include "ColorTheme.h"

//==============================================================================
class ThemedButton   : public TextButton
{
public:
    ThemedButton();
    ~ThemedButton();
    
    void paintButton (Graphics& g,
                      bool drawAsHighlighted,
                      bool drawAsPressed) override;
    
    void resized() override;
    
    void setBorders (bool left, bool top, bool right, bool bottom, int width);
    void setFontSize (float newFontSize);
    void setIcon (bool setUseIcon, Icon newIcon, float newRotation = 0);
    void setIconSize (float newIconSize);
    void setPanelButton (bool isPanelButton);
    
private:
    bool leftBorder, rightBorder, topBorder, bottomBorder, useIcon, panelButton;
    int borderWidth;
    float fontSize, iconSize, rotation;
    Icon icon;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemedButton)
};

//==============================================================================
class ThemedTextEditor   : public TextEditor
{
public:
    ThemedTextEditor();
    ~ThemedTextEditor();
        
    void setBordered (bool border);
    void setEvaluateExpressions (bool evaluate);
    double getEvaluated();
    
private:
    bool evaluateExpressions;
    String expression;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemedTextEditor)
};

//==============================================================================
class ThemedComboBox   : public ComboBox
{
public:
    ThemedComboBox();
    ~ThemedComboBox();
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemedComboBox)
};

//==============================================================================
class ThemedToggleButton   : public ToggleButton
{
public:
    ThemedToggleButton();
    ~ThemedToggleButton();
        
private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemedToggleButton)
};
