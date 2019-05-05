/*
  ==============================================================================

    ThemedComponents.cpp
    Created: 24 Apr 2019 10:14:14pm
    Author:  Lina Reyne

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
}

ThemedButton::~ThemedButton()
{
}

void ThemedButton::paintButton (Graphics& g, bool drawAsHighlighted, bool drawAsPressed)
{
    if (! isEnabled())
    g.fillAll (Theme::buttonDisabled);
    else if (drawAsHighlighted)
    g.fillAll (Theme::buttonHighlighted);
    else
    g.fillAll (Theme::mainButton);
    
    g.setColour (getToggleState() ? Theme::activeText : Theme::text);
    g.setFont (fontSize);
    g.drawText (getButtonText(), getLocalBounds(), Justification::centred);
    
    g.setColour (Theme::border);
    
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

//==============================================================================
ThemedTextEditor::ThemedTextEditor()
{
    setColour (TextEditor::backgroundColourId, Theme::mainBackground);
    setColour (TextEditor::textColourId, Theme::text);
    setColour (TextEditor::highlightColourId, Theme::buttonHighlighted);
    setColour (TextEditor::highlightedTextColourId, Theme::text);
    setColour (TextEditor::outlineColourId, Theme::border);
    setColour (TextEditor::focusedOutlineColourId, Theme::buttonHighlighted);
    setColour (CaretComponent::caretColourId, Theme::activeText);
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

//==============================================================================
ThemedComboBox::ThemedComboBox()
{
    setColour (ComboBox::backgroundColourId, Theme::mainBackground);
    setColour (ComboBox::textColourId, Theme::text);
    setColour (ComboBox::buttonColourId, Theme::mainBackground);
    setColour (ComboBox::arrowColourId, Theme::text);
    setColour (ComboBox::outlineColourId, Theme::border);
    setColour (ComboBox::focusedOutlineColourId, Theme::buttonHighlighted);
    getLookAndFeel().setColour (PopupMenu::backgroundColourId, Theme::mainBackground);
    getLookAndFeel().setColour (PopupMenu::textColourId, Theme::text);
    getLookAndFeel().setColour (PopupMenu::headerTextColourId, Theme::text);
    getLookAndFeel().setColour (PopupMenu::highlightedBackgroundColourId, Theme::buttonHighlighted);
    getLookAndFeel().setColour (PopupMenu::highlightedTextColourId, Theme::text);
}

ThemedComboBox::~ThemedComboBox()
{
}
