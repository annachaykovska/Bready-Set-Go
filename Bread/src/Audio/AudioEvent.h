#pragma once

#include <string>

struct AudioEvent
{
	const unsigned int max = 8;

	std::string type;
	unsigned int args;
	//EventArg eArgs[max];
};