/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PingPongAudioProcessor::PingPongAudioProcessor() :
	AudioProcessor(
		BusesProperties()
			.withInput("Input",  juce::AudioChannelSet::stereo(), true)
			.withOutput("Output", juce::AudioChannelSet::stereo(), true)
	),
	params(apvts)
{
	lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
	highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}

PingPongAudioProcessor::~PingPongAudioProcessor()
{

}

//==============================================================================
const juce::String PingPongAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PingPongAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PingPongAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PingPongAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PingPongAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PingPongAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PingPongAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PingPongAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PingPongAudioProcessor::getProgramName (int index)
{
    return {};
}

void PingPongAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PingPongAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // reset parameters
    params.prepareToPlay(sampleRate);
    params.reset();
    
    /** Process Spec setup for juce::dsp **/
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;
    
    /** prepare juce::dsp objects **/
    // delayLine.prepare(spec);
    
    // allocate enough memory for maxDelayTime milliseconds
    double numSamples = (Parameters::maxDelayTime / 1000.0) * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
    
//    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
//    delayLine.reset();
    
    delayLineL.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineR.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineL.reset();
    delayLineR.reset();
    
    // Debugging statements for maxDelayInSamples -> should be 220500Hz for sample rate of 44100Hz
	DBG("Sample Rate: " << sampleRate << "Hz\n");
	DBG("Max Delay (samples): " << maxDelayInSamples << "Hz\n");
	
	feedbackL = 0.f;
	feedbackR = 0.f;
	
	lowCutFilter.prepare(spec);
	lowCutFilter.reset();
	
	highCutFilter.prepare(spec);
	highCutFilter.reset();
	
	lastLowCut = -1.f;
	lastHighCut = -1.f;
	
	tempo.reset();
	
	levelL.reset();
	levelR.reset();
	
	delayInSamples = 0.f;
	targetDelay = 0.f;
	xfade = 0.f;
	xfadeInc = float(1.0 / (0.05 * sampleRate)); // 50 ms
}

void PingPongAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PingPongAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
	const juce::AudioChannelSet mono = juce::AudioChannelSet::mono();
	const juce::AudioChannelSet stereo = juce::AudioChannelSet::stereo();
	const juce::AudioChannelSet mainIn = layouts.getMainInputChannelSet();
	const juce::AudioChannelSet mainOut = layouts.getMainOutputChannelSet();
	
	if (mainIn == mono && mainOut == mono) { return true; }
	if (mainIn == mono && mainOut == stereo) { return true; }
	if (mainIn == stereo && mainOut == stereo) { return true; }
	
	return false;
}

void PingPongAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    // update parameters
    params.update();
    
    // update tempo from playhead
    tempo.update(getPlayHead());
    
    float syncedTime = float(tempo.getMillisecondsForNoteLength(params.delayNote));
    if (syncedTime > Parameters::maxDelayTime) {
		syncedTime = Parameters::maxDelayTime;
	}
    
    float sampleRate = float(getSampleRate());
    
    // input
    juce::AudioBuffer<float> mainInput = getBusBuffer(buffer, true, 0);
    int mainInputChannels = mainInput.getNumChannels();
    bool isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);
    
    // output
    juce::AudioBuffer<float> mainOutput = getBusBuffer(buffer, false, 0);
    int mainOutputChannels = mainOutput.getNumChannels();
    bool isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);
    
    float maxL = 0.f;
    float maxR = 0.f;
    
    // stereo processing
    if (isMainOutputStereo) {
		for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
			// smoothen parameters
			params.smoothen();
			
			if (params.tempoSync) {
			
				if (xfade == 0.f) {
					targetDelay = syncedTime / 1000.f * sampleRate;
					
					// first time
					if (delayInSamples == 0.f) {
						delayInSamples = targetDelay;
					}
					
					// start cross fade
					else if (!juce::approximatelyEqual(targetDelay, delayInSamples)) {
						xfade = xfadeInc;
					}
				}
			} else {
				// set delay using delayTime
				delayInSamples = params.delayTime / 1000.f * sampleRate;
				// delayLine.setDelay(delay);
			}
			
			// set cutoff freqs
			if (!juce::approximatelyEqual(params.lowCut, lastLowCut)) {
				lowCutFilter.setCutoffFrequency(params.lowCut);
				lastLowCut = params.lowCut;
			}
			
			if (!juce::approximatelyEqual(params.highCut, lastHighCut)) {
				highCutFilter.setCutoffFrequency(params.highCut);
				lastHighCut = params.highCut;
			}
		
			// Use delayLine to push and pop samples
			float dryL = inputDataL[sample];
			float dryR = inputDataR[sample];
		
			// convert to mono
			float mono = (dryL + dryR) * 0.5f;
		
			// add feedback to dry mix
//			delayLine.pushSample(0, mono * params.panL + feedbackR);
//			delayLine.pushSample(1, mono * params.panR + feedbackL);
			
			delayLineL.write(mono * params.panL + feedbackR);
			delayLineR.write(mono * params.panR + feedbackL);
		
//			float wetL = delayLine.popSample(0);
//			float wetR = delayLine.popSample(1);
			
			float wetL = delayLineL.read(delayInSamples);
			float wetR = delayLineR.read(delayInSamples);

			// crossfading ??
			if (xfade > 0.f) {
				float newL = delayLineL.read(targetDelay);
				float newR = delayLineR.read(targetDelay);
				
				wetL = (1.f - xfade) * wetL + xfade * newL;
				wetR = (1.f - xfade) * wetR + xfade * newR;
				
				xfade += xfadeInc;
				
				// done fading...
				if (xfade >= 1.f) {
					delayInSamples = targetDelay;
					xfade = 0.f;
				}
			}
		
			// get feedback from wet mix
			feedbackL = wetL * params.feedback;
			feedbackL = lowCutFilter.processSample(0, feedbackL);
			feedbackL = highCutFilter.processSample(0, feedbackL);
			
			feedbackR = wetR * params.feedback;
			feedbackR = lowCutFilter.processSample(1, feedbackR);
			feedbackR = highCutFilter.processSample(1, feedbackR);
		
			// always 100% dry, 0-100% wet mixing
			float mixL = dryL + wetL * params.mix;
			float mixR = dryR + wetR * params.mix;

			// send effect mixing
//			float mixL = dryL * (1.f - params.mix) + wetL * params.mix;
//			float mixR = dryR * (1.f - params.mix) + wetR * params.mix;
		
			// write dry/wet delay signal to buffer with output gain
			float outL = mixL * params.gain;
			float outR = mixR * params.gain;
			
			if (params.bypassed) {
				outL = dryL;
				outR = dryR;
			}
			
			outputDataL[sample] = outL;
			outputDataR[sample] = outR;
			
			maxL = std::max(maxL, std::abs(outL));
			maxR = std::max(maxR, std::abs(outR));
		}
		
		levelL.updateIfGreater(maxL);
		levelR.updateIfGreater(maxR);
		
	// mono processing
	} else {
		for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
			params.smoothen();
			
			delayInSamples = params.delayTime / 1000.f * sampleRate;
			// delayLine.setDelay(delayInSamples);
			
			float dry = inputDataL[sample];
			// delayLine.pushSample(0, dry + feedbackL);
			
			delayLineL.write(dry + feedbackL);
			
			// float wet = delayLine.popSample(0);
			
			float wet = delayLineL.read(delayInSamples);
			
			feedbackL = wet * params.feedback;
			
			float mix = dry + wet * params.mix;
			outputDataL[sample] = mix * params.gain;
		}
	}
	
	#if JUCE_DEBUG
	protectYourEars(buffer);
	#endif
}

//==============================================================================
bool PingPongAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PingPongAudioProcessor::createEditor()
{
    return new ResizeablePingPongAudioProcessorEditor (*this);
}

//==============================================================================
void PingPongAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
    
    /** Debug statement for showing xml parameter data **/
    DBG(apvts.copyState().toXmlString());
}

void PingPongAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
		apvts.replaceState(juce::ValueTree::fromXml(*xml));
	}
}

juce::AudioProcessorParameter* PingPongAudioProcessor::getBypassParameter() const
{
	return params.bypassParam;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PingPongAudioProcessor();
}
