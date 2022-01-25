#pragma once
#include <string>

class Entity;

class Component 
{
public:

	std::string name;
	Entity* owner;

};