/** \class CSimNode
* Class implements a common functionality of simulation objects
* and a composite design pattern to inherit futher.
*
* \par
* It defines the basic self organizing tree data structure and ensures
* no memory leaks by automatic deallocation of its children when the node
* is being removed.
*
* \par
* For more \see ISISO.
*/


#ifndef _CSIMNODE
#define _CSIMNODE

#include "SUniqueIDGenerator.h"
#include "SUniqueNameController.h"
#include "Historian.h"
#include <numeric>
#ifdef _DEBUG
#include <iostream>
#endif
#include "boost\property_tree\xml_parser.hpp"

#include "ISISO.h"
class CSimNode :
	public ISISO
{
public:
    /// \brief Constructs the base abstract class for simulation object handling.
    /// \param[in] nID Proposed ID for object. Given 0 next viable id will be found automaticly.
    /// \param[in] Type Type of the object.
    /// \param[in] sName Proposed name to register.
    CSimNode(int nID = 0, ObjType Type = simobject, std::string const &sName = std::string("node"));

	/// @copydoc ISISO::Simulate(double)
    /// \param[in] dInput Next input (can be feedback) value.
	double Simulate(double dInput) override = 0;

    /// @copydoc ISISO::GetOutputHistory(std::vector<double>&) const
    /// \param[out] vHist Returns output history as a vector of doubles.
    void GetOutputHistory(std::vector<double>& vHist) const override
    {
        m_OutputHistory.RetriveNSamples(vHist);
    }

	/// @copydoc ISISO::EraseOutputHistory()
    void EraseOutputHistory() override {}

    /// @copydoc ISISO::SetOutputHistory(std::vector<double>&)
    /// \param[in] vHist Sets new output history.
    virtual void SetOutputHistory(std::vector<double>& vHist) override
    {
        m_OutputHistory.SetHistory(vHist);
    }

    /// @copydoc ISISO::SetInputHistory(std::vector<double>&)
    /// \param[out] vHist Sets input history as a vector of doubles.
    virtual void SetInputHistory(std::vector<double>& vHist) override
    {
        m_InputHistory.SetHistory(vHist);
    }

    /// @copydoc ISISO::SaveState(boost::property_tree::ptree&) const
    /// \param[out] pt Property tree to store serialized object.
	void SaveState(boost::property_tree::ptree& pt) const override = 0;

    /// @copydoc ISISO::LoadState(boost::property_tree::ptree::value_type const&)
    /// \param[in] v Property tree to deserialize objects data from.
	void LoadState(boost::property_tree::ptree::value_type const& v) override = 0;

	/// @copydoc ISISO::SetID(int)
    /// \param[in] nID New, proposed ID value. When set to 0 picks first available.
	void SetID(int nID = 0) override;

	/// @copydoc ISISO::GetID()
	int GetID() const override
	{
		return m_nID;
	}

	/// @copydoc ISISO::SetName(std::string&)
    /// \param[in] sName Tries to register new name of the object.
	void SetName(std::string& sName) override;

    /// @copydoc ISISO::GetName() const
	const std::string& GetName() const override
	{
		return m_sName;
    }

    /// @copydoc ISISO::GetName(boost::property_tree::ptree&) const
    /// \param[out] nameTree Builds tree containing names of object and all its children.
    void GetName(boost::property_tree::ptree& nameTree) const override;

	/// @copydoc ISISO::SetParent(ISISO*)
    /// \param[in] Parent Pointer to the parent.
    void SetParent(ISISO* Parent) override;

	/// @copydoc ISISO::GetParent()
	ISISO* GetParent() override
	{
		return m_Parent;
	}

	/// @copydoc ISISO::AddChild(std::shared_ptr<ISISO>)
    /// \param[in] Child Adds the given child.
    void AddChild(std::shared_ptr<ISISO> Child) override;

	/// @copydoc ISISO::RemoveChild(std::shared_ptr<ISISO>)
    /// \param[in] Child Pointer to the child to remove from the tree.
	void RemoveChild(std::shared_ptr<ISISO> Child)
	{
		if (Child.get() != nullptr)
			m_lChildren.remove(Child);
	}

    /// @copydoc ISISO::GetChildren(std::list<std::weak_ptr<ISISO> >&) const
    /// \param[out] lChildren Pointes to all the children from the object as a list.
	void GetChildren(std::list<std::weak_ptr<ISISO> >& lChildren) const override;

	/// @copydoc ISISO::RemoveObject(int)
    /// \param[in] nID Searches an object with the given id and removes it from the tree.
	bool RemoveObject(int nID) override;

    /// @copydoc ISISO::SearchObject(const std::string&)
    /// \param[in] s Name of an object to search.
    ISISO* SearchObject(const std::string& s) override;

    /// Searches the tree for the first regulator object.
    ISISO* FindFirstRegulator() override;

    /// @copydoc ISISO::GetType() const
	ObjType GetType() const override
	{
		return m_Type;
	}

	/// @copydoc ISISO::SetType(ObjType)
    /// \param[in] Type Type of the object to set.
	void SetType(ObjType Type) override
	{
		m_Type = Type;
	}

	/// @copydoc ISISO::SetStreamForOutput(std::shared_ptr<std::ostream>)
    /// \param[in] oStream Output stream to store output data to.
	void SetStreamForOutput(std::shared_ptr<std::ostream> oStream) override
	{
		m_oStream = oStream;
    }

    /// @copydoc ISISO::SetVariableToStoreCurrentOutput(std::weak_ptr<double>)
    /// \param[out] dOutVal Pointer of the variable to store last output into.
    void SetVariableToStoreCurrentOutput(std::weak_ptr<double> dOutVal)
    {
        m_dOutVal = dOutVal;
    }

    /// @copydoc ISISO::SetVariableToStoreCurrentInput(std::weak_ptr<double>)
    /// \param[out] dInVal Pointer of the variable to store last input into.
    void SetVariableToStoreCurrentInput(std::weak_ptr<double> dInVal)
    {
        m_dInVal = dInVal;
    }

	/// @copydoc ISISO::RemoveStreamForOutput()
	void RemoveStreamForOutput() override
	{
		m_oStream.reset(static_cast<std::ostream*>(nullptr));
	}

	/// @copydoc ISISO::MoveObjectToFront(ISISO*)
    /// \param[in] FrontObject Pointer to the object to move.
    bool MoveObjectToFront(ISISO* FrontObject) override;

    /// @copydoc ISISO::ResetMemory()
    void ResetMemory() override;

	virtual ~CSimNode();

protected:
    /// Objects unique ID
	int m_nID;
    /// Objects unique name
	std::string m_sName;
    /// Output sample history
	CHistorian m_OutputHistory;
    /// Input sample history
	CHistorian m_InputHistory;
    /// List of children
	std::list<std::shared_ptr<ISISO> > m_lChildren;
    /// Pointer to parent
	ISISO* m_Parent;
    /// Objects type
	ObjType m_Type;
    /// Output stream
    std::shared_ptr<std::ostream> m_oStream;
    /// Pointer to variable storing last output value
    std::weak_ptr<double> m_dOutVal;
    /// Pointer to variable storing last input value
    std::weak_ptr<double> m_dInVal;
};

#endif
