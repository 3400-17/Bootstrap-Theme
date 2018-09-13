# Lab 2: Analog CIrcuitry and FFTs

### Objectives

* Learn how to make analog circuits and digital filter to interface with Arduino
* Use a microphone to detect whistle blow
* Use additional microphone to capture inputs from an IR sensor


### Teams
**Team 1**: (acoustic) 

**Team 2**: (optical)


## Internal Blink



We downloaded the newer FFT library from [this link.](http://wiki.openmusiclabs.com/wiki/ArduinoFFT)

After moving the FFT library folder inside Arduino libraries, we used the sample sketch to start coding. The input values from Analog Pin 0 was **value**, and the output frequency magnitude was **value**. 

For unit testing, we set up a signal denerator to deliver a signal to match what we epxected to see. We noticed that the frequency output from the signal generator matched with what we saw on the serial monitor, which was **value**. 


## Acoustic Team: Assemble your microphone circuit

The basic circuit for the electret microphone was the follows. 

<img src="https://cei-lab.github.io/ece3400-2018/images/lab2_fig1.png">

We generated a 660Hz tone, and measured the output from the microphone using with oscilloscope. 

## Addition circuitry

**unit tests**

**insert pictures**

## Override Button


## Optical Team: Assemble your IR circuit

Objective: be able to detect 12kHz IR beacon with an Arduino using the FFT library. 

We first looked up the phototransistor (OP598A) datasheet. 
## Measuring the frequency output of the treasure

**unit test**



