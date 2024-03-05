#pragma once

#include <AGL/agl.hpp>

class BaseEntity
{
	public:
		bool			   &exists;
		agl::Vec<float, 2> &position;

		BaseEntity(bool &exists, agl::Vec<float, 2> &position) : exists(exists), position(position)
		{
		}

		virtual ~BaseEntity()
		{
		}
};

class DoNotUse : public BaseEntity
{
	public:
		DoNotUse(bool &exists, agl::Vec<float, 2> &position) : BaseEntity(exists, position)
		{
		}
};

template <typename... T> class Signature
{
};

template <typename... T> class Entity : public DoNotUse, public T...
{
	private:
	public:
		const static Signature<T...> signature;

		Entity(bool &exists, agl::Vec<float, 2> &position) : DoNotUse(exists, position), T(exists, position)...
		{
		}

		virtual ~Entity()
		{
		}
};