/** \class 
 * SUniqueNameController
 * Responsible for registering and checking uniqueness of the chosen names.
*/

#include <mutex>
#include <memory>
#include <set>
#include <string>

#ifndef _SUniqueNameController
#define _SUniqueNameController

class SUniqueNameController
{
public:
	/// Returns the only one instance of the singleton
	static SUniqueNameController& GetInstance()
	{
        //creating the only instance of the class
		std::call_once(m_OneCreation, []()
		{
			SUniqueNameController::m_Instance.reset(new SUniqueNameController);
		});

		return *SUniqueNameController::m_Instance;
	}

    /// \brief Handles checking the uniqueness of the name and generating postfix if needed.
	/// Using referencje, returns the (fixed) name to use within the program.
    /// \param[in] sName Proposed name to register.
    /// \return Name registered. Can be different than sName.
	std::string& RegisterName(std::string& sName);

    /// \brief Unregisters name. Whether it exists in base or not.
    /// \param[in] sName Name to unregister.
	void UnRegisterName(std::string& sName);

	~SUniqueNameController();

private:
	/// Stores registered names
	std::set<std::string> m_SavedNames;

    // Singleton implementation variables
	static std::shared_ptr<SUniqueNameController> m_Instance;
	static std::once_flag m_OneCreation;

    // Disables copying and creation from outside of the class
	SUniqueNameController();
	SUniqueNameController(const SUniqueNameController&);
	SUniqueNameController& operator=(const SUniqueNameController&);
};

#endif
