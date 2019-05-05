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

//==============================================================================
/*
    This class creates the GUI view and controls when calculating dissonance and creating dissonance maps.
*/
class DissCalcView   : public Component
{
public:
    DissCalcView();
    ~DissCalcView();

    void paint (Graphics& g) override;
    void resized() override;
    
    void setData (ValueTree& data);
    
    void openDistributionPanel (ValueTree& distributionToOpen);
    ValueTree& getDistributionPanelData();
    void closeDistributionPanel();
    
    void repositionDistributionPanel (int height);
    
private:
    DissCalcPanel calcPanel;
    DistributionPanel distributionPanel;
    DissMapComponent mapComponent;
    
    ValueTree calcData;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DissCalcView)
};
