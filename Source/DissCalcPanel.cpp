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
#include "DissCalcPanel.h"
#include "DissCalcView.h"

//==============================================================================
DistributionOptions::DistributionOptions()
{
    duplicateButton.setButtonText ("Duplicate");
    setXAxisButton.setButtonText ("Set as X-Axis");
    setFromSavedButton.setButtonText ("Set Timbre");
    saveButton.setButtonText ("Save Timbre");
    removeButton.setButtonText ("Remove");
    
    duplicateButton.setBorders (true, true, true, true, 2);
    setXAxisButton.setBorders (true, false, true, true, 2);
    setFromSavedButton.setBorders (true, false, true, true, 2);
    saveButton.setBorders (true, false, true, true, 2);
    removeButton.setBorders (true, false, true, true, 2);

    addAndMakeVisible (duplicateButton);
    addAndMakeVisible (setXAxisButton);
    addAndMakeVisible (setFromSavedButton);
    addAndMakeVisible (saveButton);
    addAndMakeVisible (removeButton);
    
    distribution = nullptr;
}

DistributionOptions::~DistributionOptions()
{
    distribution = nullptr;
}

void DistributionOptions::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
}

void DistributionOptions::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (1);
    int buttonHeight = getHeight() / 5;
    
    duplicateButton.setBounds (area.removeFromTop (buttonHeight));
    setXAxisButton.setBounds (area.removeFromTop (buttonHeight));
    setFromSavedButton.setBounds (area.removeFromTop (buttonHeight));
    saveButton.setBounds (area.removeFromTop (buttonHeight));
    removeButton.setBounds (area);
}

//==============================================================================
DistributionComponent::DistributionComponent()   : distribution (IDs::OvertoneDistribution)
{
    optionsButton.setButtonText ("-");
    addAndMakeVisible (optionsButton);
    optionsButton.addListener (this);
    optionsButton.setBorders (true, false, true, true, 2);
    
    muteButton.setButtonText ("M");
    muteButton.setTooltip ("Mute");
    addAndMakeVisible (muteButton);
    muteButton.addListener (this);
    muteButton.setBorders (true, false, true, true, 2);
    
    distribution.addListener (this);
}

DistributionComponent::~DistributionComponent()
{
}

void DistributionComponent::paint (Graphics& g)
{
    // Highlight distribution if being viewed via DistributionPanel
    g.fillAll (distribution[IDs::IsViewed]
               ? Theme::buttonHighlighted
               : Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (0, getHeight(), getWidth(), getHeight(), 2);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);

    // Draw distribution name
    g.setColour (Theme::text);
    g.setFont (16.0f);
    g.drawText (distribution.getProperty (IDs::Name, "Untitled"),
                getLocalBounds().removeFromLeft (getWidth() - getHeight()).withX (getHeight() / 2),
                Justification::centredLeft, true);
    
    muteButton.setEnabled (distribution[IDs::XAxis] ? false : true);
    muteButton.setButtonText (distribution[IDs::XAxis] ? "X" : "M");
}

void DistributionComponent::resized()
{
    auto area = getLocalBounds();
    auto buttonArea = area.removeFromRight (area.getHeight() / 2);
    
    optionsButton.setBounds (buttonArea.removeFromTop (buttonArea.getHeight() / 2));
    muteButton.setBounds (buttonArea);
}

void DistributionComponent::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &muteButton)
    {
        bool muteOn = distribution.getProperty (IDs::Mute, false);
        
        distribution.setProperty (IDs::Mute, muteOn ? false : true, nullptr);
    }
    else if (clickedButton == &optionsButton)
    {
        findParentComponentOfClass<DissCalcPanel>()->openOptions (this);
    }
}

void DistributionComponent::mouseDown (const MouseEvent& event)
{
    // Open the distribution panel and set it with the clicked distribution component's data
    if (event.eventComponent == this)
    {
        if (findParentComponentOfClass<DissCalcView>()->distributionPanelIsOpen()
            && distribution == findParentComponentOfClass<DissCalcView>()->getDistributionPanelData())
            findParentComponentOfClass<DissCalcView>()->closeDistributionPanel();
        else
            findParentComponentOfClass<DissCalcView>()->openDistributionPanel (distribution);
    }
}

void DistributionComponent::setDistributionData (ValueTree& newDistribution)
{
    if (newDistribution.hasType (IDs::OvertoneDistribution))
    {
        distribution = newDistribution;
    }
}

ValueTree& DistributionComponent::getDistributionData()
{
    return distribution;
}

void DistributionComponent::valueTreePropertyChanged (ValueTree& parent,
                                                      const Identifier& ID)
{
    if (parent == distribution && ID == IDs::Mute)
    {
        jassert (distribution.getProperty (IDs::Mute).isBool());
        
        muteButton.setToggleState (distribution[IDs::Mute] ? true : false,
                                   dontSendNotification);
    }
    else if (parent == distribution && ID == IDs::IsViewed)
    {
        repaint();
    }
    else if (parent == distribution && ID == IDs::Name)
    {
        repaint();
    }
    else  if (parent == distribution && ID == IDs::XAxis)
    {
        /*
            Disable the mute button for the distribution that traverses the x-axis.
        
            If muted, all other distributions would have a static frequency,
            and the map would just be a horizontal line.
        */
        muteButton.setEnabled (distribution[IDs::XAxis] ? false : true);
        muteButton.setButtonText (distribution[IDs::XAxis] ? "X" : "M");
    }
}

//==============================================================================
DistributionList::DistributionList() :   distributions (IDs::Calculator)
{
    distributions.addListener (this);
    
    distributionHeight = 30;
}

DistributionList::~DistributionList()
{
}

void DistributionList::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
    g.drawLine (0, getHeight(), getWidth(), getHeight(), 2);
}

void DistributionList::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    for (auto* component : distributionComponents)
    {
        component->setBounds (area.removeFromTop (distributionHeight));
    }
}

void DistributionList::setCalcData (ValueTree& data)
{
    distributions = data;
}

ValueTree& DistributionList::getCalcData()
{
    return distributions;
}

void DistributionList::valueTreeChildAdded (ValueTree& parent, ValueTree& newChild)
{
    if (parent == distributions && newChild.hasType (IDs::OvertoneDistribution))
    {
        // Create distribution component and set its data as the added valuetree
        distributionComponents.add (new DistributionComponent());
        distributionComponents.getLast()->setDistributionData (newChild);
        addAndMakeVisible (distributionComponents.getLast());
        
        newChild.addListener (this);
        
        setSize (getWidth(), idealHeight());
        
        // Ensures that some distribution is always set as the x-axis/variable distribution
        if (distributionComponents.size() == 1)
        {
            newChild.setProperty (IDs::XAxis, true, nullptr);
        }
        else if (newChild[IDs::XAxis].operator bool()
                 && parent.getChildWithProperty (IDs::XAxis, true) != newChild)
        {
            parent.getChildWithProperty (IDs::XAxis, true).setProperty (IDs::XAxis,
                                                                        false, nullptr);
        }
        
        // Ensures that fundamental freqs/amps aren't initialized with illegal values
        if (newChild[IDs::FundamentalAmp].operator float() <= 0
            || newChild[IDs::FundamentalFreq].operator float() <= 0)
            newChild.setProperty (IDs::FundamentalFreq, 1, nullptr);
            newChild.setProperty (IDs::FundamentalAmp, 1, nullptr);
    }
}

void DistributionList::valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex)
{
    if (parent == distributions && removedChild.hasType (IDs::OvertoneDistribution))
    {
        distributionComponents.remove (childIndex);
        
        setSize (getWidth(), idealHeight());
    }
}

void DistributionList::valueTreeChildOrderChanged (ValueTree& parent, int oldIndex, int newIndex)
{
    if (parent == distributions)
    {
        distributionComponents.move (oldIndex, newIndex);
        
        resized();
    }
}

int DistributionList::idealHeight()
{
    // For resizing the viewport containing this DistributionList
    return distributionComponents.size() * distributionHeight;
}

//==============================================================================
DissCalc::DissCalc()
{
    view.setViewedComponent (&distributionList, false);
    view.setScrollBarsShown (false, false, true, false);
    addAndMakeVisible (view);
    
    addDistributionButton.setButtonText ("+");
    addAndMakeVisible (addDistributionButton);
    addDistributionButton.addListener (this);
    addDistributionButton.setBorders (false, false, true, true, 2);
    addDistributionButton.setFontSize (21);
    
    optionsButton.setButtonText ("-");
    addAndMakeVisible (optionsButton);
    optionsButton.setFontSize (21);
    optionsButton.addListener (this);
    optionsButton.setBorders (true, false, true, true, 2);
}

DissCalc::~DissCalc()
{
}

void DissCalc::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (0, 25, getWidth(), 25, 2);
    g.drawLine (0, getHeight(), getWidth(), getHeight(), 2);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
}

void DissCalc::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> header = area.removeFromTop (25);
     
    addDistributionButton.setBounds (header.removeFromLeft (header.getHeight()));
    optionsButton.setBounds (header.removeFromRight (header.getHeight()));
    
    view.setBounds (area);
    
    // Only resize the distribution list if it is larger than the viewport
    if (distributionList.getHeight() < area.getHeight())
        distributionList.setBounds (area);
}

void DissCalc::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &addDistributionButton)
    {
        findParentComponentOfClass<DissCalcPanel>()->undo->beginNewTransaction();
        
        distributionList.distributions.appendChild (ValueTree (IDs::OvertoneDistribution),
                                                    findParentComponentOfClass<DissCalcPanel>()->undo);
    }
    else if (clickedButton == &optionsButton)
    {
        findParentComponentOfClass<DissCalcPanel>()->openOptions (this);
    }
}

void DissCalc::setCalcData (ValueTree& distributions)
{
    if (distributions.hasType (IDs::Calculator))
        distributionList.setCalcData (distributions);
}

ValueTree& DissCalc::getCalcData()
{
    return distributionList.getCalcData();
}

//==============================================================================
CalcList::CalcList()   : dissonanceCalcs (IDs::CalculatorList)
{
    dissonanceCalcs.addListener (this);
    
    calcHeight = 175;
}

CalcList::~CalcList()
{
}

void CalcList::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
}

void CalcList::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    for (auto* calc : calcs)
    {
        calc->setBounds (area.removeFromTop (calcHeight));
    }
}

void CalcList::setCalcHeight (int newHeight)
{
    calcHeight = newHeight;
    
    setSize (getWidth(), calcHeight * calcs.size());
}

int CalcList::getCalcHeight()
{
    return calcHeight;
}

void CalcList::valueTreeChildAdded (ValueTree& parent, ValueTree& newChild)
{
    if (parent == dissonanceCalcs
        && newChild.hasType (IDs::Calculator))
    {
        calcs.add (new DissCalc());
        calcs.getLast()->setCalcData (newChild);
        addAndMakeVisible (calcs.getLast());
        
        setSize (getWidth(), calcs.size() * calcHeight);
    }
}

void CalcList::valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex)
{
    if (parent == dissonanceCalcs)
    {
        calcs.remove (childIndex);
        setSize (getWidth(), calcs.size() * calcHeight);
    }
}

//==============================================================================
DissCalcPanel::DissCalcPanel()
{
    view.setViewedComponent (&calcs, false);
    addAndMakeVisible (&view);
    view.setScrollBarsShown (false, false);
    
    addCalcButton.setButtonText ("+");
    addAndMakeVisible (&addCalcButton);
    addCalcButton.addListener (this);
    addCalcButton.setBorders (false, true, true, false, 2);
    addCalcButton.setFontSize (26);
    
    addChildComponent (options);
    options.setSize (100, 125);
    options.duplicateButton.addListener (this);
    options.setXAxisButton.addListener (this);
    options.setFromSavedButton.addListener (this);
    options.saveButton.addListener (this);
    options.removeButton.addListener (this);
    
    addChildComponent (calcOptions);
    calcOptions.setSize (100, 50);
    calcOptions.duplicateButton.addListener (this);
    calcOptions.removeButton.addListener (this);
    
    addMouseListener (this, true);
}

DissCalcPanel::~DissCalcPanel()
{
}

void DissCalcPanel::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
    g.drawLine (0, getHeight() - 35, getWidth(), getHeight() - 35);
}

void DissCalcPanel::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> footer = area.removeFromBottom (35);
    
    view.setBounds (area.withHeight (area.getHeight() - 1));
    
    if (calcs.getHeight() < area.getHeight() - 1)
        calcs.setBounds (area.withHeight (area.getHeight() - 1));
    
    addCalcButton.setBounds (footer.removeFromLeft (35));
}

void DissCalcPanel::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &addCalcButton)
    {
        undo->beginNewTransaction();
        
        calcs.dissonanceCalcs.appendChild (ValueTree (IDs::Calculator), undo);
        
        // Init the new dissonance calc with a distribution
        ValueTree newCalc = calcs.dissonanceCalcs.getChild (calcs.dissonanceCalcs.getNumChildren() - 1);
        newCalc.appendChild (ValueTree (IDs::OvertoneDistribution), undo);
        newCalc.getChild (0).setProperty (IDs::FundamentalFreq, 1, undo);
        newCalc.getChild (0).setProperty (IDs::FundamentalAmp, 1, undo);
    }
    else if (clickedButton == &options.duplicateButton)
    {
        ValueTree tree = options.distribution->getDistributionData().createCopy();
        ValueTree parent = options.distribution->distribution.getParent();
        
        // Reset these properties, if used by the distribution being copied
        if (tree[IDs::XAxis])
            tree.setProperty (IDs::XAxis, false, nullptr);
        
        if (tree[IDs::Mute])
            tree.setProperty (IDs::Mute, false, nullptr);
        
        if (tree[IDs::IsViewed])
            tree.setProperty (IDs::IsViewed, false, nullptr);
        
        // Don't copy any partials with incomplete data
        for (auto child : tree)
        {
            if (child.hasType (IDs::Partial)
                && (child[IDs::Freq].operator float() <= 0
                    || child[IDs::Amp].operator float() <= 0))
            {
                tree.removeChild (child, nullptr);
            }
        }

        undo->beginNewTransaction();
        parent.appendChild (tree, undo);
        
        options.setVisible (false);
        options.distribution->optionsButton.setToggleState (false, dontSendNotification);
    }
    else if (clickedButton == &options.setXAxisButton)
    {
        ValueTree parent = options.distribution->distribution.getParent();
        
        undo->beginNewTransaction();
        parent.getChildWithProperty (IDs::XAxis, true).setProperty (IDs::XAxis, false, undo);
        options.distribution->distribution.setProperty (IDs::XAxis, true, undo);
        
        options.setVisible (false);
        options.distribution->optionsButton.setToggleState (false, dontSendNotification);
        
        DissCalcView& view = *findParentComponentOfClass<DissCalcView>();
        
        // Update the distribution panel if the viewed distribution is the new x-axis distribution
        if (view.distributionPanelIsOpen())
        {
            if (options.distribution->distribution.getProperty (IDs::IsViewed, false))
                view.openDistributionPanel (options.distribution->distribution);
            else if (options.distribution->distribution != view.getDistributionPanelData())
                view.openDistributionPanel (view.getDistributionPanelData());
        }
    }
    else if (clickedButton == &options.setFromSavedButton)
    {
        DissCalcView* view = findParentComponentOfClass<DissCalcView>();
        
        view->savedDistributionList.show (options.distribution->distribution);
        view->closeDistributionPanel();
        
        options.setVisible (false);
        options.distribution->optionsButton.setToggleState (false, dontSendNotification);
    }
    else if (clickedButton == &options.saveButton)
    {
        DissCalcView* view = findParentComponentOfClass<DissCalcView>();
        
        view->saveDistribution.show (options.distribution->distribution);
        
        options.setVisible (false);
        options.distribution->optionsButton.setToggleState (false, dontSendNotification);
    }
    else if (clickedButton == &options.removeButton)
    {
        ValueTree parent = options.distribution->distribution.getParent();
        
        if (options.distribution->distribution.getProperty (IDs::IsViewed, false))
            findParentComponentOfClass<DissCalcView>()->closeDistributionPanel();
        
        undo->beginNewTransaction();
        parent.removeChild (options.distribution->distribution, undo);
        
        // Sets the first distribution as the x-axis distribution,
        // if the x-axis distribution is being removed.
        if (parent.getNumChildren() > 0
            && ! parent.getChildWithProperty (IDs::XAxis, true).isValid())
            parent.getChild (0).setProperty (IDs::XAxis, true, undo);
        
        options.setVisible (false);
        options.distribution = nullptr;
    }
    else if (clickedButton == &calcOptions.duplicateButton)
    {
        ValueTree tree = ValueTree (IDs::Calculator);
        ValueTree parent = calcOptions.calc->getCalcData().getParent();
        ValueTree treeToCopy = calcOptions.calc->getCalcData();
        
        parent.appendChild (tree, nullptr);
        tree.copyPropertiesAndChildrenFrom (treeToCopy, nullptr);
        
        calcOptions.setVisible (false);
        calcOptions.calc->optionsButton.setToggleState (false, dontSendNotification);
    }
    else if (clickedButton == &calcOptions.removeButton)
    {
        ValueTree calc = calcOptions.calc->getCalcData();

        undo->beginNewTransaction();
        calc.removeAllChildren (undo);
        calc.removeAllProperties (undo);
        calc.getParent().removeChild (calc, undo);
        
        calcOptions.setVisible (false);
        calcOptions.calc = nullptr;
    }
}

// Hide the options components on click
void DissCalcPanel::mouseDown (const MouseEvent& event)
{
    if (options.isVisible()
        && event.eventComponent != &options.duplicateButton
        && event.eventComponent != &options.setXAxisButton
        && event.eventComponent != &options.setFromSavedButton
        && event.eventComponent != &options.saveButton
        && event.eventComponent != &options.removeButton
        && event.eventComponent != &options.distribution->optionsButton)
    {
        options.setVisible (false);
        options.distribution->optionsButton.setToggleState (false, dontSendNotification);
    }
    
    if (calcOptions.isVisible()
        && event.eventComponent != &calcOptions.duplicateButton
        && event.eventComponent != &calcOptions.removeButton
        && event.eventComponent != &calcOptions.calc->optionsButton)
    {
        calcOptions.setVisible (false);
        calcOptions.calc->optionsButton.setToggleState (false, dontSendNotification);
    }
}

void DissCalcPanel::setCalcData (ValueTree& calcData)
{
    calcs.dissonanceCalcs = calcData;
    addCalcButton.triggerClick();
}

void DissCalcPanel::openOptions (DistributionComponent* component)
{
    // Resets the previously-clicked DistributionComponent::optionsButton if active
    if (options.distribution != nullptr
        && options.distribution->optionsButton.getToggleState())
        options.distribution->optionsButton.setToggleState (false, dontSendNotification);
    
    // Close options if the same options buttons was pushed
    if (component == options.distribution && options.isVisible())
    {
        options.setVisible (false);
        return;
    }
    
    // Enable or disable XAxis button, if needed
    if (component->getDistributionData().getProperty (IDs::XAxis, false))
    {
        options.setXAxisButton.setEnabled (false);
        options.setXAxisButton.setTooltip (String ("This timbre is already set to have a variable fundamental ")
                                           + String ("frequency represented by the x-axis."));
    }
    else if (! options.setXAxisButton.isEnabled())
    {
        options.setXAxisButton.setEnabled (true);
        options.setXAxisButton.setTooltip (String ("Sets this timbre to have a variable fundamental ")
                                           + String ("frequency represented by the x-axis."));
    }
    
    // Setup options
    options.distribution = component;
    options.setAlpha (0);
    options.setVisible (true);
    
    Point<int> topRight = getLocalPoint (&component->muteButton,
                                         Point<int> (component->muteButton.getWidth(), 0));
    
    if (topRight.getY() <= getBottom() - 100)
        options.setTopRightPosition (topRight.getX() - 1, topRight.getY());
    else
        options.setTopRightPosition (topRight.getX() - 1,
                                     topRight.getY() - options.getHeight() - 1);
    
    Desktop::getInstance().getAnimator().fadeIn (&options, 250);
    
    component->optionsButton.setToggleState (true, dontSendNotification);
}

void DissCalcPanel::openOptions(DissCalc* calc)
{
    // Resets the previously-clicked DissCalc::optionsButton if active
    if (calcOptions.calc != nullptr
        && calcOptions.calc->optionsButton.getToggleState())
        calcOptions.calc->optionsButton.setToggleState (false, dontSendNotification);
    
    // Close options if the same options buttons was pushed
    if (calc == calcOptions.calc && calcOptions.isVisible())
    {
        calcOptions.setVisible (false);
        return;
    }
    
    // Setup options
    calcOptions.calc = calc;
    calcOptions.setAlpha (0);
    calcOptions.setVisible (true);
    
    Point<int> topRight = getLocalPoint (&calc->optionsButton,
                                         Point<int> (calc->optionsButton.getWidth(),
                                                     calc->optionsButton.getHeight()));
    
    if (topRight.getY() <= getBottom() - 50)
        calcOptions.setTopRightPosition (topRight.getX() - 1, topRight.getY());
    else
        calcOptions.setTopRightPosition (topRight.getX() - 1,
                                         topRight.getY() - calcOptions.getHeight() - 1);
    
    Desktop::getInstance().getAnimator().fadeIn (&calcOptions, 250);
    
    calc->optionsButton.setToggleState (true, dontSendNotification);
}
