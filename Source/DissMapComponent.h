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
#include "DisMAL/DisMAL.h"
#include "DistributionPanel.h"

//==============================================================================
/*
    This component represents a dissonance calc without its
    corresponding distribution data. (Which is held in DissCalc)
 
    It has gui components for setting some calc data into the valuetree data model,
    callbacks received from the valuetree data model for setting DisMAL data,
    and the ability to draw dissonance maps.
*/
class DissonanceMap   : public Component,
                        public TextEditor::Listener,
                        public ComboBox::Listener,
                        public Button::Listener,
                        public ValueTree::Listener
{
public:
    DissonanceMap();
    ~DissonanceMap();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callbacks to set valuetree data
    void comboBoxChanged (ComboBox* changedBox) override;
    void textEditorFocusLost (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;
    void buttonClicked (Button* clickedButton) override;
    
    // Data model callbacks to set DisMAL data
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    
    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    ValueTree mapData;

private:
    ThemedComboBox dissonanceModel;
    ThemedTextEditor startFreq, endRatio;
    ThemedToggleButton lockScale, logSteps;
    
    DissonanceCalc calc;
    NormalisableRange<float> normalizer, denormalizer;
    
    PartialComparator comparator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissonanceMap)
};

//==============================================================================
/*
    Simple container component for DissonanceMap components.  Goes inside a MapViewport.
*/
class MapList   : public Component,
                  public ValueTree::Listener
{
public:
    MapList();
    ~MapList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // Callbacks to create or remove DissonanceMap components
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    
    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    OwnedArray<DissonanceMap> maps;
    ValueTree mapsData;
    UndoManager* undo;
private:
    int mapHeight;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapList)
};

//==============================================================================
/*
    This subclass of Viewport is only created to override visibleAreaChanged(),
    to keep this view and the DissCalcPanel views in sync.
*/
class MapViewport   : public Viewport
{
public:
    MapViewport(){}
    ~MapViewport(){}
    
    void visibleAreaChanged (const Rectangle<int>& newVisibleArea) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapViewport)
};

//==============================================================================
class DissMapComponentFooter   : public Component,
                                 public ComboBox::Listener
{
public:
    DissMapComponentFooter();
    ~DissMapComponentFooter();
    
    void paint (Graphics&) override;
    void resized() override;
    
    void comboBoxChanged (ComboBox* changedBox) override;
    
    ValueTree data;
    
private:
    ThemedComboBox gridLines;
};

//==============================================================================
/*
    This class draws the dissonance map component in the DissCalcView.
*/
class DissMapComponent    : public Component
{
public:
    DissMapComponent();
    ~DissMapComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void setData (ValueTree& calcList, UndoManager& undo);
    
private:
    MapList maps;
    MapViewport mapView;
    DissMapComponentFooter footer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissMapComponent)
};
