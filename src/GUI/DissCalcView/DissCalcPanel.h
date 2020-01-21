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
// Component representing an overtone distribution
class DistributionComponent   : public Component,
                                public Button::Listener,
                                public ValueTree::Listener
{
public:
    DistributionComponent();
    ~DistributionComponent();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callbacks
    void buttonClicked (Button* clickedButton) override;
    void mouseDown (const MouseEvent& event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseMove (const MouseEvent &event) override;
    
    // Data model callbacks
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    
    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override {}
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    // Get/set the distribution's valuetree node
    void setDistributionData (ValueTree& newDistribution);
    ValueTree& getDistributionData();
    
    ThemedButton optionsButton;
    ValueTree distribution;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionComponent)
};

//==============================================================================
// This component contains a list of DistributionComponents and is owned by a DissCalc's Viewport
class DistributionList   : public Component,
                           public ValueTree::Listener
{
public:
    DistributionList();
    ~DistributionList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // Data model callbacks
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override;
    
    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    // Get/set the valuetree data for this dissonance calc
    void setCalcData (ValueTree& data);
    ValueTree& getCalcData();
    
    // Returns the total height of the DistributionComponents in this calc
    // For setting the height of this component and its parent Viewport
    int idealHeight();
    
    ValueTree distributions;
    
private:
    int distributionHeight;
    OwnedArray<DistributionComponent> distributionComponents;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionList)
};

//==============================================================================
/*
 This subclass of Viewport is only created to override visibleAreaChanged()
 */
class CalcViewport   : public Viewport
{
public:
    CalcViewport(){}
    ~CalcViewport(){}
    
    void visibleAreaChanged (const Rectangle<int>& newVisibleArea) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalcViewport)
};

/*
    Parent component for DistributionList, with GUI controls for deleting and
    duplicating the entire dissonance calc.
*/
class DissCalc   : public Component,
                   public Button::Listener
{
public:
    DissCalc();
    ~DissCalc();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callback
    void buttonClicked (Button* clickedButton) override;
    
    // Get/set the valuetree data for this dissonance calc
    void setCalcData (ValueTree& distributions);
    ValueTree& getCalcData();
    
    ThemedButton addDistributionButton, removeButton, copyButton, lockScaleButton, logButton;

private:
    DistributionList distributionList;
    CalcViewport view;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalc)

};

//==============================================================================
// Contains a list of DissCalc components, owned by the DissCalcPanel's viewport
class CalcList   : public Component,
                   public ValueTree::Listener
{
public:
    CalcList();
    ~CalcList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    // Get/set the height of the dissonance calcs.
    // Could be used in the future to enable resizeable dissonance maps/DissCalc objects, if desired.
    void setCalcHeight (int newHeight);
    int getCalcHeight();
    
    // Data model callbacks
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;

    // Unused pure-virtual callbacks inhereted from ValueTree::Listener
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    OwnedArray<DissCalc> calcs;
    ValueTree dissonanceCalcs;
    
private:
    int calcHeight;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalcList)

};

//==============================================================================
// Component for various DistributionComponent controls
class DistributionOptions   : public Component
{
public:
    DistributionOptions();
    ~DistributionOptions();
    
    void paint (Graphics& g) override;
    void resized() override;
        
    ThemedButton removeButton, duplicateButton, setXAxisButton, setFromSavedButton, saveButton, muteButton;
    DistributionComponent* distribution;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionOptions)
};

//==============================================================================
// Panel for viewing/manipulating overtone distributions
class DissCalcPanel   : public Component,
                        public Button::Listener
{
public:
    DissCalcPanel();
    ~DissCalcPanel();

    void paint (Graphics& g) override;
    void resized() override;
    
    // GUI callbacks
    void buttonClicked (Button* clickedButton) override;
    
    // For setting the top-level valuetree node for dissonance calculations
    void setCalcData (ValueTree& calcData);
    
    Viewport view;
    UndoManager* undo;
    DistributionOptions* options;
    
private:
    CalcList calcs;
    ThemedButton addCalcButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalcPanel)
};
