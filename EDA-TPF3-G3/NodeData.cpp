#include "NodeData.h"

NodeData::NodeData(string id, uint port, ip_t ip) :
	id(id), socket(port,ip)
{
}

NodeData::NodeData(string id, uint port, uint ip1, uint ip2, uint ip3, uint ip4):
	id(id), socket(port,ip1,ip2,ip3,ip4)
{
}

NodeData::NodeData(string id, Socket& c) :
	id(id), socket(c)
{
}

NodeData::NodeData(const NodeData& c) :
	id(c.id), socket(c.socket)
{
}

void NodeData::setID(const string& s)
{
	id = s;
}

void NodeData::setSocket(const Socket& s)
{
	socket = s;
}

string NodeData::getID() const
{
	return id;
}

Socket NodeData::getSocket() const
{
	return socket;
}

NodeData& NodeData::operator=(const NodeData& data)
{
	id = data.id;
	socket = data.socket;

	return *this;
}