#include "EDAcoinsNet.h"

EDAcoinsNet::EDAcoinsNet() :
	FULLdata(), SPVdata(), MINERdata(), FULLvector(), SPVvector(), MINERvector()
{
}

EDAcoinsNet::~EDAcoinsNet()
{
	for (size_t i = 0; i < FULLvector.size(); i++)
		delete FULLvector[i];

	for (size_t i = 0; i < SPVvector.size(); i++)
		delete SPVvector[i];

	for (size_t i = 0; i < MINERvector.size(); i++)
		delete SPVvector[i];
}
void EDAcoinsNet::cycle()
{
	for (size_t i = 0; i < FULLvector.size(); i++)
		FULLvector[i]->cycle();
	for (size_t i = 0; i < SPVvector.size(); i++)
		SPVvector[i]->cycle();
}
errorType EDAcoinsNet::createFULLNode(Socket _socket)
{
	errorType creationState;
	creationState.error = false;
	creationState.datos = "";

	if (existAlready(_socket) == false) {
		FULLNode* node = new FULLNode(_socket);
		FULLvector.emplace_back(node);
		FULLdata.emplace_back(node->getData());
	}
	else {
		creationState.error = true;
		creationState.datos = "Failed to create FULL Node:\nSpecified socket or ID is already used";
	}
	notifyAllObservers(this);
	return creationState;
}
errorType EDAcoinsNet::createSPVNode(Socket _socket, NodeData FilterNode, NodeData HeaderNode)
{
	errorType creationState;
	creationState.error = false;
	creationState.datos = "";

	if (existAlready(_socket) == false) {
		SPVNode* node = new SPVNode(_socket, FilterNode, HeaderNode);
		SPVvector.emplace_back(node);
		SPVdata.emplace_back(node->getData());
	}
	else {
		creationState.error = true;
		creationState.datos = "Failed to create SPV Node:\nSpecified socket or ID is already used";
	}
	notifyAllObservers(this);
	return creationState;
}
errorType EDAcoinsNet::createMINERNode(Socket _socket)
{
	errorType creationState;
	creationState.error = false;
	creationState.datos = "";

	if (existAlready(_socket) == false) {
		MINERNode* node = new MINERNode(_socket);
		MINERvector.emplace_back(node);
		MINERdata.emplace_back(node->getData());
	}
	else {
		creationState.error = true;
		creationState.datos = "Failed to create MINER Node:\nSpecified socket or ID is already used";
	}
	notifyAllObservers(this);
	return creationState;
}
const vector<NodeData>& EDAcoinsNet::getKnownFULLdata()
{
	return FULLdata;
}

const vector<NodeData>& EDAcoinsNet::getKnownSPVdata()
{
	return SPVdata;
}

size_t EDAcoinsNet::getFULLamount()
{
	return FULLvector.size();
}

size_t EDAcoinsNet::getSPVamount()
{
	return SPVvector.size();
}

size_t EDAcoinsNet::getMINERamount()
{
	return MINERvector.size();
}

FULLNode* EDAcoinsNet::getFULLnode(size_t pos)
{
	FULLNode* node = nullptr;
	if (pos < FULLvector.size())
		node = FULLvector[pos];
	return node;
}

SPVNode* EDAcoinsNet::getSPVnode(size_t pos)
{
	SPVNode* node = nullptr;
	if (pos < SPVvector.size())
		node = SPVvector[pos];
	return node;
}

MINERNode* EDAcoinsNet::getMINERnode(size_t pos)
{
	MINERNode* node = nullptr;
	if (pos < MINERvector.size())
		node = MINERvector[pos];
	return node;
}

bool EDAcoinsNet::checkIfConnectionMade()
{
	bool rta = false;
	for (size_t i = 0;
		(rta == false) && (i < FULLvector.size());
		i++) {

		if (FULLvector[i]->getState() == NETWORK_CREATED) {
			rta = true;
		}
	}
	return rta;
}

void EDAcoinsNet::insertKnownFULLdata(NodeData data)
{
	if (existAlready(data) == false) {
		FULLdata.emplace_back(data);
	}
}

bool EDAcoinsNet::existAlready(NodeData node)
{
	if ((existAlready(node.getID()) == false)
		&& (existAlready(node.getSocket()) == false))
		return false;
	else
		return true;
}

bool EDAcoinsNet::existAlready(string ID)
{
	bool existence = false;

	for (size_t i = 0;
		(existence == false) && (i < FULLdata.size());
		i++) {

		if (FULLdata[i].getID() == ID)
			existence = true;
	}

	for (size_t i = 0;
		(existence == false) && (i < SPVdata.size());
		i++) {

		if (SPVdata[i].getID() == ID)
			existence = true;
	}

	for (size_t i = 0;
		(existence == false) && (i < MINERdata.size());
		i++) {

		if (MINERdata[i].getID() == ID)
			existence = true;
	}

	return existence;
}

bool EDAcoinsNet::existAlready(Socket socket)
{
	bool existence = false;

	for (size_t i = 0;
		(existence == false) && (i < FULLdata.size());
		i++) {

		if (FULLdata[i].getSocket() == socket)
			existence = true;
	}

	for (size_t i = 0;
		(existence == false) && (i < SPVdata.size());
		i++) {

		if (SPVdata[i].getSocket() == socket)
			existence = true;
	}

	for (size_t i = 0;
		(existence == false) && (i < MINERdata.size());
		i++) {

		if (MINERdata[i].getSocket() == socket)
			existence = true;
	}

	return existence;
}