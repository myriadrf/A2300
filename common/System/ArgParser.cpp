/** Name: ArgParser.cpp
*
* Copyright(c) 2013 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "ArgParser.hpp"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#if defined(LINUX) || defined(APPLE)
	#include "std/strtools.h"
#endif

/** Legal Boolean values*/
#define ENABLED "true"
#define DISABLE "false"

using namespace std;
using namespace A2300;

void WriteEntryFields(ArgParser::Entry& entry);

/**
* Adds a Parameter to the class with the specified order identifier.
*/
void A2300::ArgParser::AddParameter(  const std::string& sId, const std::string& sDefaultValue, 
	Entry::DataTypeEnum dType, const std::string& sDescription,
	const std::string& sGroup )
{
	Entry e(Entry::Parameter, dType, sId, sDefaultValue, sDescription, sGroup);
	e.idxParam = m_ctParameters;
	m_map[ e.keyName ] = e;
	++m_ctParameters;
}

/**
* Method adds a switch definition to the argument parser instances.
*/
void A2300::ArgParser::AddSwitch( const std::string& sSwitchID, bool bEnabled,
	const std::string& sDescription, const std::string& sGroup )
{
	Entry e(Entry::Switch, Entry::BOOL,sSwitchID, ((bEnabled) ? "true" : "false"),	sDescription, sGroup);
	m_map[ e.keyName ] = e;
	++m_ctSwitches;
}

/**
* Method adds a variable definition to the argument parser instance.
*/
void A2300::ArgParser::AddVariable(const std::string& sVariableID, const std::string& sDefaultValue, 
	Entry::DataTypeEnum dType, const std::string& sDescription, const std::string& sGroup )
{
	Entry e(Entry::Variable, dType, sVariableID, sDefaultValue, sDescription, sGroup);
	m_map[ e.keyName ] = e;
	++m_ctVariables;
}

/**
* Method returns the entry associated with the specified key.
*/
A2300::ArgParser::Entry&	A2300::ArgParser::GetEntry( const std::string& sKey )
{
	EntryMap::iterator pos;
	pos = m_map.find(sKey);

	if( pos == m_map.end() )
	{
		std::string smsg = "Argument not found: " + sKey;
		ArgParserException e(smsg);
		throw e;
	}
	else
		return pos->second;
}

/** 
* Method returns the entry associated with the specified parameter index.
*/
A2300::ArgParser::Entry&	A2300::ArgParser::GetEntry( int idxParam  )
{
	EntryMap::iterator pos;
	for( pos = m_map.begin(); pos != m_map.end(); ++pos)
	{
		Entry& e = pos->second;
		if( e.entryType == Entry::Parameter 
			&& e.idxParam == idxParam)
		{
			return e;
		}
	}

	std::stringstream smsg("Parameter Index not found: ");
	smsg << idxParam;
	ArgParserException ex(smsg.str());
	throw ex;
}

/**
* Method returns boolean value at the specified key.
*/
bool A2300::ArgParser::GetBool( const std::string& sKey )
{
	std::string s = GetString( sKey );
	const char* psz = s.c_str();
	return _stricmp( psz, ENABLED) == 0 
		|| _stricmp( psz, "true") == 0;  // Perfect match return true.
}

/**
* Method returns the long value at the specified key.
*/
int32 A2300::ArgParser::GetLong( const std::string& sKey )
{
	std::string s = GetString(sKey );
	return (int32)(atol( s.c_str() ));
}

/**
* Method returns the double value at the specified key.
*/
double A2300::ArgParser::GetDouble( const std::string& sKey )
{
	std::string s = GetString(sKey );
	return (double)(atof( s.c_str() ));
}

/**
* Method returns true if are is defind.
*/
bool A2300::ArgParser::IsDefined(  const std::string& sKey)
{
	EntryMap::iterator pos;
	pos = m_map.find(sKey);

	return (pos  != m_map.end());
}

/**
* Method returns the entry type.
*/
A2300::ArgParser::Entry::EntryTypeEnum A2300::ArgParser::EntryType(  const std::string& sKey)
{
	EntryMap::iterator pos;
	pos = m_map.find(sKey);
	return (pos  != m_map.end())? pos->second.entryType : ArgParser::Entry::Undefined;
}


/**
* Method processes ArgParser Definition Map Entries adding them to the map.
*/
void A2300::ArgParser::AddMapEntries(const A2300::_ArgParserDefMap* entries,
										   const std::string& sGroup, const char* pszScopeKey  )
{
	//Initialize Argument parser
	int ct = 0;

	while( _stricmp( entries[ct].pszID, ARGPARSER_END_TOKEN) != 0)
	{
		//Add Scope key if defined
		std::string sid;
		sid = (pszScopeKey != NULL)
			? ((std::string) pszScopeKey) + ((std::string)".") + ((std::string)entries[ct].pszID)
			: entries[ct].pszID;

		//Add the specified entry type.
		switch( entries[ct].eType)
		{
		case ArgParser::Entry::Switch:
			{
				AddSwitch( sid, _stricmp(entries[ct].pszDefault, "true")== 0,
					entries[ct].pszDescription, sGroup);
			}
			break;
		case ArgParser::Entry::Variable:
			{
				AddVariable( sid, entries[ct].pszDefault,
					entries[ct].eData, entries[ct].pszDescription, sGroup );
			}
			break;
		case ArgParser::Entry::Parameter:
			{
				AddParameter(sid, entries[ct].pszDefault, entries[ct].eData, entries[ct].pszDescription, sGroup );
			}
			break;
		default:
			throw ArgParserException("Invalid ArgParser Map Entry?");
		}

		ct++;
	}


}

/**
*Method parses the arguments.
*<remarks>
* Switches have the form "/-[id]=[true|false]".  If no value specified, then switch evaluates to "true".
* Variables have the form  "[varname]=[val]".  Help is returned by argument being either ? or help
* Method will stash values in the entries.
* Return Values:
*   0  : no arguments specified
*   -1 : request Help
*   N  : number of arguments parsed.
*
*</remarks>
*/
int A2300::ArgParser::Parse( int argc, const char* argv[] )
{
	if( argc == 0 || argv == NULL )
		return 0;

	//Save App Path
	m_sAppPath = *argv;
	argc--;
	argv++;

	//Parse the arguments
	char buff[255];
	int ctParam=0;
	const char* p = argv[0];
	try
	{	
		for(int ctArgs = 0; ctArgs < argc; ctArgs++)
		{
			// parse the parameter
			p = argv[ctArgs];
#if defined(LINUX) || defined(APPLE)
			if( p[0] == '?' || stricmp(p, "help") == 0)
#else
			if( p[0] == '?' || _stricmp(p, "help") == 0)
#endif
			{
				return -1;
			}
			else if( p[0] == '-' )
			{
				const char* p2 = 	strchr(p,'=');

				if( p2 != NULL ) // User defined switch value.
				{
					size_t len = (size_t) (p2-p)-1;
					strncpy(buff, p+1, len); // copy switch value.
					buff[len] = '\0';
					Entry& e = GetEntry( buff );
					e.userValue = p2 + 1;
				}
				else	// User selected default enabled switch or absolute path.
				{		// test for absolute path by looking for second slash
					p2 = strchr(p+1,'/');
					if( p2 != NULL )
					{	// User entered absolute path

						Entry& e = GetEntry( _itoa( ctParam, buff, 10));
						ctParam++;
						e.userValue = p;
					}
					else	// User selected default enabled switch.
					{
						Entry& e = GetEntry( (p+1));
						e.userValue = ENABLED;
					}
				}
			}
			else // User defined a variable or parameter
			{
				const char* p2 = strchr( p, '=');
				if( p2 != NULL) //It's a switch.
				{
					size_t diff = (size_t)(p2 - p);
					strncpy(buff, p, diff); // copy switch value.
					buff[diff] = '\0';
					Entry& e = GetEntry( buff );
					e.userValue = p2+1;
				}
				else
				{
					Entry& e = GetEntry( ctParam );
					ctParam++;
					e.userValue = p;
				}
			}
		}
	}
	catch( ArgParserException& e )
	{
		if( e.pinvalidEntry == NULL )
		{
			e.message = ((std::string)"Unknown Argument: ") + p;
			cout << e.message.c_str() << endl;
			throw e;
		}
	}
	return argc;
}

void A2300::ArgParser::WriteDescriptions()
{
	EntryMap::iterator pos;

	if( m_ctParameters) {
		cout << std::endl << "PARAMETERS (ordered):" << endl;
		for( pos = m_map.begin(); pos != m_map.end(); ++pos)
		{
			if( pos->second.entryType == Entry::Parameter)
			{
				WriteEntryFields( pos->second);
			}
		}
	}

	if( m_ctSwitches)
	{
		cout << std::endl << "SWITCHES:" << endl;
		for( pos = m_map.begin(); pos != m_map.end(); ++pos)
		{
			if( pos->second.entryType == Entry::Switch )
			{
				WriteEntryFields( pos->second);
			}
		}
	}

	if( m_ctVariables)
	{
		cout << endl << "VARIABLES:" <<endl;
		for( pos = m_map.begin(); pos != m_map.end(); ++pos)
		{
			if( pos->second.entryType == Entry::Variable )
			{
				WriteEntryFields( pos->second);
			}
		}
	}
}

/**
* Method writes the entry fields nicely.
*/
void WriteEntryFields( ArgParser::Entry& entry)
{
	char buff[512];
	size_t nLen;
	if( entry.group != "global") {
		nLen = sprintf( buff, "%10s - [%s] [%s] %s", entry.keyName.c_str(), entry.defaultValue.c_str(),
			entry.group.c_str(), entry.description.c_str());
	} else {
		nLen = sprintf( buff, "%10s - [%s] %s", entry.keyName.c_str(), entry.defaultValue.c_str(),
			entry.description.c_str());
	}

	//Break into nicely formatted lines.
	size_t ct = 0;
	while( nLen > 0 )
	{
		size_t ctWrite = std::min((size_t) 79, (size_t) nLen);
		if( ct > 1) fwrite( "                 ", 1, 13, stdout);
		ct += fwrite( buff + ct, 1, ctWrite , stdout);
		fwrite( "\n", 1,1, stdout);
		nLen -= ctWrite;
	}

	fwrite( "\n", 1,1,stdout);
}
