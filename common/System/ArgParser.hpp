/** 
* Name: ConfigRf.hpp
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
#ifndef _ArgParser_h__
#define _ArgParser_h__

#include <map>
#include <string>
#include <algorithm>
//#include <ctype.h>
#include <cctype>

#include "DataTypes.h"

namespace A2300
{
	class ArgParserException; // See Below.
	struct _ArgParserDefMap; // See Below

	/**
	* Class parses command line and file arguments.
	*/
	class ArgParser
	{

	public:  
		/**
		* Internal class keeps track of entry information
		*/
		class Entry
		{
		public:
			
			enum EntryTypeEnum
			{
				Undefined= 0,			
				Parameter= 1,
				Variable = 2,
				Switch   = 3
			};

			enum DataTypeEnum
			{
				STRING  = 0,
				LONG	= 1,
				DOUBLE  = 2,
				BOOL    = 3
			};
			Entry() : entryType( Undefined), dataType(STRING), idxParam(-1), group(""), keyName(""),
				defaultValue(""), description(""){}
			Entry( EntryTypeEnum etype, DataTypeEnum dtype, const std::string& skey, const std::string& sdefValue,
					const std::string& sDescription, const std::string& sGroup) 
					:	entryType( etype), dataType(dtype), idxParam(-1), keyName( skey), 
					defaultValue( sdefValue), description( sDescription), group( sGroup) {}
			
			// Entry information
			EntryTypeEnum	  entryType;
			DataTypeEnum	  dataType;
			int				  idxParam;

			std::string		  group;
			std::string		  keyName;
			std::string		  defaultValue;
			std::string		  userValue;
			std::string		  description;
		};

		/**
		* Property gets the total parameters defined in the arg parser.
		*/
		int get_ParameterCount(){ return m_ctParameters;}

	protected:
		class StringCmp
		{
		public:
			enum cmp_mode{ normal, nocase };
		private:
			cmp_mode mode;
			static bool nocase_compare( char c1, char c2)
			{ return toupper(c1) < toupper(c2);}
		public:
			StringCmp (cmp_mode m=normal) : mode(m) {};

			bool operator() (const std::string& s1, const std::string& s2) const
			{ 
				if( mode == normal ) return s1 < s2;
				else 
				{
					return std::lexicographical_compare( s1.begin(), s1.end(),
						s2.begin(), s2.end(), nocase_compare );
				}
			}
		};

		typedef std::map<std::string, ArgParser::Entry, StringCmp>  EntryMap;

		//Map mainiting list of switches and variables.
		EntryMap m_map;

		int m_ctParameters;
		int m_ctSwitches;
		int m_ctVariables;

		std::string m_sAppPath;
	public:
		ArgParser() : m_ctParameters(0), m_ctSwitches(0), m_ctVariables(0){};
		void AddMapEntries( const _ArgParserDefMap* entries, const std::string& sGroup = "global", const char* pszScopeKey = NULL);

		/**
		* Method adds a switch definition to the argument parser instances.
		*/
		void AddSwitch( const std::string& sSwitchID, bool bEnabled,
			const std::string& sDescription, const std::string& sGroup = "global");

		/**
		* Method adds a variable definition to the argument parser instance.
		*/
		void AddVariable(const std::string& sVariableID, const std::string& sDefaultValue, 
			Entry::DataTypeEnum dType, const std::string& sDescription, const std::string& sGroup = "global");

		/**
		* Adds a Parameter to the class with the specified order identifier.
		*/
		void AddParameter(  const std::string& sId, const std::string& sDefaultValue, 
			Entry::DataTypeEnum dType, const std::string& sDescription,
			const std::string& sGroup = "global");

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
		int Parse( int argc, const char* argv[]);

		/**
		* Method returns the entry associated with the specified key.
		*/
		ArgParser::Entry&	GetEntry( const std::string& sKey );

		/** 
		* Method returns the entry associated with the specified parameter index.
		*/
		ArgParser::Entry&   GetEntry( int idxParam );

		/**
		* Method returns the string defined for the element.
		*/
		std::string GetString( const std::string& sKey )
		{
			Entry en = GetEntry(sKey);
			std::string s = m_map[sKey].userValue;
			if(s.length() == 0 )
				return	m_map[sKey].defaultValue;
			return s;
		}

		/**
		* Method returns boolean value at the specified key.
		*/
		bool GetBool( const std::string& sKey );

		/**
		* Method returns the A2300::int32 value at the specified key.
		*/
		int32 GetLong( const std::string& sKey );

		/**
		* Method returns the double value at the specified key.
		*/
		double GetDouble( const std::string& sKey );

		/**
		* Method writes variable and switch descriptions to output stream.
		*/
		void WriteDescriptions();

		/**
		* Returns True if Key is defined.
		*/
		bool IsDefined(  const std::string& sKey);

		/**
		* Method returns the entry type associated with the specified key. Returns
		* undefined if key is not found.
		*/
		Entry::EntryTypeEnum EntryType(  const std::string& sKey);

		/**
		* Method returns the 0th argument for the application path.
		*/
		std::string GetAppPath( ) { return this->m_sAppPath;}
	};


	/**
	* Structure defines an argument definition structure that enables the simple table construction of 
	* command line definitions.
	*/
	struct _ArgParserDefMap
	{ 
		const char* pszID;
		const char* pszDefault;
		ArgParser::Entry::EntryTypeEnum eType;
		ArgParser::Entry::DataTypeEnum  eData;
		const char* pszDescription;
	}; 

#define ARGPARSER_BEGIN_MAP( idMap ) static const A2300::_ArgParserDefMap idMap[] = {
#define ARGPARSER_PARAM( pszID, pszDefault, pszDescription)				{pszID, pszDefault, ArgParser::Entry::Parameter, ArgParser::Entry::STRING, pszDescription },
#define ARGPARSER_VAR( pszID, dataType, pszDefault, pszDescription)		{pszID, pszDefault, ArgParser::Entry::Variable, dataType, pszDescription },
#define ARGPARSER_SWITCH( pszID, bDefault, pszDescription)				{pszID, #bDefault, ArgParser::Entry::Switch, ArgParser::Entry::BOOL, pszDescription },

#define ARGPARSER_END_TOKEN "__ENDMAP__"
#define ARGPARSER_END_MAP {ARGPARSER_END_TOKEN, NULL, ArgParser::Entry::Undefined, ArgParser::Entry::STRING, NULL} };


	/**
	* ArgParser Exception class 
	*/
	class ArgParserException
	{

	public:
		ArgParserException( std::string smsg ) : pinvalidEntry(NULL),  message( smsg ) { };
		ArgParserException( ArgParser::Entry* pentry, std::string msg ) :pinvalidEntry(pentry), message(msg) {};
		ArgParserException( const ArgParserException& rhs ) : pinvalidEntry( rhs.pinvalidEntry), message( rhs.message) {};

		ArgParser::Entry* pinvalidEntry;
		std::string message;
		std::string ToString() { return message ;}
	};

}


#endif
