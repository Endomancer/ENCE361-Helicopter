# **Helicopter control** - Thu_AM_group_1

*Designed and implemented by:*  
> Caitlin Bergervoet - 58922711  
> Owen Plant -  25494036  
> Narottam Royal - 35185738

- - -

## Project Description
This repository contains the C source code for the helicopter project in ENCE361. The project was based around the Tiva C Series  [TM4C123G LaunchPad](http://www.ti.com/tool/EK-TM4C123GXL) and the associated [Orbit BoosterPack](https://reference.digilentinc.com/orbit_boosterpack/orbit_boosterpack).

![Helicopter](docs/heliboi.png)

The design uses ramping functions for the PI controllers to control the main and tail rotors of the helicopter. Other design features include gain clamping, ADC sampling and averaging, Quadrature decoding, and UART and OLED capabilities. 

Several existing [libraries](https://eng-git.canterbury.ac.nz/nro55/tiva-libraries) for the Tiva C series were used to aid in implementation:

* TivaWare
* OrbitOLED
* FreeRTOS
* TraceRecorder

## Continuous Integration
GitLab continuous integration is supported using [CMake](https://cmake.org/).