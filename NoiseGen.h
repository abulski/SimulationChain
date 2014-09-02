/** \class CNoiseGen
* Responsible for generating white noise with given variance.
*/

#ifndef _CNOISEGEN
#define _CNOISEGEN

#include <cstdlib>
#include <cmath>
#include "Generator.h"
class CNoiseGen :
    public CGenerator
{
public:
    CNoiseGen(std::string sName = "Noise") : CGenerator(sName, noise), m_dVar(1.0),
        m_dA(0.1) {}

    /// @copydoc IGenerator::GenerateNext()
	double GenerateNext() override
	{
		++m_nI;
		if (m_nDelay >= m_nI)
			return 0.0;

		return m_dA*(rand()/(double)RAND_MAX)*sqrt(m_dVar);
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
		m_dVar = vParams.second.get<double>("Var");
		m_dA = vParams.second.get<double>("A");
		m_nI = 0;
#ifdef _DEBUG
		std::cout << "\t----------\n" << "\tSquareGen" << std::endl;
		std::cout << "\tDelay: " << vParams.second.get<int>("Delay") << std::endl;
		std::cout << "\tA: " << vParams.second.get<double>("A") << std::endl;
		std::cout << "\tVar: " << vParams.second.get<double>("Var") << std::endl;
#endif
	}

    /// \brief Sets variance of the noise.
    /// \param[in] dVar Variance of the output.
	void SetVariance(double dVar)
	{
		m_dVar = dVar;
	}

    /// \brief Sets amplitude.
    /// \param[in] dA Amplitude of the output.
	void SetAmplitude(double dA)
	{
		m_dA = dA;
	}

    /// @copydoc CGenerator::SaveState(boost::property_tree::ptree&)
	void SaveState(boost::property_tree::ptree& pt) const override
	{
		/// saving all the properties to the tree
		boost::property_tree::ptree& node = pt.add("Generator", "");
		node.put("Type", m_Type);
		node.put("Delay", m_nDelay);
		node.put("A", m_dA);
		node.put("Var", m_dVar);
        node.put("<xmlattr>.Name", m_sName);
    }

	~CNoiseGen() {}

private:
    /// variance of the signal
    double m_dVar;
    /// amplitude
    double m_dA;
};

#endif
