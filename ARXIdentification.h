#ifndef _CARXIDENTIFICATION
#define _CARXIDENTIFICATION

#include <vector>
#include <Eigen/Dense>
#include <mutex>
#include <qmutex.h>
/** \class CARXIdentification
* The purpose of this class is to allow the possibility of 
* creating a transfer function, which will predict the output of a 
* discrete dynamic system described by data contained inside the 
* instance of CARXIdentification class. 
*/
class CARXIdentification
{
public:
    CARXIdentification(); /// \brief This is the default class constructor.
    CARXIdentification(int, int, int, int, double, double); /// \brief This is the class constructor that allows to initiate the values of certain class variables.
     ///\param[in] iDegree_i This is the degree of the nominator of the predicting transfer function.
     ///\param[in] iDegree_o This is the degree of the denominator of the predicting transfer function.
     ///\param[in] iDelay This is the delaying factor of the predicting transfer function.
     ///\param[in] iHistLen This is the length of history of parameters of the predicting transfer function.
     ///\param[in] iForgettingFactor This is the forgetting factor from the Weighted Recursive Least Mean Squares method. 
     ///\param[in] dThreshold This a threshold value used in the calculation of the P matrix.
    ~CARXIdentification(); /// \brief This is the default class destructor.

    void AdjustFi(); /// \brief This method adjusts the length of the fi vector. \n 
    ///The length to which it will be adjusted to is equal to the sum of the desired degree of the nominator and denominator of the predicting transfer function plus 1). New coordinates are filled with 0s.
    void AdjustP();  /// \brief This method adjusts the length of the P matrix to the current values of the degrees of the predicting transfer function's nominator and denominator. \n
    ///New coordinates are filled with 0s.
    void AdjustTheta(char, int);/// \brief This method adjusts the length of the theta vector to the current values of the degrees of the predicting transfer function's nominator and denominator.
    ///\param[in] c This is a character variable that informs this method whether it should adjust the length of the nominator or the denominator.
    ///\param[in] iPrevDegree This is an integer variable that informs this method what was the previous degree of the currently adjusted polynomial.
    void AdjustThetaHistory();/// \brief This method adjusts the length of the history of theta parameters to the current values of the degrees of the predicting transfer function's nominator and denominator.
    void SetDelayTime(int); /// \brief This method allows to initiate the value of the time delay of the predicting transfer function.
    ///\param[in] value This is the variable which contains the new value of the predicting transfer function's time delay.
    void ChangeDelayTime(int); /// \brief This method allows to change the value of the time delay of the predicting transfer function.
    ///\param[in] value This is the variable which contains the new value of the predicting transfer function's time delay.
    void SetForgettingFactor(double); /// \brief This method allows to set the value of the forgetting factor from the Weighted Recursive Least Mean Squares method. 
    ///\param[in] value This is the variable which contains the new value of the WRLMS method's forgetting factor. 
    void SetHistoryLength(int); /// \brief This method allows to initiate the length of history of theta parameters. 
    ///\param[in] value This is the variable which contains the new value of the theta history. 
    void ChangeHistoryLength(int); /// \brief This method allows to change the length of history of theta parameters.
    ///\param[in] value This is the variable which contains the new value of the theta history. 

    void ResetWholeHistory(); /// \brief This method resets the history of inputs, outputs and theta parameters.

    std::vector<double> ReturnThetaNominator();/// \brief This method returns from the theta vector the values of the nominator of the predicting transfer function.
    /// \return vThetaNominator This vector of type double contains the values of the nominator of the predicting transfer function.
    std::vector<double> ReturnThetaDenominator();/// \brief This method returns from the theta vector the values of the denominator of the predicting transfer function.
    /// \return vThetaNominator This vector of type double contains the values of the denominator of the predicting transfer function.

    void ChangePolynomial_i_degree(int);/// \brief This method allows to change the degree of the nominator from the predicting transfer function.
    ///\param[in] value This is the variable which contains the new value of the nominator's degree.
    void ChangePolynomial_o_degree(int);/// \brief This method allows to change the degree of the denominator from the predicting transfer function.
    ///\param[in] value This is the variable which contains the new value of the denominator's degree.

    void AddInputElement(double);/// \brief This method allows to add an input value to the vector that contains the history of input values.
    ///\param[in] Iinput This is the input value that is added to input history.
    void AddOutputElement(double);/// \brief This method allows to add an output value to the vector that contains the history of output values.
    ///\param[in] Iinput This is the output value that is added to output history.

    double ComputeQualityIndicator(void); /// \brief This method allows to compute the quality indicator of the WRLMS method (unused).

    void Update(); /// \brief This method updates the fi vector, the theta vector and the P matrix.

private:
    std::vector<double> CreateTheta();
    void UpdateFi();
    void UpdatePmatrix(int);
    void UpdatePmatrixNE(int);
    void UpdateTheta();
    void UpdateThetaHistory();

    void ResetInputHistory();
    void ResetOutputHistory();
    void ResetThetaHistory();

    void SetPolynomial_i_degree(int);
    void SetPolynomial_o_degree(int);

    double ComputeWRLMSestimator();

    int m_iPolynomial_i_degree;
    int m_iPolynomial_o_degree;
    int m_iDelayTime;
    int m_iHistoryLength;

    double m_dSigma;
    double m_dForgettingFactor;
    double m_dBeta;
    double m_dBetaNE;
    double m_dAlpha;
    double m_dT;

    std::vector<double> m_vOutputHistory;
    std::vector<double> m_vInputHistory;

    Eigen::MatrixXd m_vTheta;
    Eigen::MatrixXd m_mThetaHistory;
    Eigen::MatrixXd m_vFi;
    Eigen::MatrixXd m_mP;
};

#endif


