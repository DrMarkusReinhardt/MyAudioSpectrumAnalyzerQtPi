# MyAudioSpectrumAnalyserQtPi

Qt/Qwt based audio spectrum analyzer for RaspberryPi with Pisound board.

## Compilation on comand line:
     qmake
     make
 
## Compilation in QtCreator:
     -> press green execution button  / or compile button (before)
     
## Executables:
    Executable is found in subdirectory ./bin/
    Name of executable:   ./bin/MyAudioAnalyzerQtPi


## High level description:
### Signal sampling, spectrum calculation and display:
   Stereo audio signals are sampled with the Pisound board. In the main window sterep signal and spectrum plots are shown. 
   The signals are updated periodically, the magnitude spectra are calculated in dB and displayed.
   The number of samples displayed in the signal plot can be changed with a knob.
   The lower and upper limit of the frequency range used in the spectra can also be changed. 
   
### Peak spectrum values:
    The value and location in frequency domain of the peaks of both spectra are displayed and updated periodically.
       
### Activation/deactivation:   
   Both channels can be activated and deactivated.
   Overall processing can be stopped and restarted.
   
### THD calculation:   
   For both channels THD calculation is done periodically and the result displayed in the GUI.
   The THD calculation can be parameterized (base frequency and number of overtones used in the calculation).
