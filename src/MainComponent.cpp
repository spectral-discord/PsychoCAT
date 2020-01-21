/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()   : calcData (IDs::CalculatorList)
{
    options.applicationName = "PsychoCAT";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "PsychoCAT";
    
    settings.setStorageParameters (options);
    settingsMenu.init (&settings);
    
    addAndMakeVisible (&calcView);
    calcView.setData (calcData);
    
    if (! settings.getUserSettings()->containsKey ("Window Height"))
        settings.getUserSettings()->setValue ("Window Height", 600);
    if (! settings.getUserSettings()->containsKey ("Window Height"))
        settings.getUserSettings()->setValue ("Window Height", 800);
    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (settings.getUserSettings()->getIntValue ("Window Height"),
             settings.getUserSettings()->getIntValue ("Window Height"));

    // Some platforms require permissions to open input channels
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{

    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
}

void MainComponent::resized()
{
    calcView.setBounds (getLocalBounds());
    
    settings.getUserSettings()->setValue ("windowWidth", getWidth());
    settings.getUserSettings()->setValue ("windowHeight", getHeight());
}

PropertiesFile* MainComponent::getSettings()
{
    return settings.getUserSettings();
}
