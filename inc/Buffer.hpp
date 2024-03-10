#pragma once

#include <cstdlib>
#include <vector>

class Buffer
{
	public:
		std::vector<unsigned char >data;
		int			   size;

		Buffer(int size);
		Buffer();


		void printBits();
		void mutate(int chance)
		{

			return;
		}
};
