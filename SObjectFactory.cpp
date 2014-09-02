#include "SObjectFactory.h"

ISISO* SObjectFactory::CreateObject(ObjType NewObjectType)
{
	/// Object creation based on selected type
	switch (NewObjectType)
	{
	case parallel:
		return new CSimObject(0, parallel, "ParallelObject");
		break;
	case serial:
		return new CSimObject(0, serial, "SerialObject");
		break;
	case simobject:
		return new CSimObject(0, simobject, "SimObject");
		break;
	case regulator:
        return nullptr; /// regulator is an abstract class
		break;
	case pregulator:
		return new CPRegulator;
    case pidregulator:
        return new CPIDRegulator;
    case gpcregulator:
        return new CGPC;
	default:
		return nullptr; /// invalid type
	}
}

bool SObjectFactory::IsARegulator(CSimNode* node)
{
    return IsARegulator(node->GetType());
}

bool SObjectFactory::IsARegulator(const ObjType& type)
{
    switch(type)
    {
    case regulator:
    case pregulator:
    case pidregulator:
    case gpcregulator:
        return true;
    default:
        return false;
    }
}

SObjectFactory::SObjectFactory()
{
}

SObjectFactory::~SObjectFactory()
{
}

std::once_flag SObjectFactory::m_OneCreation;
std::shared_ptr<SObjectFactory> SObjectFactory::m_OneInstance = nullptr;
