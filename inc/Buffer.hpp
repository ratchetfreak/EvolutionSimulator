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
			for (int i = 0; i < size; i++)
			{
				for (int x = 0; x < 8; x++)
				{
					int mutation = (int)floorf(((float)rand() / (float)RAND_MAX) * (float)chance);
					data[i]		 = data[i] ^ ((mutation == 0) << x);
				}
			}

			return;
		}
};
