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

`git clone https://github.com/myriadrf/A2300.git trunk`

`cd A2300`

`mkdir build`

`cd build`
`cmake ..`

`UHD_MODULE_PATH=./host/lib/UhdAdaptor/libA2300UhdAdapter.dylib uhd_fft --args "type=a2300" -A Wideband -f 890.4M -s 1M --spec "A:0" -g 0 <click "Average"> <click "Autoscale" twice (or as needed)>`
`

## Installing ASR-2300 Device Drivers and Access Permissions
 See Section 4 in the document "./host/docs/ASR-2300 GNU Software Document.pdf" This contains various procedures for configuring Linux and Windows 
 platforms to communicate with the ASR-2300 via USB interface.
 
 Procedures for Windows and Linux are somewhat different.  On Linux platforms, libusb is used, Cypress FX-3 driver is used on Windows platforms.


##Building A2300 API and Tools Using Eclipse

TBD

##Building the A2300 API and Tools Using Visual Studio

TBS 

---
### Document Version Information ##
