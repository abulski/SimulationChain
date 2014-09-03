#include "Regulator.h"


CRegulator::CRegulator(int nID, ObjType Type, std::string sName) : CSimNode(nID, Type, sName), m_dSV(0.0)
{
}

void CRegulator::ResetGenerators()
{
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
        it->get()->Reset();
}

void CRegulator::ResetMemory()
{
    CSimNode::ResetMemory();
    ResetGenerators();
}

void CRegulator::GetName(boost::property_tree::ptree& nameTree) const
{
    using boost::property_tree::ptree;
    CSimNode::GetName();

    ptree& node = nameTree.add("Regulator", "");

    // ask children to write their state data
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
    {
        node.put("Generator.<xmlattr>.Name", it->get()->GetName());
    }

    node.put("<xmlattr>.Name", m_sName);
}

void CRegulator::GetGeneratorList(std::list<std::weak_ptr<IGenerator> >& genList)
{
    auto it = m_lGen.begin();
    for(; it != m_lGen.end(); ++it)
    {
        genList.push_back(std::weak_ptr<IGenerator>(*it));
    }
}

IGenerator* CRegulator::FindGenerator(const std::string& sName)
{
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
        if(it->get()->GetName() == sName)
            return it->get();

    return nullptr;
}

ISISO* CRegulator::SearchObject(const std::string& s)
{
    //if the searched name belongs to a generator return this regulator
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
        if(it->get()->GetName() == s)
            return this;

    return CSimNode::SearchObject(s);
}

void CRegulator::LoadGeneratorState(const boost::property_tree::ptree& pt)
{
    // Create generators from stored data
    IGenerator* gen = nullptr;
    bool bNewGen;
    BOOST_FOREACH(boost::property_tree::ptree::value_type const& vals, pt)
    {
        bNewGen = false;
        if (vals.first == "Generator")
        {
            try
            {
                std::string genName = vals.second.get<std::string>("<xmlattr>.Name");
                gen = FindGenerator(genName);
            }
            catch(...)
            {}

            if(!gen)
            {
                GenType type = static_cast<GenType>(vals.second.get<int>("Type"));
                gen = SGeneratorFactory::GetInstance().CreateGenerator(type);
                bNewGen = true;
            }

            if (!gen)
                continue;
            // Load generator data
            gen->LoadState(vals);

            // Add generator to list of active generators
            if(bNewGen)
                AddGenerator(std::shared_ptr<IGenerator>(gen));
        }
    }
}

void CRegulator::SaveGeneratorHistory()
{
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
        it->get()->SaveHistory();
}

void CRegulator::LoadGeneratorHistory()
{
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
        it->get()->LoadHistory();
}

double CRegulator::GetNextGeneratorValue()
{
    //iterate over all the generators and sum the output value
    double dSum = 0.0;
    auto it = m_lGen.begin();
    for (; it != m_lGen.end(); ++it)
        dSum += it->get()->GenerateNext();

    return dSum;
}

CRegulator::~CRegulator()
{
}
