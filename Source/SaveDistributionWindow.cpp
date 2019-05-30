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

SaveDistributionWindow::SaveDistributionWindow()
{
    resetFilePath();
    
    distributionName.setTextToShowWhenEmpty ("Enter Timbre Name", Theme::border);
    distributionName.setInputRestrictions (30, String ("abcdefghijklmnopqrstuvwxyz1234567890")
                                               + String ("ABCDEFGHIJKLMNOPQRSTUVWXYZ _-'"));
    distributionName.setFont (15.f);
    addAndMakeVisible (distributionName);
    distributionName.addListener (this);
    
    saveButton.setButtonText ("Save");
    saveButton.setFontSize (15.f);
    saveButton.setBorders (true, true, true, true, 2);
    saveButton.setState (Button::buttonOver);
    addAndMakeVisible (saveButton);
    saveButton.addListener (this);
    
    cancelButton.setButtonText ("Cancel");
    cancelButton.setFontSize (15.f);
    cancelButton.setBorders (true, true, true, true, 2);
    addAndMakeVisible (cancelButton);
    cancelButton.addListener (this);
    
    addChildComponent (overwriteButton);
    overwriteButton.addListener (this);
    
    addKeyListener (this);
    
    setWantsKeyboardFocus (true);
    
    overwrite = false;
    
}

SaveDistributionWindow::~SaveDistributionWindow()
{
}

void SaveDistributionWindow::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
    
    g.setColour (Theme::text);
    g.setFont (15.f);
    g.drawText ("Save timbre as:", 15, 15, 100, 25, Justification::centred);
    
    if (file.getFileReference().existsAsFile())
    {
        g.setFont (13.f);
        g.drawMultiLineText ("A timbre with that name already exists. Overwrite?", 15, 85, 150);
    }
}

void SaveDistributionWindow::resized()
{
    distributionName.setBounds (15, 40, 150, 25);
    overwriteButton.setBounds (160, 75, 25, 25);
    
    Rectangle<int> area = getLocalBounds().removeFromBottom (40).removeFromRight (180).reduced (5);
    
    saveButton.setBounds (area.removeFromRight (area.getWidth() / 2).reduced (3, 5));
    cancelButton.setBounds (area.reduced (3, 5));
}

void SaveDistributionWindow::textEditorTextChanged (TextEditor& editor)
{
    if (&editor == &distributionName)
    {
        resetFilePath();
        
        if (editor.isEmpty())
        {
            saveButton.setEnabled (false);
            saveButton.setTooltip (String ("Set a name for the timbre \n")
                                   + String ("and its file."));
            overwriteButton.setVisible (false);
            repaint();
            return;
        }
        
        file.setPath (file.getPath()
                      + file.getFileReference().getSeparatorChar()
                      + distributionName.getText()
                      + String (".dismal"));
        
        if (file.getFileReference().existsAsFile()
            && overwrite == false)
        {
            saveButton.setEnabled (false);
            overwriteButton.setVisible (true);
            repaint();
        }
        else if (! saveButton.isEnabled()
                 || overwriteButton.isVisible())
        {
            saveButton.setEnabled (true);
            saveButton.setTooltip ("");
            saveButton.setState (Button::buttonOver);
            overwriteButton.setVisible (false);
            repaint();
        }
    }
}

void SaveDistributionWindow::textEditorReturnKeyPressed (TextEditor& editor)
{
    unfocusAllComponents();
}

void SaveDistributionWindow::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &cancelButton)
    {
        setVisible (false);
        exitModalState (1);
    }
    else if (clickedButton == &saveButton)
    {
        if (distribution.isValid()
            && ! distributionName.isEmpty()
            && file.getFileReference().hasFileExtension ("dismal"))
        {
            if (file.getFileReference().existsAsFile()
                && overwrite == false)
                return;
            
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
        }
    }
    else if (clickedButton == &overwriteButton)
    {
        overwrite = overwriteButton.getToggleState();
        saveButton.setEnabled (overwriteButton.getToggleState());
        
        if (saveButton.isEnabled())
            saveButton.setState (Button::buttonOver);
    }
}

void SaveDistributionWindow::buttonStateChanged (Button* button)
{
    if (button == &saveButton
        && button->getState() == Button::buttonNormal)
    {
        saveButton.setState (Button::buttonOver);
    }
}

bool SaveDistributionWindow::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    if (originatingComponent == this)
    {
        if (key == KeyPress::escapeKey)
            cancelButton.triggerClick();
        else if (key == KeyPress::returnKey
                 && saveButton.isEnabled())
            saveButton.triggerClick();
        
        return true;
    }
    
    return false;
}

void SaveDistributionWindow::show (ValueTree& newDistribution)
{
    resetFilePath();
    
    overwrite = false;
    overwriteButton.setToggleState (false, dontSendNotification);
    
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
            
            if (file.getFileReference().existsAsFile())
            {
                overwriteButton.setVisible (true);
                saveButton.setEnabled (false);
            }
        }
        else
        {
            saveButton.setTooltip (String ("Set a name for the timbre \n")
                                   + String ("and its file."));
            saveButton.setEnabled (false);
        }
        
        setVisible (true);
        enterModalState();
        repaint();
    }
}

void SaveDistributionWindow::resetFilePath()
{
    File dismalDirectory;
    
    if (JUCE_MAC)
    {
        dismalDirectory = "~/Library/Application Support/DisMAL/Timbres";
    }
    else
    {
        dismalDirectory = String (File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName()
                                  + file.getFileReference().getSeparatorChar()
                                  + String ("DisMAL")
                                  + file.getFileReference().getSeparatorChar()
                                  + String ("Timbres"));
    }
    
    if (! dismalDirectory.exists())
        dismalDirectory.createDirectory();
    
    file.setPath (dismalDirectory);
}
