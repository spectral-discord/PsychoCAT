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
#include "DissMapComponent.h"
#include "DissCalcView.h"

//==============================================================================
DissonanceMap::DissonanceMap()   : mapData (IDs::Calculator)
{
    startFreq.setTextToShowWhenEmpty ("Start Freq", Theme::border);
    startFreq.setInputRestrictions (10, "1234567890.");
    addAndMakeVisible (startFreq);
    startFreq.addListener (this);

    endRatio.setTextToShowWhenEmpty ("End Ratio", Theme::border);
    endRatio.setInputRestrictions (10, "1234567890.");
    addAndMakeVisible (endRatio);
    endRatio.addListener (this);
    
    logSteps.addItem ("Linear Steps", 1);
    logSteps.addItem ("Logarithmic Steps", 2);
    logSteps.setSelectedId (1);
    logSteps.addListener (this);
    addAndMakeVisible (logSteps);
    
    dissonanceModel.addItem ("Sethares", 1);
    dissonanceModel.addItem ("Vassilakis", 2);
    dissonanceModel.setTextWhenNothingSelected ("Dissonance Model");
    dissonanceModel.addListener (this);
    addAndMakeVisible (dissonanceModel);
    
    mapData.addListener (this);
}

DissonanceMap::~DissonanceMap()
{
}

void DissonanceMap::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
    
    g.setColour (Theme::border);
    g.drawLine (0, getHeight() - 25, getWidth(), getHeight() - 25, 2);
    g.drawLine (0, getHeight(), getWidth(), getHeight(), 2);
    
    if (calc.isReadyToProcess())
    {
        g.setColour (Theme::activeText);

        calc.calculateDissonanceMap();
        
        denormalizer.start = 5;
        denormalizer.end = getHeight() - 30;
        denormalizer.interval = 1;
        
        findMinAndMax (calc.get2dRawDissonanceData(), calc.getNumSteps(),
                       normalizer.start, normalizer.end);

        float dissHeight;
        float nextDissHeight;
        
        for (int i = 0; i < calc.getNumSteps(); ++i)
        {
            dissHeight = normalizer.convertTo0to1 (calc.getDissonanceAtStep (i));
            dissHeight = abs (dissHeight - 1);
            dissHeight = denormalizer.convertFrom0to1 (dissHeight);
            
            nextDissHeight = normalizer.convertTo0to1 (calc.getDissonanceAtStep (i + 1));
            nextDissHeight = abs (nextDissHeight - 1);
            nextDissHeight = denormalizer.convertFrom0to1 (nextDissHeight);
            
            g.drawLine (i + 1, dissHeight, i + 2, nextDissHeight, 2);
        }
    }
    else
    {
        int width = 10;
        int height = 10;
        
        for (int i = 0; i < calc.numOvertoneDistributions(); ++i)
        {
            height = 10;
            
            g.drawText (String (calc.getDistributionReference (i)->getFundamentalFreq())
                        + " " + String (calc.getDistributionReference (i)->getFundamentalAmp()),
                        width, height, 50, 15, Justification::centred);
            
            height += 15;
            
            for (int j = 0; j < calc.getDistributionReference (i)->numPartials(); ++j)
            {
                g.drawText (String (calc.getDistributionReference (i)->getFreqRatio (j))
                            + " " + String (calc.getDistributionReference (i)->getAmpRatio (j)),
                            width, height, 50, 15, Justification::centred);
                
                height += 15;
            }
            
            width += 50;
        }
    }
}

void DissonanceMap::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> footer = area.removeFromBottom (25).expanded (1, 0);
    
    startFreq.setBounds (footer.removeFromLeft (75));
    dissonanceModel.setBounds (footer.removeFromLeft (150).expanded (1, 0));
    logSteps.setBounds (footer.removeFromLeft (150));
    endRatio.setBounds (footer.removeFromRight (75));
    
    mapData.setProperty (IDs::NumSteps, getWidth() - 2, nullptr);
}

void DissonanceMap::comboBoxChanged (ComboBox* changedBox)
{
    if (changedBox == &logSteps)
    {
        if (changedBox->getSelectedItemIndex() == 1)
        {
            mapData.setProperty (IDs::LogSteps, true, nullptr);
        }
        else
        {
            mapData.setProperty (IDs::LogSteps, false, nullptr);
        }
    }
    else if (changedBox == &dissonanceModel)
    {
        mapData.setProperty (IDs::ModelName, changedBox->getText(), nullptr);
    }
}

void DissonanceMap::textEditorFocusLost (TextEditor& editor)
{
    if (&editor == &startFreq)
    {
        if (editor.getTextValue().getValue().operator float() >= 20
            && editor.getTextValue().getValue().operator float() <= 10000)
        {
            mapData.setProperty (IDs::StartFreq, 
                                 editor.getTextValue().getValue(),
                                 nullptr);
        }
        else
        {
            editor.setText (mapData[IDs::StartFreq].toString());
        }
    }
    else if (&editor == &endRatio)
    {
        if (editor.getTextValue().getValue().operator float() > 1)
        {
            mapData.setProperty (IDs::EndRatio,
                                 editor.getTextValue().getValue(),
                                 nullptr);
        }
        else
        {
            editor.setText (mapData[IDs::EndRatio].toString());
        }
    }
}

void DissonanceMap::textEditorReturnKeyPressed (TextEditor& editor)
{
    if (&editor == &startFreq)
    {
        if (editor.getTextValue().getValue().operator float() >= 20
            && editor.getTextValue().getValue().operator float() <= 10000)
        {
            mapData.setProperty (IDs::StartFreq,
                                 editor.getTextValue(),
                                 nullptr);
        }
        else
        {
            editor.setText (mapData[IDs::StartFreq].toString());
        }
    }
    else if (&editor == &endRatio)
    {
        if (editor.getTextValue().getValue().operator float() > 1)
        {
            mapData.setProperty (IDs::EndRatio,
                                 editor.getTextValue(),
                                 nullptr);
        }
        else
        {
            editor.setText (mapData[IDs::EndRatio].toString());
        }
    }
    
    unfocusAllComponents();
}

void DissonanceMap::valueTreeChildAdded (ValueTree& parent, ValueTree& newChild)
{
    if (newChild.hasType (IDs::OvertoneDistribution)
        && parent == mapData)
    {
        calc.addOvertoneDistribution (new OvertoneDistribution());
        
        OvertoneDistribution* dist = calc.getDistributionReference (parent.indexOf (newChild));
        
        if (newChild[IDs::FundamentalFreq].operator float() > 0
            && newChild[IDs::FundamentalAmp].operator float() > 0)
            dist->setFundamental (newChild[IDs::FundamentalFreq], newChild[IDs::FundamentalAmp]);
        
        for (int i = 0; i < newChild.getNumChildren(); ++i)
        {
            if (newChild.getChild (i).hasType (IDs::Partial)
                && newChild.getChild (i)[IDs::Freq].operator float() > 0
                && newChild.getChild (i)[IDs::Amp].operator float() > 0)
            {
                dist->addPartial (newChild.getChild (i)[IDs::Freq],
                                  newChild.getChild (i)[IDs::Amp]);
            }
        }
    }
    else if (newChild.hasType (IDs::Partial)
             && parent.hasType (IDs::OvertoneDistribution))
    {
        OvertoneDistribution* dist = calc.getDistributionReference (parent.getParent().indexOf (parent));
        
        if (newChild.hasProperty (IDs::Freq) && newChild.hasProperty (IDs::Amp)
            && newChild[IDs::Freq].operator float() > 0 && newChild[IDs::Amp].operator float() > 0)
        {
            dist->addPartial (newChild[IDs::Freq], newChild[IDs::Amp]);
        }
        else
        {
            dist->addPartial();
        }
        
        parent.sort (comparator, nullptr, false);
    }
    
    repaint();
}

void DissonanceMap::valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex)
{
    if (removedChild.hasType (IDs::OvertoneDistribution)
        && parent == mapData)
    {
        calc.removeOvertoneDistribution (childIndex);
    }
    else if (removedChild.hasType (IDs::Partial)
             && parent.hasType (IDs::OvertoneDistribution))
    {
        calc.getDistributionReference (parent.getParent().indexOf (parent))->removePartial (childIndex);
    }
    
    repaint();
}
void DissonanceMap::valueTreePropertyChanged (ValueTree& parent, const Identifier& ID)
{
    if (parent == mapData && ID == IDs::NumSteps)
    {
        calc.setNumSteps (mapData[ID]);
    }
    else if (parent == mapData && ID == IDs::LogSteps)
    {
        calc.useLogarithmicSteps (mapData[ID]);
    }
    else if (parent == mapData && ID == IDs::StartFreq)
    {
        if (parent.hasProperty (IDs::EndRatio))
            calc.setRange (mapData[ID],
                           mapData[ID].operator float()
                           * mapData[IDs::EndRatio].operator float());
        
        for (int i = 0; i < calc.numOvertoneDistributions(); ++i)
        {
            float fundamentalFreq = mapData.getChild (i)[IDs::FundamentalFreq].operator float();
            
            if (fundamentalFreq < 20)
            {
                calc.getDistributionReference (i)->setFundamentalFreq (fundamentalFreq
                                                                       * mapData[ID].operator float());
            }
        }
    }
    else if (parent == mapData && ID == IDs::EndRatio)
    {
        if (parent.hasProperty (IDs::StartFreq))
            calc.setRange (mapData[IDs::StartFreq],
                           mapData[ID].operator float()
                           * mapData[IDs::StartFreq].operator float());
    }
    else if (parent == mapData && ID == IDs::ModelName)
    {
        if (mapData[ID] == "Sethares")
            calc.setModel (new SetharesModel());
        else if (mapData[ID] == "Vassilakis")
            calc.setModel (new VassilakisModel());
    }
    else if (parent.hasType (IDs::OvertoneDistribution)
             || parent.hasType (IDs::Partial))
    {
        for (int i = 0; i < calc.numOvertoneDistributions(); ++i)
        {
            if (parent.hasType (IDs::OvertoneDistribution)
                && parent == mapData.getChild (i))
            {
                if (ID == IDs::XAxis
                    && parent[IDs::XAxis].operator bool() == true)
                {
                    calc.set2dVariableDistribution (i);
                }
                else if (ID == IDs::Mute)
                {
                    calc.getDistributionReference (i)->mute (parent[IDs::Mute]);
                }
                else if (ID == IDs::Name)
                {
                    calc.getDistributionReference (i)->setDistributionName (parent[IDs::Name]);
                }
                else if (ID == IDs::FundamentalFreq)
                {
                    if (mapData.hasProperty (IDs::StartFreq)
                        && parent[ID].operator float() < 20)
                        calc.getDistributionReference (i)->setFundamentalFreq (parent[ID].operator float()
                                                                               * mapData[IDs::StartFreq].operator float());
                    else if (parent[ID].operator float() > 0)
                        calc.getDistributionReference (i)->setFundamentalFreq (parent[ID]);
                }
                else if (ID == IDs::FundamentalAmp)
                {
                    if (parent[ID].operator float() > 0)
                        calc.getDistributionReference (i)->setFundamentalAmp (parent[ID]);
                }
            }
            else if (parent.hasType (IDs::Partial)
                     && parent.getParent() == mapData.getChild (i))
            {
                OvertoneDistribution* dist = calc.getDistributionReference (i);
                
                if (ID == IDs::Freq)
                {
                    dist->setFreqRatio (parent.getParent().indexOf (parent),
                                        parent[IDs::Freq]);
                    
                    parent.getParent().sort (comparator, nullptr, false);
                }
                else if (ID == IDs::Amp)
                {
                    if (parent[ID].operator float() > 0)
                        dist->setAmpRatio (parent.getParent().indexOf (parent),
                                           parent[IDs::Amp]);
                }
                else if (ID == IDs::Mute)
                {
                    dist->mutePartial(parent.getParent().indexOf (parent),
                                      parent[IDs::Mute]);
                }
            }
        }
    }

    repaint();
}

//==============================================================================
MapList::MapList()   : mapsData (IDs::CalculatorList)
{
    mapHeight = 175;
    mapsData.addListener (this);
}

MapList::~MapList()
{
}

void MapList::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
}

void MapList::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    for (int i = 0; i < maps.size(); ++i)
    {
        maps[i]->setBounds (area.removeFromTop (mapHeight));
    }
}

void MapList::valueTreeChildAdded (ValueTree& parent, ValueTree& newChild)
{
    if (parent == mapsData
        && newChild.hasType (IDs::Calculator))
    {
        maps.add (new DissonanceMap());
        maps.getLast()->mapData = newChild;
        addAndMakeVisible (maps.getLast());
        
        setSize (getWidth(), maps.size() * mapHeight);
        
        if (getHeight() > getParentHeight())
            setSize (getWidth(), getHeight() + 35);
    }
}

void MapList::valueTreeChildRemoved (ValueTree& parent, ValueTree& removedChild, int childIndex)
{
    if (parent == mapsData)
    {
        maps.remove (childIndex);
        
        setSize (getWidth(), maps.size() * mapHeight);
    }
}

//==============================================================================
void MapViewport::visibleAreaChanged (const Rectangle<int>& newVisibleArea)
{
    findParentComponentOfClass<DissCalcView>()->repositionDistributionPanel (newVisibleArea.getY());
}

//==============================================================================
DissMapComponent::DissMapComponent()
{
    mapView.setViewedComponent (&maps, false);
    addAndMakeVisible (mapView);
    
    mapView.setScrollBarsShown (false, false, true, false);
}

DissMapComponent::~DissMapComponent()
{
}

void DissMapComponent::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);
}

void DissMapComponent::resized()
{
    mapView.setBounds (getLocalBounds());
    
    if (maps.getHeight() < getHeight())
        maps.setBounds (getLocalBounds());
}

void DissMapComponent::setData (ValueTree& calcList)
{
    maps.mapsData = calcList;
}
