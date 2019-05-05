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
class DissonanceMap   : public Component,
                        public TextEditor::Listener,
                        public ComboBox::Listener,
                        public ValueTree::Listener
{
public:
    DissonanceMap();
    ~DissonanceMap();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void comboBoxChanged (ComboBox* changedBox) override;
    
    void textEditorFocusLost (TextEditor& editor) override;
    void textEditorReturnKeyPressed (TextEditor& editor) override;
    
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    void drawMap();
    
    ValueTree mapData;

private:
    ThemedComboBox dissonanceModel, logSteps;
    ThemedTextEditor startFreq, endRatio;
    
    DissonanceCalc calc;
    NormalisableRange<float> normalizer, denormalizer;
    
    PartialComparator comparator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissonanceMap)
};

//==============================================================================
class MapList   : public Component,
                  public ValueTree::Listener
{
public:
    MapList();
    ~MapList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    OwnedArray<DissonanceMap> maps;
    ValueTree mapsData;
private:
    int mapHeight;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapList)
};

//==============================================================================
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
    
    void setData (ValueTree& calcList);
    
private:
    MapList maps;
    MapViewport mapView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissMapComponent)
};
