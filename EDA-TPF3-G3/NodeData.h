#pragma once

#define NO_NODE_DATA NodeData("NO DATA",0,0,0,0,0)

#include "Socket.h"
class NodeData
{
public:
	NodeData(string id, uint port = 0, ip_t ip = { 0,0,0,0 });
	NodeData(string id, uint port, uint ip1, uint ip2, uint ip3, uint ip4);
	NodeData(string id, Socket& c);
	NodeData(const NodeData& c);

	void setID(const string& s);
	void setSocket(const Socket& s);

	string getID() const;
	Socket getSocket() const;

	NodeData& operator=(const NodeData& data);

	bool operator==(const NodeData& n) {
		if ((id == n.id) && (socket == n.socket))
			return true;
		else
			return false;
	}

private:
	string id;
	Socket socket;
};