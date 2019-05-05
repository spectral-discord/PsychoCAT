/*
  ==============================================================================

    ThemedComponents.h
    Created: 24 Apr 2019 10:14:14pm
    Author:  Lina Reyne

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
    
private:
    bool leftBorder, rightBorder, topBorder, bottomBorder;
    int borderWidth;
    float fontSize;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemedButton)
};

//==============================================================================
class ThemedTextEditor   : public TextEditor
{
public:
    ThemedTextEditor();
    ~ThemedTextEditor();
        
    void setBordered (bool border);
    
private:
    
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
