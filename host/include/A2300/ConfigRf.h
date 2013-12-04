/** Name: ConfigRf.h
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

#ifndef CONFIGRF_H_
#define CONFIGRF_H_

#include <A2300/A2300_Defs.h>
#include <string>

namespace A2300
{

class ConfigRf : public IConfigComponent
{
public:
	ConfigRf(int idComponent, const std::string& sname );
	virtual ~ConfigRf();

	/* Define IConfigComponent interface*/
	virtual std::string name();
	virtual int  componentId();
	virtual void Reset();
	virtual void Synch();

private:
	int  m_idComponent;
	std::string m_sName;


};

} /* namespace A2300 */
#endif /* CONFIGRF_H_ */
