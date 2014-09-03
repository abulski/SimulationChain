#include "SUniqueNameController.h"

#ifdef _DEBUG
#include <iostream>
#endif

SUniqueNameController::SUniqueNameController()
{
}

std::string& SUniqueNameController::RegisterName(std::string& sName)
{
	std::pair<std::set<std::string>::iterator, bool> result = m_SavedNames.insert(sName);

    // the second value = true if insertion took place without any problems
	if (result.second)
		return sName;

    // increment a number of the name until it is unique
	int i = 0;
	while (!result.second)
	{
		++i;
		result = m_SavedNames.insert(sName + std::to_string(i));
	}

	sName += std::to_string(i);
	return sName;
}

void SUniqueNameController::UnRegisterName(std::string& sName)
{
	m_SavedNames.erase(sName);
}


SUniqueNameController::~SUniqueNameController()
{
#ifdef _DEBUG
	std::cout << "SUniqueNameController destroyed." << std::endl;
#endif
}


/// Static members definitions
std::shared_ptr<SUniqueNameController> SUniqueNameController::m_Instance = nullptr;
std::once_flag SUniqueNameController::m_OneCreation;
