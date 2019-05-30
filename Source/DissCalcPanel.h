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
    
    ThemedButton muteButton, optionsButton;
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
    Parent component for DistributionList, with GUI controls for deleting and
    duplicating the entire dissonance calc.
*/
class DissCalc   : public Component,
                   public Button::Listener
{
public:
    DissCalc();
    ~DissCalc();
    
    void paint (Graphics& g);
    void resized();
    
    // GUI callback
    void buttonClicked (Button* clickedButton);
    
    // Get/set the valuetree data for this dissonance calc
    void setCalcData (ValueTree& distributions);
    ValueTree& getCalcData();
    
    ThemedButton addDistributionButton, optionsButton;

private:
    DistributionList distributionList;
    Viewport view;
    
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
    
    ThemedButton removeButton, duplicateButton, setXAxisButton, setFromSavedButton, saveButton;
    DistributionComponent* distribution;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionOptions)
};

// Component for various DissCalc controls
// Should combine with the above DistributionOptions to satisfy DRY
// -------------- A better solution is a later Lina problem! --------------
class CalcOptions   : public DistributionOptions
{
public:
    CalcOptions()
    {
        calc = nullptr;
        removeChildComponent (&setXAxisButton);
        removeChildComponent (&setFromSavedButton);
    }
    ~CalcOptions(){}
    
    void resized() override
    {
        Rectangle<int> area = getLocalBounds().reduced (1);
        int buttonHeight = getHeight() / 2;
        
        duplicateButton.setBounds (area.removeFromTop (buttonHeight));
        removeButton.setBounds (area);
    }
    
    DissCalc* calc;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalcOptions)
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
    
    // Mouse event callbacks
    void mouseDown (const MouseEvent& event) override;
    
    // For setting the top-level valuetree node for dissonance calculations
    void setCalcData (ValueTree& calcData);
    
    // Opens the options components for distributions or dissonance calcs
    void openOptions (DistributionComponent* component);
    void openOptions (DissCalc* calc);
    
    Viewport view;
    UndoManager* undo;
    
private:
    CalcList calcs;
    
    DistributionOptions options;
    CalcOptions calcOptions;
    
    ThemedButton addCalcButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalcPanel)
};
