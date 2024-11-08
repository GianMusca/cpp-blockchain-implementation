#pragma once
#include "observer.h"
#include "Node.h"

#define NOT_DATA	"NO DATA",0,0,0,0,0

class NODEviewer : public Observer {

public:
	NODEviewer();
	virtual void update(void*) = 0;
	
protected:
	void showNodeType();
	void printNodeData();
	NodeData nodedata;
	tipo_de_nodo type;
	unsigned long int money;
	string windowName;

};
