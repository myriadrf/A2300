This software update to the gnuradio installation will repair an issue that causes a startup
crash when parsing a path.

Directory: /home/chris/Projects/gnuradio/src/uhd/host/lib/utils/
File: paths.cpp
Issue: Static constructor causing incorrect initialization order and crash.
Error: Exception in static block load_modules
  basic_string::_S_construct null not valid
  
 
Founder: Mike Mathews
Report: Chris Center
