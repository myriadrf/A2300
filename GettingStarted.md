#Getting Started with the ASR-2300 and A2300 Project


Version: 1.0.0

Date: 12/2/2013

---


To get started using the ASR-2300, you will need to download and install the software drivers, examples, and tools from the [A2300 Project](https://github.com/myriadrf/A2300). The host drivers are tested on Linux, Mac, and Windows.

##Pre-requisites  

The A2300 supports UHD and GnuRadio functions if these capabilities are desired.  Please make sure you have downloaded and properly installed the packages on your system.  The A2300 cmake checks to see if they are installed and builds the adaptor libraries accordingly. 

For native C/C++ development, the A2300 API is available for development with projects for Eclipse and Visual Studio.

##Installation Using Git and CMake

Open a Terminal window and Enter the following commands.  

`https://github.com/myriadrf/A2300.git`

`cd A2300`

`mkdir build`

`cd build`
`cmake ..`

`UHD_MODULE_PATH=./host/lib/UhdAdaptor/libA2300UhdAdapter.dylib uhd_fft --args "type=a2300" -A Wideband -f 890.4M -s 1M --spec "A:0" -g 0 <click "Average"> <click "Autoscale" twice (or as needed)>`
`

##Building A2300 API and Tools Using Eclipse

TBD

##Building the A2300 API and Tools Using Visual Studio

TBS 

---
### Document Version Information ##
