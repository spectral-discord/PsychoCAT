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
    startFreq.setTooltip (String ("Starting frequency\n")
                          + String ("(left edge of the map)"));
    startFreq.setInputRestrictions (10, "1234567890.");
    startFreq.setFont (15.f);
    addAndMakeVisible (startFreq);
    startFreq.addListener (this);

    endRatio.setTextToShowWhenEmpty ("End Ratio", Theme::border);
    endRatio.setTooltip (String ("End frequency ratio\n")
                          + String ("(right edge of the map)"));
    endRatio.setInputRestrictions (10, "1234567890.");
    endRatio.setFont (15.f);
    addAndMakeVisible (endRatio);
    endRatio.addListener (this);
    
    logSteps.setButtonText ("Log. Steps");
    logSteps.setTooltip ("Set whether frequency step sizes\n grow linearly or logarithmically");
    logSteps.addListener (this);
    addAndMakeVisible (logSteps);
    
    dissonanceModel.addSectionHeading ("Dissonance Model");
    dissonanceModel.addItem ("Sethares", 1);
    dissonanceModel.addItem ("Vassilakis", 2);
    dissonanceModel.setTextWhenNothingSelected ("Dissonance Model");
    dissonanceModel.setTooltip ("Sets the dissonance model");
    dissonanceModel.addListener (this);
    addAndMakeVisible (dissonanceModel);
    
    lockScale.setButtonText ("Lock Scale");
    lockScale.setTooltip ("Locks the dissonance scale (Y-axis)");
    lockScale.addListener (this);
    addAndMakeVisible (lockScale);
    
    setRepaintsOnMouseActivity (true);
    setWantsKeyboardFocus (true);
    
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
    
    // Check if all the data needed to calculate a dissonance map is available
    if (calc.isReadyToProcess())
    {
        if (mapNeedsRedrawn)
        {
            calc.calculateDissonanceMap();
            mapNeedsRedrawn = false;
        
            findMinAndMax (calc.get2dRawDissonanceData(), calc.getNumSteps(),
                           normalizer.start, normalizer.end);
            
            // Lock the dissonance scale or use locked scale values unless the scale needs to be expanded
            // Might want to let dissonance values fall outside of the locked scale, we'll see...
            if (mapData[IDs::ScaleLocked].operator bool())
            {
                if (! mapData.hasProperty (IDs::ScaleMax)
                    || mapData[IDs::ScaleMax].operator float() < normalizer.end)
                    mapData.setProperty (IDs::ScaleMax, normalizer.end, nullptr);
                
                if (! mapData.hasProperty (IDs::ScaleMin)
                    || mapData[IDs::ScaleMin].operator float() > normalizer.start)
                    mapData.setProperty (IDs::ScaleMin, normalizer.start, nullptr);
                
                normalizer.start = mapData[IDs::ScaleMin];
                normalizer.end = mapData[IDs::ScaleMax];
            }
        }
        
        // Prevents attempting to draw within an invalid range
        // This should only happen when only a single partial is present (creates no dissonance by itself)
        if (normalizer.start == 0
            && normalizer.end == 0)
            return;
        
        float dissHeight;
        float nextDissHeight;
        
        g.setColour (Theme::activeText);
        
        // Draw the dissonance curve
        // (Inverted because (0, 0) is the top-left corner of the component)
        for (int i = 0; i < calc.getNumSteps() - 1; ++i)
        {
            dissHeight = normalizer.convertTo0to1 (calc.getDissonanceAtStep (i));
            dissHeight = abs (dissHeight - 1);
            dissHeight = denormalizer.convertFrom0to1 (dissHeight);
            
            nextDissHeight = normalizer.convertTo0to1 (calc.getDissonanceAtStep (i + 1));
            nextDissHeight = abs (nextDissHeight - 1);
            nextDissHeight = denormalizer.convertFrom0to1 (nextDissHeight);
            
            g.drawLine (i + 1, dissHeight, i + 2, nextDissHeight, 2);
        }
        
        // Draw mouse-over frequency and ratio boxes that show the frequency in Hz and as
        // a ratio to the start frequency at the location of the cursor
        if (isMouseOver()
            && getMouseXYRelative().getY() < getHeight() - 25
            && getMouseXYRelative().getX() > 0
            && getMouseXYRelative().getX() < getWidth() - 2)
        {
            Rectangle<int> freqBox (-1, getHeight() - 45, 80, 20);
            Rectangle<int> ratioBox (getWidth() - 50, getHeight() - 45,
                                     51, 20);
            
            g.setColour (Theme::mainBackground);
            g.fillRect (freqBox.withHeight (18));
            g.fillRect (ratioBox.withHeight (18));

            g.setColour (Theme::border);
            g.drawRect (freqBox);
            g.drawRect (ratioBox);

            g.setColour (Theme::text);
            g.drawText (String (calc.getFrequencyAtStep (getMouseXYRelative().getX() - 1), 1) + String (" Hz"),
                        freqBox.reduced (2), Justification::centred);
            g.drawText (String (calc.getFreqRatioAtStep (getMouseXYRelative().getX() - 1), 3),
                        ratioBox.reduced (2), Justification::centred);
        }
        
        // Draw map grid lines
        int gridLines = mapData.getParent()[IDs::GridLines];
        
        if (gridLines < 4)
        {
            g.drawLine (0, denormalizer.convertFrom0to1 (0.5),
                        getWidth(), denormalizer.convertFrom0to1 (0.5));
            
            if (gridLines < 3)
            {
                g.drawLine (0, denormalizer.convertFrom0to1 (0.25),
                            getWidth(), denormalizer.convertFrom0to1 (0.25));
                
                g.drawLine (0, denormalizer.convertFrom0to1 (0.75),
                            getWidth(), denormalizer.convertFrom0to1 (0.75));
                
                if (gridLines < 2)
                {
                    g.drawLine (0, denormalizer.convertFrom0to1 (0.125),
                                getWidth(), denormalizer.convertFrom0to1 (0.125));
                    
                    g.drawLine (0, denormalizer.convertFrom0to1 (0.375),
                                getWidth(), denormalizer.convertFrom0to1 (0.375));
                    
                    g.drawLine (0, denormalizer.convertFrom0to1 (0.625),
                                getWidth(), denormalizer.convertFrom0to1 (0.625));
                    
                    g.drawLine (0, denormalizer.convertFrom0to1 (0.875),
                                getWidth(), denormalizer.convertFrom0to1 (0.875));
                }
            }
        }
    }
}

void DissonanceMap::resized()
{
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> footer = area.removeFromBottom (25).expanded (1, 0);
    
    startFreq.setBounds (footer.removeFromLeft (75));
    dissonanceModel.setBounds (footer.removeFromLeft (150).expanded (1, 0));
    logSteps.setBounds (footer.removeFromLeft (100));
    lockScale.setBounds (footer.removeFromLeft (100));
    endRatio.setBounds (footer.removeFromRight (75));
    
    mapData.setProperty (IDs::NumSteps, getWidth() - 2, nullptr);
    
    denormalizer.start = 5;
    denormalizer.end = getHeight() - 30;
}

void DissonanceMap::comboBoxChanged (ComboBox* changedBox)
{
    if (changedBox == &dissonanceModel)
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
            if (mapData[IDs::StartFreq].operator float() > 0)
            {
                findParentComponentOfClass<MapList>()->undo->beginNewTransaction();
                
                mapData.setProperty (IDs::StartFreq,
                                     editor.getTextValue().getValue(),
                                     findParentComponentOfClass<MapList>()->undo);
            }
            else
            {
                mapData.setProperty (IDs::StartFreq,
                                     editor.getTextValue().getValue(),
                                     nullptr);
            }
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
            if (mapData[IDs::EndRatio].operator float() > 0)
            {
                findParentComponentOfClass<MapList>()->undo->beginNewTransaction();
                
                mapData.setProperty (IDs::EndRatio,
                                     editor.getTextValue().getValue(),
                                     findParentComponentOfClass<MapList>()->undo);
            }
            else
            {
                mapData.setProperty (IDs::EndRatio,
                                     editor.getTextValue().getValue(),
                                     nullptr);
            }
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
            findParentComponentOfClass<MapList>()->undo->beginNewTransaction();

            mapData.setProperty (IDs::StartFreq,
                                 editor.getTextValue(),
                                 findParentComponentOfClass<MapList>()->undo);
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
            findParentComponentOfClass<MapList>()->undo->beginNewTransaction();

            mapData.setProperty (IDs::EndRatio,
                                 editor.getTextValue(),
                                 findParentComponentOfClass<MapList>()->undo);
        }
        else
        {
            editor.setText (mapData[IDs::EndRatio].toString());
        }
    }
    
    unfocusAllComponents();
}

void DissonanceMap::buttonClicked (Button* clickedButton)
{
    if (clickedButton == &lockScale)
    {
        mapData.setProperty (IDs::ScaleLocked, lockScale.getToggleState(), nullptr);
        
        if (! lockScale.getToggleState())
        {
            mapData.removeProperty (IDs::ScaleMax, nullptr);
            mapData.removeProperty (IDs::ScaleMin, nullptr);
        }
    }
    else if (clickedButton == &logSteps)
    {
        mapData.setProperty (IDs::LogSteps, logSteps.getToggleState(), nullptr);
    }
}

void DissonanceMap::mouseMove (const MouseEvent& event)
{
    if (isMouseOver()
        && getMouseXYRelative().getY() < getHeight() - 25
        && getMouseXYRelative().getX() > 0
        && getMouseXYRelative().getX() < getWidth() - 2)
        repaint();
}

void DissonanceMap::valueTreeChildAdded (ValueTree& parent, ValueTree& newChild)
{
    if (newChild.hasType (IDs::OvertoneDistribution)
        && parent == mapData)
    {
        calc.addOvertoneDistribution (new OvertoneDistribution());
        
        OvertoneDistribution* dist = calc.getDistributionReference (parent.indexOf (newChild));
        
        /*
            If the new distribution's valuetree already has fundamental and
            partial data (ie, from duplicating another distribution or undo/redo),
            these will ensure that the data is set in the DisMAL data model.
        */
        if (newChild[IDs::FundamentalFreq].operator float() > 0
            && newChild[IDs::FundamentalAmp].operator float() > 0)
        {
            if (mapData.hasProperty (IDs::StartFreq)
                && newChild[IDs::FundamentalFreq].operator float() < 20)
            {
                dist->setFundamentalFreq (newChild[IDs::FundamentalFreq].operator float()
                                          * mapData[IDs::StartFreq].operator float());
            }
            else if (newChild[IDs::FundamentalFreq].operator float() > 0)
            {
                dist->setFundamentalFreq (newChild[IDs::FundamentalFreq]);
            }

            if (newChild[IDs::FundamentalAmp].operator float() > 0)
                dist->setFundamentalAmp (newChild[IDs::FundamentalAmp]);
        }
        
        for (auto subChild : newChild)
        {
            if (subChild.hasType (IDs::Partial)
                && subChild[IDs::Freq].operator float() > 0
                && subChild[IDs::Amp].operator float() > 0)
            {
                dist->addPartial (subChild[IDs::Freq],
                                  subChild[IDs::Amp]);
            }
        }
    }
    else if (newChild.hasType (IDs::Partial)
             && parent.hasType (IDs::OvertoneDistribution))
    {
        OvertoneDistribution* dist = calc.getDistributionReference (parent.getParent().indexOf (parent));
        
        // Checks if the new partial already has valuetree data to set in corresponding DisMAL data
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
    
    mapNeedsRedrawn = true;
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
    
    mapNeedsRedrawn = true;
    repaint();
}

/*  These callbacks set DisMAL data from the value tree data model.
 
    While DisMAL does the actual processing internally, the value tree data model is used
    in the app's MVC for all the functionalities it provides (undo management, callbacks, etc)
*/
void DissonanceMap::valueTreePropertyChanged (ValueTree& parent, const Identifier& ID)
{
    // So we don't have to redraw when just trying to view a distribution's partials
    if (ID == IDs::IsViewed)
        return;
    
    // Set the changed parameter in the DissonanceCalc object
    if (parent == mapData && ID == IDs::NumSteps)
    {
        if (mapData[ID].operator int() > 0)
            calc.setNumSteps (mapData[ID]);
    }
    else if (parent == mapData && ID == IDs::LogSteps)
    {
        calc.useLogarithmicSteps (mapData[ID].operator bool());
        
        logSteps.setToggleState (mapData[ID].operator bool(), dontSendNotification);
    }
    else if (parent == mapData && ID == IDs::StartFreq)
    {
        // Ranges require both start and end to init
        if (parent.hasProperty (IDs::EndRatio)
            && mapData[ID].operator float() > 0)
            calc.setRange (mapData[ID],
                           mapData[ID].operator float()
                           * mapData[IDs::EndRatio].operator float());
        
        // Update all distributions using a ratio to init their fundamental freq
        for (int i = 0; i < calc.numOvertoneDistributions(); ++i)
        {
            float fundamentalFreq = mapData.getChild (i)[IDs::FundamentalFreq];
            
            if (fundamentalFreq < 20
                && mapData[ID].operator float() > 0)
            {
                calc.getDistributionReference (i)->setFundamentalFreq (fundamentalFreq
                                                                       * mapData[ID].operator float());
            }
        }
        
        startFreq.setText (mapData[ID]);
    }
    else if (parent == mapData && ID == IDs::EndRatio)
    {
        // Ranges require both start and end to init
        if (parent.hasProperty (IDs::StartFreq)
            && mapData[ID].operator float() > 0)
            calc.setRange (mapData[IDs::StartFreq],
                           mapData[ID].operator float()
                           * mapData[IDs::StartFreq].operator float());
        
        endRatio.setText (mapData[ID]);
    }
    else if (parent == mapData && ID == IDs::ModelName)
    {
        // This will be modified when dynamic lib support is added
        if (mapData[ID] == "Sethares")
        {
            calc.setModel (new SetharesModel());
            dissonanceModel.setSelectedId (1);
        }
        else if (mapData[ID] == "Vassilakis")
        {
            calc.setModel (new VassilakisModel());
            dissonanceModel.setSelectedId (2);
        }
    }
    else if (parent == mapData && ID == IDs::ScaleLocked)
    {
        lockScale.setToggleState (mapData[ID].operator bool(), dontSendNotification);
    }
    else if (parent.hasType (IDs::OvertoneDistribution)
             || parent.hasType (IDs::Partial))
    {
        for (int i = 0; i < calc.numOvertoneDistributions(); ++i)
        {
            if (parent.hasType (IDs::OvertoneDistribution)
                && parent == mapData.getChild (i))
            {
                OvertoneDistribution* dist = calc.getDistributionReference (i);
                
                if (ID == IDs::XAxis
                    && parent[IDs::XAxis].operator bool() == true)
                {
                    calc.set2dVariableDistribution (i);
                }
                else if (ID == IDs::Mute)
                {
                    dist->mute (parent[IDs::Mute]);
                }
                else if (ID == IDs::Name)
                {
                    dist->setDistributionName (parent[IDs::Name]);
                }
                else if (ID == IDs::FundamentalFreq)
                {
                    if (mapData.hasProperty (IDs::StartFreq)
                        && parent[ID].operator float() < 20
                        && parent[ID].operator float() > 0)
                    {
                        dist->setFundamentalFreq (parent[ID].operator float()
                                                  * mapData[IDs::StartFreq].operator float());
                    }
                    else if (parent[ID].operator float() > 0)
                    {
                        dist->setFundamentalFreq (parent[ID]);
                    }
                }
                else if (ID == IDs::FundamentalAmp)
                {
                    if (parent[ID].operator float() > 0)
                        dist->setFundamentalAmp (parent[ID]);
                }
            }
            else if (parent.hasType (IDs::Partial)
                     && parent.getParent() == mapData.getChild (i))
            {
                OvertoneDistribution* dist = calc.getDistributionReference (i);
                
                if (ID == IDs::Freq && parent[ID].operator float() > 0)
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

    // Redraw the dissonance map (or remove the old map, if new data is needed)
    mapNeedsRedrawn = true;
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
    
    for (auto* map : maps)
    {
        map->setBounds (area.removeFromTop (mapHeight));
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
    findParentComponentOfClass<DissCalcView>()->repositionCalcPanel (newVisibleArea.getY());
}

//==============================================================================
DissMapComponentFooter::DissMapComponentFooter()
{
    gridLines.addItem ("lll", 1);
    gridLines.addItem ("ll", 2);
    gridLines.addItem ("l", 3);
    gridLines.addItem (" ", 4);
    addAndMakeVisible (gridLines);
    gridLines.addListener (this);
    gridLines.setTooltip ("Set grid lines");
    gridLines.setSelectedId (4);
}

DissMapComponentFooter::~DissMapComponentFooter()
{
}

void DissMapComponentFooter::paint (Graphics& g)
{
    g.fillAll (Theme::mainBackground);

    g.setColour (Theme::border);
    g.drawLine (0, 0, getWidth(), 0, 2);
}

void DissMapComponentFooter::resized()
{
    Rectangle<int> area = getLocalBounds();
    
    gridLines.setBounds (area.removeFromRight (50));
}

void DissMapComponentFooter::comboBoxChanged (ComboBox* changedBox)
{
    if (changedBox == &gridLines)
    {
        data.setProperty (IDs::GridLines, gridLines.getSelectedId(), nullptr);
    }
    
    getParentComponent()->repaint();
}

//==============================================================================
DissMapComponent::DissMapComponent()
{
    mapView.setViewedComponent (&maps, false);
    addAndMakeVisible (mapView);
    
    mapView.setScrollBarsShown (false, false, true, false);
    
    addAndMakeVisible (footer);
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
    Rectangle<int> area = getLocalBounds();
    Rectangle<int> foot = area.removeFromBottom (35);
    
    mapView.setBounds (area.withHeight (area.getHeight() + 1));
    
    if (maps.getHeight() < area.getHeight())
        maps.setBounds (area);
    
    maps.setSize (getWidth(), maps.getHeight());
    
    footer.setBounds (foot);
}

void DissMapComponent::setData (ValueTree& calcList, UndoManager& undo)
{
    maps.mapsData = calcList;
    maps.undo = &undo;
    
    footer.data = calcList;
}
