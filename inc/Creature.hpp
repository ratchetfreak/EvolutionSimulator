#pragma once

#include "../lib/AGL/agl.hpp"
#include "Food.hpp"
#include "NeuralNetwork.hpp"

#define RAY_TOTAL  10
#define RAY_LENGTH 500

#define TOTAL_INPUT	 (5 + (RAY_TOTAL * 2))
#define TOTAL_HIDDEN 6
#define TOTAL_NODES	 (TOTAL_INPUT + TOTAL_HIDDEN)

#define TOTAL_CONNECTIONS 1

class Creature
{
	private:
		agl::Vec<float, 2> position = {0, 0};
		agl::Vec<float, 2> velocity = {0, 0};
		float			   rotation = 0;

		float radius;

		agl::Vec<float, 2> worldSize = {0, 0};

		NeuralNetwork *network;

		bool eating	   = false;
		bool layingEgg = false;

	public:
		float closest;
		float closestAngle;

		Creature();

		void setPosition(agl::Vec<float, 2> position);
		void setWorldSize(agl::Vec<float, 2> worldSize);

		void updateNetwork(Food *food, int totalFood);
		void updateActions(Food *food);

		NeuralNetwork	   getNeuralNetwork();
		agl::Vec<float, 2> getPosition();
		float			   getRotation();
		bool			   getEating();
		bool			   getLayingEgg();
};
