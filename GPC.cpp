#include "GPC.h"
#include "SLogic.h"

CGPC::CGPC(int nID, ObjType Type, std::string sName)
    : CRegulator(nID, Type, sName)
{
    m_RefObj.reset(new CSimObject());
    m_StepObj.reset(new CSimObject());
    m_ARIXObj.reset(new CSimObject());

    m_LastValueFromGen = 0;
    m_noTimesIdentified = 0;
    SetParams(2, 3, 0.5, 0.4, 1);
    SetObjectForPrediction();
}

void CGPC::Initialize()
{
    // Predict required amount of samples
    double u = 1;
    m_LastValueFromGen = 0;
    m_noTimesIdentified = 0;
    m_bFirstNonZeroInput = false;
    m_ARIXObj->ResetMemory();
    m_StepObj->ResetMemory();
    m_RefObj->ResetMemory();
    CRegulator::ResetMemory();

    // create q vector
    CreateQ();
}

void CGPC::SetParams(int nL, int nH, double dRO, double dAlpha, int nK)
{
    m_nL = nL;
    m_nH = nH;
    m_nK = nK;
    m_dRO = dRO;
    m_dAlpha = dAlpha;
    m_ARIXObj->SetK(m_nK);


    std::vector<double> vB(1);
    vB[0] = 1 - dAlpha;

    std::vector<double> vA(1);
    vA[0] = -dAlpha;

    m_RefObj->SetVectorA(std::move(vA));
    m_RefObj->SetVectorB(std::move(vB));
    m_RefObj->SetK(0);
}

double CGPC::Simulate(double dInSample)
{
    // store the feedback
    m_FeedbackHistory.AddSample(dInSample);

    // calculate generator values
    double dInput = GetNextGeneratorValue();
    m_LastValueFromGen = dInput;

    if(dInput != 0)
        m_bFirstNonZeroInput = true;

    // store input value for use of function caller
    if(!m_dInVal.expired())
        *m_dInVal.lock() = dInput;

    // calculate the e val
    double dE = dInput - dInSample;

    // store the input value
    m_InputHistory.AddSample(dE);

    // calculating current q vector
    CreateQ();

    // calculating free reponse
    Eigen::VectorXd y0(m_nH);
    CalcFreeResponse(y0);
    std::cout << "y0: " << y0 << std::endl;

    // calculating reference response
    Eigen::VectorXd w0(m_nH);
    CalcRefValues(w0);
    std::cout << "w0: " << w0 << std::endl;

    auto x = (m_vQ.transpose())*(w0 - y0);

    double DELTAu = x(0);
    std::vector<double> vLastVal;
    m_OutputHistory.RetriveNSamples(vLastVal, 1);
    double u = vLastVal[0] + DELTAu;

    double dRetVal = u;

    // store value into stream
    if (m_oStream.get() != nullptr)
        *(m_oStream.get()) << dRetVal << " ";


    // store the output value
    m_OutputHistory.AddSample(dRetVal);

    // store output value for use of function caller
    if(!m_dOutVal.expired())
        *m_dOutVal.lock() = dRetVal;

    //std::cout << " Contr. signal: " << dRetVal << std::endl;

    return dRetVal;
}

void CGPC::SetObjectForPrediction(ISISO* CObj)
{
    if(CObj != NULL)
    {
        CSimObject* obj = dynamic_cast<CSimObject*>(CObj);

        // TODO

        m_StepObj.reset(obj);
        m_StepObj->SetK(0);

        return;
    }

    // default object
    m_StepObj.reset(new CSimObject());

    std::vector<double> vB(1);
    vB[0] = 0.5;
    std::vector<double> vA(1);
    vA[0] = -0.5;
    std::vector<double> vA2 = vA;
    std::vector<double> vB2 = vB;
    m_StepObj->SetVectorA(std::move(vA));
    m_StepObj->SetVectorB(std::move(vB));
    m_StepObj->SetK(0);
    m_ARIXObj->SetVectorA(std::move(vA2));
    m_ARIXObj->SetVectorB(std::move(vB2));
    m_ARIXObj->SetK(m_nK);
}

void CGPC::Identify()
{
    //return;
    if(m_bFirstNonZeroInput == false)
        return;

    if(m_noTimesIdentified <= m_nH)
    {
        m_noTimesIdentified++;
        return;
    }

    std::vector<double> vNom, vNom2;
    SLogic::GetInstance().GetLastIdentifiedNominator(vNom);
    vNom2 = vNom;

    std::vector<double> vDenom, vDenom2;
    SLogic::GetInstance().GetLastIdentifiedDenominator(vDenom);
    vDenom2 = vDenom;

    m_StepObj->SetVectorA(std::move(vDenom));
    m_StepObj->SetVectorB(std::move(vNom));
    m_ARIXObj->SetVectorA(std::move(vDenom2));
    m_ARIXObj->SetVectorB(std::move(vNom2));
}

void CGPC::CreateQ()
{
    Eigen::MatrixXd ddQp(m_nH, m_nL);

    Identify();

    m_StepObj->ResetMemory();
    // fill with predicted values
    std::vector<double> v(m_nH+m_nL);
    for(int i=m_nH-1; i>=0; --i)
        v[i] = (m_StepObj->Simulate(1));
    for(int i=0; i<m_nL; ++i)
        for(int j=m_nH; j>0; --j)
            ddQp(j-1,i) = v[m_nH-j+i];

    // calculate q
    Eigen::MatrixXd ddQ = ddQp;
    ddQ = ddQ.transpose()*ddQ;
    ddQ = ddQ + m_dRO*Eigen::MatrixXd::Identity(ddQ.rows(), ddQ.cols());
    ddQ = ddQ.inverse();
    ddQ = ddQ*ddQp.transpose();
    m_vQ = ddQ.row(0);
}

void CGPC::CalcFreeResponse(Eigen::VectorXd& vOut)
{
    m_ARIXObj->ResetMemory();

    std::vector<double> v;
    m_OutputHistory.RetriveNSamples(v);
    m_ARIXObj->SetInputHistory(v);

    std::vector<double> v2;
    m_FeedbackHistory.RetriveNSamples(v2);
    m_ARIXObj->SetOutputHistory(v2);

    for(int i=0; i<m_nK; ++i)
        vOut(0) = m_ARIXObj->Simulate(v[0]);
    for(int i=0; i<m_nH; ++i)
        vOut(i) = m_ARIXObj->Simulate(v[0]);
}

void CGPC::CalcRefValues(Eigen::VectorXd& vOut)
{
    // reset reference
    m_RefObj->ResetMemory();

    // setting feedback as reference output history
    std::vector<double> y;
    m_FeedbackHistory.RetriveNSamples(y);
    m_RefObj->SetOutputHistory(y);

    std::vector<double> u;
    m_OutputHistory.RetriveNSamples(u);
    m_RefObj->SetInputHistory(u);

    std::cout << v2str(y) << std::endl;

    SaveGeneratorHistory();

    // simulate H samples
    for(int i=0; i<m_nH; ++i)
    {
        vOut(i) = m_RefObj->Simulate(GetNextGeneratorValue());
    }

    LoadGeneratorHistory();
}

void CGPC::SaveState(boost::property_tree::ptree& pt) const
{
    // storing data into an XML file
    using boost::property_tree::ptree;

    ptree & node = pt.add("Object.Name", "");
    node.put("ID", m_nID);
    node.put("Type", m_Type);

    node.put("L", m_nL);
    node.put("H", m_nH);
    node.put("RO", m_dRO);
    node.put("Alpha", m_dAlpha);
    node.put("Setpoint", m_dSV);
    node.put("K", m_nK);

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

void CGPC::LoadState(boost::property_tree::ptree::value_type const& v)
{
    boost::property_tree::ptree ptGen = v.second;
    // Load regulator data
    double L = v.second.get<double>("L"),
           H = v.second.get<double>("H"),
           RO = v.second.get<double>("RO"),
           Alpha = v.second.get<double>("Alpha"),
           K = v.second.get<double>("K");

    SetParams(L, H, RO, Alpha, K);
    SetSetpointValue(v.second.get<double>("Setpoint"));

#ifdef _DEBUG
    std::cout << "L: " << v.second.get<double>("L") << std::endl;
    std::cout << "Setpoint: " << v.second.get<double>("Setpoint") << std::endl;
    std::cout << "H: " << v.second.get<double>("H") << std::endl;
    std::cout << "RO: " << v.second.get<double>("RO") << std::endl;
    std::cout << "Alpha: " << v.second.get<double>("Alpha") << std::endl;
#endif

    LoadGeneratorState(ptGen);
}
