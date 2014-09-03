/** \class CTriangleGen
* Responsible for generating triangle wave.
*/

#ifndef _CTRIANGLEGEN
#define _CTRIANGLEGEN

#include "Generator.h"
#include <cmath>


class CTriangleGen :
    public CGenerator
{
public:
    CTriangleGen(std::string sName = "Triangle") : CGenerator(sName, triangle),
        m_dA(1.0), m_nT(10), m_nSign(1), m_nSignBack(1) {}

	/// @copydoc IGenerator::GenerateNext()
	double GenerateNext() override
	{
		++m_nI;
		/// calculating delay
		if (m_nDelay >= m_nI)
		{
			return 0;
        }

		double dIoverT = (m_nI-m_nDelay) / (double) m_nT;
		double dRetVal = m_nSign * m_dA * (dIoverT) + m_dA*(m_nSign - 1.0)*(-0.5);

		if (dRetVal >= m_dA || dRetVal <= -1.0*m_dA)
		{
			m_nI = m_nDelay;
			m_nSign *= -1;
		}

		return dRetVal;
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
		m_nT = vParams.second.get<int>("T");
		m_dA = vParams.second.get<double>("A");
		m_nI = 0;
		m_nSign = 1;
#ifdef _DEBUG
		std::cout << "\t----------\n" << "\tTriangleGen" << std::endl;
		std::cout << "\tDelay: " << vParams.second.get<int>("Delay") << std::endl;
		std::cout << "\tT: " << vParams.second.get<int>("T") << std::endl;
		std::cout << "\tA: " << vParams.second.get<double>("A") << std::endl;
#endif
	}

    /// @copydoc CGenerator::SaveState(boost::property_tree::ptree&)
	void SaveState(boost::property_tree::ptree& pt) const override
	{
        // saving all the properties to the tree
		boost::property_tree::ptree& node = pt.add("Generator", "");
		node.put("Type", m_Type);
		node.put("Delay", m_nDelay);
		node.put("A", m_dA);
		node.put("T", m_nT);
        node.put("<xmlattr>.Name", m_sName);
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

    /// @copydoc IGenerator::SaveHistory()
    void SaveHistory() override
    {
        CGenerator::SaveHistory();

        m_nSignBack = m_nSign;
    }

    /// @copydoc IGenerator::LoadHistory()
    void LoadHistory() override
    {
        CGenerator::LoadHistory();

        m_nSign = m_nSignBack;
    }

	~CTriangleGen() {}

private:
    /// amplitude
    double m_dA;
    /// period in samples
    int m_nT;
    /// sign of the output slope
    int m_nSign;
    /// backup of the m_nSign value
    int m_nSignBack;
};

#endif
