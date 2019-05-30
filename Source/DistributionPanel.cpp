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
    muteButton.setTooltip ("Mute");
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
            // Check if it's the first entry of a freq value so an initial value isn't undone
            // Prevents partial freqs/amps from being set to 0
            if (partial[IDs::Freq].operator float() > 0)
            {
                findParentComponentOfClass<DistributionPanel>()->undo->beginNewTransaction();

                partial.setProperty (IDs::Freq, editor.getTextValue().getValue(),
                                     findParentComponentOfClass<DistributionPanel>()->undo);
            }
            else
            {
                partial.setProperty (IDs::Freq, editor.getTextValue().getValue(), nullptr);
            }
        }
        else
        {
            // TODO: Popup warning that another partial contains the input frequency
            editor.setText (partial[IDs::Freq].toString());
        }
    }
    else if (&editor == &amplitudeEditor
             && editor.getTextValue() != partial[IDs::Amp])
    {
        if (partial[IDs::Amp].operator float() > 0)
        {
            findParentComponentOfClass<DistributionPanel>()->undo->beginNewTransaction();
            
            partial.setProperty (IDs::Amp, editor.getTextValue().getValue(),
                                 findParentComponentOfClass<DistributionPanel>()->undo);
        }
        else
        {
            partial.setProperty (IDs::Amp, editor.getTextValue().getValue(), nullptr);
        }
    }
}

void PartialEditor::textEditorReturnKeyPressed (TextEditor& editor)
{
    unfocusAllComponents();
}

bool PartialEditor::containsFreq (float freq)
{
    for (auto child : partial.getParent())
    {
        if ((child[IDs::Freq].operator float() == freq
             && child.hasType (IDs::Partial))
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
    
    for (auto partial : distribution)
    {
        partial.addListener (this);
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
        
    for (auto* editor : partialEditors)
        editor->setBounds (area.removeFromTop (editorHeight).reduced (1, 0));
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
        else if (parent == distribution.getChild (i) && ID == IDs::Amp)
        {
            partialEditors[i]->amplitudeEditor.setText (distribution.getChild (i)[ID]);

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
        
        if (newChild[IDs::Freq])
        {
            partialEditors.getLast()->frequencyEditor.setText (newChild[IDs::Freq]);
            partialEditors.getLast()->amplitudeEditor.setText (newChild[IDs::Amp]);
            partialEditors.sort (comparator);
        }
        
        setSize (getWidth(), partialEditors.size() * editorHeight);
    }
}

void PartialEditorList::valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex)
{
    if (parent == distribution
        && removedChild.hasType (IDs::Partial))
    {
        for (auto* editor : partialEditors)
        {
            if (editor->getPartialData() == removedChild)
                partialEditors.removeObject (editor);
        }
        
        setSize (getWidth(), partialEditors.size() * editorHeight);
    }
}

void PartialEditorList::setDistribution (ValueTree& distributionNode)
{
    distribution = distributionNode;
    
    partialEditors.clear();
    
    for (auto partial : distribution)
    {
        if (partial.hasType (IDs::Partial))
        {
            partialEditors.add (new PartialEditor());
            addAndMakeVisible (partialEditors.getLast());
            
            partialEditors.getLast()->setPartialData (partial);
            partialEditors.getLast()->getPartialData().addListener (this);
            
            partialEditors.getLast()->frequencyEditor.setText (partial[IDs::Freq]);
            partialEditors.getLast()->amplitudeEditor.setText (partial[IDs::Amp]);
            
            if (partial[IDs::Mute])
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
    addAndMakeVisible (collapseButton);

    distributionName.setBordered (false);
    distributionName.setTextToShowWhenEmpty ("Untitled", Theme::border);
    distributionName.setInputRestrictions (30, String ("abcdefghijklmnopqrstuvwxyz1234567890")
                                           + String ("ABCDEFGHIJKLMNOPQRSTUVWXYZ _-'"));
    addAndMakeVisible (distributionName);
    
    fundamentalFreq.setTextToShowWhenEmpty ("Freq", Theme::border);
    fundamentalFreq.setInputRestrictions (10, "1234567890.");
    fundamentalFreq.setTooltip ("Sets the fundamental frequency");
    fundamentalFreq.setFont (16.f);
    addAndMakeVisible (fundamentalFreq);
    
    fundamentalAmp.setTextToShowWhenEmpty ("Amp", Theme::border);
    fundamentalAmp.setInputRestrictions (10, "1234567890.");
    fundamentalAmp.setTooltip ("Sets the fundamental amplitude");
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
    g.drawText ("Fundamental", 0, 25, getWidth(), 25, Justification::centred);
    
    g.setFont (14.0f);
    g.drawText ("Freq", 0, 42, getWidth() / 2 - 6, 25, Justification::centred);
    g.drawText ("Amp", getWidth() / 2 - 10, 42, getWidth() / 2 - 6, 25, Justification::centred);
    
    if (findParentComponentOfClass<DistributionPanel>()->getDistribution().getNumChildren() > 0)
    {
        g.setColour (Theme::border);
        g.drawLine (10, getHeight() - 25, getWidth() - 10, getHeight() - 25);
        
        g.setFont (16.0f);
        g.setColour (Theme::text);
        g.drawText ("Overtones", 0, getHeight() - 25, getWidth(), 25, Justification::centred);
    }
}

void DistributionPanelTitleBar::resized()
{
    Rectangle<int> area = getLocalBounds().removeFromTop (25);
    Rectangle<int> fundamentalArea = getLocalBounds().removeFromBottom (60).removeFromTop (30).reduced (2, 0);
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
    Rectangle<int> header = area.removeFromTop (125);
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
            titleBar.fundamentalFreq.setTooltip (String ("The fundamental frequency of this note\n")
                                                 + String ("cannot be set explicitly - it will change as it\n")
                                                 + String ("moves across the X-axis of the dissonance map."));
        }
        else if (! titleBar.fundamentalFreq.isEnabled())
        {
            titleBar.fundamentalFreq.setEnabled (true);
            titleBar.fundamentalFreq.setText (getDistribution()[IDs::FundamentalFreq]);
            titleBar.fundamentalFreq.setTooltip ("Sets the fundamental frequency");
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
        undo->beginNewTransaction();
        partialList.distribution.appendChild (ValueTree (IDs::Partial), undo);
    }
    else if (clickedButton == &options.removeButton)
    {
        undo->beginNewTransaction();
        options.partial->getPartialData().setProperty (IDs::Freq,
                                                       options.partial->getPartialData()[IDs::Freq],
                                                       undo);
        
        partialList.distribution.removeChild (options.partial->getPartialData(), undo);
        
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
        undo->beginNewTransaction();

        if (editor.getTextValue() > 0)
            getDistribution().setProperty (IDs::FundamentalFreq,
                                           editor.getTextValue(),
                                           undo);
    }
    else if (&editor == &titleBar.fundamentalAmp)
    {
        undo->beginNewTransaction();

        if (editor.getTextValue() > 0)
            getDistribution().setProperty (IDs::FundamentalAmp,
                                           editor.getTextValue(),
                                           undo);
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
