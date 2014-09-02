/** \class CGPC
 * Implementation of a GPC regulator with respect to the requirements given in the
 * WymaganiaProgr2014.pdf document available for PSS students.
 *
 * \note
 * Uses Eigen-Eigen matrix and vector mathematics library.
 */

#ifndef _CGPC
#define _CGPC

#include "Regulator.h"
#include "SimObject.h"
#include <vector>
#include "Eigen/Dense"

class CGPC : public CRegulator
{
public:
    CGPC(int nID = 0, ObjType Type = gpcregulator, std::string sName = "GPCRegulator");

    /// \brief To call on new simulation.
    void Initialize();

    /// @copydoc CSimNode::Simulate(double)
    double Simulate(double) override;

    /// \brief Sets basic GPC parameters.
    /// \param[in] nL Sets L param.
    /// \param[in] nH Sets control horizon.
    /// \param[in] dRO Sets ro parameter.
    /// \param[in] dAlpha Sets reference signal parameter.
    /// \param[in] nK Sets delay.
    void SetParams(int nL, int nH, double dRO, double dAlpha, int nK);

    /// \brief Allows to set explicitly object for prediction.
    /// \param[in] CObj Object to use for prediction.
    void SetObjectForPrediction(ISISO* CObj = NULL);

    /// @copydoc CSimNode::SaveState(boost::property_tree::ptree& pt) const
    void SaveState(boost::property_tree::ptree& pt) const override;

    /// @copydoc CSimNode::LoadState(boost::property_tree::ptree::value_type const&)
    void LoadState(boost::property_tree::ptree::value_type const& v) override;

    /// @copydoc ISISO::ResetMemory()
    void ResetMemory() override
    {
        Initialize();
    }

private:
    /// \brief Runs identification algorithm.
    void Identify();

    /// \brief Calculates new Q vector.
    void CreateQ();

    /// \brief Calculates free response vector.
    void CalcFreeResponse(Eigen::VectorXd&);

    /// \brief Calculates reference signal.
    void CalcRefValues(Eigen::VectorXd&);

    /// Object to use for Q vector calculation.
    std::shared_ptr<CSimObject> m_StepObj;
    /// Object to use for reference signal calculation.
    std::shared_ptr<CSimObject> m_RefObj;
    /// Object to use for free response calculation.
    std::shared_ptr<CSimObject> m_ARIXObj;

    /// Last calculated Q vector.
    Eigen::VectorXd m_vQ;
    /// History of feedback to use for identification.
    CHistorian m_FeedbackHistory;

    /// L parameter.
    int m_nL;
    /// H parameter.
    int m_nH;
    /// Delay.
    int m_nK;
    /// Ro parameter.
    double m_dRO;
    /// Alpha parameter.
    double m_dAlpha;
    /// Stores last value generated.
    double m_LastValueFromGen;
    /// Times identification algorithm was used.
    int m_noTimesIdentified;
    /// Was non-zero generator input observed?
    bool m_bFirstNonZeroInput;
};

#endif
