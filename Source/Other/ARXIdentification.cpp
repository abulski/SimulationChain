#include "ARXIdentification.h"
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <Eigen/Dense>


CARXIdentification::CARXIdentification(int iDegree_i, int iDegree_o, int iDelay, int iHistLen, double iForgettingFactor, double dThreshold)
{
    SetDelayTime(iDelay);
    SetHistoryLength(iHistLen);
    SetForgettingFactor(iForgettingFactor);
    SetPolynomial_i_degree(iDegree_i);
    SetPolynomial_o_degree(iDegree_o);
    m_dBetaNE = 0.5;
    m_dBeta=1000;
    m_dT = dThreshold;
    m_dSigma = 0;
    std::vector<double>vInHis(iDegree_i  + 1 + m_iDelayTime, 0);//When the identifying object is constructed, an input history is created;
                                                              //its length is equal to the number of input polynomial coefficients
    m_vInputHistory = vInHis;
    std::vector<double>vOutHis(iDegree_o + 1, 0);//When the identifying object is constructed, an output history is created;
                                                          //its length is equal to the number of output polynomial coefficients -1
    m_vOutputHistory = vOutHis;

    Eigen::MatrixXd vF = Eigen::MatrixXd::Constant(iDegree_i + iDegree_o + 1, 1, 0);
    m_vFi = vF;

    Eigen::MatrixXd vThet = Eigen::MatrixXd::Constant(iDegree_i + iDegree_o + 1, 1, 0);
    m_vTheta = vThet;

    Eigen::MatrixXd vThetaHist = Eigen::MatrixXd::Constant(m_iHistoryLength, iDegree_i + iDegree_o + 1, 0);
    m_mThetaHistory = vThetaHist;


    Eigen::MatrixXd P = Eigen::MatrixXd::Identity(iDegree_i + iDegree_o + 1, iDegree_i + iDegree_o + 1);
    P = P.array()*m_dBeta;
    m_mP = P;
}

CARXIdentification::~CARXIdentification()
{
}

void CARXIdentification::AdjustFi()
{
    Eigen::MatrixXd vFiTemp = Eigen::MatrixXd::Constant(m_iPolynomial_i_degree + m_iPolynomial_o_degree + 1, 1, 0);
    m_vFi = vFiTemp;
}

void CARXIdentification::AdjustP()
{
    Eigen::MatrixXd mPtemp = Eigen::MatrixXd::Identity(m_iPolynomial_i_degree + m_iPolynomial_o_degree + 1, m_iPolynomial_i_degree + m_iPolynomial_o_degree + 1);
    mPtemp = mPtemp.array()*m_dBeta;
    for (int i=0; i<mPtemp.rows();i++)
    {
        for (int j=0; j<mPtemp.cols();j++)
        {
            if ((i<m_mP.rows())&&(j<m_mP.cols()))
                mPtemp(i,j)=m_mP(i,j);
        }
    }
    m_mP = mPtemp;
}

void CARXIdentification::AdjustTheta(char c, int iPrevDegree)
{
    int iDeltaDegree = m_iPolynomial_i_degree - iPrevDegree;
    Eigen::MatrixXd vThetaTemp = Eigen::MatrixXd::Constant(m_iPolynomial_i_degree + m_iPolynomial_o_degree + 1, 1, 0);
    if (c == 'i')
    {

        for (int i=0; i<vThetaTemp.rows();i++)
        {
            if (iPrevDegree<m_iPolynomial_i_degree)
            {
                if(i<=iPrevDegree)
                {
                    vThetaTemp(i) = m_vTheta(i);
                }
                else if (i<=m_iPolynomial_i_degree)
                {
                    vThetaTemp(i) = 0;
                }
                else
                {
                    vThetaTemp(i) = m_vTheta(i-iDeltaDegree);
                }
            }
            else
            {
                if(i<=m_iPolynomial_i_degree)
                {
                    vThetaTemp(i) = m_vTheta(i);
                }
                else if (i>iPrevDegree)
                {
                    vThetaTemp(i-iDeltaDegree) = m_vTheta(i);
                }
            }
        }
    }
    else if(c=='o')
    {
        for (int i=0; i<vThetaTemp.rows();i++)
        {
            if (iPrevDegree<m_iPolynomial_o_degree)
            {
                if (i<=m_iPolynomial_i_degree+iPrevDegree)
                {
                    vThetaTemp(i) = m_vTheta(i);
                }
                else
                {
                    vThetaTemp(i) = 0;
                }
            }
            else
            {
                vThetaTemp(i) = m_vTheta(i);
            }
        }
    }

    m_vTheta = vThetaTemp;
}

void CARXIdentification::AdjustThetaHistory()
{
    Eigen::MatrixXd vThetaHist = Eigen::MatrixXd::Constant(m_iHistoryLength, m_iPolynomial_i_degree + m_iPolynomial_o_degree + 1, 0);
    for (int i=0; i<vThetaHist.rows();i++)
    {
        for (int j=0; j<vThetaHist.cols();j++)
        {
            if ((i<m_mThetaHistory.rows())&&(j<m_mThetaHistory.cols()))
                vThetaHist(i,j)=m_mThetaHistory(i,j);
        }
    }
    m_mThetaHistory = vThetaHist;
}

void CARXIdentification::SetPolynomial_i_degree(int value)
{
    m_iPolynomial_i_degree = value;
}

void CARXIdentification::ChangePolynomial_i_degree(int value)
{
    int iSizeChange = 0;
    int iPrev_i_degree = m_iPolynomial_i_degree;
    if (value>0)
    {
        if (value>m_iPolynomial_i_degree)
        {
            iSizeChange = value - m_iPolynomial_i_degree;
            m_iPolynomial_i_degree = value;
            m_vInputHistory.resize(m_vInputHistory.size()+iSizeChange,0);
        }
    }
    else
    {
        m_iPolynomial_i_degree = 1;
        m_vInputHistory.resize(m_iPolynomial_i_degree,0);
    }
    AdjustFi();
    AdjustP();
    AdjustTheta('i',iPrev_i_degree);
    AdjustThetaHistory();
}

void CARXIdentification::SetPolynomial_o_degree(int value)
{
    m_iPolynomial_o_degree = value;
}

void CARXIdentification::ChangePolynomial_o_degree(int value)
{
    int iSizeChange = 0;
    int iPrev_o_degree = m_iPolynomial_o_degree;
    if (value>0)
    {
        //if (value>m_iPolynomial_o_degree)
        {
            iSizeChange = value - m_iPolynomial_o_degree;
            m_iPolynomial_o_degree = value;
            m_vOutputHistory.resize(m_vOutputHistory.size()+iSizeChange,0);
        }
    }
    else
    {
        m_iPolynomial_o_degree = 1;
        m_vOutputHistory.resize(m_iPolynomial_o_degree,0);
    }
    AdjustFi();
    AdjustP();
    AdjustTheta('o',iPrev_o_degree);
    AdjustThetaHistory();
}

void CARXIdentification::SetDelayTime(int value)
{
    m_iDelayTime = value;
}

void CARXIdentification::ChangeDelayTime(int value)
{
    int iSizeChange = 0;
    if (value>0)
    {
        if (value>m_iDelayTime)
        {
            iSizeChange = value - m_iDelayTime;
            m_iDelayTime = value;
            m_vInputHistory.resize(m_vInputHistory.size()+iSizeChange,0);
        }
    }
    else
    {
        m_iDelayTime = 1;
    }


}

void CARXIdentification::SetHistoryLength(int value)
{
    m_iHistoryLength = value;
}

void CARXIdentification::ChangeHistoryLength(int value)
{
    m_iHistoryLength = value;
    AdjustThetaHistory();
}

void CARXIdentification::ResetInputHistory()
{
    std::vector<double> vZeroVector(m_vInputHistory.size(),0);
    m_vInputHistory = vZeroVector;
}

void CARXIdentification::ResetOutputHistory()
{
    std::vector<double> vZeroVector(m_vOutputHistory.size(),0);
    m_vOutputHistory = vZeroVector;
}

void CARXIdentification::ResetThetaHistory()
{
    Eigen::MatrixXd vZeroMatrix = Eigen::MatrixXd::Constant(m_mThetaHistory.rows(), m_mThetaHistory.cols(), 0);
    m_mThetaHistory = vZeroMatrix;
}

void CARXIdentification::ResetWholeHistory()
{
    ResetInputHistory();
    ResetOutputHistory();
    ResetThetaHistory();
}

std::vector<double> CARXIdentification::ReturnThetaNominator()
{
    std::vector<double> vThetaNominator(m_iPolynomial_i_degree+1,0);
    for(int i=0;i<vThetaNominator.size();i++)
        vThetaNominator[i] = m_vTheta(i);
    return vThetaNominator;
}
std::vector<double> CARXIdentification::ReturnThetaDenominator()
{
    std::vector<double> vThetaDenominator(m_iPolynomial_o_degree,0);
    for(int i=0;i<vThetaDenominator.size();i++)
        vThetaDenominator[i] = m_vTheta(m_iPolynomial_i_degree+1+i);
    return vThetaDenominator;
}

void CARXIdentification::SetForgettingFactor(double value)
{
    m_dForgettingFactor = value;
}
void CARXIdentification::AddInputElement(double iInput)
{
    m_vInputHistory.insert(m_vInputHistory.begin(), iInput);
    m_vInputHistory.pop_back();
}
void CARXIdentification::AddOutputElement(double iOutput)
{
    m_vOutputHistory.insert(m_vOutputHistory.begin(), iOutput);
    m_vOutputHistory.pop_back();
}

void CARXIdentification::UpdateFi()
{
    for (int i = 0; i < m_vFi.size(); i++)
    {
        if (i < m_iPolynomial_i_degree + 1)
            m_vFi(i) = m_vInputHistory[m_iDelayTime + i];
        else
            m_vFi(i) = (-1)*m_vOutputHistory[1 + i % (m_iPolynomial_i_degree + 1)];
    }
}

void CARXIdentification::Update()
{
    UpdateFi();
    UpdatePmatrixNE(0);
    UpdateTheta();
}

std::vector<double> CARXIdentification::CreateTheta()
{
    int iVectorLength = m_iPolynomial_i_degree + m_iPolynomial_o_degree + 1;
    std::vector<double> vTheta(iVectorLength);
    for (int i = 0; i < iVectorLength; i++)
        vTheta[i] = 0;
    return vTheta;
}

double CARXIdentification::ComputeWRLMSestimator()
{
    Eigen::MatrixXd wynik = m_vTheta.transpose()*m_vFi;
    return (m_vOutputHistory.at(0) - wynik(0,0));
}

void CARXIdentification::UpdatePmatrix(int iDisplay)
{
    Eigen::MatrixXd nominator = m_mP*m_vFi*m_vFi.transpose()*m_mP;
    Eigen::MatrixXd denominator = m_vFi.transpose()*m_mP*m_vFi;
    Eigen::MatrixXd fraction = nominator.array() / (m_dForgettingFactor + denominator(0, 0));
    Eigen::MatrixXd result;
    if (iDisplay > 0)
    {
        std::cout << "\nnominator:\n" << nominator;
        std::cout << "\ndenominator:\n" << denominator;
        std::cout << "\nfraction:\n" << fraction;
        std::cout << "\nmP - fraction:\n" << m_mP.array() - fraction.array();
    }
    result = (m_mP.array() - fraction.array());
    m_mP =  result.array() / m_dForgettingFactor;
}

void CARXIdentification::UpdatePmatrixNE(int)
{
    Eigen::MatrixXd  k;
    Eigen::MatrixXd numerator1;
    Eigen::MatrixXd denumerator1;
    double numerator2;
    Eigen::MatrixXd denumerator2;
    Eigen::MatrixXd result1;
    double result2;
    Eigen::MatrixXd result3;
    Eigen::MatrixXd tempP;

    //calculating temporary P matrix
    numerator1 = m_mP*m_vFi;
    denumerator1 = m_vFi.transpose()*m_mP*m_vFi;
    denumerator1 = denumerator1.array()+1;
    result1 = numerator1.array()/denumerator1(0);
    k = result1;
    tempP = k*m_vFi.transpose()*m_mP;

    //calculating the alpha coefficient
    numerator2 = pow(ComputeWRLMSestimator(),2);
    denumerator2 = m_vFi.transpose()*m_mP*m_vFi;
    m_dSigma = sqrt(m_dBetaNE*pow(m_dSigma,2) + (1-m_dBetaNE)*pow(ComputeWRLMSestimator(),2));
    denumerator2 = (denumerator2.array()+1)*1000*m_dSigma;
    result2 = numerator2/denumerator2(0,0);
    m_dAlpha = -result2+1;
    result3 =(tempP.array()/m_dAlpha);
    if (result3.trace()<=m_dT)
        m_mP = m_mP.array() - tempP.array()/m_dAlpha;
    else
        m_mP = m_mP - tempP;
}

void CARXIdentification::UpdateTheta()
{
    double estimator = ComputeWRLMSestimator();
    m_vTheta = m_vTheta + m_mP*m_vFi*estimator;
    UpdateThetaHistory();
}

void CARXIdentification::UpdateThetaHistory()
{
    for (int i = m_mThetaHistory.rows()-1; i >= 0; i--)
    {
        for (int j = 0; j < m_mThetaHistory.cols(); j++)
        {
            if (i != 0)
                m_mThetaHistory(i, j) = m_mThetaHistory(i - 1, j);
            else
                m_mThetaHistory(i, j) = m_vTheta(j, i);
        }

    }
}
