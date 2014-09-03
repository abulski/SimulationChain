/** \enum ObjType
 * Indicates functionality of the object in the simulation loop.
 */

#ifndef _OBJTYPE
#define _OBJTYPE

enum ObjType 
{
	parallel = 1,
	serial = 2,
	simobject = 3,
	regulator = 4,
    pregulator = 5,
    pidregulator = 6,
    gpcregulator = 7
};

#endif
