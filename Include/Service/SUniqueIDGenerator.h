/** \class
* SUniqueIDGenerator.
* Basic singleton class to handle dispensing unique ids.
*/

#include <memory>
#include <mutex>

#ifndef _SUNIQUEIDGENERATOR
#define _SUNIQUEIDGENERATOR

class SUniqueIDGenerator
{
public:
	/// Returns the only one instance of the singleton
	static SUniqueIDGenerator& GetInstance()
	{
        //creating the only instance of the class
		std::call_once(m_OneCreation, []()
		{
			SUniqueIDGenerator::m_Instance.reset(new SUniqueIDGenerator);
		});

		return *SUniqueIDGenerator::m_Instance;
	}
	
    /// \brief Retrives next unique ID
	/// Returns 0 if no unique IDs left. This usually means that there are large 
	/// ranges of IDs not used because of toxic reservations.
	unsigned int GetNextID();

    /// \brief Reserves an ID. Returns false if reservation failed (usually because chosen ID is reserved).
	/// TODO handle bad reservations which disable huge ranges of available IDs
	bool ReserveID(int nID);

	~SUniqueIDGenerator();

	static const int MAX_ID = 100000;

private:
	std::mutex m_mutCounter;
	unsigned int m_nCounter;

    //Singleton implementation static variables
	static std::once_flag m_OneCreation;
	static std::shared_ptr<SUniqueIDGenerator> m_Instance;

    //Nonusable elements
	SUniqueIDGenerator();
	SUniqueIDGenerator(const SUniqueIDGenerator&);
	SUniqueIDGenerator& operator=(const SUniqueIDGenerator&);

};

#endif
