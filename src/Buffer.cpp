#include "../inc/Buffer.hpp"

#include <stdio.h>

Buffer::Buffer(int size)
{
	this->size = size;
	data.resize(this->size);

	return;
}

Buffer::Buffer()
{

}


void Buffer::printBits()
{
	for (int x = 0; x < size; x++)
	{
		for (int i = 0; i < 8; i++)
		{
			printf("%d", !!((data[x] << i) & 0x80));
		}

		printf(" ");
	}

	printf("\n");
}
