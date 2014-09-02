/*! \class CGenerator
* Abstract class to implement common functionality of generator classes.
*
* \par
* Class is connected to SUniqueNameController to provide it with unique name
* registration.
*/

#ifndef _CGENERATOR
#define _CGENERATOR

#include "IGenerator.h"
#include "SUniqueNameController.h"

class CGenerator : public IGenerator
{
public:
    /// \brief Abstract class constructor to initialize common components.
    /// \param[in] sName Proposed name of the generator to register.
    /// \param[in] type Type of the generator.
    CGenerator(std::string& sName, GenType type);

    /// @copydoc IGenerator::GetType()
    /// \return Type of the generator object.
    GenType GetType() const override;

    /// @copydoc IGenerator::GetName()
    /// \return Name of the object.
    const std::string& GetName() const override;

    /// @copydoc IGenerator::SetName()
    /// \param[in] sName Name of the object instance to register.
    void SetName(std::string& sName) override;

    /// @copydoc IGenerator::SetDelay(int)
    /// \param[in] nD Delay in samples.
    void SetDelay(int nD) override;

    /// @copydoc IGenerator::SaveHistory()
    void SaveHistory() override;

    /// @copydoc IGenerator::LoadHistory()
    void LoadHistory() override;

    /// @copydoc IGenerator::LoadState(boost::property_tree::ptree::value_type const&)
    /// \param[in] vParams Property tree containing data to deserialize the object.
    virtual void LoadState(boost::property_tree::ptree::value_type const& vParams) = 0;

    /// @copydoc IGenerator::SaveState(boost::property_tree::ptree &)
    /// \param[out] pt Property tree to store serialized object.
    virtual void SaveState(boost::property_tree::ptree& pt) const = 0;

protected:
    /// generator type
    const GenType m_Type;
    /// generator name
    std::string m_sName;
    /// number of sample in a sequence
    int m_nI;
    /// saved state of the m_nI variable
    int m_nIBack;
    /// delay of the output in samples
    int m_nDelay;
};

#endif // _CGENERATOR
