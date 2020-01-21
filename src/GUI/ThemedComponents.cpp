/*
  ==============================================================================
 
    This file is part of the Psychotonal CAT (Composition and Analysis Tools) app
    Copyright (c) 2019 - Spectral Discord
    http://spectraldiscord.com
 
    This program is provided under the terms of GPL v3
    https://opensource.org/licenses/GPL-3.0
 
  ==============================================================================
*/

#include "ThemedComponents.h"

//==============================================================================
ThemedButton::ThemedButton()
{
    setColour (0x1000102, Theme::text);
    setColour (0x1000103, Theme::activeText);
    setButtonText ("+");
    
    setClickingTogglesState (false);
    setToggleState (false, dontSendNotification);
    
    leftBorder = false;
    rightBorder = false;
    topBorder = false;
    bottomBorder = false;
    borderWidth = 1;
    
    fontSize = 14;
    useIcon = false;
    iconSize = 14;
    
    rotation = 0;
    panelButton = false;
}

ThemedButton::~ThemedButton()
{
}

void ThemedButton::paintButton (Graphics& g, bool drawAsHighlighted, bool drawAsPressed)
{
    if (panelButton)
        g.fillAll (Theme::headerBackground);
    else if (! isEnabled())
        g.fillAll (Theme::buttonDisabled);
    else if (drawAsHighlighted)
        g.fillAll (getToggleState() ? Theme::mainButton : Theme::buttonHighlighted);
    else if (getToggleState())
        g.fillAll (Theme::buttonHighlighted);
    else
        g.fillAll (Theme::mainButton);
    
    Colour color;
    
    if (drawAsHighlighted)
    {
        if (getToggleState())
            color = Theme::text;
        else if (panelButton)
        {
            if (icon == FontAwesome_WindowClose
                || icon == FontAwesome_TimesCircle
                || icon == FontAwesome_Backspace)
            {
                color = Theme::mainBackground;
                g.fillRect (getLocalBounds().reduced (getHeight() / 3.5, getHeight() / 4));
            }
            else if (icon == FontAwesome_CheckCircle)
            {
                g.fillRect (getLocalBounds().reduced (getHeight() / 4, getHeight() / 4));
            }
            
            color = Theme::buttonHighlighted;
        }
        else
            color = Theme::mainBackground;
    }
    else if (getToggleState() || panelButton)
        color = Theme::mainBackground;
    else
        color = Theme::text;
    
    if (useIcon && icon == FontAwesome_Backspace)
    {
        String backspace ("M576 64H205.26A63.97 63.97 0 0 0 160 82.75L9.37 233.37c-12.5 12.5-12.5 32.76 0 45.25L160 429.25c12 12 28.28 18.75 45.25 18.75H576c35.35 0 64-28.65 64-64V128c0-35.35-28.65-64-64-64zm-84.69 254.06c6.25 6.25 6.25 16.38 0 22.63l-22.62 22.62c-6.25 6.25-16.38 6.25-22.63 0L384 301.25l-62.06 62.06c-6.25 6.25-16.38 6.25-22.63 0l-22.62-22.62c-6.25-6.25-6.25-16.38 0-22.63L338.75 256l-62.06-62.06c-6.25-6.25-6.25-16.38 0-22.63l22.62-22.62c6.25-6.25 16.38-6.25 22.63 0L384 210.75l62.06-62.06c6.25-6.25 16.38-6.25 22.63 0l22.62 22.62c6.25 6.25 6.25 16.38 0 22.63L429.25 256l62.06 62.06z");
        
        Path path = Drawable::parseSVGPath(backspace);
        auto tr = path.getTransformToScaleToFit(getLocalBounds().toFloat(), false, Justification::centred);
        path.applyTransform(tr);
        g.setColour(color);
        g.fillPath(path);
    }
    else if (useIcon && icon == FontAwesome_FolderOpenO)
    {
        FontAwesome::drawAt (g, icon, iconSize, color, 4, 4);
    }
    else if (useIcon)
    {
        FontAwesome::drawCenterdRotated (g, icon, iconSize, color, getLocalBounds(), rotation);
    }
    else
    {
        g.setColour (color);
        g.setFont (fontSize);
    
        Font font = g.getCurrentFont();
        font.setBold (true);
        g.setFont (font);
        
        g.drawText (getButtonText(), getLocalBounds(), Justification::centred);
    }
    
    g.setColour (Theme::headerBackground);
    
    if (leftBorder == true)     g.drawLine (0, 0, 0, getHeight(), borderWidth);
    if (rightBorder == true)    g.drawLine (getWidth(), 0, getWidth(), getHeight(), borderWidth);
    if (topBorder == true)      g.drawLine (0, 0, getWidth(), 0, borderWidth);
    if (bottomBorder == true)   g.drawLine (0, getHeight(), getWidth(), getHeight(), borderWidth);
}

void ThemedButton::resized()
{
}

void ThemedButton::setBorders (bool left, bool top, bool right, bool bottom, int width)
{
    leftBorder = left;
    topBorder = top;
    rightBorder = right;
    bottomBorder = bottom;
    borderWidth = width;
}

void ThemedButton::setFontSize (float newFontSize)
{
    fontSize = newFontSize;
}

void ThemedButton::setPanelButton (bool isPanelButton)
{
    panelButton = isPanelButton;
}

void ThemedButton::setIcon (bool setUseIcon, Icon newIcon, float newRotation)
{
    useIcon = setUseIcon;
    icon = newIcon;
    rotation = newRotation;
    repaint();
}

void ThemedButton::setIconSize (float newIconSize)
{
    iconSize = newIconSize;
}

//==============================================================================
ThemedTextEditor::ThemedTextEditor()
{
    setColour (TextEditor::backgroundColourId, Theme::mainBackground);
    setColour (TextEditor::textColourId, Theme::text);
    setColour (TextEditor::highlightColourId, Theme::buttonHighlighted);
    setColour (TextEditor::highlightedTextColourId, Theme::text);
    setColour (TextEditor::outlineColourId, Theme::headerBackground);
    setColour (TextEditor::focusedOutlineColourId, Theme::buttonHighlighted);
    setColour (CaretComponent::caretColourId, Theme::activeText);
    
    setInputRestrictions (30, String ("abcdefghijklmnopqrstuvwxyz1234567890")
                          + String ("ABCDEFGHIJKLMNOPQRSTUVWXYZ _-'"));
}

ThemedTextEditor::~ThemedTextEditor()
{
}

void ThemedTextEditor::setBordered (bool border)
{
    setColour (TextEditor::outlineColourId,
               border ? Theme::border : Colour (0x00000000));

    setColour (TextEditor::focusedOutlineColourId,
               border ? Theme::buttonHighlighted : Colour (0x00000000));
}

void ThemedTextEditor::setEvaluateExpressions (bool evaluate)
{
    evaluateExpressions = evaluate;
    
    if (evaluate)
    {
        setInputRestrictions (10, "1234567890.+-*/()^");
    }
    else
    {
        setInputRestrictions (30, String ("abcdefghijklmnopqrstuvwxyz1234567890")
                              + String ("ABCDEFGHIJKLMNOPQRSTUVWXYZ _-'"));
        expression = "";
    }
}

double ThemedTextEditor::getEvaluated()
{
    if (! getText().isEmpty() && evaluateExpressions == true && getText().containsOnly ("1234567890.+-*/ "))
    {
        String error;
        Expression exp (getText(), error);
        
        if (error.isEmpty())
        {
            double result = exp.evaluate(Expression::Scope(), error);
            
            if (error.isEmpty() && result > 0)
            {
                expression = getText();
                
                return result;
            }
        }
    }
    
    return -1;
}

//==============================================================================
ThemedComboBox::ThemedComboBox()
{
    setColour (ComboBox::backgroundColourId, Theme::mainBackground);
    setColour (ComboBox::textColourId, Theme::text);
    setColour (ComboBox::buttonColourId, Theme::mainBackground);
    setColour (ComboBox::arrowColourId, Theme::text);
    setColour (ComboBox::outlineColourId, Theme::headerBackground);
    setColour (ComboBox::focusedOutlineColourId, Theme::buttonHighlighted);
    getLookAndFeel().setColour (PopupMenu::backgroundColourId, Theme::mainBackground);
    getLookAndFeel().setColour (PopupMenu::textColourId, Theme::text);
    getLookAndFeel().setColour (PopupMenu::headerTextColourId, Theme::text);
    getLookAndFeel().setColour (PopupMenu::highlightedBackgroundColourId, Theme::headerBackground);
    getLookAndFeel().setColour (PopupMenu::highlightedTextColourId, Theme::text);
}

ThemedComboBox::~ThemedComboBox()
{
}

//==============================================================================
ThemedToggleButton::ThemedToggleButton()
{
    setColour (ToggleButton::textColourId, Theme::text);
    setColour (ToggleButton::tickColourId, Theme::activeText);
    setColour (ToggleButton::tickDisabledColourId, Theme::text);
}

ThemedToggleButton::~ThemedToggleButton()
{
    setLookAndFeel (nullptr);
}
