/** \class CPulseGen
* Responsible for generating pulse signal.
*/

#ifndef _CPULSEGEN
#define _CPULSEGEN

#include <limits>
#include "Generator.h"

class CPulseGen :
    public CGenerator
{
public:
    CPulseGen(std::string sName = "Pulse") : CGenerator(sName, pulse) {}

    /// @copydoc IGenerator::GenerateNext()
	double GenerateNext() override
	{
		/// calculating delay
		++m_nI;
		if (m_nDelay + 1 == m_nI)
			///then returning largest finite value
			return 1.0;//std::numeric_limits<double>::max(); 

		return 0.0;
	}

	/// @copydoc IGenerator::Reset()
	void Reset() override
	{
		m_nI = 0;
	}

    /// @copydoc CGenerator::LoadState(boost::property_tree::ptree::value_type const&)
	void LoadState(boost::property_tree::ptree::value_type const& vParams)
	{
		m_nDelay = vParams.second.get<int>("Delay");
		m_nI = 0;
#ifdef _DEBUG
		std::cout << "\t----------\n" << "\tPulseGen" << std::endl;
		std::cout << "\tDelay: " << vParams.second.get<int>("Delay") << std::endl;
#endif
	}

    /// @copydoc CGenerator::SaveState(boost::property_tree::ptree&)
	void SaveState(boost::property_tree::ptree& pt) const override
	{
		/// saving all the properties to the tree
		boost::property_tree::ptree& node = pt.add("Generator", "");
		node.put("Type", m_Type);
		node.put("Delay", m_nDelay);
        node.put("<xmlattr>.Name", m_sName);
    }

    ~CPulseGen() {}
};

#endif
