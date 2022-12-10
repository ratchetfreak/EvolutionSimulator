#include "../inc/Creature.hpp"

Buffer::Buffer(int size)
{
	this->size = size;
	data = new unsigned char[this->size];

	return;
}

Buffer::~Buffer()
{
	delete[] data;

	return;
}

CreatureData::CreatureData(float sight, float speed, float tough, int totalConnections)
{
	this->sight			   = sight;
	this->speed			   = speed;
	this->tough			   = tough;
	this->totalConnections = totalConnections;

	connection = new Connection[totalConnections];

	return;
}

CreatureData::~CreatureData()
{
	delete[] connection;

	return;
}

void CreatureData::setConnection(int index, int startNode, int endNode, float weight)
{
	connection[index].startNode = startNode;
	connection[index].endNode	= endNode;
	connection[index].weight	= weight;

	return;
}

int CreatureData::getTotalConnections()
{
	return totalConnections;
}

Connection *CreatureData::getConnection()
{
	return connection;
}

float vectorAngle(agl::Vec<float, 2> vec)
{
	float angle = atan(vec.x / vec.y);

	if (vec.y < 0)
	{
		angle *= -1;

		if (vec.x > 0)
		{
			angle = PI - angle;
		}
		else
		{
			angle = -(PI + angle);
		}
	}

	return angle;
}

float loop(float min, float max, float value)
{
	return value - (max + abs(min)) * int(value / max);
}

Creature::Creature()
{
	return;
}

void Creature::setup(CreatureData *creatureData)
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

	network = new NeuralNetwork(TOTAL_NODES, 5 + (RAY_TOTAL * 2), creatureData->getConnection(),
								creatureData->getTotalConnections());
}

void Creature::clear()
{
	delete network;

	position = {0, 0};
	velocity = {0, 0};
	rotation = 0;

	return;
}

Creature::~Creature()
{
	this->clear();
}

void Creature::setPosition(agl::Vec<float, 2> position)
{
	this->position = position;

	return;
}

void Creature::setVelocity(agl::Vec<float, 2> velocity)
{
	this->velocity = velocity;

	return;
}

void Creature::setRotation(float rotation)
{
	this->rotation = rotation;

	return;
}

void Creature::setHealth(float health)
{
	this->health = health;

	return;
}

bool isVisible()
{
	return false;
}

float closerObject(agl::Vec<float, 2> offset, float nearestDistance)
{
	return nearestDistance;
}

void Creature::updateNetwork(Food *food, int totalFood, List<Creature *> *existingCreature,
							 agl::Vec<float, 2> worldSize)
{
	for (int x = 0; x < RAY_TOTAL; x++)
	{
		float nearestDistance = RAY_LENGTH;
		int	  type			  = 0;

		for (int i = 0; i < totalFood; i++)
		{
			if (!food[i].exists)
			{
				continue;
			}

			agl::Vec<float, 2> offset	= position - food[i].position;
			float			   distance = offset.length();

			if (distance > nearestDistance)
			{
				continue;
			}

			float foodRotation	= vectorAngle(offset);
			float creatureAngle = rotation;
			float rayAngle		= (((float)x / (RAY_TOTAL - 1)) * PI) - (PI / 2);

			rayAngle -= creatureAngle;

			float angleDifference	 = loop(-PI, PI, foodRotation - rayAngle);
			float maxAngleDifference = (PI / RAY_TOTAL) / 2;

			if (angleDifference < maxAngleDifference && angleDifference > -maxAngleDifference)
			{
				nearestDistance = distance;
				type			= 1;
			}
		}

		for (int i = 0; i < existingCreature->getLength(); i++)
		{
			if (existingCreature->get(i) == this)
			{
				continue;
			}

			agl::Vec<float, 2> offset	= position - existingCreature->get(i)->getPosition();
			float			   distance = offset.length();

			if (distance > nearestDistance)
			{
				continue;
			}

			float creatureRotation = vectorAngle(offset);
			float creatureAngle	   = rotation;
			float rayAngle		   = (((float)x / (RAY_TOTAL - 1)) * PI) - (PI / 2);

			rayAngle -= creatureAngle;

			float angleDifference	 = loop(-PI, PI, creatureRotation - rayAngle);
			float maxAngleDifference = (PI / RAY_TOTAL) / 2;

			if (angleDifference < maxAngleDifference && angleDifference > -maxAngleDifference)
			{
				nearestDistance = distance;
				type			= -1;
			}
		}

		network->setInputNode((x + 5), (RAY_LENGTH - nearestDistance) / RAY_LENGTH);
		network->setInputNode((x + 5) + RAY_TOTAL, type);
	}

	network->setInputNode(0, 1);

	network->update();

	return;
}

void Creature::updateActions(Food *food)
{
	velocity = {0, 0};

	float speed = 0;

	if (network->getNode(FOWARD_OUTPUT).value > 0)
	{
		speed += network->getNode(FOWARD_OUTPUT).value * 2.5;
	}

	if (network->getNode(BACKWARD_OUTPUT).value > 0)
	{
		speed -= network->getNode(BACKWARD_OUTPUT).value * 2.5;
	}

	if (network->getNode(RIGHT_OUTPUT).value > 0)
	{
		rotation += 0.05 * network->getNode(RIGHT_OUTPUT).value;
	}

	if (network->getNode(LEFT_OUTPUT).value > 0)
	{
		rotation -= 0.05 * network->getNode(LEFT_OUTPUT).value;
	}

	if (network->getNode(EAT_OUTPUT).value > 0)
	{
		eating = true;
	}
	else
	{
		eating = false;
	}

	if (network->getNode(LAYEGG_OUTPUT).value > 0)
	{
		layingEgg = true;
	}
	else
	{
		layingEgg = false;
	}

	rotation = loop(-PI, PI, rotation);

	energy -= speed;

	velocity.x = cos(rotation - (PI / 2)) * speed;
	velocity.y = sin(rotation - (PI / 2)) * speed;

	position.x += velocity.x;
	position.y += velocity.y;

	return;
}

CreatureData* Creature::saveData()
{
	CreatureData *creatureData = new CreatureData(sight, speed, tough, network->getTotalConnections());

	for (int i = 0; i < TOTAL_CONNECTIONS; i++)
	{
		creatureData->setConnection(i, network->getConnection(i).startNode, network->getConnection(i).endNode,
									network->getConnection(i).weight);
	}

	return creatureData;
}

NeuralNetwork Creature::getNeuralNetwork()
{
	return *network;
}

agl::Vec<float, 2> Creature::getPosition()
{
	return position;
}

agl::Vec<float, 2> Creature::getVelocity()
{
	return velocity;
}

float Creature::getRotation()
{
	return rotation;
}

bool Creature::getEating()
{
	return eating;
}

bool Creature::getLayingEgg()
{
	return layingEgg;
}

float Creature::getHealth()
{
	return health;
}
