This repository is for the Frequency Limiter, an experimental VST3 plugin using the JUCE framework.

The plugin works by applying a thresholding function to the frequency magnitude of the audio signal.
Whenever the magnitude is above a given threshold, the frequency band is limited. The plugin performs
the calculations on a sidechain which can be windowed by a few different windowing functions.
When the sidechain exceeds the threshold, the actual signal frequency spectrum is affected.

The plugin provides controls for the threshold, dry/wet mix, output gain and the choice of windowing function.

This plugin relies on the [KissFFT Library](https://github.com/mborgerding/kissfft)

The software is licensed under GPL v3.