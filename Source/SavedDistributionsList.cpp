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
#include "SavedDistributionsList.h"
#include "DissCalcView.h"

//==============================================================================
SavedDistributionsList::SavedDistributionsList()
{
    searchBar.setTextToShowWhenEmpty ("Search", Theme::border);
    searchBar.addListener (this);
    addAndMakeVisible (searchBar);
    
    openButton.setButtonText ("Open");
    openButton.setBorders (true, true, true, true, 1);
    openButton.setEnabled (false);
    openButton.addListener (this);
    addAndMakeVisible (openButton);
    
    view.setViewedComponent (&viewComponent, false);
    view.setScrollBarsShown (false, false, true, false);
    addAndMakeVisible (view);
    
    addMouseListener (this, true);
    
    addKeyListener (this);
    setWantsKeyboardFocus (true);
}

SavedDistributionsList::~SavedDistributionsList()
{
}

void SavedDistributionsList::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::text);
    g.setFont (16.f);
    g.drawText ("Saved Timbres", 15, 10, 150, 25, Justification::centredLeft);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
    g.drawRect (view.getBounds().expanded (5));
}

void SavedDistributionsList::resized()
{
    view.setBounds (Rectangle<int> (15, 40, getWidth() - 30, getHeight() - 90).reduced (5));
    viewComponent.setBounds (view.getBounds());
    
    Rectangle<int> footer = getLocalBounds().removeFromBottom (50);
    
    openButton.setBounds (footer.removeFromRight (100).reduced (15));
    searchBar.setBounds (footer.removeFromRight (footer.getWidth() - 15).reduced (0, 15));
}

void SavedDistributionsList::show (ValueTree& treeNode)
{
    searchBar.setText ("");
    selectedFile = "";
    
    distributionNode = treeNode;
    
    displayFiles();
    
    toFront (false);
    setVisible (true);
    enterModalState();
    unfocusAllComponents();
}

void SavedDistributionsList::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &openButton)
    {
        FileIO io = selectedFile;
        
        bool x = distributionNode[IDs::XAxis];
        
        undo->beginNewTransaction();
        distributionNode.copyPropertiesAndChildrenFrom (io.loadTreeFromFile(),
                                                        undo);
        
        distributionNode.setProperty (IDs::XAxis, x, undo);
        
        exitModalState (1);
        setVisible (false);
    }
    else
    {
        for (auto* button : fileButtons)
        {
            if (clickedButton == button)
            {
                unfocusAllComponents();

                if (button == fileButtons[fileList.indexOf (selectedFile)])
                {
                    selectedFile = "";
                    
                    button->setState (Button::buttonNormal);
                    openButton.setEnabled (false);
                }
                else
                {
                    selectedFile = fileList[fileButtons.indexOf (button)];
                    
                    openButton.setEnabled (true);
                    
                    for (auto* other : fileButtons)
                    {
                        if (other->getState() == Button::buttonOver
                            && other != button)
                            other->setState (Button::buttonNormal);
                    }
                }
                
                return;
            }
        }
    }
}

void SavedDistributionsList::buttonStateChanged (Button* button)
{
    if (button == fileButtons[fileList.indexOf (selectedFile)]
        && button->getState() == Button::buttonNormal)
    {
        button->setState (Button::buttonOver);
        unfocusAllComponents();
    }
}

void SavedDistributionsList::textEditorTextChanged (TextEditor& editor)
{
    if (&editor == &searchBar)
    {
        displayFiles (editor.getText());
    }
}

void SavedDistributionsList::textEditorReturnKeyPressed (TextEditor& editor)
{
    if (&editor == &searchBar)
    {
        unfocusAllComponents();
    }
}

bool SavedDistributionsList::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    if (originatingComponent == this)
    {
        if (key == KeyPress::escapeKey)
        {
            setVisible (false);
            exitModalState (1);
        }
        else if (key == KeyPress::returnKey
                 && openButton.isEnabled())
        {
            openButton.triggerClick();
        }
        
        return true;
    }
    
    return false;
}

void SavedDistributionsList::mouseDoubleClick (const MouseEvent& event)
{
    for (auto fileButton : fileButtons)
    {
        if (event.eventComponent == fileButton)
        {
            selectedFile = fileList[fileButtons.indexOf (fileButton)];
            
            openButton.setEnabled (true);
            openButton.triggerClick();
            
            return;
        }
    }
}

void SavedDistributionsList::displayFiles (String searchParam)
{
    File dismalDirectory;
    
    if (JUCE_MAC)
    {
        dismalDirectory = "~/Library/Application Support/DisMAL/Timbres";
    }
    else
    {
        dismalDirectory = String (File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName()
                                  + dismalDirectory.getSeparatorChar()
                                  + String ("DisMAL")
                                  + dismalDirectory.getSeparatorChar()
                                  + String ("Timbres"));
    }
    
    if (! dismalDirectory.exists())
        dismalDirectory.createDirectory();
    
    fileList = dismalDirectory.findChildFiles (File::findFiles, true,
                                               searchParam + String ("*.dismal"));
    
    fileList.sort();
    fileButtons.clear();

    for (int i = 0; i < fileList.size(); ++i)
    {
        fileButtons.add (new ThemedButton());
        
        FileIO current = fileList[i];
        ValueTree tree = current.loadTreeFromFile();
        String buttonText = (tree[IDs::Name].toString()
                             + String (" - ")
                             + String (tree.getNumChildren())
                             + String ("p : "));
        
        for (auto child : tree)
        {
            buttonText.append (child[IDs::Freq].toString(), 4);

            if (tree.indexOf (child) != tree.getNumChildren() - 1)
            {
                buttonText.append (", ", 2);
            }
        }
                
        fileButtons[i]->setButtonText (buttonText);
        fileButtons[i]->addListener (this);
        
        if (i != fileList.size() - 1)
            fileButtons[i]->setBorders (false, false,
                                        false, true, 2);
    }
    
    if (! fileList.contains (selectedFile))
    {
        openButton.setEnabled (false);
    }
    else
    {
        openButton.setEnabled (true);
        fileButtons[fileList.indexOf (selectedFile)]->setState (Button::buttonOver);
    }
    
    viewComponent.setSize (view.getWidth(),
                           fileButtons.size() * 25);
    
    Rectangle<int> viewArea = viewComponent.getLocalBounds();
    
    for (auto* current : fileButtons)
    {
        viewComponent.addAndMakeVisible (current);
        current->setBounds (viewArea.removeFromTop (25));
    }
}
