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
#include "../../../DisMAL/FileIO.h"

//==============================================================================
/*
*/
class SavedDistributionsList    : public Component,
                                  public Button::Listener,
                                  public TextEditor::Listener,
                                  public KeyListener
{
public:
    SavedDistributionsList();
    ~SavedDistributionsList();
    
    // Inits & shows the component
    void show (ValueTree& treeNode);
    
    // GUI callbacks
    void buttonClicked (Button* clickedButton) override;
    void buttonStateChanged (Button* button) override;
    void textEditorTextChanged (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    void mouseDoubleClick (const MouseEvent& event) override;

    void paint (Graphics& g) override;
    void resized() override;
    
    UndoManager* undo;

private:
    // GUI components
    Viewport view;
    Component viewComponent;
    OwnedArray<ThemedButton> fileButtons;
    ThemedTextEditor searchBar;
    ThemedButton openButton, closeButton;
    
    // Data
    ValueTree distributionNode;
    File selectedFile;
    Array<File> fileList;
    
    
    void displayFiles (String searchParam = "");
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SavedDistributionsList)
};
