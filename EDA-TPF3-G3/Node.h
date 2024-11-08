#pragma once

//NOTA: favor de pasar todas las defs. q vean en el .h al .cpp

#include "subject.h"
#include "NodeData.h"

//usar vector o lo q consideren necesario
#include "server.h"
#include "client.h"

//
#include "errorType.h"
#include "typeEnums.h"


class Node :
	public Subject
{
public:
	Node(NodeData data, tipo_de_nodo nodetype = NODO_UNKNOWN) : ownData(data), myMoney(0), nodeType(nodetype) {}
	NodeData getData() { return ownData; } //ID, IP y Puerto
	longN getMyMoney() { return myMoney; }
	tipo_de_nodo getNodeType() { return nodeType; }
	virtual void cycle() = 0;

protected:
	tipo_de_nodo nodeType;
	NodeData ownData;
	longN myMoney;
};