// Name:  ConfigRf.cpp
//
// Copyright 2013 Loctronix Corporation
// http://www.loctronix.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <A2300/ConfigRf.h>

namespace A2300 {

ConfigRf::ConfigRf(int idComponent, const std::string& sname)
: m_idComponent( idComponent), m_sName ( sname)
{
	// TODO Auto-generated constructor stub

}

ConfigRf::~ConfigRf()
{
	// TODO Auto-generated destructor stub
}


std::string ConfigRf::name()
{
	return m_sName;
}
int  ConfigRf::componentId()
{
	return m_idComponent;
}
void ConfigRf::Reset()
{
	//TODO
}
void ConfigRf::Synch()
{
	//TODO
}

} /* namespace A2300 */
