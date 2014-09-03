/** \class SLogic
 * Implemenets Control module of the MVC model. \n
 * 1. Responsible for connecting GUI with simulation data model and organising data flow. \n
 * 2. Enables saving and loading the state of the program from and external file. \n
 * 3. Features simulation in an external thread. \n
 * 4. Owns root of the simulation chain and identification algorithm visible to the user via GUI.
 * \note
 * Implements multi-threading safe singleton pattern.
 * \warning
 * This class is not a RAII class. It needs to be explicitly destroyed
 * by the end of the application lifetime to avoid potential invalid
 * memory accesses.
*/

//#define _DEBUG

#ifndef _SLOGIC
#define _SLOGIC
#include <memory>
#include <mutex>
#include <iostream>
#include <fstream>
#include "SimObject.h"
#include "PRegulator.h"
#include "PIDRegulator.h"
#include "NoiseGen.h"
#include "PulseGen.h"
#include "SinGen.h"
#include "SquareGen.h"
#include "StepGen.h"
#include "TriangleGen.h"
#include "SObjectFactory.h"
#include "mainwindow.h"
#include <QtConcurrent/QtConcurrentRun>
#include <thread>
#include <QMetaObject>
#include "ARXIdentification.h"

class SLogic
{
public:
    /// \brief Returns the only one instance of the singleton
    static SLogic& GetInstance()
    {
        //creating the only instance of the class
        std::call_once(SLogic::m_OneCreation, []()
        {
            SLogic::m_Instance.reset(new SLogic());
        });

        return *SLogic::m_Instance;
    }

    /// \brief Main program flow is defined in this function
    void Run();

    /// \brief Main simulation procedure. Takes time and period as milliseconds.
    /// \param[in] nTime Simulation interval.
    /// \param[in] nPeriod Simulation period.
    void RunSimulation(int nTime, int nPeriod);

    /// \brief Runs or pauses simulation. Uses m_nTime and m_nPeriod values.
    void ToggleSimulation();

    /// \brief Sets parameters. Accepts values in miliseconds.
    /// \param[in] nTime Simulation interval.
    /// \param[in] nPeriod Simulation period.
    void SetSimulationParameters(int nTime, int nPeriod)
    {
        m_nTime = nTime;
        m_nPeriod = nPeriod;
    }

    /// \brief Checks if simulation can be executed properly
    /// \return True if everything is set up properly (data loaded etc).
    bool IsSimulatonChainReady();

    /// \brief Resets all memory values from the simulation chain. Does not delete
    /// objects parameters or generators.
    void ResetSimulation()
    {
        // delete memory of every simulation object
        m_SimRoot->ResetMemory();

        // remove last stored value
        m_dLastSimVal = 0;
    }

    /// \brief Deletes the only instance. This must be called after the Run() function in order
    /// to dealocate program memory in managed way.
    static void DestroyInstance()
    {
        m_Instance.reset();
    }

    /// \brief Commands to write the simualtion chain data into the specified file
    /// \param[in] sFileName Filename to save data into.
    /// \return True if successfully saved.
    bool SaveSimChain(const std::string sFileName);

    /// \brief Tries to load the simulation chain from the .xml file
    /// \param[in] sFileName Filename to load data from.
    /// \return True if successfully loaded.
    bool LoadSimChain(const std::string sFileName);

    /// \brief Sets object to save its output to specified file
    /// \param[in] sObjName Object name to search for.
    /// \param[in] sFileName File to save output data to.
    /// \return True if successfully set up the data transmission.
    bool SaveObjectOutputToFile(const std::string sObjName, std::string sFileName = "");

    /// \brief Disables the file output for a chosen object
    /// \param[in] sObjName Object name to search for.
    /// \return True if successfully found the object.
    bool StopSavingObjectOutputToFile(const std::string sObjName);

    /// \brief Returns simulation period
    int GetPeriod() const
    {
        return m_nPeriod;
    }

    /// \brief Changes identification parameters.
    /// \param[in] nNomDegree Nominator degree.
    /// \param[in] nDenomDegree Denominator degree.
    /// \param[in] nDelay Delay value.
    /// \param[in] nTreshold Treshold for estimator safety.
    /// \param[in] dForgettingFactor Forgetting factor.
    void ChangeIdentificationParams(int nNomDegree,
         int nDenomDegree, int nDelay, int nTreshold, double dForgettingFactor)
    {
        std::lock_guard<std::mutex> guard(identifyMutex);
        m_ARXIdentAlg.reset(new CARXIdentification(nNomDegree, nDenomDegree, nDelay, 10, dForgettingFactor, nTreshold));
        m_ARXIdentAlg->ResetWholeHistory();
    }

    /// \brief Retrieves last identified nominator.
    /// \param[out] vNom Vector with nominator values.
    void GetLastIdentifiedNominator(std::vector<double>& vNom)
    {
        std::lock_guard<std::mutex> guard(identifyMutex);
        vNom = m_ARXIdentAlg->ReturnThetaNominator();
    }

    /// \brief Retrieves last identified denominator.
    /// \param[out] vNom Vector with denominator values.
    void GetLastIdentifiedDenominator(std::vector<double>& vDenom)
    {
        std::lock_guard<std::mutex> guard(identifyMutex);
        vDenom = m_ARXIdentAlg->ReturnThetaDenominator();
    }

    /// \brief Set the GUI handle. This method has to be called as soon as possible
    /// after creation of the SLogic instance.
    /// \param[in] MainWindowPtr Pointer to the GUI main window.
    void SetGUIInstance(MainWindow* MainWindowPtr)
    {
        m_GUIHandle = MainWindowPtr;
    }

    /// \brief When user selects an object in GUI, this method is called.
    /// It searches object with the given name and builds its property tree.
    /// \param[in] sObjectName Objects name.
    void ObjectFocusChange(std::string& sObjName);

    /// \brief Helper method to call recursively when building property tree.
    /// \param[in] qsParentName Name of the parent node.
    /// \param[in] p Property tree to build gui tree from.
    void ExtractPropertySubtree(QString& qsParentName, const boost::property_tree::ptree& p);

    /// \brief Called from GUI when the value is edited.
    /// \param[in] sObjName Name of the object edited.
    /// \param[in] sKey Key edited.
    /// \param[in] sValue New value of the key.
    void SelectedObjectEdited(const std::string& sObjName, const std::string& sKey, const std::string& sValue);

    /// \brief Called from SelectedObjectEdited(). Helper method to update simulation tree.
    /// \param[in] p Updated property tree.
    /// \param[in] sObjName Name of the object edited.
    /// \param[in] sKey Key edited.
    /// \param[in] sNewValue New value of the key.
    bool UpdateTreeValue(boost::property_tree::ptree& p, const std::string& sObjName, const std::string& sKey, const std::string& sNewValue);

    ~SLogic();

private:
    /// \brief Simulation main working route.
    /// \param[in] nTime Simulation interval.
    /// \param[in] nPeriod Simulation period.
    void m_RunSimulation(int nTime, int nPeriod);

    /// Property tree of the last selected object
    boost::property_tree::ptree m_SelectedObjectProperties;

    /// GUI handle pointer
    MainWindow* m_GUIHandle;

    /// ARX object identification algorithm
    std::shared_ptr<CARXIdentification> m_ARXIdentAlg;

    /// Mutex for identification access.
    std::mutex identifyMutex;
    /// Mutex for simulation tree access.
    std::mutex simTreeMutex;

    /// Simulation interval.
    int m_nTime;
    /// Simulation period.
    int m_nPeriod;
    /// Simulation handle.
    QFuture<void> m_Handle;
    /// Last value received from simulation.
    double m_dLastSimVal;

    /// Root of the simulation chain.
    std::shared_ptr<CSimObject> m_SimRoot;

    // variables for singleton implementation
    static std::once_flag m_OneCreation;
    static std::shared_ptr<SLogic> m_Instance;

    // nonusable elements
    SLogic();
    SLogic(const SLogic&);
    SLogic& operator=(const SLogic&);
};


#endif
