/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "SaveDistributionWindow.h"
#include "MainComponent.h"

SaveDistributionWindow::SaveDistributionWindow()
{    
    distributionName.setTextToShowWhenEmpty ("Enter Timbre Name", Theme::border);
    distributionName.setFont (15.f);
    addAndMakeVisible (distributionName);
    distributionName.addListener (this);
    
    saveButton.setIcon (true, FontAwesome_Save);
    saveButton.setIconSize (24);
    saveButton.setPanelButton (true);
    addAndMakeVisible (saveButton);
    saveButton.addListener (this);
    
    cancelButton.setIcon (true, FontAwesome_WindowClose);
    cancelButton.setIconSize (22);
    cancelButton.setPanelButton (true);
    addAndMakeVisible (cancelButton);
    cancelButton.addListener (this);
    cancelButton.setWantsKeyboardFocus (false);
    
    overwriteButton.setIcon (true, FontAwesome_CheckCircle);
    overwriteButton.setIconSize (34);
    overwriteButton.setPanelButton (true);
    addChildComponent (overwriteButton);
    overwriteButton.addListener (this);

    overwriteCancelButton.setIcon (true, FontAwesome_TimesCircle);
    overwriteCancelButton.setIconSize (34);
    overwriteCancelButton.setPanelButton (true);
    addChildComponent (overwriteCancelButton);
    overwriteCancelButton.addListener (this);

    addKeyListener (this);
    setWantsKeyboardFocus (true);
}

SaveDistributionWindow::~SaveDistributionWindow()
{
}

void SaveDistributionWindow::paint (Graphics& g)
{
    g.fillAll (Theme::headerBackground);
    
    g.setColour (Theme::mainBackground);
    Font f = g.getCurrentFont();
    f.setHeight (22);
    f.setBold (true);
    g.setFont (f);
    g.drawText ("Save Timbre", 10, 5, 150, 25, Justification::centredLeft);

    if (overwriteButton.isVisible())
    {
        Font f = g.getCurrentFont();
        f.setBold (true);
        f.setHeight (18);
        g.setFont (f);
        
        g.drawMultiLineText ("Name already in use. Overwrite?", 10, 55, getWidth() - 20, Justification::centred);
    }
}

void SaveDistributionWindow::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> header = area.removeFromTop (35);
    Rectangle<int> saveArea = area.removeFromTop (45);
    cancelButton.setBounds (header.removeFromRight (header.getHeight()).reduced (3));
    cancelButton.setTopLeftPosition (cancelButton.getX() - 1, cancelButton.getY());
    
    saveButton.setBounds (saveArea.removeFromRight (45).reduced (10));
    distributionName.setBounds (saveArea.reduced (10).withWidth (saveArea.getWidth() - 10));
    
    area = area.withTop (area.getY() - 5).reduced (55, 0);
    overwriteButton.setBounds (area.removeFromRight (45).reduced (5));
    overwriteCancelButton.setBounds (area.reduced (5));
}

void SaveDistributionWindow::textEditorTextChanged (TextEditor& editor)
{
    if (&editor == &distributionName)
    {
        resetFilePath();
        
        if (editor.isEmpty())
        {
            saveButton.setEnabled (false);
            return;
        }
        
        file.setPath (file.getPath()
                      + file.getFileReference().getSeparatorChar()
                      + distributionName.getText()
                      + String (".dismal"));
        
        if (! saveButton.isEnabled())
            saveButton.setEnabled (true);
    }
}

void SaveDistributionWindow::textEditorReturnKeyPressed (TextEditor& editor)
{
    if (! editor.isEmpty() && saveButton.isVisible())
        saveButton.triggerClick();
}

void SaveDistributionWindow::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &cancelButton)
    {
        if (overwriteButton.isVisible())
        {
            setSize (getWidth(), getHeight() - 40);
            saveButton.setVisible (true);
            distributionName.setVisible (true);
            overwriteButton.setVisible (false);
            overwriteCancelButton.setVisible (false);
        }
        
        setVisible (false);
        exitModalState (1);
        distributionName.setText ("");
    }
    else if (clickedButton == &saveButton)
    {
        if (distribution.isValid()
            && ! distributionName.isEmpty()
            && file.getFileReference().hasFileExtension ("dismal"))
        {
            if (file.getFileReference().existsAsFile())
            {
                setSize (getWidth(), getHeight() + 40);
                saveButton.setVisible (false);
                distributionName.setVisible (false);
                overwriteButton.setVisible (true);
                overwriteCancelButton.setVisible (true);
                repaint();
            }
            else
            {
                save();
            }
        }
    }
    else if (clickedButton == &overwriteButton)
    {
        save();
    }
    else if (clickedButton == &overwriteCancelButton)
    {
        setSize (getWidth(), getHeight() - 40);
        saveButton.setVisible (true);
        distributionName.setVisible (true);
        overwriteButton.setVisible (false);
        overwriteCancelButton.setVisible (false);
        repaint();
    }
}

bool SaveDistributionWindow::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    if (originatingComponent == this)
    {
        if (key == KeyPress::escapeKey)
        {
            cancelButton.triggerClick();
        }
        else if (key == KeyPress::returnKey
                 && saveButton.isEnabled()
                 && saveButton.isVisible())
        {
            saveButton.triggerClick();
        }
        
        return true;
    }
    
    return false;
}

void SaveDistributionWindow::show (ValueTree& newDistribution)
{
    resetFilePath();
    
    distribution = newDistribution;
    
    if (distribution.hasType (IDs::OvertoneDistribution))
    {
        if (distribution.hasProperty (IDs::Name))
        {
            file.setPath (file.getPath()
                          + file.getFileReference().getSeparatorChar()
                          + distribution[IDs::Name].toString()
                          + String (".dismal"));
            
            distributionName.setText (distribution[IDs::Name].toString());
        }
        else
        {
            saveButton.setEnabled (false);
        }
        
        setVisible (true);
        enterModalState();
        repaint();
    }
}

void SaveDistributionWindow::save()
{
    distribution.setProperty (IDs::Name,
                              distributionName.getText(),
                              nullptr);
    
    
    ValueTree temp = distribution.createCopy();
    
    temp.removeProperty (IDs::IsViewed, nullptr);
    temp.removeProperty (IDs::XAxis, nullptr);
    temp.removeProperty (IDs::Mute, nullptr);
    temp.setProperty (IDs::FundamentalFreq, 1, nullptr);
    temp.setProperty (IDs::FundamentalAmp, 1, nullptr);
    
    for (auto child : temp)
    {
        child.removeProperty (IDs::Mute, nullptr);
        
        if (child[IDs::Freq].operator float() <= 0
            || child[IDs::Amp].operator float() <= 0)
            temp.removeChild (child, nullptr);
    }
    
    file.saveTreeToFile (temp, true);
    
    setVisible (false);
    exitModalState (1);
    distributionName.setText ("");

    if (overwriteButton.isVisible())
    {
        setSize (getWidth(), getHeight() - 40);
        saveButton.setVisible (true);
        distributionName.setVisible (true);
        overwriteButton.setVisible (false);
        overwriteCancelButton.setVisible (false);
    }
}

void SaveDistributionWindow::resetFilePath()
{
    File dismalDirectory (findParentComponentOfClass<MainComponent>()->getSettings()->getValue ("Saved Distribution Location"));
    
    if (! dismalDirectory.exists())
        dismalDirectory.createDirectory();
    
    file.setPath (dismalDirectory);
}
