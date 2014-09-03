/** \class CStepGen
* Responsible for generating step signal.
*/

#ifndef _CSTEPGEN
#define _CSTEPGEN

#include "Generator.h"
class CStepGen :
    public CGenerator
{
public:
    CStepGen(std::string sName = "Step") : CGenerator(sName, step),
        m_dK(1) {}

	/// @copydoc IGenerator::GenerateNext()
	double GenerateNext() override
	{
		++m_nI;
		if (m_nDelay >= m_nI)
			return 0.0;

        return 1.0*m_dK;
	}

	/// @copydoc IGenerator::Reset()
	void Reset() override
	{
		m_nI = 0;
    }

    /// @copydoc CGenerator::LoadState(boost::property_tree::ptree::value_type const&)
    void LoadState(boost::property_tree::ptree::value_type const& vParams) override
	{
		m_nDelay = vParams.second.get<int>("Delay");
        m_dK = vParams.second.get<double>("K");
		m_nI = 0;
#ifdef _DEBUG
		std::cout << "\t----------\n" << "\tStepGen" << std::endl;
		std::cout << "\tDelay: " << vParams.second.get<int>("Delay") << std::endl;
#endif
	}

    /// @copydoc CGenerator::SaveState(boost::property_tree::ptree&)
	void SaveState(boost::property_tree::ptree& pt) const override
	{
        // saving all the properties to the tree
		boost::property_tree::ptree& node = pt.add("Generator", "");
		node.put("Type", m_Type);
		node.put("Delay", m_nDelay);
        node.put("K", m_dK);
        node.put("<xmlattr>.Name", m_sName);
    }

	~CStepGen() {}

private:
    /// gain
    double m_dK;
};

#endif
