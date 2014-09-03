/** \class CSquareGen
* Responsible for generating pulse signal.
*/

#ifndef _CSQUAREGEN
#define _CSQUAREGEN

#include "Generator.h"

class CSquareGen :
    public CGenerator
{
public:
    CSquareGen(std::string sName = "Square") : CGenerator(sName, square), m_dA(1.0),
        m_dD(0.5), m_nT(10), m_nCurrSign(1), m_nCurrSignBack(1) {}

	/// @copydoc IGenerator::GenerateNext()
	double GenerateNext() override
	{
		++m_nI;
		/// calculating delay
		if (m_nDelay >= m_nI)
		{
			return 0;
		}

		/// calculating moment of switch
		int nSamplePeriod = (int)(m_nT*m_dD);

		/// if the period is a factor of the current sample value (reduced by delay) 
		/// switch the wave sign
		if ((m_nI - m_nDelay) % nSamplePeriod == 0)
		{
			m_nCurrSign *= -1;
		}

		return m_nCurrSign*m_dA;
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

    /// \brief Sets duty cycle in range <0,1>
    /// \param[in] nDC Duty cycle in range <0,1>.
	void SetDutyCycle(double nDC)
	{
		m_dD = nDC;
	}

    /// @copydoc CGenerator::LoadState(boost::property_tree::ptree::value_type const&)
	void LoadState(boost::property_tree::ptree::value_type const& vParams)
	{
		m_nDelay = vParams.second.get<int>("Delay");
		m_nT = vParams.second.get<int>("T");
		m_dA = vParams.second.get<double>("A");
		m_dD = vParams.second.get<double>("DutyCycle");
        //m_nI = 0;
        //m_nCurrSign = 1;
#ifdef _DEBUG
		std::cout << "\t----------\n" << "\tSquareGen" << std::endl;
		std::cout << "\tDelay: " << vParams.second.get<int>("Delay") << std::endl;
		std::cout << "\tT: " << vParams.second.get<int>("T") << std::endl;
		std::cout << "\tA: " << vParams.second.get<double>("A") << std::endl;
		std::cout << "\tDutyCycle: " << vParams.second.get<double>("DutyCycle") << std::endl;
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
		node.put("DutyCycle", m_dD);
        node.put("<xmlattr>.Name", m_sName);
    }

    /// @copydoc IGenerator::SaveHistory()
    void SaveHistory() override
    {
        CGenerator::SaveHistory();

        m_nCurrSignBack = m_nCurrSign;
    }

    /// @copydoc IGenerator::LoadHistory()
    void LoadHistory() override
    {
        CGenerator::LoadHistory();

        m_nCurrSign = m_nCurrSignBack;
    }

	~CSquareGen() {}

private:
    /// amplitude
    double m_dA;
    /// period in samples
    int m_nT;
    /// duty cycle <0,1>
    double m_dD;
    ///curent sign of the wave
    int m_nCurrSign;
    ///backupt of the m_nCurrSign value
    int m_nCurrSignBack;
};

#endif
