#include "StdAfx.h"
#include "Singleton.h"

namespace Eruption { namespace Utility {

	// static member definition
	map<size_t, Singleton*> Singleton::singletons;

} }