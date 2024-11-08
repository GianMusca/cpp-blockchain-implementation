#pragma once
#include "observer.h"
#include "EDAcoinsNet.h"
#include <string>
using namespace std;

class NETviewer : public Observer
{
public:
	NETviewer(const string& id);

	void cycle(); //OLI
	//imprime en la ventana (con el nombre de myWindowName)
	//los FULL conocidos y los SPV conocidos
	//notese q la idea es q este viewer y el controller del net
	//compartan la misma ventana. De ahi q tienen el mismo nombre

	virtual void update(void*);
private:
	string myWindowName;
	vector<NodeData> listofFULL;
	vector<NodeData> listofSPV;
};

