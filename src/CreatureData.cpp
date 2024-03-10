#include "../inc/CreatureData.hpp"

CreatureData::CreatureData()
{
	netStr = nullptr;

	return;
}

CreatureData::CreatureData(float sight, int hue, std::vector<SegmentData> &segs, std::vector<in::Connection> &cons, int maxCon)
{
	this->sight = sight;
	this->hue	= hue;

	// default design

	sd = segs;

	std::vector<in::Connection> vec = cons;

	while (vec.size() < maxCon)
	{
		in::Connection c;
		c.id		= -1;
		c.valid		= false;
		c.exists	= false;
		c.weight	= -1;
		c.startNode = -1;
		c.endNode	= -1;
		vec.push_back(c);
	}

	netStr = std::make_unique<in::NetworkStructure>(maxCon, totalSegJoints(sd) * 2 + 2, 0, totalSegJoints(sd), vec);

	return;
}

CreatureData CreatureData::clone(){
  CreatureData result;
		result.netStr = std::make_unique<in::NetworkStructure>(*netStr);

		result.sight      = sight;
		result.hue        = hue;
		result.startEnergy= startEnergy;
		result.preference = preference;
		result.metabolism = metabolism;
		result.useNEAT    =useNEAT;
		result.usePG      =usePG;

		std::vector<SegmentData> sd;
  return result;
}

int CreatureData::totalSegJoints(std::vector<SegmentData> &segs)
{
	int total = 0;

	for (int i = 0; i < segs.size(); i++)
	{
		if (i != 0)
		{
			total++;
		}

		total += segs[i].branch.size() * 2;
	}

	return total;
}
