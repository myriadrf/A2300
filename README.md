A2300 Open Source Project
=====
This open source project provides access to the hardware designs, firmware, programmable FPGA logic, and host system drivers for the Loctronix ASR-2300 SDR/motion sensing module. The module is a multi-channel transceiver supporting RF reception and transmission in for different bands simultaneously between 300 MHz and 3.8 GHz. Nine RF paths are built into the board, making it easy to implement a complete radio solution for many popular UHF, cellular, ISM, and GPS bands.

This project provides the basic tools and APIs for developing open-source-based solutions utilizing the ASR-2300 module.


![ASR-2300](http://www.loctronix.com/images/ASR-2300-300w.png )
*ASR-2300 MIMO SDR / Motion Sensing Module*


## Device Specifications ##

For information regarding specifications or purchase of completed units, please visit [http://www.loctronix.com/datasheets/ASR/LTX-ASR2300-Datasheet.pdf.](http://www.loctronix.com/datasheets/ASR/LTX-ASR2300-Datasheet.pdf)

## Directory Description ##

- **common** – common C library shared between embedded firmware and host software. These provide standard communication routines for communicating with the ASR-23 device. Also included are common data types and utilities. This library is most commonly compiled as a static library and linked with the application or shared object needing access to the functionality. It is been tested in compiled on a variety of compilers including Windows, Linux, and armed compilers.

- **design** – directory contains high-level design documents and specifications permission for the ASR-2300 module and post APIs. Also put in this directory are development and testing information relating to the module.

- **firmware** – contains embedded firmware for the Renesas RL78 and Cypress FX3 processors. The RL78 microcontroller provides command control functions for the module and manages data flow for sensor and other low data rate information. The FX3 provides the superspeed USB 3.0 interface and can also be customized for conditional on board processing within the limits of available RAM.

- **hardware** – contains the hardware designs and other information needed to build and construct the various versions of the ASR-2300 module.

- **hdl** – contains the FPGA logic and supporting libraries for constructing both standard and custom DSP logic utilizing commons constructs that abstract some of the complexities of the hardware implementation.

- **host** – the host directory contains host processor APIs and libraries for accessing the ASR-2300 module via USB. Support is provided for both lightweight, embedded C/C++ development and GNU Radio development.

## Licensing ##
Unless otherwise specified, all software source files are licensed using the GNU General Public license version 2. All hardware related designs and source artifacts are licensed under the  “Creative Commons Attribution-ShareAlike 3.0 Unported”. To read the details of the license please review "license.html" or visit http://creativecommons.org/licenses/by-sa/3.0/legalcode.

For specific information see the COPYING.MD file(s).