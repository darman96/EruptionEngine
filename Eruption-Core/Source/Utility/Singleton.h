#pragma once

/*

Singleton base class
with collection of all singletons 

all singletons need to be friends with the base class!!!

Access Singletons via the template Get function
Singleton::Get<SingletonClass>();

*/

#include <typeinfo>

namespace Eruption { namespace Utility {

	class Singleton
	{

	public:

	private:

		static map<size_t, Singleton*> singletons;

	public:

		template<typename T>
		static T* Get()
		{
			size_t typeHash = typeid(T).hash_code();

			if (singletons[typeHash] == nullptr)
			{
				singletons[typeHash] = (T*)new T();
			}

			return (T*)singletons[typeHash];
		}

	private:

	};

} }

