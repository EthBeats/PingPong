/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 6 Jul 2024 11:30:20am
    Author:  Ethan Miller

  ==============================================================================
*/

#include "LookAndFeel.h"

//==============================================================================
// read ttf file from BinaryData into a typeface pointer
const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::LatoMedium_ttf,
																					BinaryData::LatoMedium_ttfSize);
juce::Font Fonts::getFont(float height)
{
	// Font(typeface) is depreciated in JUCE 8 -> use FontOptions instead
	// add legacy metrics for JUCE 7 style for now...
	return juce::FontOptions(typeface).withHeight(height)
									  .withMetricsKind(juce::TypefaceMetricsKind::legacy);
}

//==============================================================================
RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
	// set up custom colors
	setColour(juce::Label::textColourId, Colors::Knob::label);
	setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
	setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
	setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
	setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
}

RotaryKnobLookAndFeel::~RotaryKnobLookAndFeel()
{
}

//==============================================================================
RotaryKnobLookAndFeel* RotaryKnobLookAndFeel::get()
{
	// get the address of this instance
	static RotaryKnobLookAndFeel instance;
	return &instance;
}

void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics &g,
											 int x, int y,
											 int width, [[maybe_unused]] int height,
											 float sliderPos, const float rotaryStartAngle,
											 const float rotaryEndAngle, juce::Slider &slider)
{
	juce::Rectangle<float> bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
	juce::Rectangle<float> knobRect = bounds.reduced(10.f, 10.f);
	
	// draw drop shadow along path (expand bounds to get rid of weird lines)
	juce::Path path;
	path.addEllipse(knobRect.expanded(1.f, 3.f));
	dropShadow.drawForPath(g, path);
	
	// draw knob outline
	g.setColour(Colors::Knob::outline);
	g.fillEllipse(knobRect);
	
	// fill knob gradient
	juce::Rectangle<float> innerRect = knobRect.reduced(2.0f, 2.0f);
	juce::ColourGradient gradient { Colors::Knob::gradientTop, 0.0f,
									innerRect.getY(),
									Colors::Knob::gradientBottom, 0.0f,
									innerRect.getBottom(), false };
	g.setGradientFill(gradient);
	g.fillEllipse(innerRect);
	
	// draw track around knob
	juce::Point<float> center = bounds.getCentre();
	float radius = bounds.getWidth() / 2.f;
	float lineWidth = 3.f;
	float arcRadius = radius - lineWidth / 2.f;
	
	juce::Path backgroundArc;
	backgroundArc.addCentredArc(center.x, center.y,
								arcRadius, arcRadius, 0.f,
								rotaryStartAngle, rotaryEndAngle, true);
	
	juce::PathStrokeType strokeType { lineWidth,
									  juce::PathStrokeType::curved,
									  juce::PathStrokeType::rounded };
	g.setColour(Colors::Knob::trackBackground);
	g.strokePath(backgroundArc, strokeType);
	
	// draw the knob dial
	float dialRadius = innerRect.getHeight() / 2.f - lineWidth;
	float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
	
	// JUCE considers 0 degrees at the top of the circle so everything is shifted by -pi/2
	juce::Point<float> dialStart { center.x + 10.f * std::cos(toAngle - juce::MathConstants<float>::halfPi),
								   center.y + 10.f * std::sin(toAngle - juce::MathConstants<float>::halfPi) };
	juce::Point<float> dialEnd { center.x + dialRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
								 center.y + dialRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi) };
								 
	juce::Path dialPath;
	dialPath.startNewSubPath(dialStart);
	dialPath.lineTo(dialEnd);
	g.setColour(Colors::Knob::dial);
	g.strokePath(dialPath, strokeType);
	
	// color in the knob track
	if (slider.isEnabled()) {
		float fromAngle = rotaryStartAngle;
		if (slider.getProperties()["drawFromMiddle"]) {
			fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.f;
		}
	
		juce::Path valueArc;
		valueArc.addCentredArc(center.x, center.y,
							   arcRadius, arcRadius, 0.f,
							   fromAngle, toAngle, true);
		g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
		g.strokePath(valueArc, strokeType);
	}
}

juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label &label)
{
	// get custom font and set to all labels
	return Fonts::getFont();
}

//==============================================================================
/**
*/
class RotaryKnobLabel  : public juce::Label
{
public:
	RotaryKnobLabel() : juce::Label() { }
	
	//==============================================================================
	void mouseWheelMove(const juce::MouseEvent&,
						const juce::MouseWheelDetails&) override { }
	
	std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
	{
		return createIgnoredAccessibilityHandler(*this);
	}
	
	juce::TextEditor* createEditorComponent() override
	{
		// customize text editor component
		juce::TextEditor* ed = new juce::TextEditor(getName());
		
		ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
		copyAllExplicitColoursTo(*ed);
		
		ed->setBorder(juce::BorderSize<int> { });
		ed->setIndents(2, 1);
		ed->setJustification(juce::Justification::centredTop);
		
		ed->setPopupMenuEnabled(false);
		ed->setInputRestrictions(8);
		
		return ed;
	}
	
};

juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider &slider)
{
	// set up new RotaryKnobLabel class
	juce::Label* l = new RotaryKnobLabel();
	
	l->setJustificationType(juce::Justification::centred);
	l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
	
	l->setColour(juce::Label::textColourId,
				 slider.findColour(juce::Slider::textBoxTextColourId));
	l->setColour(juce::TextEditor::textColourId, Colors::Knob::value);
	l->setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
	l->setColour(juce::TextEditor::highlightColourId,
				 slider.findColour(juce::Slider::rotarySliderFillColourId));
	l->setColour(juce::TextEditor::backgroundColourId,
				 Colors::Knob::textBoxBackground);
				 
	return l;
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(juce::Graphics &g,
													 [[maybe_unused]] int width,
													 [[maybe_unused]] int height,
													 juce::TextEditor &textEditor)
{
	// make the text editor background rounded
	g.setColour(Colors::Knob::textBoxBackground);
	g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(4, 0).toFloat(), 4.f);
}

//==============================================================================
MainLookAndFeel::MainLookAndFeel()
{
	// set up custom colors
	setColour(juce::GroupComponent::textColourId, Colors::Group::label);
	//setColour(juce::GroupComponent::outlineColourId, Colors::Group::outline);
}

MainLookAndFeel::~MainLookAndFeel()
{
}

//==============================================================================
void MainLookAndFeel::drawGroupComponentOutline (juce::Graphics& g, int width, int height,
												 const juce::String& text,
												 const juce::Justification& position,
												 juce::GroupComponent& group)
{
	const float textH = 16.0f;
    const float indent = 3.0f;
    const float textEdgeGap = 5.0f;
    auto cs = 5.0f;

    juce::Font f (withDefaultMetrics (juce::FontOptions { textH }));

    juce::Path p;
    auto x = indent;
    auto y = f.getAscent() - 3.0f;
    auto w = juce::jmax (0.0f, (float) width - x * 2.0f);
    auto h = juce::jmax (0.0f, (float) height - y  - indent);
    cs = juce::jmin (cs, w * 0.5f, h * 0.5f);
    auto cs2 = 2.0f * cs;

    auto textW = text.isEmpty() ? 0
                                : juce::jlimit (0.0f,
                                          juce::jmax (0.0f, w - cs2 - textEdgeGap * 2),
                                          (float) f.getStringWidth (text) + textEdgeGap * 2.0f);
    auto textX = cs + textEdgeGap;

    if (position.testFlags (juce::Justification::horizontallyCentred))
        textX = cs + (w - cs2 - textW) * 0.5f;
    else if (position.testFlags (juce::Justification::right))
        textX = w - cs - textW - textEdgeGap;

    p.startNewSubPath (x + textX + textW, y);
    p.lineTo (x + w - cs, y);

    p.addArc (x + w - cs2, y, cs2, cs2, 0, juce::MathConstants<float>::halfPi);
    p.lineTo (x + w, y + h - cs);

    p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, juce::MathConstants<float>::halfPi, juce::MathConstants<float>::pi);
    p.lineTo (x + cs, y + h);

    p.addArc (x, y + h - cs2, cs2, cs2, juce::MathConstants<float>::pi, juce::MathConstants<float>::pi * 1.5f);
    p.lineTo (x, y + cs);

    p.addArc (x, y, cs2, cs2, juce::MathConstants<float>::pi * 1.5f, juce::MathConstants<float>::twoPi);
    p.lineTo (x + textX, y);

    auto alpha = group.isEnabled() ? 1.f : 0.5f;

//    g.setColour (group.findColour (juce::GroupComponent::outlineColourId)
//                    .withMultipliedAlpha (alpha));
	g.setGradientFill(juce::ColourGradient::vertical(Colors::Group::outlineTop.withMultipliedAlpha(alpha), 0, Colors::Group::outlineBottom.withMultipliedAlpha(alpha), height));

    g.strokePath (p, juce::PathStrokeType (1.0f));

    g.setColour (group.findColour (juce::GroupComponent::textColourId)
                    .withMultipliedAlpha (alpha));
    g.setFont (f);
    g.drawText (text,
                juce::roundToInt (x + textX), 0,
                juce::roundToInt (textW),
                juce::roundToInt (textH),
                juce::Justification::centred, true);
}

juce::Font MainLookAndFeel::getLabelFont([[maybe_unused]] juce::Label &label)
{
	// get custom font and set to all labels
	return Fonts::getFont();
}

//==============================================================================
ButtonLookAndFeel::ButtonLookAndFeel()
{
	setColour(juce::TextButton::ColourIds::textColourOffId, Colors::Button::text);
	setColour(juce::TextButton::ColourIds::textColourOnId, Colors::Button::textToggled);
	setColour(juce::TextButton::ColourIds::buttonColourId, Colors::Button::background);
	setColour(juce::TextButton::ColourIds::buttonOnColourId, Colors::Button::backgroundToggled);
}

ButtonLookAndFeel::~ButtonLookAndFeel()
{
}

//==============================================================================
ButtonLookAndFeel* ButtonLookAndFeel::get()
{
	// get the address of this instance
	static ButtonLookAndFeel instance;
	return &instance;
}

void ButtonLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &button,
											 const juce::Colour &backgroundColour,
											 [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
											 bool shouldDrawButtonAsDown)
{
	juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
	float cornerSize = bounds.getHeight() * 0.25f;
	juce::Rectangle<float> buttonRect = bounds.reduced(1.f, 1.f).withTrimmedBottom(1.f);
	
	if (shouldDrawButtonAsDown) {
		buttonRect.translate(0.f, 1.f);
	}
	
	g.setColour(backgroundColour);
	g.fillRoundedRectangle(buttonRect, cornerSize);
	
	g.setColour(Colors::Button::outline);
	g.drawRoundedRectangle(buttonRect, cornerSize, 2.f);
}

void ButtonLookAndFeel::drawButtonText(juce::Graphics &g, juce::TextButton &button,
									   [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
									   bool shouldDrawButtonAsDown)
{
	juce::Rectangle<float> bounds = button.getLocalBounds().toFloat();
	juce::Rectangle<float> buttonRect = bounds.reduced(1.f, 1.f).withTrimmedBottom(1.f);
	
	if (shouldDrawButtonAsDown) {
		buttonRect.translate(0.f, 1.f);
	}
	
	if (button.getToggleState()) {
		g.setColour(button.findColour(juce::TextButton::ColourIds::textColourOnId));
	} else {
		g.setColour(button.findColour(juce::TextButton::ColourIds::textColourOffId));
	}
	
	g.setFont(Fonts::getFont());
	g.drawText(button.getButtonText(), buttonRect, juce::Justification::centred);
}
