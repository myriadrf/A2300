#Getting Started with the ASR-2300 and A2300 Project


Version: 1.0.0

Date: 12/2/2013

---


To get started using the ASR-2300, you will need to download and install the software drivers, examples, and tools from the [A2300 Project](https://github.com/myriadrf/A2300). The host drivers are tested on Linux, Mac, and Windows.

For additional information please visit the links:
[A2300 Wiki] (http://myriadrf.org/wiki/home/projects/A2300)

##Pre-requisites  

The A2300 supports UHD and GnuRadio functions if these capabilities are desired.  Please make sure you have downloaded and properly installed the packages on your system.  The A2300 cmake checks to see if they are installed and builds the adaptor libraries accordingly. 

* [Installing GNU Radio](http://gnuradio.org/redmine/projects/gnuradio/wiki/InstallingGRFromSource)
* [Installing UHD] (http://code.ettus.com/redmine/ettus/projects/uhd/wiki/UHD_Build)


For native C/C++ development, the A2300 API is available for development with projects for Eclipse and Visual Studio.

##Installation Using Git and CMake

Open a Terminal window and Enter the following commands.  

`git clone https://github.com/myriadrf/A2300.git`

`cd A2300`

`mkdir build`

`cd build`
cd 
`cmake ..`

`UHD_MODULE_PATH=./host/lib/UhdAdaptor/libA2300UhdAdapter.dylib uhd_fft --args "type=a2300" -A Wideband -f 890.4M -s 1M --spec "A:0" -g 0 <click "Average"> <click "Autoscale" twice (or as needed)>`
`

## Installing ASR-2300 Device Drivers and Access Permissions
See Section 4 in the document "./host/docs/ASR-2300 GNU Software Document.pdf" This contains various procedures for configuring Linux and Windows 
platforms to communicate with the ASR-2300 via USB interface.
 
Procedures for Windows and Linux are somewhat different.  On Linux platforms, libusb is used, Cypress FX-3 driver is used on Windows platforms.


##Building A2300 API and Tools Using Eclipse

In a terminal, change to a directory that will contain the A2300 code.  Download the code `git clone https://github.com/myriadrf/A2300.git`.

Run Eclipse and create a workspace in the root `<installdir>/A2300`.  Note this assumes CDT package is installed for C/C++ development.

In the project explorer panel.  Right click and select import, select `General/Existing Projects into Workspace` option and click _next.

Specify the root directory as `<installdir>/A2300` or click browse to select the directory.

Select all the projects found and click finish.

Before you can build the files you will need to make sure that `<installdir>/A2300/host/include/A2300/libusb_ext.h` exists.  This can be generated automatically by running cmake in place or you can create the file 
from the source attached at the end of this document.

There should be at least 7 projects in the explorer tree. Under the project menu, select build all. If UHD was proprely installed, all seven projects should build and link without error.  


To execute the applications in the debugger you will need to define several environment variables:

### A2300 Identify, A2300RxToFile, A2300Update Debugger configuration

  _Environment Variables:
    
    `LD_LIBRARY_PATH` -- Define path to A2300USB shared object e.g. <installdir>/A2300/host/lib/A2300Usb/Debug  
    
### uhd_A2300_probe_shared

  _Environment Variables:
  
    `LD_LIBRARY_PATH` -- Define path to UHD if not in standard location.
    `UHD_MODULE_PATH` -- Define path to A2300 UhdAdaptor shared library e.g. <installdir>/A2300/host/lib/UhdAdaptor/Debug/libUhdAdaptor.so.

Browse to the <installdir>/A2300 and

##Building the A2300 API and Tools Using Visual Studio

TBS 

--
### Non-cmake, manually create lisusb_ext.h source
// Name: UsbDevice.h
//
// Copyright(c) 2013 Loctronix Corporation
// http://www.loctronix.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef LIBUSB_EXT_H_
#define LIBUSB_EXT_H_

// Using libusb on linux/osx (via libusb) and CyAPI on windows
#ifdef  __cplusplus
extern "C" {
#endif
#ifdef HAVE_LIBUSB
#include <libusb-1.0/libusb.h>
#elif defined(WIN32)
#include <Windows.h>
#include <CyAPI.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBUSB_EXT_H_ */





