#include "SGeneratorFactory.h"

IGenerator* SGeneratorFactory::CreateGenerator(GenType NewGenType)
{
    // Generator creation based on selected type
	switch (NewGenType)
	{
	case noise:
		return new CNoiseGen;
	case sine:
		return new CSineGen;
	case step:
		return new CStepGen;
	case pulse:
		return new CPulseGen;
	case square:
		return new CSquareGen;
	case triangle:
		return new CTriangleGen;
	default:
        return nullptr; // invalid type
	}
}


std::once_flag SGeneratorFactory::m_OneCreation;
std::shared_ptr<SGeneratorFactory> SGeneratorFactory::m_OneInstance = nullptr;
