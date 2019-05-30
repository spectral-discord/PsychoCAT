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
#include "IDs.h"
#include "ThemedComponents.h"

//==============================================================================
// Component that displays and edits partial data
class PartialEditor   : public Component,
                        public Button::Listener,
                        public TextEditor::Listener,
                        public ValueTree::Listener
{
public:
    PartialEditor();
    ~PartialEditor();
    
    // For sorting by ascending frequency
    friend bool operator< (const PartialEditor& first, const PartialEditor& second) noexcept
    {
        return (first.frequencyEditor.getText().getFloatValue()
                < second.frequencyEditor.getText().getFloatValue());
    }
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callbacks
    void buttonClicked (Button* clickedButton) override;
    void textEditorFocusLost (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;
    
    // Data model callbacks
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    
    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override {}
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    // Get/set the valuetree for the partial's data
    void setPartialData (ValueTree& data);
    ValueTree& getPartialData();
    
    // Checks if the overtone distribution that this partial belongs to
    // already contains a partial with the given frequency
    bool containsFreq (float freq);
    
    ThemedTextEditor frequencyEditor, amplitudeEditor;
    ThemedButton optionsButton, muteButton;
    
private:
    ValueTree partial;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PartialEditor)
};

//==============================================================================
// This class enables the sorting of PartialEditor components as well as Partial valuetree nodes
class PartialComparator
{
public:
    PartialComparator(){}
    ~PartialComparator(){}
    
    int compareElements (PartialEditor* first, PartialEditor* second)
    {
        if (first->frequencyEditor.isEmpty()
            && second->frequencyEditor.isEmpty())
        {
            return 0;
        }
        else if (second->frequencyEditor.isEmpty())
        {
            return -1;
        }
        else if (first->frequencyEditor.isEmpty())
        {
            return 1;
        }
        else if (first->getPartialData()[IDs::Freq].operator float()
            < second->getPartialData()[IDs::Freq].operator float())
        {
            first->frequencyEditor.setText (first->getPartialData()[IDs::Freq]);
            second->frequencyEditor.setText (second->getPartialData()[IDs::Freq]);

            return -1;
        }
        else if (first->getPartialData()[IDs::Freq].operator float()
                 > second->getPartialData()[IDs::Freq].operator float())
        {
            first->frequencyEditor.setText (first->getPartialData()[IDs::Freq]);
            second->frequencyEditor.setText (second->getPartialData()[IDs::Freq]);
            
            return 1;
        }
        
        return 0;
    }
    
    int compareElements (const ValueTree& first, const ValueTree& second)
    {
        if (first[IDs::Freq].operator float() < second[IDs::Freq].operator float())
        {
            return -1;
        }
        else if (first[IDs::Freq].operator float() > second[IDs::Freq].operator float())
        {
            return 1;
        }
        
        return 0;
    }
};

//==============================================================================
/*
    Contains all partials for the currently viewed overtone distribution.
 
    This component goes in a viewport owned by DistributionPanel.
*/
class PartialEditorList   : public Component,
                            public ValueTree::Listener
{
public:
    PartialEditorList();
    ~PartialEditorList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // Data model callbacks
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    
    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    void setDistribution (ValueTree& distributionNode);
    ValueTree distribution;
    
private:
    OwnedArray<PartialEditor> partialEditors;
    int editorHeight;
    
    PartialComparator comparator;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PartialEditorList)
};

//==============================================================================
/*
    Simple component that will become visible and reposition when a partial's
    options button is clicked.
*/
class PartialOptions   : public Component
{
public:
    PartialOptions();
    ~PartialOptions();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    PartialEditor* partial;
    
    ThemedButton removeButton;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PartialOptions)
};

//==============================================================================
/*
    Title bar for the distribution panel.
 
    Contains fundamental freq/amp editors, distribution name editor, and a close panel button.
*/
class DistributionPanelTitleBar   : public Component,
                                    public Button::Listener
{
public:
    DistributionPanelTitleBar();
    ~DistributionPanelTitleBar();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callback
    void buttonClicked (Button* clickedButton) override;
    
    ThemedTextEditor distributionName, fundamentalFreq, fundamentalAmp;
    ShapeButton collapseButton;
    Path arrowShape;
    Line<float> arrowLine;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionPanelTitleBar)
};

//==============================================================================
/*
    Panel for viewing/editing overtone distribution data
*/
class DistributionPanel   : public Component,
                            public Button::Listener,
                            public TextEditor::Listener
{
public:
    DistributionPanel();
    ~DistributionPanel();

    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callbacks
    void buttonClicked (Button* clickedButton) override;
    void textEditorTextChanged (TextEditor& editor) override;
    void textEditorFocusLost (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;

    // Mouse activity callback
    void mouseDown (const MouseEvent& event) override;
    
    // Sets and draws the distribution panel's data, slides the panel into position
    void openOptions (PartialEditor* component);
    
    // Get/set the currently viewed distribution data
    void setDistribution (ValueTree& newDistribution);
    ValueTree& getDistribution();
    
    UndoManager* undo;

private:
    DistributionPanelTitleBar titleBar;
    Viewport distributionListView;
    PartialEditorList partialList;
    
    PartialOptions options;
    
    ThemedButton addButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionPanel)
};

