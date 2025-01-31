#include "../inc/Creature.hpp"
#include "../inc/Buffer.hpp"
#include <thread>

#define MASSCALC(size, dens) size.x *size.y *dens

Creature::Creature() : Entity<PhysicsObj>(exists, position)
{
	return;
}

void Creature::setup(CreatureData creatureData, SimulationRules *simulationRules, Environment &env,
					 agl::Vec<float, 2> pos)
{
	// INPUT
	// constant
	// x pos
	// y pos
	// rotation
	// speed
	// Ray[i] distance to object
	// Ray[i] object type (-1 = creature, 0 = nothing, 1 = food)
	//
	// OUTPUT
	// Move foward
	// Move backward
	// Turn right
	// Turn left
	// Eat
	// Lay egg

	this->creatureData = std::move(creatureData);

	// sight = 1;
	// speed = 1;
	// size = 1;

	this->health = 100;
	this->life	 = 60 * 60 * 10;

	this->maxEnergy = 100;
	this->maxHealth = 100;
	this->maxLife	= 60 * 60;

	this->maxBiomass	= 100;
	this->biomass		= 0;
	this->energyDensity = 0.0;

	// this->radius = 12.5 * size;

	this->eggHealthCost = (this->maxHealth / 2);
	this->eggEnergyCost = (this->maxEnergy / 10);
	this->eggTotalCost	= eggHealthCost + eggEnergyCost;
	this->eggDesposit	= 0;

	this->energy = creatureData.startEnergy + 1;

	PhysicsObj *lastSpine = nullptr;

	int totalJoints = 0;

	for (int i = 0; i < creatureData.sd.size(); i++)
	{
		if (i != 0)
		{
			TestObj &en = env.addEntity<TestObj>();

			en.setup(lastSpine->position +
						 agl::Vec<float, 2>{0, (lastSpine->size.y / 2) + (creatureData.sd[i].size.y / 2)},
					 creatureData.sd[i].size, VITEDENS * creatureData.sd[i].size.x * creatureData.sd[i].size.y);

			PhysicsObj::addJoint(en, {0, -en.size.y / 2}, *lastSpine, {0, lastSpine->size.y / 2});
			totalJoints++;

			en.maxMotor = std::min(en.size.x, lastSpine->size.x) * (1 / 4.f);

			segments.emplace_back(&en);
		}
		else
		{
			PhysicsObj::setup(pos, creatureData.sd[0].size,
							  VITEDENS * creatureData.sd[i].size.x * creatureData.sd[i].size.y);
			segments.emplace_back(this);
		}
    (void) totalJoints;

		lastSpine = segments.back();

		PhysicsObj *lastLimb[2] = {nullptr, nullptr};

		lastLimb[0] = lastSpine;
		lastLimb[1] = lastSpine;

		for (int x = 0; x < creatureData.sd[i].branch.size(); x++)
		{
			for (int s = -1; s < 2; s += 2)
			{
				TestObj &en = env.addEntity<TestObj>();

				en.setup(lastLimb[(s + 1) / 2]->position +
							 agl::Vec<float, 2>{
								 (lastLimb[(s + 1) / 2]->size.x / 2) + (creatureData.sd[i].branch[x].size.y / 2), 0} *
								 (float)s,

						 creatureData.sd[i].branch[x].size, MASSCALC(creatureData.sd[i].branch[x].size, VITEDENS));

				en.rotation = (float)PI / 2 * -s;

				PhysicsObj::addJoint(en, {0, -en.size.y / 2}, *lastLimb[(s + 1) / 2],
									 {lastLimb[(s + 1) / 2]->size.x / 2 * s, 0});
				totalJoints++;

				segments.emplace_back(&en);

				en.maxMotor = std::min(en.size.x, lastLimb[(s + 1) / 2]->size.y) * (1 / 4.f);

				lastLimb[(s + 1) / 2] = &en;
			}
		}
	}

	network = std::make_unique<in::NeuralNetwork>(*creatureData.netStr);

	network->setActivation(in::tanh);
	network->learningRate = .1f;
}

void Creature::clear()
{
	network->destroy();
	

	position = {0, 0};
	velocity = {0, 0};
	force	 = {0, 0};
	rotation = 0;
	// radius	  = 0;
	network	  = nullptr;
	eating	  = false;
	layingEgg = false;
	energy	  = 0;
	health	  = 0;

	return;
}

Creature::~Creature()
{
	this->clear();
}

float closerObject(agl::Vec<float, 2> offset, float nearestDistance)
{
	return nearestDistance;
}

void Creature::learnBrain(SimulationRules &simRules)
{
}

void Creature::updateNetwork()
{
	// if (creatureData.usePG)
	// {
	// 	int memSlot = (maxLife - life) % simulationRules->memory;
	//
	// 	if (memSlot == 0)
	// 	{
	// 		for (int i = 0; i < network->structure.totalOutputNodes; i++)
	// 		{
	// 			shift[i] = (((rand() / (float)RAND_MAX) * 2) - 1) * simulationRules->exploration;
	// 		}
	// 	}
	//
	// 	for (int i = 0; i < network->structure.totalInputNodes; i++)
	// 	{
	// 		memory[memSlot].state[i] = network->inputNode[i]->value;
	// 		if (std::isnan(memory[memSlot].state[i]))
	// 		{
	// 			std::cout << life << " nan " << i << '\n';
	// 		}
	// 	}
	//
	// 	if (std::isnan(reward))
	// 	{
	// 		std::cout << "reward" << '\n';
	// 	}
	// 	memory[memSlot].reward = reward;
	// 	reward				   = 0;
	//
	// 	for (int i = 0; i < network->structure.totalOutputNodes; i++)
	// 	{
	// 		memory[memSlot].action[i] = network->outputNode[i].value;
	// 		if (std::isnan(memory[memSlot].action[i]))
	// 		{
	// 			std::cout << life << " nan  action " << i << '\n';
	// 		}
	// 	}
	// }
	//
	// if ((maxLife - life) % simulationRules->memory == 0 && maxLife != life && creatureData.usePG)
	// {
	// 	float loss = 0;
	//
	// 	for (int x = simulationRules->memory - 1; x >= 0; x--)
	// 	{
	// 		for (int y = 1; (x + y) < simulationRules->memory; y++)
	// 		{
	// 			float old = memory[x].reward;
	// 			memory[x].reward += memory[x + y].reward * std::pow(simulationRules->vaporize, y);
	// 		}
	//
	// 		loss += memory[x].reward;
	// 	}
	//
	// 	loss /= simulationRules->memory;
	//
	// 	int oldLoss = loss;
	// 	loss -= baseline;
	//
	// 	baseline = oldLoss;
	//
	// 	std::vector<float> gradients;
	//
	// 	network->setupGradients(&gradients);
	//
	// 	for (int i = 0; i < simulationRules->memory; i++)
	// 	{
	// 		for (int x = 0; x < network->structure.totalInputNodes; x++)
	// 		{
	// 			network->setInputNode(x, memory[i].state[x]);
	// 		}
	//
	// 		network->update();
	//
	// 		std::vector<float> target(network->structure.totalOutputNodes);
	//
	// 		for (int x = 0; x < network->structure.totalOutputNodes; x++)
	// 		{
	// 			target[x] = memory[i].action[x];
	// 		}
	//
	// 		network->calcGradients(&gradients, target);
	// 	}
	//
	// 	network->learningRate = simulationRules->learningRate;
	// 	network->applyGradients(gradients, loss, simulationRules->memory);
	// }

	int node = 2;

	network->setInputNode(0, 1);
	network->setInputNode(1, sinf((maxLife - life) / 20.f));

	for (int i = 0; i < segments.size(); i++)
	{
		if (segments[i]->rootConnect != nullptr)
		{
			network->setInputNode(node, segments[i]->getJointAngle() / (float)(PI / 2));
			node++;
			network->setInputNode(node, segments[i]->motor / (float)(PI / 2));
			node++;
		}
	}

	network->update();

	// for (int i = 0; i < network->structure.totalOutputNodes; i++)
	// {
	// 	network->outputNode[i].value += shift[i];
	// 	network->outputNode[i].value =
	// std::clamp<float>(network->outputNode[i].value, -1, 1);
	// }
}

void Creature::updateActions()
{
	int node = 0;

	for (auto &seg : segments)
	{
		if (seg->rootConnect != nullptr)
		{
			float ang = seg->getJointAngle();
			float net = network->outputNode[node].value * (float)(PI / 2);

			// float net = sin(frame / 20.);
			// std::cout << ang << '\n';

			float diff = ang - net;
			// std::cout << diff << '\n';

			seg->motor = (1 / 6.f * diff) * seg->maxMotor;
			// std::cout << agl::radianToDegree(joint[i].getAngle()) << '\n';

			node++;
		}
	}

	life--;

	return;
}
