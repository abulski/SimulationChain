#include "SLogic.h"

#ifdef _DEBUG
#include <assert.h>
#endif

void SLogic::Run()
{
    // load stored data
	LoadSimChain("ChainDataIn.xml");
	
    // create generator
	IGenerator* gen = new CStepGen();

    // add generator
	try
	{ 
        // requires downcasting with dynamic_cast
		ISISO* reg_obj = m_SimRoot->SearchObject("PRegulator");
		CRegulator* reg = dynamic_cast<CRegulator*>(reg_obj);
		reg->AddGenerator(std::shared_ptr<IGenerator>(gen));
		IGenerator* gen2 = new CSineGen;
        reg->AddGenerator(std::shared_ptr<IGenerator>(gen2));
	}
	catch (std::bad_cast& e)
	{
        // TODO error handling
	}

    // assign output files to selected objects
	SaveObjectOutputToFile("PRegulator");
	SaveObjectOutputToFile("SimulationRoot");

    // run simulation with negative feedback
	double next = 0;
	for (int i = 0; i < 100; ++i)
	{
		next = m_SimRoot->Simulate(next);
		std::cout << next << std::endl;
	}

    // store loaded data to prove its integrity
	SaveSimChain("ChainDataOut.xml");
}

void SLogic::RunSimulation(int nTime, int nPeriod)
{
    // if simualtion chain is not ready - return
    if (!IsSimulatonChainReady())
        return;

    // prepare the simulation chain
    m_GUIHandle->ResetXAxis();
    ResetSimulation();

    // Run simulation in a concurrent thread
    QFuture<void> handle = QtConcurrent::run(this, (&SLogic::m_RunSimulation), nTime, nPeriod);
}

void SLogic::ToggleSimulation()
{
    // if simualtion chain is not ready - return
    if (!IsSimulatonChainReady())
        return;
    // if the thread has been started - toggle the pause
    if(!m_Handle.isStarted())
        m_Handle.togglePaused();
    else
    {
        // If not run simulation in a concurrent thread
        m_Handle = QtConcurrent::run(this, (&SLogic::m_RunSimulation), m_nTime, m_nPeriod);
    }
}

bool SLogic::IsSimulatonChainReady()
{
    // very basic check - definitly not good enough to make this procedure reliable
    std::list<std::weak_ptr<ISISO> > children;
    simTreeMutex.lock();
    m_SimRoot->GetChildren(children);
    simTreeMutex.unlock();
    int childCount = children.size();
    if (childCount == 0)
        return false;
    return true;
}

bool SLogic::SaveSimChain(const std::string sFileName)
{
    // directives to make using boost functionality more readable
	using boost::property_tree::write_xml;
	using boost::property_tree::ptree;
	using boost::property_tree::xml_writer_settings;
	using boost::property_tree::xml_parser_error;

    // output file stream
	std::ofstream fs;
	fs.open(sFileName);
	if (!fs)
	{
        // TODO error handling
		return false;
	}

    // initializing tree structure and XML parser
	ptree pt;
	xml_writer_settings<char> settings('\t', 1);

    // initializing chain-saving objects state into tree structure
	m_SimRoot->SaveState(pt);

    //after building the tree, save it as XML file
	try
	{
		write_xml(fs, pt, settings);
	}
	catch (xml_parser_error& e)
	{
        // TODO error handling
	}

	return true;
}

void SLogic::ObjectFocusChange(std::string& sObjName)
{
    // fetch object data
    simTreeMutex.lock();
    ISISO* obj = m_SimRoot->SearchObject(sObjName);
    if(obj == nullptr)
        return;
    m_SelectedObjectProperties.clear();
    obj->SaveState(m_SelectedObjectProperties);
    simTreeMutex.unlock();

    // clear the property view
    QMetaObject::invokeMethod(m_GUIHandle, "ClearTreeWidget", Qt::QueuedConnection,
                                  Q_ARG(int, 1));


    // create property root
    QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetRoot", Qt::QueuedConnection,
                                  Q_ARG(QString, "Properties"),
                                  Q_ARG(QString, ""),
                                  Q_ARG(int, 1));

    // build tree structure
    QString rootName = "Properties";
    ExtractPropertySubtree(rootName, m_SelectedObjectProperties);
}

void SLogic::ExtractPropertySubtree(QString& qsParentName, const boost::property_tree::ptree& p)
{
    boost::property_tree::ptree::const_iterator it = p.begin();
    for(; it != p.end(); ++it)
    {
        //if the second field doesnt contain ptree
        if(it->second.empty())
        {
            //add element to tree
            QString qsName = QString::fromStdString(it->first.data());
            QString qsValue = QString::fromStdString(it->second.data());
            QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetElement",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, qsParentName),
                                          Q_ARG(QString, qsName),
                                          Q_ARG(QString, qsValue),
                                          Q_ARG(int, 1));
            continue;
        }

        //if the second field contains ptree show a new branch
        QString qsName = QString::fromStdString(it->second.get<std::string>("<xmlattr>.Name", ""));

        if(qsName == "")
        {
            ExtractPropertySubtree(qsParentName, it->second);
            continue;
        }

        QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetElement",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, qsParentName),
                                      Q_ARG(QString, qsName),
                                      Q_ARG(QString, ""),
                                      Q_ARG(int, 1));
        ExtractPropertySubtree(qsName, it->second);
    }
}

bool SLogic::LoadSimChain(const std::string sFileName)
{
    // directives to make boost functionality more readable
	using boost::property_tree::read_xml;
	using boost::property_tree::ptree;
	using boost::property_tree::xml_parser_error;
	using boost::property_tree::ptree_bad_data;

    // output file stream
	std::ifstream fs;
	fs.open(sFileName);
	if (!fs)
	{
        // TODO error handling
		return false;
	}

    // initializing tree structure
	ptree pt;

	try
	{
        // Running XML parser
		read_xml(fs, pt);
	}
	catch (xml_parser_error& e)
	{
        // TODO error handling
	}

    // deleting current simulation chain
    m_SimRoot.reset();
    m_SimRoot = std::shared_ptr<CSimObject>(new CSimObject(0, serial, "SimulationRoot"));
    //m_GUIHandle->AddTreeWidgetRoot("SimulationRoot", "Object");
    QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetRoot", Qt::QueuedConnection,
                                  Q_ARG(QString, "SimulationRoot"),
                                  Q_ARG(QString, "Object"),
                                  Q_ARG(int, 0));

	try
	{
		BOOST_FOREACH(ptree::value_type const& v, pt.get_child("Object"))
		{
			if (v.first == "Name")
			{
#ifdef _DEBUG
				std::cout << "\n\n-----------------------" << std::endl;
				std::cout << "Object detected in file." << std::endl;
				std::cout << "Name: " << v.second.get<std::string>("<xmlattr>.Name", "no_name") << std::endl;
				std::cout << "ID: " << v.second.get<int>("ID") << std::endl;
                std::cout << "Type: " << v.second.get<int>("Type") << std::endl;
                std::cout << "Parent name: " << v.second.get<std::string>("Parent", "0") << std::endl;
#endif
				
                // find the parent object
				std::string sParentName;
                sParentName = v.second.get<std::string>("Parent");
				ISISO* ParentObject = m_SimRoot->SearchObject(sParentName);

                // if parent name is invalid or null - discard data
				if (sParentName == "0" || ParentObject == nullptr)
					continue;

                // create object of proper type
				ObjType type = static_cast<ObjType>(v.second.get<int>("Type"));
				ISISO* NewObject = SObjectFactory::GetInstance().CreateObject(type);

                // checking if created object is valid
				if (NewObject == nullptr)
					continue;

                // Set parent
				NewObject->SetParent(ParentObject);

                // Setting basic object data
                std::string sName = v.second.get<std::string>("<xmlattr>.Name", "no_name");
                NewObject->SetName(sName);
                // TODO Think of better way of handling IDs
				//SetID(v.second.get<int>("ID"));
				NewObject->SetType(type);
				
                // Letting object set up its own data
				NewObject->LoadState(v);

                QString qsName = QString::fromStdString(sName);
                QString qsParentName = QString::fromStdString(sParentName);
                if(SObjectFactory::GetInstance().IsARegulator(type))
                {
                    QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetElement",
                                                  Qt::QueuedConnection,
                                                  Q_ARG(QString, qsParentName),
                                                  Q_ARG(QString, qsName),
                                                  Q_ARG(QString, "Regulator"),
                                                  Q_ARG(int, 0));

                    CRegulator* reg = nullptr;
                    try
                    {
                        reg = dynamic_cast<CRegulator*>(NewObject);
                    }
                    catch (...)
                    {
                        //TODO error handling
                        throw;
                    }

                    std::list<std::weak_ptr<IGenerator> > genList;
                    reg->GetGeneratorList(genList);

                    std::list<std::weak_ptr<IGenerator> >::iterator it = genList.begin();
                    for(; it != genList.end(); ++it)
                    {
                        QString genName = QString::fromStdString(it->lock()->GetName());
                        QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetElement",
                                                      Qt::QueuedConnection,
                                                      Q_ARG(QString, qsName),
                                                      Q_ARG(QString, genName),
                                                      Q_ARG(QString, "Generator"),
                                                      Q_ARG(int, 0));
                    }
                }
                else
                {
                    QMetaObject::invokeMethod(m_GUIHandle, "AddTreeWidgetElement",
                                                  Qt::QueuedConnection,
                                                  Q_ARG(QString, qsParentName),
                                                  Q_ARG(QString, qsName),
                                                  Q_ARG(QString, "Object"),
                                                  Q_ARG(int, 0));
                }
			}
		}
	}
	catch (ptree_bad_data& e)
	{
        // TODO error handling
		std::cout << "ptree_bad_data" << std::endl;
		throw;
	}
	catch (...)
	{
        // TODO erro handling
		std::cout << "Error reading file data." << std::endl;
	}

	return true;
}


bool SLogic::SaveObjectOutputToFile(const std::string sObjName, std::string sFileName)
{
    // searching for chosen object
	ISISO* obj = m_SimRoot->SearchObject(sObjName);
    // checking if found
	if (obj == nullptr)
		return false;

    // file stream creation
	std::shared_ptr<std::ofstream> os(new std::ofstream);
	
    // checking if default value is set right, assigning objects name if not
	if (sFileName == "")
		sFileName = sObjName + ".txt";

	os->open(sFileName);
	if (!os)
	{
        // TODO error handling
		return false;
	}

    // assigning stream to object
	obj->SetStreamForOutput(os);
	return true;
}


bool SLogic::StopSavingObjectOutputToFile(const std::string sObjName)
{
    // searching for chosen object
	ISISO* obj = m_SimRoot->SearchObject(sObjName);
    // checking if found
	if (obj == nullptr)
		return false;

    // disabling the saving procedure
	obj->RemoveStreamForOutput();
	return true;
}

void SLogic::SelectedObjectEdited(const std::string& sObjName, const std::string& sKey, const std::string& sValue)
{
    boost::property_tree::ptree p = m_SelectedObjectProperties;
    if(m_SelectedObjectProperties.get_child_optional("Object"))
        p = m_SelectedObjectProperties.get_child("Object");
    std::string sName = p.get<std::string>("Name.<xmlattr>.Name");

    UpdateTreeValue(p, sObjName, sKey, sValue);
    //std::cout << p.get_child("xmlattr.Name").data() << std::endl;

    // directives to make boost functionality more readable
    using boost::property_tree::write_xml;
    using boost::property_tree::xml_writer_settings;
    using boost::property_tree::ptree;
    using boost::property_tree::xml_parser_error;
    using boost::property_tree::ptree_bad_data;

    std::ofstream fs;
    fs.open("sFileName.xml");

    // initializing tree structure and XML parser
    xml_writer_settings<char> settings('\t', 1);

    //after building the tree, save it as XML file
    try
    {
        write_xml(fs, p, settings);
    }
    catch (xml_parser_error& e)
    {
        // TODO error handling
    }


    ISISO* obj = m_SimRoot->SearchObject(sName);
    if(obj == nullptr)
        return;
    //p = p.front();

    m_SelectedObjectProperties = p;
    obj->LoadState(p.front());
}

bool SLogic::UpdateTreeValue(boost::property_tree::ptree& p, const std::string& sObjName, const std::string& sKey, const std::string& sNewValue)
{
    std::string sName;
    BOOST_FOREACH(boost::property_tree::ptree::value_type& v, p)
    {
        if(v.second.empty())
            continue;
        sName = v.second.get<std::string>("<xmlattr>.Name","");
        if(sName == sObjName)
        {
            std::cout << "Found value" << std::endl;
            v.second.put(sKey, sNewValue);
            return true;
        }

        if(UpdateTreeValue(v.second, sObjName, sKey, sNewValue))
            return true;
    }

    return false;
}

void SLogic::m_RunSimulation(int nTime, int nPeriod)
{
    // variables to store the important values
    std::shared_ptr<double> dInVal(new double);
    std::shared_ptr<double> dOutVal(new double);
    std::shared_ptr<double> dObjInVal(new double);
    std::shared_ptr<double> dObjOutVal(new double);

    // find regulator
    simTreeMutex.lock();
    CRegulator* reg = nullptr;
    try
    {
        reg = dynamic_cast<CRegulator*>(m_SimRoot->FindFirstRegulator());
    }
    catch (...)
    {
        // if regulator not found - return
        return;
    }

    // link it with variables
    reg->SetVariableToStoreCurrentInput(dInVal);
    reg->SetVariableToStoreCurrentOutput(dOutVal);

    // find object
    CSimObject* obj = nullptr;
    try
    {
        obj = dynamic_cast<CSimObject*>(m_SimRoot->SearchObject("SimObject"));
    }
    catch (...)
    {
        return;
    }
    simTreeMutex.unlock();

    obj->SetVariableToStoreCurrentInput(dObjInVal);
    obj->SetVariableToStoreCurrentOutput(dObjOutVal);

    // theta
    std::string str = "";

    int nNumOfIter = nTime / nPeriod;
    for (int i = 0; i < nNumOfIter; ++i)
    {
        // run simulation with negative feedback
        simTreeMutex.lock();
        m_dLastSimVal = m_SimRoot->Simulate(m_dLastSimVal);
        simTreeMutex.unlock();

        // update plot output
        QMetaObject::invokeMethod(m_GUIHandle, "AddPointToOutputSignal", Q_ARG(double, m_dLastSimVal));
        QMetaObject::invokeMethod(m_GUIHandle, "AddPointToGeneratorSignal", Q_ARG(double, *dInVal));
        QMetaObject::invokeMethod(m_GUIHandle, "AddPointToControlSignal", Q_ARG(double, *dOutVal));

        // run identification
        identifyMutex.lock();
        m_ARXIdentAlg->AddInputElement(*dObjInVal);
        m_ARXIdentAlg->AddOutputElement(*dObjOutVal);
        m_ARXIdentAlg->Update();

        // display current theta
        std::vector<double> nom = m_ARXIdentAlg->ReturnThetaNominator();
        std::vector<double> denom =  m_ARXIdentAlg->ReturnThetaDenominator();
        identifyMutex.unlock();
        str = "N: " + v2str(nom) + "\nD: " + v2str(denom);
        QString s = QString::fromStdString(str);
        QMetaObject::invokeMethod(m_GUIHandle, "DisplayTheta", Q_ARG(QString, s));

        // wait a given period
        std::this_thread::sleep_for(std::chrono::milliseconds(nPeriod));
    }
}

SLogic::SLogic() : m_nPeriod(10), m_nTime(1000), m_dLastSimVal(0)
{
    // creating a simualtion root
	m_SimRoot = std::shared_ptr<CSimObject>(new CSimObject(1, serial, "SimulationRoot"));

    // creating an identification object instance with default values
    m_ARXIdentAlg.reset(new CARXIdentification(1, 2, 0,20, 0.99,100));
}


SLogic::~SLogic()
{
}

std::once_flag SLogic::m_OneCreation;
std::shared_ptr<SLogic> SLogic::m_Instance = nullptr;
