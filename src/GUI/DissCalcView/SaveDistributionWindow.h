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
#include "IDs.h"
#include "../../../DisMAL/DisMAL.h"

class SaveDistributionWindow   : public Component,
                                 public Button::Listener,
                                 public TextEditor::Listener,
                                 public KeyListener
{
public:
    SaveDistributionWindow();
    ~SaveDistributionWindow();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callbacks
    void textEditorReturnKeyPressed (TextEditor& editor) override;
    void textEditorTextChanged (TextEditor& editor) override;
    void buttonClicked (Button* clickedButton) override;
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    
    // Inits & shows the component
    void show (ValueTree& newDistribution);
    void save();

private:
    // Data
    ValueTree distribution;
    FileIO file;
    
    // GUI components
    ThemedTextEditor distributionName;
    ThemedButton saveButton, cancelButton, overwriteButton, overwriteCancelButton;
    
    void resetFilePath();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaveDistributionWindow)
};
