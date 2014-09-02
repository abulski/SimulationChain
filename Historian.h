/** \class  CHistorian
 * Handles storing and dispensing given amount of samples.
 *
 * \par
 * Using double-ended queue internally to ensure high performance when adding and retrieving samples.
*/

#include <deque>
#include <memory>
#include <vector>

#ifndef _CHISTORIAN
#define _CHISTORIAN

#ifdef _DEBUG
#include <iostream>
#endif

class CHistorian
{
public:
    /// \brief Constructs historian object.
    /// \param[in] nMaxSamples Maximum samples to store.
    CHistorian(int nMaxSamples = 10) : m_nMaxSamples(nMaxSamples), m_nSamplesStored(0){}

    /// \brief Sets maximum samples stored.
    /// \param[in] nMaxSamples Maximum samples to store.
    void SetMaxSamples(unsigned int nMaxSamples);

    /// \brief Returns number indicating maximum samples stored
    /// \return Number indication maximum samples stored.
	unsigned int GetMaxSamples() const
	{
		return m_nMaxSamples;
	}

    /// \brief Returns number indicating how many samples were stored were added to
	/// the object from the beginning of its existance 
	unsigned int GetNumOfSamplesStored()
	{
		return m_nSamplesStored;
	}

    /// \brief Insert a sample to the history record
    /// \param[in] dSample New sample value.
	void AddSample(double dSample);

    /// \brief Return N samples stored. If The number of stored samples is lower than desired
	/// the returned vector will have last nN - numOfSamplesStored set to 0.
    /// \return Unique pointer to a copy of stored samples.
	std::unique_ptr<std::vector<double> > RetriveNSamples(int nN = 0) const;

    /// \brief Return N samples stored. If The number of stored samples is lower than desired
    /// the returned vector will have last nN - numOfSamplesStored set to 0.
    /// \param[out] v Vector of stored samples.
    /// \param[in] nN Number of samples to retrieve. nN = 0 gives all samples.
    void RetriveNSamples(std::vector<double>& v, int nN = 0) const;

    /// \brief Clears stored samples and copies values from vector
    /// \param[in] v Sets new history of stored values.
    void SetHistory(std::vector<double>& v);

    /// \brief Clears stored samples
    void Clear()
    {
        deqSamples.clear();
        m_nSamplesStored = 0;
    }

    /// \brief Returns last stored sample
    double LastSample()
    {
        return *(deqSamples.end());
    }

	~CHistorian();

private:
    /// Maximum stored samples.
	unsigned int m_nMaxSamples;
    /// Holds information about number of samples currently stored.
    unsigned int m_nSamplesStored;
    /// Stores the actual samples.
	std::deque<double> deqSamples;
};

#endif
