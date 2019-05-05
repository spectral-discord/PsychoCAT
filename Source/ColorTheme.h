/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Theme
{
public:
    static const Colour
    
    mainBackground,     mapBackground,      headerBackground,
    buttonDisabled,
    mainButton,         buttonPressed,      buttonHighlighted,
    border,             text,               activeText;
    
private:
    Theme();
    
    JUCE_DECLARE_NON_COPYABLE (Theme)
};
