#include "Generator.h"

CGenerator::CGenerator(std::string& sName, GenType type) : m_Type(type), m_nI(0), m_nDelay(0)
{
    SetName(sName);
}

GenType CGenerator::GetType() const
{
    return m_Type;
}

const std::string& CGenerator::GetName() const
{
    return m_sName;
}

void CGenerator::SetName(std::string& sName)
{
    // first unregister current name
    SUniqueNameController::GetInstance().UnRegisterName(m_sName);
    // then register new one
    m_sName = SUniqueNameController::GetInstance().RegisterName(sName);
    sName = m_sName;
}

void CGenerator::SetDelay(int nD)
{
    m_nDelay = nD;
}

void CGenerator::SaveHistory()
{
    m_nIBack = m_nI;
}

void CGenerator::LoadHistory()
{
    if(!m_nI)
        return;

    m_nI = m_nIBack;
}
