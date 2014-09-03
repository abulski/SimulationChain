#include "PIDRegulator.h"


CPIDRegulator::CPIDRegulator(int nID, ObjType Type, std::string sName) : CRegulator(nID, Type, sName),
    m_dK(1.0), m_dTp(0.0), m_dTi(0.0), m_dILast(0.0), m_dDLast(0.0), m_dTd(0.0), m_nN(0.0),
    m_dLastInput(0.0)
{
}

double CPIDRegulator::Simulate(double dInSample)
{
    // calculate generator values
    double dInput = GetNextGeneratorValue();

    // store input value for use of function caller
    if(!m_dInVal.expired())
        *m_dInVal.lock() = dInput;

    // calculate the e val
    double dE = dInput - dInSample;

    // store the input value
    m_InputHistory.AddSample(dE);

    // determine the P regulator value
    double dP = m_dK * dE;

    // determine the I regulator value
    double dI = 0;
    if(m_dTi != 0)
        dI = m_dILast + dE*m_dK*m_dTp/m_dTi;
    m_dILast = dI;

    // determine the D regulator value
    double dD = 0;
    if(m_dTd != 0)
       dD = m_dDLast*m_dTd/(m_dTd + m_nN*m_dTp) - (dInput - m_dLastInput)*m_dK*m_nN*m_dTd/(m_dTd + m_nN*m_dTp);

    double dRetVal = dP + dI + dD;

    // store value into stream
    if (m_oStream.get() != nullptr)
        *(m_oStream.get()) << dRetVal << " ";

    // store the output value
    m_OutputHistory.AddSample(dRetVal);

    // store input value
    m_dLastInput = dInput;

    // store output value for use of function caller
    if(!m_dOutVal.expired())
        *m_dOutVal.lock() = dRetVal;

    return dRetVal;
}

void CPIDRegulator::SaveState(boost::property_tree::ptree& pt) const
{
    // storing data into an XML file
	using boost::property_tree::ptree;

	ptree & node = pt.add("Object.Name", "");
	node.put("ID", m_nID);
	node.put("Type", m_Type);
	node.put("Gain", m_dK);
    node.put("Tp", m_dTp);
    node.put("Ti", m_dTi);
    node.put("Td", m_dTd);
    node.put("N", m_nN);
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

CPIDRegulator::~CPIDRegulator()
{
}

void CPIDRegulator::LoadState(boost::property_tree::ptree::value_type const& v)
{
    boost::property_tree::ptree ptGen = v.second;
    // Load regulator data
    SetGain(v.second.get<double>("Gain"));
    SetTp(v.second.get<double>("Tp"));
    SetTi(v.second.get<double>("Ti"));
    SetTd(v.second.get<double>("Td"));
    SetN(v.second.get<int>("N"));
    SetSetpointValue(v.second.get<double>("Setpoint"));

#ifdef _DEBUG
    std::cout << "Gain: " << v.second.get<double>("Gain") << std::endl;
    std::cout << "Setpoint: " << v.second.get<double>("Setpoint") << std::endl;
    std::cout << "Tp: " << v.second.get<double>("Tp") << std::endl;
    std::cout << "Ti: " << v.second.get<double>("Ti") << std::endl;
    std::cout << "Td: " << v.second.get<double>("Td") << std::endl;
    std::cout << "N: " << v.second.get<int>("N") << std::endl;
#endif

    LoadGeneratorState(ptGen);
}
