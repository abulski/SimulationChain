#include "SimNode.h"
#include "SObjectFactory.h"

CSimNode::CSimNode(int nID, ObjType Type, std::string const &sName) : m_Parent(nullptr),
    //m_InWindow(nullptr),
    //m_OutWindow(nullptr),
    //m_FunOut(nullptr),
    //m_FunIn(nullptr),
    m_oStream(static_cast<std::ostream*>(nullptr))
{
    std::string sName2 = sName;
	try
	{
        SetName(sName2);
		SetType(Type);
		SetID(nID);
	}
	catch (std::string e)
	{
        // TODO exception handling
	}

#ifdef _DEBUG
	/// DEBUG
	std::cout << "Created object: " << m_sName << ", ID: " << m_nID << std::endl;
#endif
}

void CSimNode::SetID(int nID)
{
    // using generated value
	if (nID == 0)
		m_nID = SUniqueIDGenerator::GetInstance().GetNextID();
	else
		/// trying to reserve ID
		m_nID = SUniqueIDGenerator::GetInstance().ReserveID(nID);

    // if m_nID equals 0 throw an exception
	if (m_nID == 0)
		throw std::string("Bad ID reservation or generation.");
}

void CSimNode::SetName(std::string& sName)
{
    // first unregister the current name
	SUniqueNameController::GetInstance().UnRegisterName(m_sName);
    // then register new one
	m_sName = SUniqueNameController::GetInstance().RegisterName(sName);
	sName = m_sName;
}

void CSimNode::GetName(boost::property_tree::ptree& nameTree) const
{
    using boost::property_tree::ptree;

    ptree& node = nameTree.add("Object", "");

    /// ask children to write their state data
    auto it = m_lChildren.begin();
    for (; it != m_lChildren.end(); ++it)
        (*it)->GetName(node);

    node.put("<xmlattr>.Name", m_sName);
}

void CSimNode::SetParent(ISISO* Parent)
{
    if (Parent == this || Parent == m_Parent)
        return;
    ISISO* temp = m_Parent;
    m_Parent = Parent;
    Parent->AddChild(std::shared_ptr<ISISO>(this));

    /// inform old parent about the child loss
    if (temp != nullptr && temp != Parent)
        temp->RemoveChild(std::shared_ptr<ISISO>(this));
}

void CSimNode::AddChild(std::shared_ptr<ISISO> Child)
{
    /// prevent setting an object its own child
    if (Child.get() == this)
        return;

    // search whether Child is not in list yet
    auto it = m_lChildren.begin();
    for (; it != m_lChildren.end(); ++it)
    if (*it == Child)
        return;

    // inform the Child about its new parent
    m_lChildren.push_back(Child);
    Child->SetParent(this);
}

void CSimNode::GetChildren(std::list<std::weak_ptr<ISISO> >& lChildren) const
{
	auto it = m_lChildren.begin();
	for (; it != m_lChildren.end(); ++it)
		lChildren.push_back(*it);
}

bool CSimNode::RemoveObject(int nID)
{
    // check if the searched object is a children
	auto it = m_lChildren.begin();
	for (; it != m_lChildren.end(); ++it)
	{
		if ((*it)->GetID() == nID)
		{
			(*it).reset();
			return true;
		}
	}

    // command children to search for the object
	it = m_lChildren.begin();
	for (; it != m_lChildren.end(); ++it)
	{
		if ((*it)->RemoveObject(nID))
			return true;
	}

	return false;
}

ISISO* CSimNode::SearchObject(const std::string& s)
{
    ISISO* obj = nullptr;
    if (m_sName == s)
        return this;

    // search all the children
    auto it = m_lChildren.begin();
    for (; it != m_lChildren.end(); ++it)
    if (obj = (*it)->SearchObject(s))
        return obj;

    return obj;
}

ISISO* CSimNode::FindFirstRegulator()
{
    ISISO* obj = nullptr;

    // make the factory check whether its an instance of a regulator
    if(SObjectFactory::GetInstance().IsARegulator(this))
        return this;

    // search all the children
    auto it = m_lChildren.begin();
    for (; it != m_lChildren.end(); ++it)
    if (obj = (*it)->FindFirstRegulator())
        return obj;

    return obj;
}

bool CSimNode::MoveObjectToFront(ISISO* FrontObject)
{
    auto it = m_lChildren.begin();
    for (; it != m_lChildren.end(); ++it)
    if (it->get() == FrontObject)
    {
        std::shared_ptr<ISISO> temp(*it);
        m_lChildren.erase(it);
        m_lChildren.push_front(temp);
        return true;
    }
    return false;
}

void CSimNode::ResetMemory()
{
    m_OutputHistory.Clear();
    m_InputHistory.Clear();

    // reseting all the children
    auto it = m_lChildren.begin();
    for(; it != m_lChildren.end(); ++it)
    {
        (*it)->ResetMemory();
    }
}

CSimNode::~CSimNode()
{
}
