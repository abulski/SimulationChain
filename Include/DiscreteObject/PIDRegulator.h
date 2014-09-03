/** \class CPIDRegulator
* Simple PID Regulator with noise cancellation filter.
*/

#ifndef _CPIDREGUALTOR
#define _CPIDREGUALTOR

#include "boost\foreach.hpp"
#include "Regulator.h"
class CPIDRegulator :
    public CRegulator
{
public:
    CPIDRegulator(int nID = 0, ObjType Type = pidregulator, std::string sName = "PIDRegulator");

    /// @copydoc CSimNode::Simulate(double)
    double Simulate(double dInSample) override;

	/// Allows to set gain
    /// \param[in] dK Gain to set.
	void SetGain(double dK)
	{
		m_dK = dK;
	}

    /// Allows to set Tp
    /// \param[in] dTp Proportional constant to set.
    void SetTp(double dTp)
    {
        m_dTp = dTp;
    }

    /// Allows to set Td
    /// \param[in] dTd Derivative constant to set.
    void SetTd(double dTd)
    {
        m_dTd = dTd;
    }

    /// Allows to set Ti
    /// \param[in] dTi Integral constant to set.
    void SetTi(double dTi)
    {
        m_dTi = dTi;
    }

    /// Allows to set N
    /// \param[in] nN D filter rate to set.
    void SetN(int nN)
    {
        m_nN = nN;
    }

    /// @copydoc CSimNode::SaveState(boost::property_tree::ptree& pt) const
	void SaveState(boost::property_tree::ptree& pt) const override;

    /// @copydoc CSimNode::LoadState(boost::property_tree::ptree::value_type const&)
    void LoadState(boost::property_tree::ptree::value_type const& v) override;

    /// @copydoc ISISO::ResetMemory()
    void ResetMemory() override
    {
        CRegulator::ResetMemory();
        m_dILast = 0;
        m_dDLast = 0;
    }

    virtual ~CPIDRegulator();

protected:
    /// Gain
    double m_dK;
    /// Period
    double m_dTp;
    /// Integrate constant
    double m_dTi;
    /// Last integral output part
    double m_dILast;
    /// Last derivative part output
    double m_dDLast;
    /// Derivative constant
    double m_dTd;
    /// D filter rate
    int m_nN;
    /// Stores last input sample
    double m_dLastInput;
};

#endif
