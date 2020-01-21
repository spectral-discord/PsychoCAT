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
#include "MainComponent.h"

//==============================================================================
SavedDistributionsList::SavedDistributionsList()
{
    searchBar.setTextToShowWhenEmpty ("Search", Theme::border);
    searchBar.addListener (this);
    addAndMakeVisible (searchBar);
    
    openButton.setIcon (true, FontAwesome_FolderOpen);
    openButton.setIconSize (22);
    openButton.setPanelButton (true);
    openButton.setEnabled (false);
    openButton.addListener (this);
    addAndMakeVisible (openButton);
    openButton.setTooltip ("Open");
    
    closeButton.setIcon (true, FontAwesome_WindowClose);
    closeButton.addListener (this);
    closeButton.setPanelButton (true);
    closeButton.setIconSize (26);
    addAndMakeVisible (closeButton);
    
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
    g.fillAll (Theme::headerBackground);
    
    g.setColour (Theme::mainBackground);
    Font f = g.getCurrentFont();
    f.setHeight (22.f);
    f.setBold (true);
    g.setFont (f);
    
    g.drawText ("Saved Timbres", 10, 5, 150, 25, Justification::centredLeft);
    g.fillRect (getLocalBounds().reduced (3).withTop (35).withBottom (getHeight() - 45));
}

void SavedDistributionsList::resized()
{
    view.setBounds (Rectangle<int> (15, 40, getWidth() - 30, getHeight() - 90).reduced (5));
    viewComponent.setBounds (view.getBounds());
    
    Rectangle<int> footer = getLocalBounds().removeFromBottom (45);
    Rectangle<int> header = getLocalBounds().removeFromTop (35);

    openButton.setBounds (footer.removeFromRight (45).reduced (10));
    searchBar.setBounds (footer.reduced (10).withWidth (footer.getWidth() - 10));
    
    closeButton.setBounds (header.removeFromRight (header.getHeight()).reduced (3));
    closeButton.setTopLeftPosition (closeButton.getX() - 1, closeButton.getY());
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
        ValueTree current = distributionNode;
        ValueTree parent = distributionNode.getParent();
        ValueTree newCalc (IDs::OvertoneDistribution);
        distributionNode = newCalc;

        FileIO io = selectedFile;
        int index = parent.indexOf (current);
        bool x = current[IDs::XAxis];
        newCalc.copyPropertiesAndChildrenFrom (io.loadTreeFromFile(), nullptr);
        
        undo->beginNewTransaction();
        
        parent.removeChild (index, undo);
        parent.addChild (newCalc, index, undo);
        newCalc.setProperty (IDs::XAxis, x, undo);
        
        exitModalState (1);
        setVisible (false);
    }
    else if (clickedButton == &closeButton)
    {
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
    File dismalDirectory (findParentComponentOfClass<MainComponent>()->getSettings()->getValue ("Saved Distribution Location"));
    
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
        
        if (fileList.size() > 1)
        {
            bool bottom = false;
            bool top = false;
            
            if (i != fileList.size() - 1)
                bottom = true;
            
            if (i != 0)
                top = true;
            
            fileButtons[i]->setBorders (false, top, false, bottom, 2);
        }
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
