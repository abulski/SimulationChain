/** \class CRegulator
* Regulator abstract base class with all the necessary functionality.
*
* \par
* Main features:
* - storing list of the generators,
* - implementation of the necessary generator functionalities, eg. searching generator with
* the given name, bulk saving and loading generator states,
* - SearchObject method override to search through generator list as well.
*/

#ifndef _CREGUALTOR
#define _CREGUALTOR

#include "SimNode.h"
#include "IGenerator.h"
#include "boost\foreach.hpp"
#include "SGeneratorFactory.h"

class CRegulator :
	public CSimNode
{
public:
	CRegulator(int nID = 0, ObjType Type = regulator, std::string sName = "regulator");

    /// @copydoc ISISO::Simulate(double)
	virtual double Simulate(double) override = 0;

    /// \brief Sets setopoint value.
    /// \param[in] dSV Sets setpoint value.
	void SetSetpointValue(double dSV)
	{
		m_dSV = dSV;
	}

    /// \brief Returns setpoint value.
	double GetSetpointValue() const
	{
		return m_dSV;
	}

    /// \brief Adds generator to the list.
    /// \param[in] Generator to add.
	void AddGenerator(std::shared_ptr<IGenerator> Gen)
	{
		m_lGen.push_back(Gen);
	}

    /// \brief Resets generator to the entry state.
    void ResetGenerators();

    /// \brief Drops the generator list.
	void DeleteGenerators()
	{
		m_lGen.clear();
    }

    /// @copydoc ISISO::ResetMemory()
    void ResetMemory() override;

    /// @copydoc ISISO::GetName(boost::property_tree::ptree&) const
    /// \param[out] nameTree Tree with all collected names of the children and generators.
    void GetName(boost::property_tree::ptree& nameTree) const override;

    /// \brief Returns list of generators.
    /// \param[out] genList List to store generator pointers.
    void GetGeneratorList(std::list<std::weak_ptr<IGenerator> >& genList);

    /// \brief Searches for a generator holding given name.
    /// \param[in] sName Name of the generator to find.
    /// \return Pointer to found generator or nullptr.
    IGenerator* FindGenerator(const std::string& sName);

    /// @copydoc ISISO::SearchObject(const std::string&)
    /// \brief In case of regulator if the name is the name of its
    /// generator this method returns the regulator to which
    /// the generator belongs.
    /// \param[in] s Name of the object or generator to find.
    /// \return Pointer to found object or nullptr.
    ISISO* SearchObject(const std::string& s) override;

    /// \brief Deserializes generators data.
    /// \param[in] pt Tree holding generator parameters.
    void LoadGeneratorState(const boost::property_tree::ptree& pt);

    /// \brief Saves generators states to load using LoadGeneratorHistory().
    void SaveGeneratorHistory();

    /// \brief Loads generators states saved with SaveGeneratorHistory().
    void LoadGeneratorHistory();

	virtual ~CRegulator();

protected:

    /// \brief Will predict next generated value from all generators.
    /// Use with LoadGeneratorHistory() and SaveGeneratorHistory() methods.
    /// \return Future generator value.
    double GetNextGeneratorValue();

    ///setpoint value
    double m_dSV;
    ///list containing generators
    std::list<std::shared_ptr<IGenerator> > m_lGen;
};

#endif _CREGULATOR
