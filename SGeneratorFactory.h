/** \class SGeneratorFactory
* Responsible for creating generator objects which implement IGenerator interface.
* Implements multi-threading safe singleton pattern.
*/

#ifndef _SGENERATORFACTORY
#define _SGENERATORFACTORY

#include <memory>
#include <mutex>
#include "GenType.h"
#include "NoiseGen.h"
#include "PulseGen.h"
#include "SinGen.h"
#include "StepGen.h"
#include "SquareGen.h"
#include "TriangleGen.h"

class SGeneratorFactory
{
public:	
	/// Allows to access the only instance of the factory.
	static SGeneratorFactory& GetInstance()
	{
        // Call creation function only once
		std::call_once(SGeneratorFactory::m_OneCreation, []()
		{
			SGeneratorFactory::m_OneInstance.reset(new SGeneratorFactory());
		});

		return *(SGeneratorFactory::m_OneInstance);
	}

    /// \brief Creates a generator of the given type.
    /// \param[in] genType Type of the generator to create.
    /// \return Pointer to the newly created generator or nullptr.
	IGenerator* CreateGenerator(GenType);

	~SGeneratorFactory() {}

private:
    // Singleton implementation variables
    static std::once_flag m_OneCreation;
	static std::shared_ptr<SGeneratorFactory> m_OneInstance;

    // Prevent remote usage
	SGeneratorFactory() {}
	SGeneratorFactory(const SGeneratorFactory&) {}
	SGeneratorFactory& operator=(SGeneratorFactory&) {}
};

#endif
