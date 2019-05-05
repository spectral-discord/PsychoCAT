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
#include "DistributionPanel.h"
#include "DissCalcView.h"

//==============================================================================
PartialOptions::PartialOptions()
{
    removeButton.setButtonText ("Remove");
    removeButton.setBorders (true, true, true, true, 2);
    addAndMakeVisible (removeButton);
    
    partial = nullptr;
}

PartialOptions::~PartialOptions()
{
    partial = nullptr;
}

void PartialOptions::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
}

void PartialOptions::resized()
{
    removeButton.setBounds (getLocalBounds().reduced (1));
}

//==============================================================================
PartialEditor::PartialEditor()   : partial (IDs::Partial)
{    
    optionsButton.setButtonText ("-");
    addAndMakeVisible (optionsButton);
    optionsButton.addListener (this);
    optionsButton.setBorders (true, true, true, true, 2);
    
    muteButton.setButtonText ("M");
    addAndMakeVisible (muteButton);
    muteButton.addListener (this);
    muteButton.setBorders (true, false, true, true, 2);
    muteButton.setFontSize (13);
    
    frequencyEditor.setTextToShowWhenEmpty ("Freq", Theme::border);
    frequencyEditor.setInputRestrictions (10, "1234567890.");
    frequencyEditor.setFont (16.f);
    addAndMakeVisible (frequencyEditor);
    frequencyEditor.addListener (this);
    
    amplitudeEditor.setTextToShowWhenEmpty ("Amp", Theme::border);
    amplitudeEditor.setInputRestrictions (10, "1234567890.");
    amplitudeEditor.setFont (16.f);
    addAndMakeVisible (amplitudeEditor);
    amplitudeEditor.addListener (this);
    
    addMouseListener (getTopLevelComponent(), true);
    partial.addListener (this);
}

PartialEditor::~PartialEditor()
{
}

void PartialEditor::paint (Graphics &g)
{
    g.fillAll (Theme::mainBackground);
}

void PartialEditor::resized()
{
    auto area = getLocalBounds().reduced (1, 0);
    auto buttonArea = area.removeFromRight (getHeight() / 2).reduced (1);
    
    optionsButton.setBounds (buttonArea.removeFromTop (buttonArea.getHeight() / 2));
    muteButton.setBounds (buttonArea);
    
    frequencyEditor.setBounds (area.removeFromLeft (area.getWidth() / 2).reduced (1));
    amplitudeEditor.setBounds (area.reduced (1));
}

void PartialEditor::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &muteButton)
    {
        bool muteOn = partial.getProperty (IDs::Mute, false);
        
        partial.setProperty (IDs::Mute, muteOn ? false : true, nullptr);
    }
    else if (clickedButton == &optionsButton)
    {
        findParentComponentOfClass<DistributionPanel>()->openOptions (this);
    }
}

void PartialEditor::textEditorFocusLost (TextEditor& editor)
{
    if (&editor == &frequencyEditor
        && editor.getTextValue() != partial[IDs::Freq])
    {
        if (! containsFreq (editor.getText().getFloatValue()))
        {
            partial.setProperty (IDs::Freq, editor.getTextValue().getValue(), nullptr);         // Set undo manager
        }
        else
        {
            // Popup warning that another partial contains the input frequency
            editor.setText (partial[IDs::Freq].toString());
        }
    }
    else if (&editor == &amplitudeEditor
             && editor.getTextValue() != partial[IDs::Amp])
    {
        partial.setProperty (IDs::Amp, editor.getTextValue().getValue(), nullptr);              // Set undo manager
    }
}

void PartialEditor::textEditorReturnKeyPressed (TextEditor& editor)
{
    /*
    if (&editor == &frequencyEditor
        && editor.getTextValue() != partial[IDs::Freq])
    {
        if (! containsFreq (editor.getText().getFloatValue()))
        {
            partial.setProperty (IDs::Freq, editor.getTextValue().getValue(), nullptr);         // Set undo manager
        }
        else
        {
            // Popup warning that another partial contains the input frequency
            editor.setText (partial[IDs::Freq].toString());
        }
    }
    else if (&editor == &amplitudeEditor
             && editor.getTextValue() != partial[IDs::Amp])
    {
        partial.setProperty (IDs::Amp, editor.getTextValue().getValue(), nullptr);              // Set undo manager
    }
    */
    unfocusAllComponents();
}

bool PartialEditor::containsFreq (float freq)
{
    ValueTree distribution = partial.getParent();
    
    for (int i = 0; i < distribution.getNumChildren(); ++i)
    {
        float thisFreq = distribution.getChild (i)[IDs::Freq];
        
        if ((thisFreq == freq && distribution.getChild (i).hasType (IDs::Partial))
            || freq == 1.0
            || freq <= 0)
        {
            return true;
        }
    }
    
    return false;
}

void PartialEditor::setPartialData (ValueTree& data)
{
    partial = data;
}

ValueTree& PartialEditor::getPartialData()
{
    return partial;
}

void PartialEditor::valueTreePropertyChanged (ValueTree& parent, const Identifier& ID)
{
    if (parent == partial && ID == IDs::Mute)
    {
        jassert (partial[IDs::Mute].isBool());
        
        muteButton.setToggleState (partial[IDs::Mute] ? true : false,
                                   dontSendNotification);
    }
}

//==============================================================================
PartialEditorList::PartialEditorList()   : distribution (IDs::OvertoneDistribution)
{
    distribution.addListener (this);
    
    for (int i = 0; i < distribution.getNumChildren(); ++i)
    {
        distribution.getChild (i).addListener (this);
    }
        
    editorHeight = 30;
}

PartialEditorList::~PartialEditorList()
{
}

void PartialEditorList::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
}

void PartialEditorList::resized()
{
    Rectangle<int> area = getLocalBounds();
        
    for (int i = 0; i < partialEditors.size(); ++i)
        partialEditors[i]->setBounds (area.removeFromTop (editorHeight).reduced (1, 0));
}

void PartialEditorList::valueTreePropertyChanged (ValueTree& parent, const Identifier& ID)
{
    for (int i = 0; i < distribution.getNumChildren(); ++i)
    {
        if (parent == distribution.getChild (i) && ID == IDs::Freq)
        {
            partialEditors.sort (comparator);
            resized();
                                    
            return;
        }
        else if (parent == distribution.getChild (i) && ID == IDs::Mute)
        {
            jassert (partialEditors[i]->getPartialData()[IDs::Mute].isBool());
            
            ThemedButton& m = partialEditors[i]->muteButton;
            
            m.setToggleState (distribution.getChild (i)[IDs::Mute] ? true : false,
                              dontSendNotification);
            
            return;
        }
    }
}

void PartialEditorList::valueTreeChildAdded (ValueTree& parent, ValueTree& newChild)
{
    if (parent == distribution)
    {
        partialEditors.add (new PartialEditor());
        addAndMakeVisible (partialEditors.getLast());
        
        newChild.addListener (this);
        partialEditors.getLast()->setPartialData (newChild);
        
        setSize (getWidth(), partialEditors.size() * editorHeight);
    }
}

void PartialEditorList::valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex)
{
    if (parent == distribution
        && removedChild.hasType (IDs::Partial))
    {
        for (int i = 0; i < partialEditors.size(); ++i)
        {
            if (partialEditors[i]->getPartialData() == removedChild)
                partialEditors.remove (i);
        }
        
        setSize (getWidth(), partialEditors.size() * editorHeight);
    }
}

void PartialEditorList::setDistribution (ValueTree& distributionNode)
{
    distribution = distributionNode;
    
    partialEditors.clear();
    
    for (int i = 0; i < distribution.getNumChildren(); ++i)
    {
        if (distribution.getChild (i).hasType (IDs::Partial))
        {
            ValueTree partialNode = distribution.getChild (i);
            
            partialEditors.add (new PartialEditor());
            addAndMakeVisible (partialEditors.getLast());
            
            partialEditors.getLast()->setPartialData (partialNode);
            partialEditors.getLast()->getPartialData().addListener (this);
            
            partialEditors.getLast()->frequencyEditor.setText (partialNode[IDs::Freq]);
            partialEditors.getLast()->amplitudeEditor.setText (partialNode[IDs::Amp]);
            
            if (partialNode[IDs::Mute])
                partialEditors.getLast()->muteButton.setToggleState (true,
                                                                     dontSendNotification);
        }
    }
    
    partialEditors.sort (comparator);
    
    setSize (getWidth(), partialEditors.size() * editorHeight);
    resized();
}

//==============================================================================
DistributionPanelTitleBar::DistributionPanelTitleBar()   : collapseButton ("Collapse",
                                                                           Theme::text,
                                                                           Theme::activeText,
                                                                           Theme::activeText),
                                                           arrowLine (0, 0, -10, 0)
{
    arrowShape.addArrow (arrowLine, 1.8, 5.4, 5.4);
    collapseButton.setShape (arrowShape, false, true, false);
    collapseButton.setBorderSize (BorderSize<int> (5));
    collapseButton.addListener (this);
    
    distributionName.setBordered (false);
    distributionName.setTextToShowWhenEmpty ("Untitled", Theme::border);
    
    addAndMakeVisible (collapseButton);
    addAndMakeVisible (distributionName);
    
    fundamentalFreq.setTextToShowWhenEmpty ("Freq", Theme::border);
    fundamentalFreq.setInputRestrictions (10, "1234567890.");
    fundamentalFreq.setFont (16.f);
    addAndMakeVisible (fundamentalFreq);
    
    fundamentalAmp.setTextToShowWhenEmpty ("Amp", Theme::border);
    fundamentalAmp.setInputRestrictions (10, "1234567890.");
    fundamentalAmp.setFont (16.f);
    addAndMakeVisible (fundamentalAmp);
}

DistributionPanelTitleBar::~DistributionPanelTitleBar()
{
}

void DistributionPanelTitleBar::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (0, 25, getWidth(), 25);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
    
    g.setColour (Theme::text);
    g.setFont (16.0f);
    g.drawText ("Freq", 0, 25, getWidth() / 2 - 6, 25, Justification::centred);
    g.drawText ("Amp", getWidth() / 2 - 10, 25, getWidth() / 2 - 6, 25, Justification::centred);
}

void DistributionPanelTitleBar::resized()
{
    Rectangle<int> area = getLocalBounds().removeFromTop (25);
    Rectangle<int> fundamentalArea = getLocalBounds().removeFromBottom (30).reduced (2, 0);
    fundamentalArea.removeFromRight (15);
    
    collapseButton.setBounds (area.removeFromRight (area.getHeight() * 1.4));
    distributionName.setBounds (area.reduced (1));
    
    fundamentalFreq.setBounds (fundamentalArea.removeFromLeft (fundamentalArea.getWidth() / 2).reduced (1));
    fundamentalAmp.setBounds (fundamentalArea.reduced (1));
}

void DistributionPanelTitleBar::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &collapseButton)
        findParentComponentOfClass<DissCalcView>()->closeDistributionPanel();
}

//==============================================================================
DistributionPanel::DistributionPanel()
{
    addButton.setButtonText ("+");
    addButton.addListener (this);
    addAndMakeVisible (addButton);
    addButton.setBorders (false, true, true, false, 2);
    addButton.setFontSize (26);

    distributionListView.setViewedComponent (&partialList, false);
    distributionListView.setScrollBarsShown (false, false, true, false);
    addAndMakeVisible (distributionListView);
    
    addAndMakeVisible (titleBar);
    
    addChildComponent (options);
    options.setSize (100, 25);
    options.removeButton.addListener (this);
    
    titleBar.distributionName.addListener (this);
    titleBar.fundamentalFreq.addListener (this);
    titleBar.fundamentalAmp.addListener (this);

    
    addMouseListener (this, true);
}

DistributionPanel::~DistributionPanel()
{
}

void DistributionPanel::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 2);
    g.drawLine (0, getHeight() - 35, getWidth(), getHeight() - 35);
}

void DistributionPanel::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> header = area.removeFromTop (80);
    Rectangle<int> footer = area.removeFromBottom (35);
    
    titleBar.setBounds (header);
    addButton.setBounds (footer.removeFromLeft (footer.getHeight()));
    distributionListView.setBounds (area.withHeight (area.getHeight() - 1));
    partialList.setBounds (area.withHeight (area.getHeight() - 1));
}

void DistributionPanel::setDistribution (ValueTree& newDistribution)
{
    if (newDistribution.hasType (IDs::OvertoneDistribution))
    {
        partialList.setDistribution (newDistribution);
        
        titleBar.distributionName.setText (getDistribution().getProperty (IDs::Name, ""));
        
        titleBar.fundamentalFreq.setText (getDistribution()[IDs::FundamentalFreq]);
        titleBar.fundamentalAmp.setText (getDistribution()[IDs::FundamentalAmp]);
        
        if (newDistribution.hasProperty (IDs::XAxis)
            && newDistribution[IDs::XAxis])
        {
            titleBar.fundamentalFreq.setEnabled (false);
            titleBar.fundamentalFreq.setText ("X-Axis");
        }
        else if (! titleBar.fundamentalFreq.isEnabled())
        {
            titleBar.fundamentalFreq.setEnabled (true);
            titleBar.fundamentalFreq.setText (getDistribution()[IDs::FundamentalFreq]);
        }
    }
}

ValueTree& DistributionPanel::getDistribution()
{
    return partialList.distribution;
}

void DistributionPanel::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &addButton)
    {
        partialList.distribution.appendChild (ValueTree (IDs::Partial), nullptr);          // Set undo manager
    }
    else if (clickedButton == &options.removeButton)
    {
        partialList.distribution.removeChild (options.partial->getPartialData(),
                                              nullptr);
        
        options.setVisible (false);
        options.partial = nullptr;
    }
}

void DistributionPanel::textEditorTextChanged (TextEditor& editor)
{
    if (&editor == &titleBar.distributionName)
    {
        if (editor.isEmpty())
        getDistribution().removeProperty (IDs::Name, nullptr);
        else
        getDistribution().setProperty (IDs::Name, editor.getTextValue().toString(), nullptr);
    }
}

void DistributionPanel::textEditorFocusLost (TextEditor& editor)
{
    if (&editor == &titleBar.distributionName)
    {
        if (editor.isEmpty())
        getDistribution().removeProperty (IDs::Name, nullptr);
    }
    else if (&editor == &titleBar.fundamentalFreq)
    {
        if (editor.getTextValue() > 0)
            getDistribution().setProperty (IDs::FundamentalFreq,
                                           editor.getTextValue(),
                                           nullptr);
    }
    else if (&editor == &titleBar.fundamentalAmp)
    {
        getDistribution().setProperty (IDs::FundamentalAmp,
                                       editor.getTextValue(),
                                       nullptr);
    }
}

void DistributionPanel::textEditorReturnKeyPressed (TextEditor& editor)
{
    unfocusAllComponents();
}

void DistributionPanel::mouseDown (const MouseEvent& event)
{
    if (options.isVisible()
        && event.eventComponent != &options.removeButton
        && event.eventComponent != &options.partial->optionsButton)
    {
        options.setVisible (false);
        options.partial->optionsButton.setToggleState (false, dontSendNotification);
    }
}

void DistributionPanel::openOptions (PartialEditor* component)
{
    // Resets the previously-clicked DistributionComponent::optionsButton if active
    if (options.partial != nullptr
        && options.partial->optionsButton.getToggleState())
        options.partial->optionsButton.setToggleState (false, dontSendNotification);
    
    // Close options if the same options buttons was pushed
    if (component == options.partial
        && options.isVisible())
    {
        options.setVisible (false);
        return;
    }
    
    // Setup options
    options.partial = component;
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
