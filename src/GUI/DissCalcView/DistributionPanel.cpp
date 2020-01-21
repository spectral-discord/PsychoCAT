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
    removeButton.setIcon (true, FontAwesome_TrashO);
    removeButton.setTooltip ("Remove");
    removeButton.setIconSize (16);
    removeButton.setBorders (true, true, true, true, 3);
    addAndMakeVisible (removeButton);
    
    muteButton.setButtonText ("M");
    muteButton.setTooltip ("Mute");
    muteButton.setFontSize (16);
    muteButton.setBorders (true, true, true, true, 3);
    addAndMakeVisible (muteButton);

    partial = nullptr;
}

PartialOptions::~PartialOptions()
{
    partial = nullptr;
}

void PartialOptions::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::headerBackground);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 6);
    g.drawLine (0, 0, getWidth(), 0, 6);
    g.drawLine (0, getHeight(), getWidth(), getHeight(), 6);
}

void PartialOptions::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    muteButton.setVisible (true);
    muteButton.setBounds (area.removeFromLeft (getHeight() - 6)
                          .reduced (3, 6)
                          .withY (6));
    
    if (partial != nullptr && partial->getPartialData().hasType (IDs::Partial))
    {
        removeButton.setBounds (Rectangle<int> (getHeight(), getHeight())
                                .reduced (6, 6)
                                .withY (6)
                                .withRightX (getWidth() - 6));
        removeButton.setVisible (true);
    }
    else
    {
        removeButton.setVisible (false);
    }
}

//==============================================================================
PartialEditor::PartialEditor()   : partial (IDs::Partial)
{    
    optionsButton.setIcon (true, FontAwesome_EllipsisV);
    optionsButton.setPanelButton (true);
    optionsButton.setIconSize (27);
    addAndMakeVisible (optionsButton);
    optionsButton.addListener (this);
    
    frequencyEditor.setTextToShowWhenEmpty ("Freq", Theme::border);
    frequencyEditor.setEvaluateExpressions (true);
    frequencyEditor.setFont (16.f);
    addAndMakeVisible (frequencyEditor);
    frequencyEditor.addListener (this);
    
    amplitudeEditor.setTextToShowWhenEmpty ("Amp", Theme::border);
    amplitudeEditor.setEvaluateExpressions (true);
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
    g.fillAll (Theme::headerBackground);
}

void PartialEditor::resized()
{
    auto area = getLocalBounds();
    auto buttonArea = area.removeFromRight (getHeight() / 2);
    
    optionsButton.setBounds (buttonArea.reduced (3).withY (buttonArea.getY() + 2));
    
    frequencyEditor.setBounds (area.removeFromLeft (area.getWidth() / 2).reduced (2).withTop (0).withRight (area.getWidth()));
    amplitudeEditor.setBounds (area.reduced (2).withTop (0).withX (area.getWidth() + 1).withRight (area.getWidth() * 2));
}

void PartialEditor::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &optionsButton)
    {
        findParentComponentOfClass<DissCalcView>()->openOptions (this);
    }
}

void PartialEditor::textEditorFocusLost (TextEditor& editor)
{
    if (&editor == &frequencyEditor
        && frequencyEditor.getEvaluated() != partial[IDs::Freq].operator float())
    {
        if (frequencyEditor.getEvaluated() > 0 && ! containsFreq (frequencyEditor.getEvaluated()))
        {
            // Check if it's the first entry of a freq value so an initial value isn't undone
            // Prevents partial freqs/amps from being set to 0
            if (partial[IDs::Freq].operator float() > 0)
            {
                findParentComponentOfClass<DistributionPanel>()->undo->beginNewTransaction();

                partial.setProperty (IDs::Freq, frequencyEditor.getEvaluated(),
                                     findParentComponentOfClass<DistributionPanel>()->undo);
            }
            else
            {
                partial.setProperty (IDs::Freq, frequencyEditor.getEvaluated(), nullptr);
            }
        }
        else
        {
            // TODO: Popup warning that another partial contains the input frequency
            editor.setText (partial[IDs::Freq].toString());
        }
    }
    else if (&editor == &amplitudeEditor
             && amplitudeEditor.getEvaluated() != partial[IDs::Amp].operator float())
    {
        if (amplitudeEditor.getEvaluated() > 0)
        {
            if (partial[IDs::Amp].operator float() > 0)
            {
                findParentComponentOfClass<DistributionPanel>()->undo->beginNewTransaction();
                
                partial.setProperty (IDs::Amp, amplitudeEditor.getEvaluated(),
                                     findParentComponentOfClass<DistributionPanel>()->undo);
            }
            else
            {
                partial.setProperty (IDs::Amp, amplitudeEditor.getEvaluated(), nullptr);
            }
        }
        else
        {
            editor.setText (partial[IDs::Amp].toString());
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
    if (parent == partial)
    {
        if (ID == IDs::Mute)
        {
            jassert (partial[IDs::Mute].isBool());
            
            amplitudeEditor.setEnabled (! partial[ID]);
            amplitudeEditor.setText (partial[ID] ? "Muted" : partial[IDs::Amp]);
            amplitudeEditor.setBounds (partial[ID]
                                       ? amplitudeEditor.getBounds().reduced (1)
                                       : amplitudeEditor.getBounds().expanded (1));
        }
        else if (ID == IDs::Freq)
        {
            frequencyEditor.setText (partial[ID]);
        }
        else if (ID == IDs::Amp)
        {
            amplitudeEditor.setText (partial[ID]);
        }
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
    g.fillAll (Theme::headerBackground);
}

void PartialEditorList::resized()
{
    Rectangle<int> area = getLocalBounds();
        
    for (auto* editor : partialEditors)
        editor->setBounds (area.removeFromTop (editorHeight));
}

void PartialEditorList::valueTreePropertyChanged (ValueTree& parent, const Identifier& ID)
{
    if (parent == distribution)
    {
        if (ID == IDs::FundamentalFreq)
        {
            findParentComponentOfClass<DistributionPanel>()->titleBar.fundamentalFreq.setText (parent[ID]);
        }
        else if (ID == IDs::FundamentalAmp)
        {
            findParentComponentOfClass<DistributionPanel>()->titleBar.fundamentalAmp.setText (parent[ID]);
        }
        else if (ID == IDs::FundamentalMute)
        {
            ThemedTextEditor& fAmp = findParentComponentOfClass<DistributionPanel>()->titleBar.fundamentalAmp;
            fAmp.setText (parent[ID] ? "Muted" : parent[IDs::FundamentalAmp]);
            fAmp.setEnabled (parent[ID] ? false : true);
            fAmp.setBounds (parent[ID] ? fAmp.getBounds().reduced (1) : fAmp.getBounds().expanded (1));
        }
    }
    
    if (parent.getParent() == distribution)
    {
        for (int i = 0; i < distribution.getNumChildren(); ++i)
        {
            if (parent == distribution.getChild (i) && ID == IDs::Freq)
            {
                partialEditors.sort (comparator);
                resized();
                return;
            }
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
        
        findParentComponentOfClass<DistributionPanel>()->titleBar.repaint();
        
        if (newChild[IDs::Mute])
        {
            jassert (newChild[IDs::Mute].isBool());
            
            ThemedTextEditor& amp = partialEditors[parent.indexOf (newChild)]->amplitudeEditor;
            
            amp.setEnabled (! newChild[IDs::Mute]);
            amp.setText (newChild[IDs::Mute] ? "Muted" : newChild[IDs::Amp]);
            amp.setBounds (newChild[IDs::Mute]
                           ? amp.getBounds().reduced (1)
                           : amp.getBounds().expanded (1));
        }
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
        
        PartialOptions* options = findParentComponentOfClass<DistributionPanel>()->options;
        
        if (options->partial != nullptr
            && options->partial->getPartialData() == removedChild)
        {
            options->partial = nullptr;
            options->setVisible (false);
        }
        
        findParentComponentOfClass<DistributionPanel>()->titleBar.repaint();
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
            
            PartialEditor* editor = partialEditors.getLast();
            
            editor->setPartialData (partial);
            editor->getPartialData().addListener (this);
            
            editor->frequencyEditor.setText (partial[IDs::Freq]);
            editor->amplitudeEditor.setEnabled (! partial[IDs::Mute]);
            editor->amplitudeEditor.setText (partial[IDs::Mute] ? "Muted" : partial[IDs::Amp]);
            editor->amplitudeEditor.setBounds (partial[IDs::Mute]
                                               ? editor->amplitudeEditor.getBounds().reduced (1)
                                               : editor->amplitudeEditor.getBounds().expanded (1));
        }
    }
    
    partialEditors.sort (comparator);
    
    setSize (getWidth(), partialEditors.size() * editorHeight);
    resized();
}

//==============================================================================
DistributionPanelTitleBar::DistributionPanelTitleBar()
{
    collapseButton.setIcon (true, FontAwesome_Backspace);
    collapseButton.setIconSize (24);
    collapseButton.setPanelButton (true);
    collapseButton.addListener (this);
    addAndMakeVisible (collapseButton);

    distributionName.setBordered (false);
    distributionName.setTextToShowWhenEmpty ("Untitled", Theme::border);
    addAndMakeVisible (distributionName);
    
    optionsButton.setIcon (true, FontAwesome_EllipsisV);
    optionsButton.setPanelButton (true);
    optionsButton.setIconSize (27);
    addAndMakeVisible (optionsButton);
    optionsButton.addListener (this);
    
    fundamentalFreq.setTextToShowWhenEmpty ("Freq", Theme::border);
    fundamentalFreq.setEvaluateExpressions (true);
    fundamentalFreq.setTooltip (String ("Sets the fundamental frequency\n\n")
                                + String ("This can be set with:\n")
                                + String ("1. A real frequency value in Hz\n")
                                + String ("2. A ratio to the start frequency"));
    fundamentalFreq.setFont (16.f);
    addAndMakeVisible (fundamentalFreq);
    
    fundamentalAmp.setTextToShowWhenEmpty ("Amp", Theme::border);
    fundamentalAmp.setEvaluateExpressions (true);
    fundamentalAmp.setTooltip (String ("Sets the fundamental amplitude\n\n")
                               + String ("Amplitudes refer to the magnitudes of ")
                               + String ("partials, as you would find in the spectral ")
                               + String ("analysis of a sound, rather than a specific ")
                               + String ("measure such as dB, LKFS, or phons"));
    fundamentalAmp.setFont (16.f);
    addAndMakeVisible (fundamentalAmp);
    setWantsKeyboardFocus (true);
}

DistributionPanelTitleBar::~DistributionPanelTitleBar()
{
}

void DistributionPanelTitleBar::paint (Graphics& g)
{
    g.fillAll (Theme::headerBackground);
    
    g.setColour (Theme::headerBackground);
    g.fillRect (0, 0, getWidth(), 25);
    
    g.setColour (Theme::mainBackground);
    Font f = g.getCurrentFont();
    f.setHeight (18.f);
    f.setBold (true);
    g.setFont (f);
    g.drawText ("Fundamental", 0, 25, getWidth(), 25, Justification::centred);
    
    if (findParentComponentOfClass<DistributionPanel>()->getDistribution().getNumChildren() > 0)
    {
        g.drawLine (10, getHeight() - 26, getWidth() - 10, getHeight() - 26, 3);
        g.drawText ("Overtones", 0, getHeight() - 25, getWidth(), 25, Justification::centred);
    }
    else
    {
        g.fillRect (getLocalBounds().removeFromBottom (27).reduced (3, 0));
    }
}

void DistributionPanelTitleBar::resized()
{
    Rectangle<int> area = getLocalBounds().removeFromTop (25);
    Rectangle<int> fundamentalArea = getLocalBounds().removeFromBottom (60).removeFromTop (30);
    
    collapseButton.setBounds (area.removeFromRight (area.getHeight() + 10).reduced (3));
    distributionName.setBounds (area.reduced (3));
    
    int optionsWidth = fundamentalArea.getHeight() / 2 - 4;
    
    optionsButton.setBounds (fundamentalArea.removeFromRight (optionsWidth)
                             .reduced (3).withLeft (getWidth() - optionsWidth - 2));
    
    fundamentalFreq.setBounds (fundamentalArea.removeFromLeft (fundamentalArea.getWidth() / 2 - 1).reduced (1, 0).withX (2));
    fundamentalAmp.setBounds (fundamentalArea.reduced (1, 0).withRight (getWidth() - optionsWidth - 3));
}

void DistributionPanelTitleBar::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &collapseButton)
        findParentComponentOfClass<DissCalcView>()->closeDistributionPanel();
}

//==============================================================================
DistributionPanel::DistributionPanel()
{
    addButton.setIcon (true, FontAwesome_Plus);
    addAndMakeVisible (&addButton);
    addButton.addListener (this);
    addButton.setIconSize (20);

    distributionListView.setViewedComponent (&partialList, false);
    distributionListView.setScrollBarsShown (false, false, true, false);
    addAndMakeVisible (distributionListView);
    
    addAndMakeVisible (titleBar);
    
    addChildComponent (options);
    
    titleBar.distributionName.addListener (this);
    titleBar.fundamentalFreq.addListener (this);
    titleBar.fundamentalAmp.addListener (this);
    titleBar.optionsButton.addListener (this);
    
    addMouseListener (this, true);
    setWantsKeyboardFocus (true);
}

DistributionPanel::~DistributionPanel()
{
}

void DistributionPanel::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::headerBackground);
    g.drawLine (getWidth(), 0, getWidth(), getHeight(), 6);
    g.drawLine (0, 0, 0, getHeight(), 6);
    g.fillRect (getLocalBounds().removeFromBottom (35));
}

void DistributionPanel::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> header = area.removeFromTop (110);
    Rectangle<int> footer = area.removeFromBottom (35);
    
    titleBar.setBounds (header);
    addButton.setBounds (footer.removeFromRight (footer.getHeight()).reduced (3));

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
        
        if (getDistribution()[IDs::FundamentalMute])
        {
            if (titleBar.fundamentalAmp.isEnabled())
                titleBar.fundamentalAmp.setBounds (titleBar.fundamentalAmp.getBounds().reduced (1));
            titleBar.fundamentalAmp.setText ("Muted");
            titleBar.fundamentalAmp.setEnabled (false);
        }
        else
        {
            if (! titleBar.fundamentalAmp.isEnabled())
                titleBar.fundamentalAmp.setBounds (titleBar.fundamentalAmp.getBounds().expanded (1));
            titleBar.fundamentalAmp.setText (getDistribution()[IDs::FundamentalAmp]);
            titleBar.fundamentalAmp.setEnabled (true);
        }
        
        if (newDistribution.hasProperty (IDs::XAxis)
            && newDistribution[IDs::XAxis])
        {
            if (titleBar.fundamentalFreq.isEnabled())
                titleBar.fundamentalFreq.setBounds (titleBar.fundamentalFreq.getBounds().reduced (1));
            
            titleBar.fundamentalFreq.setEnabled (false);
            titleBar.fundamentalFreq.setText ("X-Axis");
            titleBar.fundamentalFreq.setTooltip (String ("The fundamental frequency of this note\n")
                                                 + String ("cannot be set explicitly - it will change as it\n")
                                                 + String ("moves across the X-axis of the dissonance map."));
        }
        else if (! titleBar.fundamentalFreq.isEnabled())
        {
            titleBar.fundamentalFreq.setBounds (titleBar.fundamentalFreq.getBounds().expanded (1));
            titleBar.fundamentalFreq.setEnabled (true);
            titleBar.fundamentalFreq.setText (getDistribution()[IDs::FundamentalFreq]);
            titleBar.fundamentalFreq.setTooltip (String ("Sets the fundamental frequency\n\n")
                                                 + String ("This can be set with:\n")
                                                 + String ("1. A real frequency value in Hz\n")
                                                 + String ("2. A ratio to the start frequency"));
        }
        
        titleBar.repaint();
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
    else if (clickedButton == &options->removeButton)
    {
        undo->beginNewTransaction();
        options->partial->getPartialData().setProperty (IDs::Freq,
                                                       options->partial->getPartialData()[IDs::Freq],
                                                       undo);
        
        partialList.distribution.removeChild (options->partial->getPartialData(), undo);
        
        options->setVisible (false);
        options->partial = nullptr;
    }
    else if (clickedButton == &options->muteButton)
    {
        if (options->partial != nullptr)
        {
            ValueTree partial = options->partial->getPartialData();
            partial.setProperty (IDs::Mute, ! partial[IDs::Mute], nullptr);
        }
        else
        {
            partialList.distribution.setProperty (IDs::FundamentalMute,
                                                  ! partialList.distribution[IDs::FundamentalMute],
                                                  nullptr);
        }
        
        options->setVisible (false);
        options->partial = nullptr;
    }
    else if (clickedButton == &titleBar.optionsButton)
    {
        findParentComponentOfClass<DissCalcView>()->openOptions (nullptr, true);
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
        if (titleBar.fundamentalFreq.getEvaluated() > 0)
        {
            undo->beginNewTransaction();

            getDistribution().setProperty (IDs::FundamentalFreq,
                                           titleBar.fundamentalFreq.getEvaluated(),
                                           undo);
        }
    }
    else if (&editor == &titleBar.fundamentalAmp)
    {
        if (titleBar.fundamentalAmp.getEvaluated() > 0)
        {
            undo->beginNewTransaction();

            getDistribution().setProperty (IDs::FundamentalAmp,
                                           titleBar.fundamentalAmp.getEvaluated(),
                                           undo);
            
        }
    }
}

void DistributionPanel::textEditorReturnKeyPressed (TextEditor& editor)
{
    unfocusAllComponents();
}
