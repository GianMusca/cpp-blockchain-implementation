#include "layout.h"

Connection& Connection::operator=(const Connection& c)
{
	this->ownData = c.ownData;
	this->myNeighbours = c.myNeighbours;
	return *this;
}
