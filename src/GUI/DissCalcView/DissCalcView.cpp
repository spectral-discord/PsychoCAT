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
DissCalcView::DissCalcView()   : tips (this)
{
    addAndMakeVisible (&distributionPanel);
    addAndMakeVisible (&calcPanel);
    addAndMakeVisible (&mapComponent);
    
    getLookAndFeel().setColour (0x1001b00, Theme::mainBackground);
    getLookAndFeel().setColour (0x1001c00, Theme::text);
    getLookAndFeel().setColour (0x1001c10, Theme::headerBackground);
    
    addChildComponent (saveDistribution);
    addChildComponent (savedDistributionList);
    
    addKeyListener (this);
    
    setWantsKeyboardFocus (true);
    
    addChildComponent (distributionOptions);
    distributionOptions.muteButton.addListener (&calcPanel);
    distributionOptions.duplicateButton.addListener (&calcPanel);
    distributionOptions.setXAxisButton.addListener (&calcPanel);
    distributionOptions.setFromSavedButton.addListener (&calcPanel);
    distributionOptions.saveButton.addListener (&calcPanel);
    distributionOptions.removeButton.addListener (&calcPanel);
    
    addChildComponent (partialOptions);
    partialOptions.muteButton.addListener (&distributionPanel);
    partialOptions.removeButton.addListener (&distributionPanel);

    calcPanel.options = &distributionOptions;
    distributionPanel.options = &partialOptions;
    addMouseListener (this, true);
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
    
    saveDistribution.centreWithSize (200, 80);
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

    if (distributionOptions.isVisible())
    {
        distributionOptions.setTopLeftPosition (distributionOptions.getX(),
                                    distributionOptions.distribution->getScreenY() - getTopLevelComponent()->getScreenY() - 3);
        
        if (distributionOptions.getBottom() > getHeight() - 35 || distributionOptions.getY() < 0)
        {
            distributionOptions.setVisible (false);
            distributionOptions.distribution->optionsButton.setToggleState (false, dontSendNotification);
            distributionOptions.distribution = nullptr;
        }
    }
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

void DissCalcView::openOptions (DistributionComponent* component)
{
    // Resets the previously-clicked DistributionComponent::optionsButton if active
    if (distributionOptions.distribution != nullptr
        && distributionOptions.distribution->optionsButton.getToggleState())
        distributionOptions.distribution->optionsButton.setToggleState (false, dontSendNotification);
    
    // Close options if the same options buttons was pushed
    if (component == distributionOptions.distribution && distributionOptions.isVisible())
    {
        distributionOptions.setVisible (false);
        distributionOptions.distribution = nullptr;
        return;
    }
        
    // Setup options
    distributionOptions.distribution = component;
    distributionOptions.setAlpha (0);
    distributionOptions.setVisible (true);
    distributionOptions.toFront (false);
    
    distributionOptions.muteButton.setToggleState (distributionOptions.distribution->getDistributionData()[IDs::Mute], dontSendNotification);
    distributionOptions.muteButton.setTooltip (distributionOptions.distribution->getDistributionData()[IDs::Mute]
                                               ? "Unmute"
                                               : "Mute");
    
    int height = component->getHeight() + 4;
    int width = component->getDistributionData()[IDs::XAxis] ? height * 4 - 33 : height * 6 - 51;
    
    distributionOptions.setSize (width, height);
    distributionOptions.setTopLeftPosition (getLocalPoint (component, Point<int> (component->getWidth(), -3)));
    Desktop::getInstance().getAnimator().fadeIn (&distributionOptions, 250);
    
    component->optionsButton.setToggleState (true, dontSendNotification);
}

void DissCalcView::openOptions (PartialEditor* component, bool isFundamental)
{
    // Close options if the same options buttons was pushed
    if (component == partialOptions.partial && partialOptions.isVisible())
    {
        partialOptions.setVisible (false);
        partialOptions.partial = nullptr;
        return;
    }
    
    // Setup options
    if (component != nullptr && ! isFundamental)
        partialOptions.partial = component;
    
    partialOptions.setAlpha (0);
    partialOptions.setVisible (true);
    partialOptions.toFront (true);
    
    if (component != nullptr && ! isFundamental)
    {
        partialOptions.muteButton.setToggleState (partialOptions.partial->getPartialData()[IDs::Mute], dontSendNotification);
        partialOptions.muteButton.setTooltip (partialOptions.partial->getPartialData()[IDs::Mute] ? "Unmute" : "Mute");
    }
    else if (isFundamental)
    {
        partialOptions.muteButton.setToggleState (distributionPanel.getDistribution()[IDs::FundamentalMute], dontSendNotification);
        partialOptions.muteButton.setTooltip (distributionPanel.getDistribution()[IDs::FundamentalMute] ? "Unmute" : "Mute");
    }
    
    int height = component != nullptr && ! isFundamental ? 33 : 35;
    int width = component != nullptr && ! isFundamental ? height * 2 - 12 : height * 1 - 3;
    
    partialOptions.setSize (width, height);
    partialOptions.setTopLeftPosition (component != nullptr && ! isFundamental
                                       ? getLocalPoint (component, Point<int> (component->getWidth(), -3))
                                       : getLocalPoint (&distributionPanel, Point<int> (distributionPanel.getWidth(), 48)));
    
    Desktop::getInstance().getAnimator().fadeIn (&partialOptions, 250);
}

// Hide the options components on click
void DissCalcView::mouseDown (const MouseEvent& event)
{
    if (distributionOptions.isVisible()
        && event.eventComponent != &distributionOptions.duplicateButton
        && event.eventComponent != &distributionOptions.setXAxisButton
        && event.eventComponent != &distributionOptions.setFromSavedButton
        && event.eventComponent != &distributionOptions.saveButton
        && event.eventComponent != &distributionOptions.removeButton
        && event.eventComponent != &distributionOptions.muteButton
        && event.eventComponent != &distributionOptions.distribution->optionsButton)
    {
        distributionOptions.setVisible (false);
        distributionOptions.distribution->optionsButton.setToggleState (false, dontSendNotification);
    }
    
    if (partialOptions.isVisible()
        && event.eventComponent != &partialOptions.removeButton
        && event.eventComponent != &partialOptions.muteButton
        && event.eventComponent != &partialOptions.partial->optionsButton)
    {
        partialOptions.partial = nullptr;
        partialOptions.setVisible (false);
    }
}
