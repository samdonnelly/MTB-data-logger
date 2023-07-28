# MTB Data Logger

This repository contains code for a mountain bike data logging system I built. The system attaches to my mountain bike and records data which is used to tune my suspension and get feedback on my riding style.

## Embedded Software

The embedded software runs on an STM32F4. This repository contain the application code for the project and it uses my <a href="https://github.com/samdonnelly/STM32F4-driver-library.git">STM32F4 library</a> for hardware and device drivers. 

Data gets recorded in real time and includes front and rear suspension position, wheel speed, bike orientation and acceleration, and geographic location. The system contains buttons for user control as well as LEDs and a screen for feedback. Data logs are saved on an SD card and can be exchanged between the system and an external device via Bluetooth. 

The base code for this project, such as register definitions, interrupt and fault handlers, main, etc., is generated by STMCubeMX. Most of the generated code resides in the <a href="https://github.com/samdonnelly/STM32F4-driver-library.git">STM32F4 library</a> and referenced as needed. Main can be found in the 'Core' folder and is used simply to call the application code for this project. Application code is located in the 'sources' and 'headers' folders. 

## Data Analysis

Data recorded from the system gets interpreted from scripts in the 'analysis' folder. The <a href="https://github.com/samdonnelly/MTB-data-logger/blob/main/analysis/mtbdl_data_analysis.py">mtbdl_data_analysis.py</a> script can be run to perform the analysis. All other scripts in the folder are supporting scripts and should not be run directly. 

A file_locations.py script needs to be created in the 'analysis' folder for each local copy of this repository. It is not included in the remote repository because directories will change between each machine. This script specifies the location of where raw data logs are stored, where formatted data is stored and where script analysis outputs can be saved to. Read the comments in the <a href="https://github.com/samdonnelly/MTB-data-logger/blob/main/analysis/mtbdl_data_analysis.py">mtbdl_data_analysis.py</a> script to see how these directories should be named. 


## Further Information

For details and documentation on the mechanical and electrical design and build of this project, visit my <a href="https://samueldonnelly11.wixsite.com/builds">project website</a>.
