# wt-synth
## Open Source, Wavetable Synthesizer

# Introduction 
Current full sized synthesizers are cumbersome, overpriced, and a full time commitment. It is no industry secret that electronic instruments, specifically the extremely valuable, versatile synthesizer can run over one thousand dollars.
wt-synth creates an open source synthesizer that is cheap enough for ametour musical enthusiasts. 

## Goals
* Open source to be available to everyone.
* The budget of hardware must be affordable for users to invest totally under 100 dollars.
* Be universal enough for a tech enthusiast to implement some of the required components
* Customization that allowed the user to adjust it to their musical taste via customizable instrument presets. 


## Installation Instructions 

### What is required to run _WaveSynth_?

## Required Hardware

* _Raspberry Pi 3b+_
    - This project was designed and tested on a Raspberry Pi 3b+, but any models beyond this should have no issues running the project. 

## Required Software
* _Python 3_
    - Necessary for running the USB MIDI Python file. If you don't have it installed, you can find it [here](https://www.python.org/download/releases/3.0/).
* _GCC Compiler_ 
    - Front end support for any .C files in the project (USB.c, DDS.c, etc..). If you don't have it installed, you can find it [here](https://gcc.gnu.org/).

## Pre-requisites 
* Raspberry Pi with Raspian Installed - To install the OS Raspian, follow their instructions [here](https://www.raspberrypi.org/documentation/installation/).

* 
### Installation Process

1. The three main pieces of software this project requires are _Raspian_, a _GCC Compiler_, and _Python_.
To install the most recent GCC compiler, download their program [here](https://gcc.gnu.org/).
To install the most recently version of python, download their program [here](https://www.python.org/downloads/).

2. _Gitclone_/download the _WaveSynth_ project from the Github repository. The method you choose to use is up to you! 
_Gitcloning_ will prompt you to download Github Desktop if you haven't already. After running their setup, it will prompt you for your Github login information. Once you've logged in, go to "Clone a repository from the Internet". You can either select a project you've interacted with, or in the upper bar of the "Clone a repository" pop-up window, select "URL". You can then input the URL of the project that you can get by pressing the green "Clone or Download" button in the master branch of the project code. 
Additionally, if you wish to simply download a ZIP of the project code, you can click that same green "Clone or Download" button and select "Download ZIP". 

When doing this from the terminal of the Pi, the command: 
`git clone [http address of source code]` 
will download the code into the current directory you are in. 


3. Install RTMidi for python file
From the terminal of the Raspberry Pi, enter the following line of code:
`$ pip install python-rtmidi`
This will install RTMidi, which the Python file requires to run.

4. Install ALSA Asoundlib
From the terminal of the Raspberry Pu, enter the following line of code:
`sudo apt-get install alsa-utils`
This will prompt installation for the ALSA Asoundlib, which has been configured to process and output the sound of the project. It will prompt you to enter your root password and ask if you wish to continue. Affirming these will begin the installation.

5. If you have Make installed, you can run our project file with that to make an executable, 
or
You can enter:
`gcc -g -o wavesynth wavesynth.c dds.c USB.c audio.c libs/json.c config/config.c -lpthread -lasound -lm`
This will compile the project files and give you an executable `WaveSynth` to use our project! 
