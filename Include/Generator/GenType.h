/*! \enum GenType
* Indicates functionality of the object in the simulation loop.
*/

#ifndef _GENTYPE
#define _GENTYPE

enum GenType
{
	noise = 1,
	sine = 2,
	step = 3,
	pulse = 4,
	square = 5,
	triangle = 6
};

#endif
