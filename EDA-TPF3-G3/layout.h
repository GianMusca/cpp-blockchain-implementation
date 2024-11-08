#pragma once
#include "NodeData.h"
#include <vector>
using namespace std;

struct Connection {
	Connection(const NodeData& ownData) : ownData(ownData), myNeighbours() {}
	Connection(const Connection& c) : ownData(c.ownData), myNeighbours(c.myNeighbours) {}
	Connection& operator=(const Connection& c);

	NodeData ownData;
	vector<NodeData> myNeighbours;

	bool operator==(const NodeData& node) {
		if (ownData == node)
			return true;
		else
			return false;
	}
};

typedef vector<Connection> Layout;