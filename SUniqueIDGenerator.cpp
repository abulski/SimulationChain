#include "SUniqueIDGenerator.h"


SUniqueIDGenerator::SUniqueIDGenerator() : m_nCounter(0)
{

}

/// Retrives next unique ID
/// Returns 0 if no unique IDs left. This usually means that there are large 
/// ranges of IDs not used because of toxic reservations.
unsigned int SUniqueIDGenerator::GetNextID()
{
	m_mutCounter.lock();
	if (m_nCounter++ >= MAX_ID)
	{
		m_mutCounter.unlock();
		return 0;
	}
	m_mutCounter.unlock();

	return m_nCounter;
}

/// Reserves an ID. Returns false if reservation failed (usually because chosen ID is reserved).
/// TODO handle bad reservations which disable huge ranges of available IDs
bool SUniqueIDGenerator::ReserveID(int nID)
{
	m_mutCounter.lock();
	if (m_nCounter < nID && nID <= MAX_ID)
	{
		m_nCounter = nID;
		m_mutCounter.unlock();
		return true;
	}
	m_mutCounter.unlock();
	return false;
}

SUniqueIDGenerator::~SUniqueIDGenerator()
{
}

/// Static parameters initialization
std::once_flag SUniqueIDGenerator::m_OneCreation;
std::shared_ptr<SUniqueIDGenerator> SUniqueIDGenerator::m_Instance = nullptr;
