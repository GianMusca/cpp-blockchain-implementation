#pragma once
#include "subject.h"

#include "FULLNode.h"
#include "SPVNode.h"
#include "MINERNode.h"

#include "NodeData.h"
#include "errorType.h"

#include <vector>
#include <string>
using namespace std;

class EDAcoinsNet :
	public Subject
{
public:
	EDAcoinsNet();
	~EDAcoinsNet();

	//ALEX.EXE
	void cycle();

	//Accedidos por NETcontroller
	errorType createFULLNode(Socket _socket);
	errorType createSPVNode(Socket _socket, NodeData FilterNode, NodeData HeaderNode);
	errorType createMINERNode(Socket _socket);

	//Accedidos por NETviewer
	const vector<NodeData>& getKnownFULLdata();
	const vector<NodeData>& getKnownSPVdata();

	size_t getFULLamount(); //de ESTA maquina
	size_t getSPVamount();
	size_t getMINERamount();
	FULLNode* getFULLnode(size_t pos);
	SPVNode* getSPVnode(size_t pos);
	MINERNode* getMINERnode(size_t pos);

	bool checkIfConnectionMade();

	void insertKnownFULLdata(NodeData data);

private:
	vector<NodeData> FULLdata; //listas de nodos full conocidos (del MANIFIESTO -todas- y las siguientes creadas -EN ESTA MAQUINA-)
	//empieza con los datos de los FULL del genesis, y se incrementa cada vez q se cre exitosamente un FULL (en ESTA maquina)
	vector<NodeData> SPVdata;
	//empieza en 0 y se incrementa cada vez q se crea exitosamente un SPV (en ESTA maquina)
	vector<NodeData> MINERdata;

	vector<FULLNode*> FULLvector;//presentes en ESTA maquina
	vector<SPVNode*> SPVvector;
	vector<MINERNode*> MINERvector;

	bool existAlready(NodeData node);
	bool existAlready(string ID);
	bool existAlready(Socket socket);
};
