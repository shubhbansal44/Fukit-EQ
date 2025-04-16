/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FukitEQAudioProcessor::FukitEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

FukitEQAudioProcessor::~FukitEQAudioProcessor()
{
}

//==============================================================================
const juce::String FukitEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FukitEQAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FukitEQAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FukitEQAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FukitEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FukitEQAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int FukitEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FukitEQAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FukitEQAudioProcessor::getProgramName(int index)
{
    return {};
}

void FukitEQAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void FukitEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FukitEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FukitEQAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void FukitEQAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool FukitEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *FukitEQAudioProcessor::createEditor()
{
    // return new FukitEQAudioProcessorEditor(*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FukitEQAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FukitEQAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout FukitEQAudioProcessor::createParameterLayout()
{
    // The layout object that will store all the parameters for the plugin
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Add a parameter for the Low Cut Filter Frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "LowCut Freq",  // Unique parameter ID
        "LowCut Freq",  // Human-readable name
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), // Range from 20Hz to 20kHz with step 1Hz
        20.f            // Default value
    ));

    // Add a parameter for the High Cut Filter Frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "HighCut Freq",
        "HighCut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        20000.f
    ));

    // Add a parameter for the Peak Filter Frequency
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        750.f
    ));

    // Add a parameter for the Peak Filter Gain
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Gain",
        "Peak Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), // Gain from -24dB to +24dB
        0.0f
    ));

    // Add a parameter for the Peak Filter Quality (Q Factor)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Quality",
        "Peak Quality",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), // Q range from 0.1 to 10
        1.f
    ));

    // Create a string array for slope choices (used for low and high cut filters)
    juce::StringArray stringArray;
    for (int i = 0; i < 4; ++i)
    {
        juce::String str;
        str << (12 + i * 12) << " db/oct"; // e.g., "12 db/oct", "24 db/oct", etc.
        stringArray.add(str);
    }

    // Add a choice parameter for the Low Cut Slope
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "lowCut Slope",     // Parameter ID
        "lowCut Slope",     // Display name
        stringArray,        // Options: "12 db/oct", "24 db/oct", etc.
        0                   // Default index (0 = "12 db/oct")
    ));

    // Add a choice parameter for the High Cut Slope
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "highCut Slope",
        "highCut Slope",
        stringArray,
        0
    ));

    // Return the fully constructed parameter layout
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new FukitEQAudioProcessor();
}
