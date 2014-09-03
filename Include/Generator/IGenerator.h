/** \interface IGenerator
* Intefrace to be inherited by generator subclasses.
*
* \par
* To be used by generators in particular.
*
* \par
* Main features:
* - generating samples,
* - saving and loading state - serialization,
* - saving and loading history, so generators can be used for prediction.
*/

#ifndef _IGENERATOR
#define _IGENERATOR

#include "GenType.h"
#include "boost\property_tree\ptree.hpp"

class IGenerator
{
public:

    /// \brief Used to generate next signal sample.
    /// \return Next generated sample of the output.
	virtual double GenerateNext() = 0;

    /// \brief Resets the generator to the enter state.
	virtual void Reset() = 0;

    /// \brief Sets the delay in samples.
	virtual void SetDelay(int) = 0;

    /// \brief Sets generator parameters.
	virtual void LoadState(boost::property_tree::ptree::value_type const&) = 0;

    /// \brief Saves generator to given tree.
    virtual void SaveState(boost::property_tree::ptree&) const = 0;

    /// \brief Gets type of the generator.
	virtual GenType GetType() const = 0;

    /// \brief Gets name.
    virtual const std::string& GetName() const = 0;

    /// \brief Sets name.
    virtual void SetName(std::string& sName) = 0;

    /// \brief Internally saves a current state in relation with the output signal generation.
    virtual void SaveHistory() = 0;

    /// \brief Restores an internal state saved with SaveHistory() method.
    virtual void LoadHistory() = 0;

    virtual ~IGenerator() {}
};

#endif
