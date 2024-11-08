/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "SPVNode.h"

/*******************************************************************************
* CONSTANT AND MACRO DEFINITIONS USING #DEFINE
******************************************************************************/
#define CRLF "\x0D\x0A"

/*******************************************************************************
	CONSTRUTOR
 ******************************************************************************/
SPVNode::SPVNode(Socket _socket, NodeData FilterNode, NodeData HeaderNode) : Node(NO_NODE_DATA), filterNode(FilterNode), headerNode(HeaderNode),
	cryptohandler(NODO_SPV), utxohandler(NODO_SPV,nullptr,nullptr)
{
	
	ownData.setID(cryptohandler.getMyPublicKey());
	ownData.setSocket(_socket);
	nodeType = NODO_SPV;
	//Create Genesis Server
	Server* genesisServer = new Server(ownData.getSocket().getPort());
	genesisServer->startConnection();			//Preguntar si esto funcaria
	servers.push_back(genesisServer);
}

/*******************************************************************************
	DESTRUCTOR
 ******************************************************************************/
SPVNode::~SPVNode() 
{
	cout << "Destroyed SPV node" << endl;

	if (!servers.empty()) {
		for (int i = 0; i < servers.size(); i++)
			delete servers[i];
		servers.clear();
	}
	if (!clients.empty()) {
		for (int i = 0; i < clients.size(); i++)
			delete clients[i];
		clients.clear();
	}
}

/*******************************************************************************
	CYCLE
 ******************************************************************************/
void SPVNode::cycle() {
	keepSending();
	keepListening();
}
/***********************************************************************************
	METHODS USED BY CONTROLLER
***********************************************************************************/
errorType SPVNode::makeTX(const vector<Vout>& receivers, longN fee)
{
	errorType ret;
	Transaction newTx;
	if (makeSmartTX(fee, receivers, newTx)) {
		ret.error = false;
		ret.datos = "Valid transaction. Have a nice day, beach";
	}
	else {
		ret.error = true;
		ret.datos = "INVALID TRANSACTION DETECTED, NICE TRY BEACH";
	}
	postTransaction(newTx);
	notifyAllObservers(this);
	return ret;
}

errorType SPVNode::changeFilterNode(NodeData FilterNode) { errorType err = { false,"" }; filterNode = FilterNode; notifyAllObservers(this);  return err; }
errorType SPVNode::changeHeaderNode(NodeData HeaderNode) { errorType err = { false,"" }; headerNode = HeaderNode; notifyAllObservers(this); return err; }

/***********************************************************************************
	METHODS USED BY VIEWER
***********************************************************************************/
NodeData SPVNode::getData(){ return ownData; }
NodeData SPVNode::getFilterNodeData() { return filterNode; }
NodeData SPVNode::getHeaderNodeData() { return headerNode; }

errorType SPVNode::getVerificationError()
{
	errorType err;
	if (blockVerification.size() == 0) {
		err.error = false;
		err.datos = "";
	}
	else {
		size_t popping = blockVerification.size() - 1;
		err = blockVerification[popping];
		blockVerification.pop_back();
	}
	return err;
}

/***********************************************************************************
	NETWORKING SH*T
***********************************************************************************/
void SPVNode::keepListening() {
	vector<vector<Server*>::iterator> deleteThis;
	vector<Server*> doneServers;
	servers.back()->listening();

	if ((*(servers.end() - 1))->getDoneListening()) {
		cout << "Latest Server picked up something!" << endl;
		Server* newServer = new Server(ownData.getSocket().getPort());
		newServer->startConnection();
		servers.push_back(newServer);
		cout << "New Server created and pushed!" << endl;
	}
	auto i = servers.begin();
	for (; i != servers.end() - 1; i++) {
		if (!(*i)->getDoneDownloading())
			(*i)->receiveMessage();
		else if (!(*i)->getDoneSending())
			(*i)->sendMessage(serverResponse((*i)->getState(),(*i)->getMessage()));
		if ((*i)->getDoneSending()) {
			cout << "Server done servering" << endl;
			doneServers.push_back(*i);
			deleteThis.push_back(i);
		}
	}
	//Handle finished servers
	auto j = doneServers.begin();
	for (; j != doneServers.end(); j++) {
		switch ((*j)->getState()) {
		case MERKLE:														//SPV NODES ONLY CARE ABOUT RECEIVING MERKLEBLOCKS ON SERVER
			JSONHandler.saveMerkleBlock((*j)->getMessage(), mBlocks);
			//REQUEST CONFIRMATION OF VALIDATION
			getBlockHeader(mBlocks.back().blockId);							//SUPER DUDOSO LO QUE ACABO DE HACER, PERO ONDA, SUPER DUDOSO
			break;
		}
	}
	i = servers.begin();
	for (; i != servers.end() - 1; i++) {
		if ((*i)->getDoneSending())
			delete* i;
	}
	auto k = deleteThis.begin();
	for (; k != deleteThis.end(); k++) {
		servers.erase(*k);
	}
	if (!deleteThis.empty())
		notifyAllObservers(this);
}

void SPVNode::keepSending() {
	if (clients.empty())
		return;
	vector<vector<Client*>::iterator> deleteThis;
	vector<Client*> doneClients;

	auto i = clients.begin();
	for (; i != clients.end(); i++) {
		if ((*i)->getRunning() == 0) {
			cout << "Client did it's job!" << endl;
			doneClients.push_back(*i);
			deleteThis.push_back(i);
		}
		else
			(*i)->sendRequest();
	}
	//Handle finished clients
	auto j = doneClients.begin();
	for (; j != doneClients.end(); j++) {
		if ((*j)->getClientType() == GETClient) {
			//ANALIZE BLOCK HEADER RECEIVED
			//AND COMPARE TO LATEST MERKLE BLOCK RECEIVED IN mBlocks
			verifyMerkleBlock(JSONHandler.saveBlockHeader((*j)->getResponse()));
			//POP BACK LATEST MERKLE BLOCK IN LIST
			mBlocks.pop_back();
		}
		//And delete completed client
		delete* j;
	}
	auto k = deleteThis.begin();
	for (; k != deleteThis.end(); k++) {
		clients.erase(*k);
	}
	if (!deleteThis.empty())
		notifyAllObservers(this);
}
/***********************************************************************************
	SEREVR REPONSE
***********************************************************************************/
string SPVNode::serverResponse(STATE rta, string msg)
{
	string message;

	switch (rta)
	{
	case MERKLE:
		message = createServerOkRsp("/eda_coin/send_merkle_block");
		break;

	case ERR:
		message = createServerErrRsp();
		break;
	}

	return message;

}

string SPVNode::createServerErrRsp()
{
	string message;
	char dateLine[100];
	char expiresLine[100];
	createDates(dateLine, expiresLine);
	string content = JSONHandler.createJsonErr(); //Estari bueno pasarle un errorType

	message += "HTTP/1.1 404 Not Found";
	message += CRLF;
	message += dateLine;
	message += CRLF;
	message += "Cache-Control: public, max-age=30";
	message += CRLF;
	message += expiresLine;
	message += CRLF;
	message += "Content-Length: ";
	message += to_string(content.length());
	message += CRLF;
	message += "Content-Type: application/x-www-form-urlencoded";
	message += CRLF;
	message += CRLF;
	message += content;
	message += CRLF;

	return message;
}


string SPVNode::createServerOkRsp(string path)
{
	string message;
	char dateLine[100];
	char expiresLine[100];
	createDates(dateLine, expiresLine);

	string content = JSONHandler.createJsonOk();

	message += "HTTP/1.1 200 OK";
	message += CRLF;
	message += dateLine;
	message += CRLF;
	message += "Location: 127.0.0.1" + path;
	message += CRLF;
	message += "Cache-Control: max-age=30";
	message += CRLF;
	message += expiresLine;
	message += CRLF;
	message += "Content-Length: ";
	message += to_string(content.length());
	message += CRLF;
	message += "Content-Type: application/x-www-form-urlencoded";
	message += CRLF;
	message += CRLF;
	message += content;
	message += CRLF;

	return message;
}


void SPVNode::createDates(char* c1, char* c2)
{
	//Fecha actual
	time_t currentTime = time(nullptr);
	struct tm t;
	struct tm* currTime = &t;
	gmtime_s(currTime, &currentTime);
	strftime(c1, 100, "Date: %a, %d %b %G %X GMT", currTime);

	//Fecha de expiracion
	struct tm t2 = t;
	struct tm* nextTime = &t2;
	if (nextTime->tm_sec >= 30) {
		if (nextTime->tm_min == 59) {
			nextTime->tm_hour++;
		}
		nextTime->tm_min = ((nextTime->tm_min) + 1) % 60;
	}
	nextTime->tm_sec = ((nextTime->tm_sec) + 30) % 60;
	strftime(c2, 100, "Expires: %a, %d %b %G %X GMT", nextTime);
}


bool SPVNode::makeSmartTX(longN fee, const vector<Vout>& receivers, Transaction& tx)
{
	tx.txId.clear();
	tx.nTxIn = 0;
	tx.nTxOut = 0;
	tx.vIn.clear();
	tx.vOut.clear();

	bool validez = utxohandler.createTX(ownData.getID(), receivers, tx, fee);
	cryptohandler.signAllVinsInTx(tx);
	cryptohandler.hashTx(tx);
	return validez;
}

/***********************************************************************************
	POSTING / GETTING METHODS
***********************************************************************************/
errorType SPVNode::getBlockHeader(string id)
{
	errorType err = { false,"" };
	Client* client = new Client(headerNode);
	string header = JSONHandler.createHeader(id);
	client->GET("/eda_coin/get_block_header/", header);
	client->sendRequest();
	clients.push_back(client);
	notifyAllObservers(this);
	return err;
}

errorType SPVNode::postTransaction(Transaction tx)
{
	Client* client = new Client(headerNode);
	string tx_ = JSONHandler.createJsonTx(tx);
	client->POST("/eda_coin/send_tx", tx_);
	errorType err = client->sendRequest();
	clients.push_back(client);
	notifyAllObservers(this);
	return err;
}



void SPVNode::verifyMerkleBlock(Block head) {
	errorType error;
	newIDstr headRoot = head.getMerkleRoot();
	if (headRoot != head.getRootFromPath(mBlocks.end()->merklePath)) {
		error.error = true;
		error.datos = "Merkle Block and Block Header don't match!";
	}
	else {
		error.error = false;
		error.datos = "Merkle Block and Block Header match!";
	}
	blockVerification.push_back(error);
	notifyAllObservers(this);
}

errorType SPVNode::postFilter()
{
	Client* client = new Client(filterNode);
	string id = JSONHandler.createJsonFilter(ownData);
	client->POST("/eda_coin/send_filter", id);
	errorType err = client->sendRequest();
	clients.push_back(client);
	notifyAllObservers(this);
	return err;
}