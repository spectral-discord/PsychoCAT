/*
  ==============================================================================

    This file is part of Psychotonal CAT (Composition and Analysis Tools)
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DissMapComponent.h"
#include "DissCalcPanel.h"
#include "DistributionPanel.h"
#include "SaveDistributionWindow.h"
#include "SavedDistributionsList.h"

//==============================================================================
/*
    This class creates the GUI view and controls when calculating dissonances and creating dissonance maps.
*/
class DissCalcView   : public Component,
                       public KeyListener
{
public:
    DissCalcView();
    ~DissCalcView();

    void paint (Graphics& g) override;
    void resized() override;
    
    // Sets the top-level valuetree node to hold all dissonance calculation data
    void setData (ValueTree& data);
    
    void openDistributionPanel (ValueTree& distributionToOpen);
    void closeDistributionPanel();
    bool distributionPanelIsOpen();
    ValueTree& getDistributionPanelData();
    
    void repositionCalcPanel (int height);
    
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override;
    void mouseDown (const MouseEvent& event) override;
    
    // Opens the options components for distributions or partials
    void openOptions (DistributionComponent* component);
    void openOptions (PartialEditor* component, bool isFundamental = false);
    
    SaveDistributionWindow saveDistribution;
    SavedDistributionsList savedDistributionList;
    
private:
    DissCalcPanel calcPanel;
    DistributionPanel distributionPanel;
    DissMapComponent mapComponent;
    DistributionOptions distributionOptions;
    PartialOptions partialOptions;

    ValueTree calcData;
    UndoManager undo;
    
    TooltipWindow tips;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalcView)
};
