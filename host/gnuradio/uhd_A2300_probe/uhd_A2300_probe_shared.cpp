//
// Copyright 2010-2011 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Note: Local modifications for A2300 by Loctronix Corporation.


#include <uhd/utils/safe_main.hpp>
#include <uhd/version.hpp>
#include <uhd/device.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/property_tree.hpp>
#include <uhd/usrp/dboard_id.hpp>
#include <uhd/usrp/mboard_eeprom.hpp>
#include <uhd/usrp/dboard_eeprom.hpp>

#include <boost/algorithm/string.hpp> //for split
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

namespace po = boost::program_options;
using namespace uhd;

static std::string indent(size_t level){
    return (level)? (indent(level-1) + " ") : "";
}

static std::string make_border(const std::string &text){
    std::stringstream ss;
    ss << boost::format("  _____________________________________________________") << std::endl;
    ss << boost::format(" /") << std::endl;
    std::vector<std::string> lines; boost::split(lines, text, boost::is_any_of("\n"));
    while (lines.back().empty()) lines.pop_back(); //strip trailing newlines
    if (lines.size()) lines[0] = "    " + lines[0]; //indent the title line
    BOOST_FOREACH(const std::string &line, lines){
        ss << boost::format("|   %s") % line << std::endl;
    }
    //ss << boost::format(" \\_____________________________________________________") << std::endl;
    return ss.str();
}

static std::string prop_names_to_pp_string(const std::vector<std::string> &prop_names){
    std::stringstream ss; size_t count = 0;
    BOOST_FOREACH(const std::string &prop_name, prop_names){
        ss << ((count++)? ", " : "") << prop_name;
    }
    return ss.str();
}

static std::string get_device_pp_string(property_tree::sptr tree){
    std::stringstream ss;
    ss << boost::format("Device: %s") % (tree->access<std::string>("/name").get()) << std::endl;
    ss << std::endl;
    return ss.str();
}

/*
 * Ltx ASR-2300 property example.
 */
static std::string get_A2300_Identify_Info(property_tree::sptr tree)
{
	int iGainDb;

    std::stringstream ss;
    std::cout << "Device Information" << std::endl;
    ss << boost::format("Name  : %s") % (tree->access<std::string>("/name").get()) << std::endl;

    ss << boost::format("FPGA_0  : %s") % (tree->access<std::string>("/fpga_revision").get()) << std::endl;
    ss << boost::format("FPGA_1  : %s") % (tree->access<std::string>("/fpga_revision").get()) << std::endl;

    ss << boost::format("Rx0 Gain: %d") % (tree->access<int>("/rf/0/rx0/gain/value").get()) << std::endl;
    ss << boost::format("Tx0 Gain: %d") % (tree->access<int>("/rf/0/tx0/gain/value").get()) << std::endl;

    ss << boost::format("Rx0 Bandwidth: %d Hz") % (tree->access<int>("/rf/0/rx0/bandwidth/value").get()) << std::endl;
    ss << boost::format("Rx0 Frequency: %d Hz") % (tree->access<int>("/rf/0/rx0/frequency/value").get()) << std::endl;

    // Set/Get Gain...
    iGainDb = 12;
    tree->access<int>("/rf/0/rx0/gain/value").set(iGainDb);
    ss << boost::format("Rx0 Gain: %d") % (tree->access<int>("/rf/0/rx0/gain/value").get()) << std::endl;

    iGainDb = 18;
    tree->access<int>("/rf/0/rx0/gain/value").set(iGainDb);
    ss << boost::format("Rx0 Gain: %d") % (tree->access<int>("/rf/0/rx0/gain/value").get()) << std::endl;

    return ss.str();
}

void print_tree(const uhd::fs_path &path, uhd::property_tree::sptr tree){
    std::cout << path << std::endl;
    BOOST_FOREACH(const std::string &name, tree->list(path)){
        print_tree(path / name, tree);
    }
}
#include <dlfcn.h>

int UHD_SAFE_MAIN(int argc, char *argv[]){

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("version", "print the version string and exit")
        ("args", po::value<std::string>()->default_value(""), "device address args")
        ("tree", "specify to print a complete property tree")
        ("string", po::value<std::string>(), "query a string value from the properties tree")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);


    //print the help message
    if (vm.count("help")){
        std::cout << boost::format("UHD ASR-2300 Probe %s") % desc << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("version")){
        std::cout << uhd::get_version_string() << std::endl;
        return EXIT_SUCCESS;
    }

    std::string sargs = vm["args"].as<std::string>();

    device::sptr dev = device::make(sargs);
    property_tree::sptr tree = dev->get_tree();

    if (vm.count("string")){
        std::cout << tree->access<std::string>(vm["string"].as<std::string>()).get() << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("tree") != 0)
    	print_tree("/", tree);
    else
    {
    	// Output Loctronix examples.
    	std::cout << make_border( get_A2300_Identify_Info(tree)) << std::endl;
    }
    return EXIT_SUCCESS;
}
