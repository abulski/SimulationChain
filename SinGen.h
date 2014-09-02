/** \class CSineGen
* Responsible for generating sinus signal.
*/

#ifndef _CSINEGEN
#define _CSINEGEN

#include "Generator.h"
#include <cmath>

static const double PI = 3.1415926;

class CSineGen :
    public CGenerator
{
public:
    CSineGen(std::string sName = "Sine") : CGenerator(sName, sine), m_dA(1.0),
        m_nT(30) {}

	/// @copydoc IGenerator::GenerateNext()
	double GenerateNext() override
	{
		++m_nI;
		/// calculating delay
		if (m_nDelay >= m_nI)
		{
			return 0;
		}

		double dRetVal = sin(2 * PI * m_nI / (double) m_nT);

		return m_dA * dRetVal;
	}

	/// @copydoc IGenerator::Reset()
	void Reset() override
	{
		m_nI = 0;
    }

    /// \brief Sets amplitude.
    /// \param[in] dA Amplitude of the output.
	void SetAmplitude(double dA)
	{
		m_dA = dA;
	}

    /// \brief Sets period in samples.
    /// \param[in] nT Period of the output in samples.
	void SetPeriod(int nT)
	{
		m_nT = nT;
	}

    /// @copydoc CGenerator::LoadState(boost::property_tree::ptree::value_type const&)
    void LoadState(boost::property_tree::ptree::value_type const& vParams) override
	{
		m_nDelay = vParams.second.get<int>("Delay");
		m_nT = vParams.second.get<int>("T");
		m_dA = vParams.second.get<double>("A");
		m_nI = 0;

#ifdef _DEBUG
		std::cout << "\t----------\n" << "\tSineGen" << std::endl;
		std::cout << "\tDelay: " << vParams.second.get<int>("Delay") << std::endl;
		std::cout << "\tT: " << vParams.second.get<int>("T") << std::endl;
		std::cout << "\tA: " << vParams.second.get<double>("A") << std::endl;
#endif
	}

    /// @copydoc CGenerator::SaveState(boost::property_tree::ptree&)
	void SaveState(boost::property_tree::ptree& pt) const override
	{
		/// saving all the properties to the tree
		boost::property_tree::ptree& node = pt.add("Generator", "");
		node.put("Type", m_Type);
		node.put("Delay", m_nDelay);
		node.put("A", m_dA);
		node.put("T", m_nT);
        node.put("<xmlattr>.Name", m_sName);
    }

	~CSineGen() {}

private:
    /// amplitude
    double m_dA;
    /// period in samples
    int m_nT;
};

#endif
