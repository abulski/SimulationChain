#include "PRegulator.h"


CPRegulator::CPRegulator(int nID, ObjType Type, std::string sName) : CRegulator(nID, Type, sName), m_dK(1.0)
{
}

double CPRegulator::Simulate(double dInSample)
{
    // calculate next generator value
    double dSum = GetNextGeneratorValue();

    // store input value for use of function caller
    if(!m_dInVal.expired())
        *m_dInVal.lock() = dSum;

    // determine the final value
    double dRetVal = m_dK*(dSum - dInSample);

    if (m_oStream.get() != nullptr)
        *(m_oStream.get()) << dRetVal << " ";

    // store output value for use of function caller
    if(!m_dOutVal.expired())
        *m_dOutVal.lock() = dRetVal;

    return dRetVal;
}

void CPRegulator::SaveState(boost::property_tree::ptree& pt) const
{
    // storing data into an XML file
	using boost::property_tree::ptree;

	ptree & node = pt.add("Object.Name", "");
	node.put("ID", m_nID);
	node.put("Type", m_Type);
	node.put("Gain", m_dK);
	node.put("Setpoint", m_dSV);

    // if doesnt have a parent insert 0
	if (m_Parent != nullptr)
        node.put("Parent", m_Parent->GetName());
	else
        node.put("Parent", 0);

    // saving generators
	auto genit = m_lGen.begin();
	for (; genit != m_lGen.end(); ++genit)
		(*genit)->SaveState(node);

    // adding generator name
	node.put("<xmlattr>.Name", m_sName);

    // ask children to write their state data
	auto it = m_lChildren.begin();
	for (; it != m_lChildren.end(); ++it)
		(*it)->SaveState(pt);

}

void CPRegulator::LoadState(boost::property_tree::ptree::value_type const& v)
{
    boost::property_tree::ptree ptGen = v.second;
    // Load regulator data
    SetGain(v.second.get<double>("Gain"));
    SetSetpointValue(v.second.get<double>("Setpoint"));

#ifdef _DEBUG
    std::cout << "Gain: " << v.second.get<double>("Gain") << std::endl;
    std::cout << "Setpoint: " << v.second.get<double>("Setpoint") << std::endl;
#endif
    LoadGeneratorState(ptGen);
}

CPRegulator::~CPRegulator()
{
}
