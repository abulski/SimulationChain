#include "SimObject.h"


std::ostream& operator<<(std::ostream& out, const std::vector<double>& v)
{
	auto it = v.begin();
	for (; it != v.end(); ++it)
		out << (*it) << " ";
	return out;
}

std::string v2str(const std::vector<double>& v)
{
	std::string s;
	auto it = v.begin();
	for (; it != v.end(); ++it)
		s += std::to_string(*it) + " ";
	return s;
}

std::vector<double>& str2v(const std::string& s, std::vector<double>& out_v)
{
    // transforming string into a vector of doubles
	int nPos = s.find_first_of(' ');
	int nPosBack = 0;
	while (nPos <= s.length())
    {
		out_v.push_back(std::stod(s.substr(nPosBack, nPos)));
		nPosBack = nPos + 1;
        nPos = s.find_first_of(' ', nPosBack);
	}

	return out_v;
}

CSimObject::CSimObject(int nID, ObjType Type, std::string sName) : CSimNode(nID, Type, sName), m_nK(0)
{
}

double CSimObject::Simulate(double dInSample)
{
    double out_result = 0;

    // store input value for use of function caller
    if(!m_dInVal.expired())
        *m_dInVal.lock() = dInSample;

    // If the object has children and there is a "parallel" or "serial" flag set, run the simulation
    // exclusively on children
	if (m_lChildren.size())
	{
		auto it = m_lChildren.begin();
		if (m_Type == parallel)
		{
            // run in parallel - redirect input to each object
            // and sum the outputs
			for (; it != m_lChildren.end(); ++it)
				out_result += (*it)->Simulate(dInSample);
		}
		else
		{
            // else run as serial
			for (; it != m_lChildren.end(); ++it)
				dInSample = (*it)->Simulate(dInSample);
			out_result = dInSample;
		}
	}
	else
    // If the object is a leaf of the tree and has m_vA and m_vB run the simulation
	if (m_vA.size() != 0 && m_vB.size() != 0)
    {

        // Storing new input sample
        m_InputHistory.AddSample(dInSample);

        //a * y(i)
        // get the stored output points
        std::unique_ptr<std::vector<double> > yi = m_OutputHistory.RetriveNSamples(m_vA.size());
        // multiply A with vector of stored output samples
        double nMultAYi = std::inner_product(m_vA.begin(), m_vA.end(), yi->begin(), 0.0);
        //nMultAYi = (*yi)[1]*m_vA[0];


#ifdef _DEBUG
		//std::cout << "\n" << "y1: " << (*yi)[0] << std::endl;
		//std::cout         << "y2: " << (*yi)[1] << std::endl;
		//std::cout << "\n" << "ay: " << nMultAYi << std::endl;
#endif

        //z^-k * b * u(i)
        // get the stored output points
        std::unique_ptr<std::vector<double> > ui = m_InputHistory.RetriveNSamples(m_vB.size() + m_nK);
        // multiply B with vector of stored input samples
        double nMultBUi = std::inner_product(m_vB.begin(), m_vB.end(), (ui->begin() + m_nK), 0.0);

#ifdef _DEBUG
		//std::cout << "bu: " << nMultBUi << std::endl;
#endif
        //Noise
		double e = 0.0 * ((double) rand() / (double) (0.5*RAND_MAX) - 1); ///from -1 to 1

        //calculate output
        out_result = nMultBUi - nMultAYi + e;

        //store the results for future use
		m_OutputHistory.AddSample(out_result);
	}
	else
        // if we get there there is clearly something wrong - the object doesn't seem to be initiated
		throw std::string("Name: ") + m_sName + std::string(", ID: ") + std::to_string(m_nID) + std::string(". Object missing simulation purpose. Probably not properly initiated.");

    // store output value for use of function caller
    if(!m_dOutVal.expired())
        *m_dOutVal.lock() = out_result;

    //if the additional output stream is available write the data into it
	if (m_oStream != nullptr)
		*(m_oStream) << out_result << ' ';
	return out_result;
}

void CSimObject::SaveState(boost::property_tree::ptree& pt) const
{
    // storing data into an XML file
	using boost::property_tree::ptree;

	ptree & node = pt.add("Object.Name", "");
	node.put("ID", m_nID);
	node.put("Type", m_Type);
	node.put("Stationary", m_bStationary);
	node.put("K", m_nK);
	node.put("VectorA", v2str(m_vA));
	node.put("VectorB", v2str(m_vB));

    // if doesnt have a parent insert 0
	if (m_Parent != nullptr)
		node.put("Parent.name", m_Parent->GetName());
	else
		node.put("Parent.name", 0);

	node.put("<xmlattr>.Name", m_sName);

    // ask children to write their state data
	auto it = m_lChildren.begin();
	for (; it != m_lChildren.end(); ++it)
		(*it)->SaveState(pt);

}

void CSimObject::LoadState(boost::property_tree::ptree::value_type const& v)
{
    // directives to make boost functionality more readable
	using boost::property_tree::read_xml;
	using boost::property_tree::ptree;
	using boost::property_tree::xml_parser_error;
	using boost::property_tree::ptree_bad_data;

    // Setting object data
	std::vector<double> vA,
		vB;
	SetStationary(v.second.get<bool>("Stationary"));
	SetK(v.second.get<int>("K"));
	str2v(v.second.get<std::string>("VectorA"), vA);
	str2v(v.second.get<std::string>("VectorB"), vB);
	SetVectorA(std::move(vA));
	SetVectorB(std::move(vB));
}

void CSimObject::SetVectorA(std::vector<double>&& vA)
{
    m_vA = std::move(vA);
    // determine the amount of samples that needed to properly calculate output
    // minimal size is size of the vector.
    int nMinAKSize = m_vA.size();
    int nMinSampleSize = (nMinAKSize > m_OutputHistory.GetMaxSamples()) ? nMinAKSize : m_OutputHistory.GetMaxSamples();
    m_OutputHistory.SetMaxSamples(nMinSampleSize);
}

void CSimObject::SetVectorB(std::vector<double>&& vB)
{
    m_vB = std::move(vB);
    // determine the amount of samples that needed to properly calculate output
    int nMinBKSize = m_vB.size() + m_nK;
    int nMinSampleSize = (nMinBKSize > m_InputHistory.GetMaxSamples()) ? nMinBKSize : m_InputHistory.GetMaxSamples();
    m_InputHistory.SetMaxSamples(nMinSampleSize);
}

CSimObject::~CSimObject()
{

#ifdef _DEBUG
    // DEBUG
	std::cout << "Destroyed object: " << m_sName << ", ID: " << m_nID << std::endl;
#endif
	SUniqueNameController::GetInstance().UnRegisterName(m_sName);
}
