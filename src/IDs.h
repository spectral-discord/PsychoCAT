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

namespace IDs
{
    #define DECLARE_ID(name) const juce::Identifier name (#name);
    
    // Tree
    DECLARE_ID (Partial); 
    // Properties
    DECLARE_ID (Freq);
    DECLARE_ID (Amp);
    DECLARE_ID (Mute);
    
    // Tree
    DECLARE_ID (OvertoneDistribution);
    // Properties
    DECLARE_ID (Name);
    DECLARE_ID (FundamentalFreq);
    DECLARE_ID (FundamentalAmp);
    DECLARE_ID (FundamentalMute);
    DECLARE_ID (MinInterval);
    DECLARE_ID (XAxis);
    DECLARE_ID (YAxis);
    DECLARE_ID (IsViewed);
    // Children: Partial[], Fundamental
    
    // Tree
    DECLARE_ID (Calculator);
    // Properties
    DECLARE_ID (ModelName);
    DECLARE_ID (StartFreq);
    DECLARE_ID (EndRatio);
    DECLARE_ID (LogSteps);
    DECLARE_ID (NumSteps);
    DECLARE_ID (PreprocessorName);
    DECLARE_ID (ScaleLocked);
    DECLARE_ID (ScaleMin);
    DECLARE_ID (ScaleMax);
    DECLARE_ID (GridLines);
    // Children: OvertoneDistribution[]
    
    // Tree
    DECLARE_ID (CalculatorList);
    // Properties
    DECLARE_ID (ShowMinima);
    DECLARE_ID (ShowMaxima);
    // Children: Calculator[]
    
    // Tree
    DECLARE_ID (Tuning);
    // Properties
    DECLARE_ID (Notes);
    DECLARE_ID (ReferenceFreq);
    DECLARE_ID (RepeatRatio);
}
