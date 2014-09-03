/** \class CPRegulator
* Simple P Regulator implementation.
*/

#ifndef _CPREGUALTOR
#define _CPREGUALTOR

#include "Regulator.h"
class CPRegulator :
	public CRegulator
{
public:
	CPRegulator(int nID = 0, ObjType Type = regulator, std::string sName = "PRegulator");

    /// @copydoc CSimNode::Simulate(double)
    double Simulate(double dInSample) override;

	/// Allows to set gain
    /// \param[in] dK Gain to set.
	void SetGain(double dK)
	{
		m_dK = dK;
	}

    /// @copydoc CSimNode::SaveState(boost::property_tree::ptree& pt) const
	void SaveState(boost::property_tree::ptree& pt) const override;

    /// @copydoc CSimNode::LoadState(boost::property_tree::ptree::value_type const&)
    void LoadState(boost::property_tree::ptree::value_type const& v) override;

	virtual ~CPRegulator();

protected:
    /// Gain
    double m_dK;
};

#endif
