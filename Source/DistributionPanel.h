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
class PartialEditor   : public Component,
                        public Button::Listener,
                        public TextEditor::Listener,
                        public ValueTree::Listener
{
public:
    PartialEditor();
    ~PartialEditor();
    
    friend bool operator< (const PartialEditor& first, const PartialEditor& second) noexcept
    {
        return (first.frequencyEditor.getText().getFloatValue()
                < second.frequencyEditor.getText().getFloatValue());
    }
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void buttonClicked (Button* clickedButton) override;
    void textEditorFocusLost (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;
    
    void setPartialData (ValueTree& data);
    ValueTree& getPartialData();
    
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override {}
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    bool containsFreq (float freq);
    
    ThemedTextEditor frequencyEditor, amplitudeEditor;
    ThemedButton optionsButton, muteButton;
    
private:
    ValueTree partial;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PartialEditor)
};

//==============================================================================
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
        else if (first->getPartialData()[IDs::Freq]
            < second->getPartialData()[IDs::Freq])
        {
            return -1;
        }
        else if (first->getPartialData()[IDs::Freq]
                 > second->getPartialData()[IDs::Freq])
        {
            return 1;
        }
        
        return 0;
    }
    
    int compareElements (const ValueTree& first, const ValueTree& second)
    {
        if (first.getProperty (IDs::Freq) < second.getProperty (IDs::Freq))
        {
            return -1;
        }
        else if (first.getProperty (IDs::Freq) > second.getProperty (IDs::Freq))
        {
            return 1;
        }
        
        return 0;
    }
};

//==============================================================================
class PartialEditorList   : public Component,
                            public ValueTree::Listener
{
public:
    PartialEditorList();
    ~PartialEditorList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void addPartialEditor (ValueTree& newPartialNode);
    
    void setDistribution (ValueTree& distributionNode);
    
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    ValueTree distribution;
    
private:
    OwnedArray<PartialEditor> partialEditors;
    int editorHeight;
    
    PartialComparator comparator;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PartialEditorList)
};

//==============================================================================
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

class DistributionPanelTitleBar   : public Component,
public Button::Listener
{
public:
    DistributionPanelTitleBar();
    ~DistributionPanelTitleBar();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void buttonClicked (Button* clickedButton) override;
    
    ThemedTextEditor distributionName, fundamentalFreq, fundamentalAmp;
    ShapeButton collapseButton;
    Path arrowShape;
    Line<float> arrowLine;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionPanelTitleBar)
};

//==============================================================================

class DistributionPanel   : public Component,
                            public Button::Listener,
                            public TextEditor::Listener
{
public:
    DistributionPanel();
    ~DistributionPanel();

    void paint (Graphics& g) override;
    void resized() override;
    
    void buttonClicked (Button* clickedButton) override;
    
    void textEditorTextChanged (TextEditor& editor) override;
    void textEditorFocusLost (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;

    void mouseDown (const MouseEvent& event) override;
    
    void openOptions (PartialEditor* component);
    
    void setDistribution (ValueTree& newDistribution);
    ValueTree& getDistribution();

private:
    DistributionPanelTitleBar titleBar;
    Viewport distributionListView;
    PartialEditorList partialList;
    
    PartialOptions options;
    
    ThemedButton addButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionPanel)
};

