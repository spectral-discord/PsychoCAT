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
DissCalcView::DissCalcView()   : tips (this, 1200)
{
    addAndMakeVisible (&distributionPanel);
    addAndMakeVisible (&calcPanel);
    addAndMakeVisible (&mapComponent);
    
    getLookAndFeel().setColour (0x1001b00, Theme::mainBackground);
    getLookAndFeel().setColour (0x1001c00, Theme::text);
    getLookAndFeel().setColour (0x1001c10, Theme::border);
    
    addChildComponent (saveDistribution);
    addChildComponent (savedDistributionList);
    
    addKeyListener (this);
    
    setWantsKeyboardFocus (true);
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
    
    saveDistribution.centreWithSize (200, 140);
    savedDistributionList.centreWithSize (400, 350);
    
    distributionPanel.toFront (false);
    calcPanel.toFront (false);
    saveDistribution.toFront (false);
}

void DissCalcView::setData (ValueTree& data)
{
    if (data.hasType (IDs::CalculatorList))
    {
        calcData = data;
        calcPanel.setCalcData (data);
        mapComponent.setData (data, undo);
        
        calcPanel.undo = &undo;
        distributionPanel.undo = &undo;
        savedDistributionList.undo = &undo;
    }
}

void DissCalcView::openDistributionPanel (ValueTree& distributionToOpen)
{
    distributionPanel.getDistribution().setProperty (IDs::IsViewed, false, nullptr);

    distributionPanel.setDistribution (distributionToOpen);
    
    // Animate the distribution panel into a visible position
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
    // Animate the distribution panel to hide behind the dissonance calc panel
    Desktop::getInstance().getAnimator().animateComponent (&distributionPanel,
                                                           distributionPanel.getBounds().withX (25),
                                                           1, 250, false, 1, 1);
    
    distributionPanel.getDistribution().setProperty (IDs::IsViewed, false, nullptr);
}

bool DissCalcView::distributionPanelIsOpen()
{
    if (distributionPanel.getX() == 25)
        return false;
    
    return true;
}

void DissCalcView::repositionCalcPanel (int height)
{
    calcPanel.view.setViewPosition (0, height);
}

bool DissCalcView::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    if (! isParentOf (originatingComponent)
        && originatingComponent != this)
        return false;
    
    if (key == KeyPress ('z', ModifierKeys::commandModifier, 'z'))
    {
        undo.undo();
        
        return true;
    }
    else if (key == KeyPress ('z', 9, 'z'))
    {
        undo.redo();
        
        return true;
    }
    
    return false;
}
