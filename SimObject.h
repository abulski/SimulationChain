/** \class CSimObject
 * Simulated object base class with all necessary properties.
*/


#include "SimNode.h"

#ifndef _CSIMOBJECT
#define _CSIMOBJECT

std::ostream& operator<<(std::ostream& out, const std::vector<double>& v);

///temporary function to convert vector into a string
std::string v2str(const std::vector<double>& v);

///temporary function to convert string into vector
std::vector<double>& str2v(const std::string& s, std::vector<double>& out_v);


class CSimObject :
	public CSimNode
{
public:
    /// \brief Constructs SimObject.
    /// \warning THIS CONSTRUCTOR HAS TO RUN CSimNode CONSTRUCTOR OR THE OBJECT WILL BE CORRUPTED
	CSimObject(int nID = 0, ObjType Type = simobject, std::string sName = "object");

    /// @copydoc CSimNode::Simulate(double)
	double Simulate(double dInSample) override;

    /// @copydoc CSimNode::SaveState(boost::property_tree::ptree& pt) const
	void SaveState(boost::property_tree::ptree& pt) const override;

    /// @copydoc CSimNode::LoadState(boost::property_tree::ptree::value_type const&)
    void LoadState(boost::property_tree::ptree::value_type const& v) override;

    /// \brief Function to set vector a of the model.
    /// \param[in] vA Vector A to std::move() to the object.
    void SetVectorA(std::vector<double>&& vA);

    /// \brief Function to retrive vector a of the model.
    /// \return Reference to the A vector.
	const std::vector<double>& GetVectorA() const
	{
		return m_vA;
	}

    /// \brief Function to set vector b of the model.
    /// \param[in] vB Vector B to std::move() to the object.
    void SetVectorB(std::vector<double>&& vB);

    /// \brief Function to retrive vector b of the model.
    /// \return Reference to the B vector.
	const std::vector<double>& GetVectorB() const
	{
		return m_vB;
	}

    /// \brief Set K value.
    /// \param[in] New delay value.
	void SetK(int nK)
	{
		m_nK = nK;
	}

    /// \brief Receive K value.
    /// \return Delay value.
	int GetK() const
	{
		return m_nK;
	}

    /// \brief Set if object is stationary.
    /// \param[in] Either stationary (true) or not (false).
	void SetStationary(bool bStationary)
	{
		m_bStationary = bStationary;
	}

	virtual ~CSimObject();

protected:
    /// Is stationary?
	bool m_bStationary;
    /// Vector with denominator values.
	std::vector<double> m_vA;
    /// Vector with nominator values.
	std::vector<double> m_vB;
	int m_nK;
};

#endif
