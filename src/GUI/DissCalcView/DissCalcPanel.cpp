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
    muteButton.setButtonText ("M");
    duplicateButton.setIcon (true, FontAwesome_Clone);
    setXAxisButton.setIcon (true, FontAwesome_Times);
    setFromSavedButton.setIcon (true, FontAwesome_FolderOpenO);
    saveButton.setIcon (true, FontAwesome_Save);
    removeButton.setIcon (true, FontAwesome_TrashO);
    
    muteButton.setTooltip ("Mute");
    duplicateButton.setTooltip ("Clone");
    setXAxisButton.setTooltip ("Set as the x-axis variable");
    setFromSavedButton.setTooltip ("Open saved");
    saveButton.setTooltip ("Save");
    removeButton.setTooltip ("Remove");
    
    setXAxisButton.setIconSize (15);
    muteButton.setFontSize (16);
    removeButton.setIconSize (16);
    saveButton.setIconSize (16);
    
    muteButton.setBorders (true, true, true, true, 3);
    duplicateButton.setBorders (true, true, true, true, 3);
    setXAxisButton.setBorders (true, true, true, true, 3);
    setFromSavedButton.setBorders (true, true, true, true, 3);
    saveButton.setBorders (true, true, true, true, 3);
    removeButton.setBorders (true, true, true, true, 3);

    addAndMakeVisible (muteButton);
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
    
    g.setColour (Theme::headerBackground);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 6);
    g.drawLine (0, 0, getWidth(), 0, 6);
    g.drawLine (0, getHeight(), getWidth(), getHeight(), 6);
}

void DistributionOptions::resized()
{
    Rectangle<int> area = getLocalBounds();
    int xDisplacement = 0;
    
    if (! distribution->getDistributionData()[IDs::XAxis])
    {
        muteButton.setVisible (true);
        muteButton.setBounds (area.removeFromLeft (getHeight() - 6)
                              .reduced (3, 6)
                              .withY (6));
        muteButton.setBounds (muteButton.getBounds().withX (muteButton.getX() - 3));
        
        setXAxisButton.setVisible (true);
        setXAxisButton.setBounds (area.removeFromLeft (getHeight() - 6)
                                  .reduced (3, 6)
                                  .withY (6));
        setXAxisButton.setBounds (setXAxisButton.getBounds().withX (setXAxisButton.getX() - 6));
        
        xDisplacement = 6;
    }
    else
    {
        muteButton.setVisible (false);
        setXAxisButton.setVisible (false);
    }
    
    duplicateButton.setBounds (area.removeFromLeft (getHeight() - 6)
                               .reduced (3, 6)
                               .withY (6));
    duplicateButton.setBounds (duplicateButton.getBounds().withX (duplicateButton.getX() - 3 - xDisplacement));

    setFromSavedButton.setBounds (area.removeFromLeft (getHeight() - 6)
                                  .reduced (3, 6)
                                  .withY (6));
    setFromSavedButton.setBounds (setFromSavedButton.getBounds().withX (setFromSavedButton.getX() - 6 - xDisplacement));

    saveButton.setBounds (area.removeFromLeft (getHeight() - 6)
                          .reduced (3, 6)
                          .withY (6));
    saveButton.setBounds (saveButton.getBounds().withX (saveButton.getX() - 9 - xDisplacement));

    
    removeButton.setBounds (Rectangle<int> (getHeight(), getHeight())
                            .reduced (6, 6)
                            .withY (6)
                            .withRightX (getWidth() - 6));
}


//==============================================================================
DistributionComponent::DistributionComponent()   : distribution (IDs::OvertoneDistribution)
{
    optionsButton.setIcon (true, FontAwesome_Bars);
    addChildAndSetID (&optionsButton, "options");
    optionsButton.setVisible (true);
    optionsButton.addListener (this);
    optionsButton.setBorders (true, true, true, true, 3);
    
    distribution.addListener (this);
}

DistributionComponent::~DistributionComponent()
{
}

void DistributionComponent::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);

    if (isMouseOver())
    {
        g.setColour(Theme::buttonHighlighted);
        g.fillRect(getLocalBounds().reduced (4, 3).removeFromLeft (getWidth() - 32).withY (2));
    }
    else if (distribution[IDs::IsViewed])
    {
        g.setColour (Theme::headerBackground);
        g.fillRect(getLocalBounds().reduced (4, 3).removeFromLeft (getWidth() - 32).withY (2));
    }
    
    // Draws a line between distributions
    if (distribution.getParent().indexOf (distribution) + 1 != distribution.getParent().getNumChildren())
    {
        g.setColour (Theme::headerBackground);
        g.drawLine (7, getHeight(), getWidth() - 7, getHeight(), 4);
    }

    // Draw distribution name
    g.setColour (distribution[IDs::IsViewed] || isMouseOver() ? Theme::mainBackground : Theme::text);
    Font f = g.getCurrentFont();
    f.setHeight (16.f);
    f.setBold (true);
    g.setFont (f);
    g.drawText (distribution.getProperty (IDs::Name, "Untitled"),
                getLocalBounds().removeFromLeft (getWidth() - getHeight()).withX (getHeight() / 2),
                Justification::centredLeft, true);
    
    f.setHeight (14.f);
    f.setBold (true);
    g.setFont (f);
    
    if (distribution[IDs::Mute])
    {
        g.drawText ("M",
                    getLocalBounds().removeFromRight (getHeight() + 11).withY (-1),
                    Justification::centredLeft, true);
    }
    else if (distribution[IDs::XAxis])
    {
        g.drawText ("X",
                    getLocalBounds().removeFromRight (getHeight() + 9).withY (-1),
                    Justification::centredLeft, true);
    }
}

void DistributionComponent::resized()
{
    optionsButton.setBounds (getLocalBounds().removeFromRight (getHeight())
                             .reduced (4)
                             .withRightX (getWidth() - 3)
                             .withY (3));
}

void DistributionComponent::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &optionsButton)
    {
        findParentComponentOfClass<DissCalcView>()->openOptions (this);
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

void DistributionComponent::mouseMove (const MouseEvent& event)
{
    if (event.eventComponent == this
        && getLocalBounds().reduced (4, 3).removeFromLeft (getWidth() - 32).withY (2).contains (event.getMouseDownPosition()))
    {
        repaint();
    }
}

void DistributionComponent::mouseExit (const MouseEvent& event)
{
    if (event.eventComponent == this)
    {
        repaint();
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
    if (parent == distribution)
    {
        if (ID == IDs::Name || ID == IDs::IsViewed || ID == IDs::Mute || ID == IDs::XAxis)
            repaint();
        
        if (ID == IDs::XAxis && parent[IDs::Mute])
            parent.setProperty (IDs::Mute, false, nullptr);
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
        distributionComponents.insert (parent.indexOf (newChild), new DistributionComponent());
        distributionComponents[parent.indexOf (newChild)]->setDistributionData (newChild);
        addAndMakeVisible (distributionComponents[parent.indexOf (newChild)]);
        
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
        
        if (removedChild[IDs::IsViewed])
            findParentComponentOfClass<DissCalcView>()->closeDistributionPanel();
        
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
void CalcViewport::visibleAreaChanged (const Rectangle<int>& newVisibleArea)
{
    DistributionOptions* options = findParentComponentOfClass<DissCalcPanel>()->options;
    
    if (options->isVisible())
    {
        options->setTopLeftPosition (options->getX(),
                                     options->distribution->getScreenY() - getTopLevelComponent()->getScreenY() - 3);
        
        if (! newVisibleArea.contains (options->distribution->getBoundsInParent()))
        {
            options->setVisible (false);
            
            if (options->distribution->findChildWithID ("options") != nullptr)
                options->distribution->optionsButton.setToggleState (false, dontSendNotification);
            
            options->distribution = nullptr;
        }
    }
}

//==============================================================================
DissCalc::DissCalc()
{
    view.setViewedComponent (&distributionList, false);
    view.setScrollBarsShown (false, false, true, false);
    addAndMakeVisible (view);
    
    addDistributionButton.setIcon (true, FontAwesome_Plus);
    addAndMakeVisible (addDistributionButton);
    addDistributionButton.addListener (this);
    addDistributionButton.setIconSize (13);
    addDistributionButton.setTooltip ("Add a timbre");

    copyButton.setIcon (true, FontAwesome_Clone);
    addAndMakeVisible (copyButton);
    copyButton.addListener (this);
    copyButton.setIconSize (12);
    copyButton.setTooltip ("Clone");
    
    removeButton.setIcon (true, FontAwesome_TrashO);
    addAndMakeVisible (removeButton);
    removeButton.addListener (this);
    removeButton.setIconSize (15);
    removeButton.setTooltip ("Remove");

    logButton.setIcon (true, FontAwesome_LongArrowUp, -0.25);
    addAndMakeVisible (logButton);
    logButton.addListener (this);
    logButton.setIconSize (16);
    logButton.setTooltip ("Use logarithmic frequency range\n(X-axis, currently linear)");

    lockScaleButton.setIcon (true, FontAwesome_Unlock);
    addAndMakeVisible (lockScaleButton);
    lockScaleButton.addListener (this);
    lockScaleButton.setIconSize (15);
    lockScaleButton.setTooltip ("Lock the dissonance scale (Y-axis");
}

DissCalc::~DissCalc()
{
}

void DissCalc::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::headerBackground);
    g.fillRect (getLocalBounds().removeFromTop (25));
    
    logButton.setIcon (true,
                       getCalcData()[IDs::LogSteps] ? FontAwesome_Share : FontAwesome_LongArrowUp,
                       getCalcData()[IDs::LogSteps] ? 0 : -0.25);
    
    logButton.setTooltip (getCalcData()[IDs::LogSteps]
                          ? "Use linear frequency range\n(X-axis, currently logarithmic)"
                          : "Use logarithmic frequency range\n(X-axis, currently linear)");
    
    logButton.setIconSize (getCalcData()[IDs::LogSteps] ? 14 : 16);
    
    lockScaleButton.setIcon (true, getCalcData()[IDs::ScaleLocked] ? FontAwesome_Lock : FontAwesome_Unlock);
    lockScaleButton.setTooltip (getCalcData()[IDs::ScaleLocked]
                                ? "Unlock the dissonance scale (Y-axis)"
                                : "Lock the dissonance scale (Y-axis)");
}

void DissCalc::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> header = area.removeFromTop (25);
    
    int height = header.getHeight();
     
    removeButton.setBounds (header.removeFromLeft (height).reduced (3));
    
    addDistributionButton.setBounds (header.removeFromRight (height)
                                     .reduced (3)
                                     .withRightX (header.getWidth() + (height * 2)));
    
    copyButton.setBounds (header.removeFromRight (height)
                          .reduced (3)
                          .withRightX (header.getWidth() + (height * 2) + 3));
    
    lockScaleButton.setBounds (header.removeFromRight (height)
                               .reduced (3)
                               .withRightX (header.getWidth() + (height * 2) + 6));
    
    logButton.setBounds (header.removeFromRight (height)
                         .reduced (3)
                         .withRightX (header.getWidth() + (height * 2) + 9));
    
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
    else if (clickedButton == &copyButton)
    {
        ValueTree tree = ValueTree (IDs::Calculator);
        getCalcData().getParent().appendChild (tree, nullptr);
        tree.copyPropertiesAndChildrenFrom (getCalcData(), nullptr);
    }
    else if (clickedButton == &removeButton)
    {
        DissCalcPanel* panel = findParentComponentOfClass<DissCalcPanel>();
        ValueTree calc = getCalcData();
        UndoManager* undo = panel->undo;
        
        undo->beginNewTransaction();
        calc.removeAllChildren (undo);
        calc.removeAllProperties (undo);
        calc.getParent().removeChild (calc, undo);
        
        panel->options->setVisible (false);
        panel->options->distribution = nullptr;
    }
    else if (clickedButton == &lockScaleButton)
    {
        getCalcData().setProperty (IDs::ScaleLocked, ! getCalcData()[IDs::ScaleLocked], nullptr);
        
        if (! getCalcData()[IDs::ScaleLocked])
        {
            getCalcData().removeProperty (IDs::ScaleMax, nullptr);
            getCalcData().removeProperty (IDs::ScaleMin, nullptr);
        }
        
        lockScaleButton.setIcon (true, getCalcData()[IDs::ScaleLocked] ? FontAwesome_Lock : FontAwesome_Unlock);
        lockScaleButton.setTooltip (getCalcData()[IDs::ScaleLocked]
                                    ? "Unlock the dissonance scale (Y-axis)"
                                    : "Lock the dissonance scale (Y-axis)");
    }
    else if (clickedButton == &logButton)
    {
        getCalcData().setProperty (IDs::LogSteps, ! getCalcData()[IDs::LogSteps], nullptr);
        
        logButton.setIcon (true,
                           getCalcData()[IDs::LogSteps] ? FontAwesome_Share : FontAwesome_LongArrowUp,
                           getCalcData()[IDs::LogSteps] ? 0 : -0.25);
        
        logButton.setTooltip (getCalcData()[IDs::LogSteps]
                              ? "Use linear frequency range\n(X-axis, currently logarithmic)"
                              : "Use logarithmic frequency range\n(X-axis, currently linear)");
        
        logButton.setIconSize (getCalcData()[IDs::LogSteps] ? 14 : 16);
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
    g.fillAll (Theme::accentBackground);
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
        calcs.insert(parent.indexOf (newChild), new DissCalc());
        calcs[parent.indexOf (newChild)]->setCalcData (newChild);
        addAndMakeVisible (calcs[parent.indexOf (newChild)]);
        
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
    
    addCalcButton.setIcon (true, FontAwesome_Plus);
    addAndMakeVisible (&addCalcButton);
    addCalcButton.addListener (this);
    addCalcButton.setIconSize (20);
    
    addMouseListener (this, true);
}

DissCalcPanel::~DissCalcPanel()
{
}

void DissCalcPanel::paint (Graphics& g)
{
    g.fillAll (Theme::accentBackground);
    g.setColour (Theme::headerBackground);
    g.fillRect (getLocalBounds().removeFromBottom (35));
}

void DissCalcPanel::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> footer = area.removeFromBottom (35);
    
    view.setBounds (area.withHeight (area.getHeight() - 1));
    
    if (calcs.getHeight() < area.getHeight() - 1)
        calcs.setBounds (area.withHeight (area.getHeight() - 1));
    
    addCalcButton.setBounds (footer.removeFromLeft (35).reduced (3));
}

void DissCalcPanel::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &addCalcButton)
    {
        undo->beginNewTransaction();
        ValueTree newCalc (IDs::Calculator);
        calcs.dissonanceCalcs.appendChild (newCalc, undo);
        
        // Init the new dissonance calc with a distribution
        newCalc.appendChild (ValueTree (IDs::OvertoneDistribution), undo);
        newCalc.getChild (0).setProperty (IDs::FundamentalFreq, 1, undo);
        newCalc.getChild (0).setProperty (IDs::FundamentalAmp, 1, undo);
        newCalc.setProperty (IDs::LogSteps, false, undo);
        newCalc.setProperty (IDs::ScaleLocked, false, undo);
        newCalc.setProperty (IDs::Mute, false, undo);
    }
    else if (clickedButton == &options->muteButton)
    {
        ValueTree distribution = options->distribution->getDistributionData();
        
        undo->beginNewTransaction();
        distribution.setProperty (IDs::Mute, ! distribution[IDs::Mute], undo);
        options->muteButton.setToggleState (distribution[IDs::Mute], dontSendNotification);
        
        options->setVisible (false);
        options->distribution->optionsButton.setToggleState (false, dontSendNotification);
        options->distribution = nullptr;
    }
    else if (clickedButton == &options->duplicateButton)
    {
        ValueTree tree = options->distribution->getDistributionData().createCopy();
        ValueTree parent = options->distribution->distribution.getParent();
        
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
        
        options->setVisible (false);
        options->distribution->optionsButton.setToggleState (false, dontSendNotification);
        options->distribution = nullptr;
    }
    else if (clickedButton == &options->setXAxisButton)
    {
        ValueTree parent = options->distribution->distribution.getParent();
        
        undo->beginNewTransaction();
        parent.getChildWithProperty (IDs::XAxis, true).setProperty (IDs::XAxis, false, undo);
        options->distribution->distribution.setProperty (IDs::XAxis, true, undo);
        
        options->setVisible (false);
        options->distribution->optionsButton.setToggleState (false, dontSendNotification);
        
        DissCalcView& view = *findParentComponentOfClass<DissCalcView>();
        
        // Update the distribution panel if the viewed distribution is the new x-axis distribution
        if (view.distributionPanelIsOpen())
        {
            if (options->distribution->distribution.getProperty (IDs::IsViewed, false))
                view.openDistributionPanel (options->distribution->distribution);
            else if (options->distribution->distribution != view.getDistributionPanelData())
                view.openDistributionPanel (view.getDistributionPanelData());
        }
        
        options->distribution = nullptr;
    }
    else if (clickedButton == &options->setFromSavedButton)
    {
        DissCalcView* view = findParentComponentOfClass<DissCalcView>();
        
        view->savedDistributionList.show (options->distribution->distribution);
        view->closeDistributionPanel();
        
        options->setVisible (false);
        options->distribution->optionsButton.setToggleState (false, dontSendNotification);
        options->distribution = nullptr;
    }
    else if (clickedButton == &options->saveButton)
    {
        DissCalcView* view = findParentComponentOfClass<DissCalcView>();
        
        view->saveDistribution.show (options->distribution->distribution);
        
        options->setVisible (false);
        options->distribution->optionsButton.setToggleState (false, dontSendNotification);
        options->distribution = nullptr;
    }
    else if (clickedButton == &options->removeButton)
    {
        ValueTree parent = options->distribution->distribution.getParent();
        
        if (options->distribution->distribution.getProperty (IDs::IsViewed, false))
            findParentComponentOfClass<DissCalcView>()->closeDistributionPanel();
        
        undo->beginNewTransaction();
        parent.removeChild (options->distribution->distribution, undo);
        
        // Sets the first distribution as the x-axis distribution,
        // if the x-axis distribution is being removed.
        if (parent.getNumChildren() > 0
            && ! parent.getChildWithProperty (IDs::XAxis, true).isValid())
            parent.getChild (0).setProperty (IDs::XAxis, true, undo);
        
        options->setVisible (false);
        options->distribution = nullptr;
    }
}

void DissCalcPanel::setCalcData (ValueTree& calcData)
{
    calcs.dissonanceCalcs = calcData;
    addCalcButton.triggerClick();
}
