/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "SettingsMenu.h"

GeneralOptions::GeneralOptions()
{
    loadPreviousSession.setButtonText ("Load Previous Session");
    loadPreviousSession.setTooltip ("Automatically save your session and reloads it upon opening the PsychoCAT app.");
    loadPreviousSession.setToggleState (false, dontSendNotification);
    addAndMakeVisible (loadPreviousSession);
    
    distributionExportLocation.setTextToShowWhenEmpty ("", Theme::border);
    loadPreviousSession.setTooltip ("The default file directory to export overtone distributions to.");
    distributionExportLocation.setFont (15.f);
    addAndMakeVisible (distributionExportLocation);
    
    tuningExportLocation.setTextToShowWhenEmpty ("", Theme::border);
    tuningExportLocation.setTooltip ("The default file directory to export tunings to.");
    tuningExportLocation.setFont (15.f);
    addAndMakeVisible (tuningExportLocation);
    
    savedDistributionLocation.setTextToShowWhenEmpty ("", Theme::border);
    savedDistributionLocation.setTooltip ("Overtone distributions will be stored here as '.dismal' files. This is for internal app use.");
    savedDistributionLocation.setFont (15.f);
    addAndMakeVisible (savedDistributionLocation);
    
    distributionExportLocationLabel.setText ("Overtone Distribution Export Location", dontSendNotification);
    distributionExportLocationLabel.setTooltip ("The default file directory to export overtone distributions to.");
    distributionExportLocationLabel.setFont (13);
    distributionExportLocationLabel.attachToComponent (&distributionExportLocation, false);
    
    tuningExportLocationLabel.setText ("Tuning Export Location", dontSendNotification);
    tuningExportLocationLabel.setTooltip ("The default file directory to export tunings to.");
    tuningExportLocationLabel.setFont (13);
    tuningExportLocationLabel.attachToComponent (&tuningExportLocation, false);
    
    savedDistributionLocationLabel.setText ("Saved Distribution Location (Internal Use)", dontSendNotification);
    savedDistributionLocationLabel.setTooltip ("Overtone distributions will be stored here as '.dismal' files. This is for internal app use.");
    savedDistributionLocationLabel.setFont (13);
    savedDistributionLocationLabel.attachToComponent (&savedDistributionLocation, false);
}

GeneralOptions::~GeneralOptions()
{
}

void GeneralOptions::paint(Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
}

void GeneralOptions::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (10, 10);
    
    loadPreviousSession.setBounds (area.removeFromTop (25).withRight (area.getRight()));
    area.removeFromTop (30);
    distributionExportLocation.setBounds (area.removeFromTop (25));
    area.removeFromTop (30);
    tuningExportLocation.setBounds (area.removeFromTop (25));
    area.removeFromTop (30);
    savedDistributionLocation.setBounds (area.removeFromTop (25));
}

//==============================================================================
MapOptions::MapOptions()
{
    logSteps.setButtonText ("Use Logarithmic Step Sizes");
    logSteps.setTooltip ("Sets the default step size to logarithmic instead of linear.");
    addAndMakeVisible (logSteps);
    
    dissonanceModel.setTooltip ("The default dissonance model to use in dissonance calculations.");
    dissonanceModel.addItem ("Sethares", 1);
    dissonanceModel.addItem ("Vassilakis", 2);
    dissonanceModel.setTextWhenNothingSelected ("None Selected");
    addAndMakeVisible (dissonanceModel);
    
    hearingRangePreprocessor.setButtonText ("Hearing Range Filter");
    hearingRangePreprocessor.setTooltip ("Use the hearing range filter preprocessor by default.");
    addAndMakeVisible (hearingRangePreprocessor);
    
    dissonanceModelLabel.setText ("Default Dissonance Model", dontSendNotification);
    dissonanceModelLabel.setTooltip ("The default dissonance model to use in dissonance calculations.");
    dissonanceModelLabel.setFont (14);
    dissonanceModelLabel.attachToComponent (&dissonanceModel, true);
}

MapOptions::~MapOptions()
{
}

void MapOptions::paint(Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
}

void MapOptions::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (10, 10);
    
    logSteps.setBounds (area.removeFromTop (25).withRight (area.getRight()));
    area.removeFromTop (10);
    dissonanceModel.setBounds (area.removeFromTop (25).withWidth (125).withRight (area.getRight()));
    area.removeFromTop (10);
    hearingRangePreprocessor.setBounds (area.removeFromTop (25).withRight (area.getRight()));
}

//==============================================================================
OptimizationOptions::OptimizationOptions()
{
    stepSize.setTooltip ("Sets the step size used for iterative optimization attempts. Larger step sizes may cause some minima to go undetected, while improving processing time.");
    stepSize.setInputRestrictions (10, "1234567890.");
    stepSize.setFont (15.f);
    addAndMakeVisible (stepSize);
    
    stopValue.setTooltip ("Sets the value for the stopping criteria. This determines the precision of the resulting minima and maxima. Lower values result in more precise optimizations. Lower values can increase processing time, but not as much as the step size parameter.");
    stopValue.setInputRestrictions (10, "1234567890.");
    stopValue.setFont (15.f);
    addAndMakeVisible (stopValue);
    
    minInterval.setTooltip ("Sets the allowed minimum interval between two minima or maxima. This is necessary to prevent the algorithm from reporting multiple minima around the true minima, but can also be used to automatically select the interval with the lowest (or highest) dissonance among multiple close-lying minima. This has no effect on processing time.");
    minInterval.setInputRestrictions (10, "1234567890.");
    minInterval.setFont (15.f);
    addAndMakeVisible (minInterval);
    
    stepSizeLabel.setText ("Step Size", dontSendNotification);
    stepSizeLabel.setTooltip ("Sets the step size used for iterative optimization attempts. Larger step sizes may cause some minima to go undetected, while improving processing time.");
    stepSizeLabel.setFont (14);
    stepSizeLabel.attachToComponent (&stepSize, true);
    
    stopValueLabel.setText ("Stop Value", dontSendNotification);
    stopValueLabel.setTooltip ("Sets the value for the stopping criteria. This determines the precision of the resulting minima and maxima. Lower values result in more precise optimizations. Lower values can increase processing time, but not as much as the step size parameter.");
    stopValueLabel.setFont (14);
    stopValueLabel.attachToComponent (&stopValue, true);
    
    minIntervalLabel.setText ("Minimum Interval", dontSendNotification);
    minIntervalLabel.setTooltip ("Sets the allowed minimum interval ratio between two minima or maxima. This is necessary to prevent the algorithm from reporting multiple minima around the true minima, but can also be used to automatically select the interval with the lowest (or highest) dissonance among multiple close-lying minima. This has no effect on processing time.");
    minIntervalLabel.setFont (14);
    minIntervalLabel.attachToComponent (&minInterval, true);
}

OptimizationOptions::~OptimizationOptions()
{
}

void OptimizationOptions::paint(Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
}

void OptimizationOptions::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (10, 10);
    
    stepSize.setBounds (area.removeFromTop (25).withWidth (100).withRight (area.getRight()));
    area.removeFromTop (10);
    stopValue.setBounds (area.removeFromTop (25).withWidth (100).withRight (area.getRight()));
    area.removeFromTop (10);
    minInterval.setBounds (area.removeFromTop (25).withWidth (100).withRight (area.getRight()));
}

//==============================================================================
PreprocessorOptions::PreprocessorOptions()
{
    hearingRangeStart.setTooltip ("Sets the lower limit of the hearing range preprocessor. If the hearing range preprocessor is active, partials with frequencies below this limit will not be included in dissonance calculations.");
    hearingRangeStart.setInputRestrictions (10, "1234567890.");
    hearingRangeStart.setFont (15.f);
    addAndMakeVisible (hearingRangeStart);
    
    hearingRangeEnd.setTooltip ("Sets the upper limit of the hearing range preprocessor. If the hearing range preprocessor is active, partials with frequencies above this limit will not be included in dissonance calculations.");
    hearingRangeEnd.setInputRestrictions (10, "1234567890.");
    hearingRangeEnd.setFont (15.f);
    addAndMakeVisible (hearingRangeEnd);
    
    hearingRangeStartLabel.setText ("Hearing Range Start", dontSendNotification);
    hearingRangeStartLabel.setTooltip ("Sets the lower limit of the hearing range preprocessor. If the hearing range preprocessor is active, partials with frequencies below this limit will not be included in dissonance calculations.");
    hearingRangeStartLabel.setFont (14);
    hearingRangeStartLabel.attachToComponent (&hearingRangeStart, true);
    
    hearingRangeEndLabel.setText ("Hearing Range Start", dontSendNotification);
    hearingRangeEndLabel.setTooltip ("Sets the lower limit of the hearing range preprocessor. If the hearing range preprocessor is active, partials with frequencies below this limit will not be included in dissonance calculations.");
    hearingRangeEndLabel.setFont (14);
    hearingRangeEndLabel.attachToComponent (&hearingRangeEnd, true);
}

PreprocessorOptions::~PreprocessorOptions()
{
}

void PreprocessorOptions::paint(Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
}

void PreprocessorOptions::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (10, 10);
    
    hearingRangeStart.setBounds (area.removeFromTop (25).withWidth (100).withRight (area.getRight()));
    area.removeFromTop (10);
    hearingRangeEnd.setBounds (area.removeFromTop (25).withWidth (100).withRight (area.getRight()));
}

//==============================================================================
SettingsMenu::SettingsMenu()
{
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
    
    generalButton.setButtonText ("General");
    generalButton.setFontSize (15.f);
    generalButton.setBorders (false, false, false, false, 2);
    addAndMakeVisible (generalButton);
    generalButton.addListener (this);
    
    dissonanceMapsButton.setButtonText ("Dissonance Maps");
    dissonanceMapsButton.setFontSize (15.f);
    dissonanceMapsButton.setBorders (false, false, false, false, 2);
    addAndMakeVisible (dissonanceMapsButton);
    dissonanceMapsButton.addListener (this);
    
    optimizationButton.setButtonText ("Optimization");
    optimizationButton.setFontSize (15.f);
    optimizationButton.setBorders (false, false, false, false, 2);
    addAndMakeVisible (optimizationButton);
    optimizationButton.addListener (this);
    
    preprocessorsButton.setButtonText ("Preprocessors");
    preprocessorsButton.setFontSize (15.f);
    preprocessorsButton.setBorders (false, false, false, false, 2);
    addAndMakeVisible (preprocessorsButton);
    preprocessorsButton.addListener (this);
    
    addChildComponent (general);
    addChildComponent (maps);
    addChildComponent (optimizations);
    addChildComponent (preprocessors);
    
    addAndMakeVisible (view);
    view.setViewedComponent (&general, false);
    
    addKeyListener (this);
    setWantsKeyboardFocus (true);
}

SettingsMenu::~SettingsMenu()
{
}

void SettingsMenu::init (ApplicationProperties* properties)
{
    settings = properties->getUserSettings();
    
    if (! settings->containsKey ("Load Previous Session"))
        settings->setValue ("Load Previous Session", true);
    
    if (! settings->containsKey ("Distribution Export Location"))
    {
        File directory (File::getSpecialLocation (File::userDocumentsDirectory).getFullPathName());
        
        settings->setValue ("Distribution Export Location", directory.getFullPathName());
        
        if (! directory.exists())
            directory.createDirectory();
    }
    
    if (! settings->containsKey ("Tuning Export Location"))
    {
        File directory (File::getSpecialLocation (File::userDocumentsDirectory).getFullPathName());
        
        settings->setValue ("Tuning Export Location", directory.getFullPathName());
        
        if (! directory.exists())
            directory.createDirectory();
    }
    
    if (! settings->containsKey ("Saved Distribution Location"))
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
        
        settings->setValue ("Saved Distribution Location", dismalDirectory.getFullPathName());
    }
    
    if (! settings->containsKey ("Optim. Step Size"))
        settings->setValue ("Optim. Step Size", 1.0008);
    
    if (! settings->containsKey ("Optim. Stop Value"))
        settings->setValue ("Optim. Stop Value", 0.00005);
    
    if (! settings->containsKey ("Optim. Min. Interval"))
        settings->setValue ("Optim. Min. Interval", 1.001);
    
    if (! settings->containsKey ("Hearing Range Start"))
        settings->setValue ("Hearing Range Start", 20);
    
    if (! settings->containsKey ("Hearing Range End"))
        settings->setValue ("Hearing Range End", 20000);
}

void SettingsMenu::paint(Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawRect (getLocalBounds());
    g.drawRect (getLocalBounds().removeFromLeft (130));
    
    g.drawLine (25, 38, 65, 38);
    g.drawLine (25, 68, 65, 68);
    g.drawLine (25, 98, 65, 98);
}

void SettingsMenu::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> tabs = area.removeFromLeft (130).reduced (10, 5);
    Rectangle<int> buttons = area.removeFromBottom (40).removeFromRight (180).reduced (5);
    
    area.setWidth (area.getWidth() - 10);
    area.setHeight (area.getHeight() - 5);
    area.setY (5);
    area.reduce (0, 5);
    
    view.setBounds (area);
    general.setBounds (area);
    maps.setBounds (area);
    optimizations.setBounds (area);
    preprocessors.setBounds (area);

    generalButton.setBounds (tabs.removeFromTop (25).reduced (5, 0));
    tabs.removeFromTop (5);
    dissonanceMapsButton.setBounds (tabs.removeFromTop (25).reduced (5, 0));
    tabs.removeFromTop (5);
    optimizationButton.setBounds (tabs.removeFromTop (25).reduced (5, 0));
    tabs.removeFromTop (5);
    preprocessorsButton.setBounds (tabs.removeFromTop (25).reduced (5, 0));
    tabs.removeFromTop (5);
    
    saveButton.setBounds (buttons.removeFromRight (buttons.getWidth() / 2).reduced (3, 5));
    cancelButton.setBounds (buttons.reduced (3, 5));
}

void SettingsMenu::setVisiblity (bool show)
{
    if (show)
    {
        view.setViewedComponent (&general, false);
        generalButton.setToggleState (true, dontSendNotification);
        
        setVisible (true);
        enterModalState();
    }
    else
    {
        setVisible (false);
        exitModalState (1);
    }
}

void SettingsMenu::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &generalButton)
    {
        view.setViewedComponent (&general, false);
        
        generalButton.setToggleState (true, dontSendNotification);
        dissonanceMapsButton.setToggleState (false, dontSendNotification);
        optimizationButton.setToggleState (false, dontSendNotification);
        preprocessorsButton.setToggleState (false, dontSendNotification);
        
        general.loadPreviousSession.setToggleState (settings->getBoolValue ("Load Previous Session"), dontSendNotification);
        general.distributionExportLocation.setText (settings->getValue ("Distribution Export Location"));
        general.tuningExportLocation.setText (settings->getValue ("Tuning Export Location"));
        general.savedDistributionLocation.setText (settings->getValue ("Tuning Export Location"));
    }
    else if (clickedButton == &dissonanceMapsButton)
    {
        view.setViewedComponent (&maps, false);
        
        generalButton.setToggleState (false, dontSendNotification);
        dissonanceMapsButton.setToggleState (true, dontSendNotification);
        optimizationButton.setToggleState (false, dontSendNotification);
        preprocessorsButton.setToggleState (false, dontSendNotification);
        
        if (settings->containsKey ("Log Steps"))
            maps.logSteps.setToggleState (settings->getBoolValue ("Log Steps"), dontSendNotification);
        
        if (settings->containsKey ("Hearing Range Preprocessor"))
            maps.hearingRangePreprocessor.setToggleState (settings->getBoolValue ("Hearing Range Preprocessor"), dontSendNotification);
        
        if (settings->containsKey ("Dissonance Model"))
            maps.dissonanceModel.setSelectedId (settings->getIntValue ("Dissonance Model"));
    }
    else if (clickedButton == &optimizationButton)
    {
        view.setViewedComponent (&optimizations, false);
        
        generalButton.setToggleState (false, dontSendNotification);
        dissonanceMapsButton.setToggleState (false, dontSendNotification);
        optimizationButton.setToggleState (true, dontSendNotification);
        preprocessorsButton.setToggleState (false, dontSendNotification);
        
        optimizations.stepSize.setText (String (settings->getDoubleValue ("Optim. Step Size")));
        optimizations.stopValue.setText (String (settings->getDoubleValue ("Optim. Stop Value")));
        optimizations.minInterval.setText (String (settings->getDoubleValue ("Optim. Min. Interval")));
    }
    else if (clickedButton == &preprocessorsButton)
    {
        view.setViewedComponent (&preprocessors, false);
        
        generalButton.setToggleState (false, dontSendNotification);
        dissonanceMapsButton.setToggleState (false, dontSendNotification);
        optimizationButton.setToggleState (false, dontSendNotification);
        preprocessorsButton.setToggleState (true, dontSendNotification);
        
        preprocessors.hearingRangeStart.setText (String (settings->getDoubleValue ("Hearing Range Start")));
        preprocessors.hearingRangeEnd.setText (String (settings->getDoubleValue ("Hearing Range End")));
    }
    else if (clickedButton == &cancelButton)
    {
        setVisiblity (false);
    }
    else if (clickedButton == &saveButton)
    {
        settings->setValue ("Load Previous Session", general.loadPreviousSession.getToggleStateValue());
        
        if (File::isAbsolutePath (general.distributionExportLocation.getText())
            && File (general.distributionExportLocation.getText()).isDirectory())
        {
            settings->setValue ("Distribution Export Location", general.distributionExportLocation.getText());
            
            if (! File (general.distributionExportLocation.getText()).exists())
                File (general.distributionExportLocation.getText()).createDirectory();
        }
        if (File::isAbsolutePath (general.tuningExportLocation.getText())
            && File (general.tuningExportLocation.getText()).isDirectory())
        {
            settings->setValue ("Tuning Export Location", general.tuningExportLocation.getText());
            
            if (! File (general.tuningExportLocation.getText()).exists())
                File (general.tuningExportLocation.getText()).createDirectory();
        }

        if (File::isAbsolutePath (general.savedDistributionLocation.getText())
            && File (general.savedDistributionLocation.getText()).isDirectory())
        {
            settings->setValue ("Saved Distribution Location", general.savedDistributionLocation.getText());
            
            if (! File (general.savedDistributionLocation.getText()).exists())
                File (general.savedDistributionLocation.getText()).createDirectory();
        }

        settings->setValue ("Log Steps", maps.logSteps.getToggleStateValue());
        settings->setValue ("Hearing Range Preprocessor", maps.hearingRangePreprocessor.getToggleStateValue());
        
        if (maps.dissonanceModel.getSelectedItemIndex() >= 0)
            settings->setValue ("Dissonance Model", maps.dissonanceModel.getSelectedItemIndex());
        
        settings->setValue ("Optim. Step Size", optimizations.stepSize.getText().getFloatValue());
        settings->setValue ("Optim. Stop Value", optimizations.stopValue.getText().getFloatValue());
        settings->setValue ("Optim. Min. Interval", optimizations.minInterval.getText().getFloatValue());
        
        settings->setValue ("Hearing Range Start", preprocessors.hearingRangeStart.getText().getFloatValue());
        settings->setValue ("Hearing Range End", preprocessors.hearingRangeEnd.getText().getFloatValue());

        setVisiblity (false);
    }
}

bool SettingsMenu::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    if (originatingComponent == this)
    {
        if (key == KeyPress::escapeKey)
            setVisiblity (false);
        
        return true;
    }
    
    return false;
}
