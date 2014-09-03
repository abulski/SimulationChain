#include "Historian.h"

void CHistorian::SetHistory(std::vector<double>& v)
{
    m_nMaxSamples = v.size();
    m_nSamplesStored = v.size();

    deqSamples.clear();
    for(int i=m_nSamplesStored-1; i>=0; --i)
        AddSample(v[i]);
}

void CHistorian::SetMaxSamples(unsigned int nMaxSamples)
{
    // Rejecting the samples which exceed the limit
    if (deqSamples.size() > nMaxSamples)
    {
        int nRejected = (int(deqSamples.size()) - nMaxSamples);
        for (int i = 0; i < nRejected; ++i)
            deqSamples.pop_front();
    }

    m_nMaxSamples = nMaxSamples;
}

void CHistorian::AddSample(double dSample)
{
    // push newest sample into the back of the queue
	deqSamples.push_back(dSample);
    // if the amonth of stored data exceeds size of the queue, delete a front sample
	if (deqSamples.size() > m_nMaxSamples)
		deqSamples.pop_front();

    // increment number of samples stored
	++m_nSamplesStored;
}

std::unique_ptr<std::vector<double> > CHistorian::RetriveNSamples(int nN) const
{
    //default parameter is 0 - fetch all the stored data
	if (nN == 0)
		nN = GetMaxSamples();

	std::unique_ptr<std::vector<double> > v(new std::vector<double>(nN, 0));

    // determine number of samples to retrive from deque
	int nRet = (nN > deqSamples.size()) ? deqSamples.size() : nN;

    // copy elements from deque to vector
	auto it = deqSamples.rbegin();
	for (int i = 0; i < nRet; ++i)
		(*v)[i] = *(it++);

	return v;
}

void CHistorian::RetriveNSamples(std::vector<double>& v, int nN) const
{
    //default parameter is 0 - fetch all the stored data
    if (nN == 0)
        nN = GetMaxSamples();

    v.resize(nN,0);

    // determine number of samples to retrive from deque
    int nRet = (nN > deqSamples.size()) ? deqSamples.size() : nN;

    // copy elements from deque to vector
    auto it = deqSamples.rbegin();
    for (int i = 0; i < nRet; ++i)
        v[i] = *(it++);
}

CHistorian::~CHistorian()
{
}
