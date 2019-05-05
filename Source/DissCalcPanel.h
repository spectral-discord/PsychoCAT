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

class DistributionComponent   : public Component,
                                public Button::Listener,
                                public ValueTree::Listener
{
public:
    DistributionComponent();
    ~DistributionComponent();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void buttonClicked (Button* clickedButton) override;
    void mouseDown (const MouseEvent& event) override;
    
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override;
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override {}
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override {}
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
    
    void setDistributionData (ValueTree& newDistribution);
    ValueTree& getDistributionData();
    
    ThemedButton muteButton, optionsButton;
    ValueTree distribution;

private:    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionComponent)
};

//==============================================================================

class DistributionList   : public Component,
                           public ValueTree::Listener
{
public:
    DistributionList();
    ~DistributionList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void setCalcData (ValueTree& data);
    ValueTree& getCalcData();
    
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
    void valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex) override;
    void valueTreePropertyChanged (ValueTree& parent, const Identifier& ID) override {}
    void valueTreeParentChanged (ValueTree& adoptedTree) override {}
    void valueTreeRedirected (ValueTree& redirectedTree) override {}
        
    int idealHeight();
    
    ValueTree distributions;
    
private:
    int distributionHeight;
    OwnedArray<DistributionComponent> distributionComponents;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionList)
};

//==============================================================================

class DissCalc   : public Component,
                   public Button::Listener
{
public:
    DissCalc();
    ~DissCalc();
    
    void paint (Graphics& g);
    void resized();
    
    void buttonClicked (Button* clickedButton);
    
    void setCalcData (ValueTree& distributions);
    ValueTree& getCalcData();
    ThemedButton addDistributionButton, optionsButton;

private:
    DistributionList distributionList;
    Viewport view;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalc)

};

//==============================================================================

class CalcList   : public Component,
                   public ValueTree::Listener
{
public:
    CalcList();
    ~CalcList();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    void setCalcHeight (int newHeight);
    int getCalcHeight();
    
    void valueTreeChildAdded (ValueTree& parent, ValueTree& newChild) override;
    void valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex) override;
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

class DistributionOptions   : public Component
{
public:
    DistributionOptions();
    ~DistributionOptions();
    
    void paint (Graphics& g) override;
    void resized() override;
    
    ThemedButton removeButton, duplicateButton, setXAxisButton, setFromSavedButton;
    DistributionComponent* distribution;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistributionOptions)
};

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

class DissCalcPanel   : public Component,
                        public Button::Listener
{
public:
    DissCalcPanel();
    ~DissCalcPanel();

    void paint (Graphics& g) override;
    void resized() override;
    
    void buttonClicked (Button* clickedButton) override;
    void mouseDown (const MouseEvent& event) override;
    
    void setCalcData (ValueTree& calcData);
    
    void openOptions (DistributionComponent* component);
    void openOptions (DissCalc* calc);
    
    Viewport view;

private:
    CalcList calcs;
    
    DistributionOptions options;
    CalcOptions calcOptions;
    
    ThemedButton addCalcButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalcPanel)
};
