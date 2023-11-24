#pragma once

#include <AGL/agl.hpp>
#include <cstdlib>
#include "macro.hpp"
#include "Environment.hpp"
#include "PhysicsObj.hpp"

class Food : public Entity<PhysicsObj>
{
	public:
		Food()
		{
			mass	 = 1;
			radius = 5;
		}

		int	  id;
		float energy;

#ifdef ACTIVEFOOD
		agl::Vec<float, 2> nextPos;

		void nextRandPos(agl::Vec<float, 2> worldSize)
		{
			constexpr float range = ACTIVEFOOD;

			float xOffset = rand() / (float)RAND_MAX;
			xOffset -= .5;
			xOffset *= 2 * range;

			float yOffset = rand() / (float)RAND_MAX;
			yOffset -= .5;
			yOffset *= 2 * range;

			nextPos.x = std::max((float)0, std::min(worldSize.x, position.x + xOffset));
			nextPos.y = std::max((float)0, std::min(worldSize.y, position.y + yOffset));
		}
#endif
};
