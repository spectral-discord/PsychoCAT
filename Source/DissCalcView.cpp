/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DissCalcView.h"

//==============================================================================
DissCalcView::DissCalcView()
{
    addAndMakeVisible (&distributionPanel);
    addAndMakeVisible (&calcPanel);
    addAndMakeVisible (&mapComponent);
}

DissCalcView::~DissCalcView()
{
}

void DissCalcView::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
}

void DissCalcView::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    distributionPanel.setBounds (25, 0, 150, getHeight());
    calcPanel.setBounds (area.removeFromLeft (175));
    mapComponent.setBounds (area);
    
    distributionPanel.toFront (false);
    calcPanel.toFront (false);
}

void DissCalcView::setData (ValueTree& data)
{
    if (data.hasType (IDs::CalculatorList))
    {
        calcData = data;
        calcPanel.setCalcData (data);
        mapComponent.setData (data);
    }
}

void DissCalcView::openDistributionPanel (ValueTree& distributionToOpen)
{
    distributionPanel.getDistribution().setProperty (IDs::IsViewed, false, nullptr);

    distributionPanel.setDistribution (distributionToOpen);
    
    Desktop::getInstance().getAnimator().animateComponent (&distributionPanel,
                                                           distributionPanel.getBounds().withX (175),
                                                           1, 250, false, 1, 1);
    
    distributionToOpen.setProperty (IDs::IsViewed, true, nullptr);
}

ValueTree& DissCalcView::getDistributionPanelData()
{
    return distributionPanel.getDistribution();
}

void DissCalcView::closeDistributionPanel()
{
    Desktop::getInstance().getAnimator().animateComponent (&distributionPanel,
                                                           distributionPanel.getBounds().withX (25),
                                                           1, 250, false, 1, 1);
    
    distributionPanel.getDistribution().setProperty (IDs::IsViewed, false, nullptr);
}

void DissCalcView::repositionDistributionPanel (int height)
{
    calcPanel.view.setViewPosition (0, height);
}
