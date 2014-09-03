/** \class SObjectFactory
* Responsible for creating discrite objects which implement ISISO interface.
* \note Implements multi-threading safe singleton pattern.
*/

#ifndef _SOBJECTFACTORY
#define _SOBJECTFACTORY

#include <memory>
#include <mutex>
#include "PRegulator.h"
#include "PIDRegulator.h"
#include "SimObject.h"
#include "GPC.h"

class SObjectFactory
{
public:
	/// Allows to access the only instance of the factory.
	static SObjectFactory& GetInstance()
	{
        // Call creation function only once
		std::call_once(SObjectFactory::m_OneCreation, []()
		{
			SObjectFactory::m_OneInstance.reset(new SObjectFactory());
		});

		return *(SObjectFactory::m_OneInstance);
	}

    /// \brief Creates a new type of discrite object. Returns nullptr if invalid type is given.
    /// \param[in] NewObjectType Type of the object to create.
    /// \return Created object pointer or nullptr.
	ISISO* CreateObject(ObjType NewObjectType);

    /// \brief Checks whether the object is a type of regulator.
    /// \param[in] node An object to test.
    /// \return True if object is regulator.
    bool IsARegulator(CSimNode*);

    /// \brief Checks whether the object is a type of regulator.
    /// \param[in] node An object to test.
    /// \return True if object is regulator.
    bool IsARegulator(const ObjType&);

	~SObjectFactory();

private:
    // Singleton implementation variables
	static std::once_flag m_OneCreation;
	static std::shared_ptr<SObjectFactory> m_OneInstance;

    // Prevent remote usage
	SObjectFactory();
	SObjectFactory(const SObjectFactory&);
	SObjectFactory& operator=(SObjectFactory&);
};

#endif
