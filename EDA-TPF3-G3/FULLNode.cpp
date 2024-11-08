/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "FULLNode.h"
#include "layoutGeneratorHandler.h"
#include "json.hpp"

 /*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define CRLF "\x0D\x0A"
#define INIT_CHALLENGE 3
#define MINING_MID 60000
#define MINING_UPPER 70000
#define MINING_LOWER 50000
#define BLOCKS_FOR_AVERAGE 10

 /*******************************************************************************
  * NAMESPACES
  ******************************************************************************/
using json = nlohmann::json;

/*******************************************************************************
	CONSTRUCTOR
 ******************************************************************************/
FULLNode::FULLNode(Socket _socket, tipo_de_nodo tipoNodo) 
	: Node(NO_NODE_DATA),
	cryptohandler(tipoNodo), utxohandler(tipoNodo, &blockChain, &txs)
{
	ownData.setID(cryptohandler.getMyPublicKey());
	ownData.setSocket(_socket);
	nodeType = tipoNodo;

	nodeState = IDLE;
	//JSONHandler.saveBlockChain(blockChain, "BlockChain.json");
	JSONHandler.getNodesInLayout("manifest.json", ownData, nodesInManifest);
	port = ownData.getSocket().getPort();

	//Timing sh*t
	clock = chrono::system_clock::now();
	int timing = rand() % 1000 + 1;
	timing *= 10;
	cout << "Node " << ownData.getID() << " created with timing = " << timing << endl;
	chrono::duration<int, milli> dur(timing);
	timeout = dur;

	//Create Genesis Server
	Server* genesisServer = new Server(ownData.getSocket().getPort());
	genesisServer->startConnection();			//Preguntar si esto funcaria
	servers.push_back(genesisServer);
	
	challenge = INIT_CHALLENGE;
	newBlocks = 0;
	chrono::duration<int, milli> durr(0);
	miningAverage = durr;
}

/*******************************************************************************
	DESTRUCTOR
 ******************************************************************************/
FULLNode::~FULLNode() {
	cout << "Destroyed node" << endl;

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
void FULLNode::cycle() {
	int gotReady = -1;
	cout << "Node" << ownData.getID() << " Node state: " << nodeState << endl;
	bool gotSomething = false;
	switch (nodeState) {
	case IDLE:
		servers.back()->listening();
		if (servers.back()->getDoneListening()) {
			if (servers.back()->getDoneDownloading()) {
				if (servers.back()->getDoneSending()) {
					cout << "Server done servering" << endl;
					//HANDLE FINISHED SERVER
					if (servers.back()->getState() == PING) {					//If layout was correctly received		//SPEAK WITH NETWORKING PPL
						cout << "Node " << ownData.getID() << " Just received PING! Entering WAITING LAYOUT state!" << endl;
						nodeState = WAITING_LAYOUT;
					}
					else {
						gotSomething = false;
					}
					delete servers.back();
					servers.pop_back();											//Remove useless server
					Server* newServer = new Server(port);
					newServer->startConnection();								//Create new server
					cout << "Node" << ownData.getID() << " created server to RECEIVE LAYOUT " << endl;
					servers.push_back(newServer);
				}
				else {
					gotSomething = true;
					cout << "Node " << ownData.getID() << " Is RESPONING to a message" << endl;
					servers.back()->sendMessage(serverResponse(servers.back()->getState(), servers.back()->getMessage()));
				}
			}
			else {
				cout << "Node " << ownData.getID() << " Is RECEIVING a message" << endl;
				servers.back()->receiveMessage();
			}
		}
		else {
			cout << "Node " << ownData.getID() << " Is listening to VOID" << endl;
			servers.back()->listening();
		}
		//Pick random timeout
		//if (nodeState == IDLE && !isLedaderNode && !gotSomething) {
		if (nodeState == IDLE && !gotSomething) {
			if (chrono::system_clock::now() > clock + timeout) {	//If timout ocurred
				nodeState = COLLECTING_MEMBERS;						//We take care of the layout
				cout << "Node " << ownData.getID() << " Just got charged with creating the NETWORK! Entering COLLECTING MEMBERS state!" << endl;
				isLedaderNode = true;
				for (int i = 0; i < nodesInManifest.size(); i++) {
					if (!(ownData == nodesInManifest[i]))
						postPing(nodesInManifest[i]);		//Ping each node in manifest who isn't me (just a bit, rest of sending is done in COLLECTING_MEMBERS)
				}
			}
		}
		break;
	case COLLECTING_MEMBERS:									//Look at me, I build the network now
		for (int i = 0; i < clients.size(); i++) {
			cout << "Node " << ownData.getID() << " Trying to PING PORT " << clients[i]->getReceiverData().getSocket().getPort() << endl;
			clients[i]->sendRequest();
			if (clients[i]->getRunning() == 0 && nodesInManifest.size() != neighbourhood.size()) {
				if (clients[i]->getTranslatedResponse() == MSG_NETWORK_READY) {							//SPEAK WITH NETWORKING PPL
					nodeState = SENDING_LAYOUT;
					manageNetworkReady(clients[i]->getResponse());
					gotReady = i;
					break;
				}
				else if (clients[i]->getTranslatedResponse() == MSG_NETWORK_NOT_READY) {				//SPEAK WITH NETWORKING PPL
					neighbourhood.push_back(clients[i]->getReceiverData());
					cout << "Node " << ownData.getID() << " ADDED " << clients[i]->getReceiverData().getSocket().getPort() << endl;
					delete clients[i];								//Destroy client
					clients.erase(clients.begin() + i);				//Remove client from list
				}
				else {												//If none of the responses is received
					postPing(clients[i]->getReceiverData());				//Post ping again
					delete clients[i];								//Destroy failed client
					clients.erase(clients.begin() + i);				//Remove failed client from list
				}
			}
		}
		if (nodesInManifest.size() == neighbourhood.size()) {				//If the amount of nodes in the manifest is equal to the amount of nodes who responded NETWORK_NOT_READY
			/*bool allResponded = true;
			for (int i = 0; i < nodesInManifest.size(); i++) {
				bool found = false;
				for (int j = 0; j < network.size(); j++) {
					if (nodesInManifest[i].getID() == network[j].getID()  && nodesInManifest[i].getSocket().getIP() == network[j].getSocket().getIP() && nodesInManifest[i].getSocket().getPort() == network[j].getSocket().getPort())
						found = true;
				}
				if (!found)
					allResponded = false;
			}		//COMMENT DOUBLE FOR IF UNRESPONSIVE, JUST COMPARING SIZES ((?) MIGHT BE ENOUGH TO ENSURE CONNECTION
			if (allResponded)*/
			nodeState = SENDING_LAYOUT;
		}
		if (nodeState == SENDING_LAYOUT) {
			makeLayout();
			layoutMsg = JSONHandler.createJsonLayout(layout);
			if (gotReady != -1) {
				addNeighbour(clients[gotReady]->getReceiverData());						//SPEAK WITH NETWORKING PPL
			}
			for (int i = 0; i < clients.size(); i++) {
				delete clients[i];								//Destroy client
				clients.erase(clients.begin() + i);				//Remove client from list
			}
			clients.empty();
			for (int i = 0; i < neighbourhood.size(); i++)
				postLayout(neighbourhood[i]);				//Start posting Layout to all nodes in network
		}
		break;
	case WAITING_LAYOUT:
		servers.back()->listening();
		if (servers.back()->getDoneListening()) {
			if (servers.back()->getDoneDownloading()) {
				if (servers.back()->getDoneSending()) {
					cout << "Server done servering" << endl;
					if (servers.back()->getState() == LAYOUT) {					//If layout was correctly received		//SPEAK WITH NETWORKING PPL
						JSONHandler.readLayout(servers.back()->getMessage(), ownData, neighbourhood);		//Read layout, and add my neighbours
						cout << "Node " << ownData.getID() << "Got message in WAITING LAYOUT, WAS LAYOUT YAY!!" << endl;
						for (int i = 0; i < neighbourhood.size(); i++) {
							cout << "Node " << ownData.getID() << "'s new neighbour is Node " << neighbourhood[i].getID() << endl;
							for (int j = 0; j < nodesInManifest.size(); j++) {
								if (nodesInManifest[j].getID() == neighbourhood[i].getID())
									neighbourhood[i].setSocket(nodesInManifest[j].getSocket());
							}
						}
						makeLayout();
						nodeState = NETWORK_CREATED;								//And now we work as usual
						clock = chrono::system_clock::now();
						for (int i = 0; i < clients.size(); i++) {
							delete clients[i];								//Destroy client
							clients.erase(clients.begin() + i);				//Remove client from list
						}
					}
					else {
						cout << "Node " << ownData.getID() << "Got message in WAITING LAYOUT, but SERVER STATE wasn't GOT_LAYOUT!" << endl;
					}
					delete servers.back();
					servers.pop_back();											//Remove useless server
					Server* newServer = new Server(port);
					newServer->startConnection();								//Create new server
					servers.push_back(newServer);
				}
				else {
					cout << "Node " << ownData.getID() << " Is RESPONDING to a message" << endl;
					servers.back()->sendMessage(serverResponse(servers.back()->getState(), servers.back()->getMessage()));
				}
			}
			else {
				cout << "Node " << ownData.getID() << " Is RECEIVING a message" << endl;
				servers.back()->receiveMessage();
			}
		}
		break;
	case SENDING_LAYOUT:
		for (int i = 0; i < clients.size(); i++) {
			clients[i]->sendRequest();
			if (clients[i]->getRunning() == 0) {
				if (clients[i]->getTranslatedResponse() == HTTP_OK) {					//SPEAK WITH NETWORKING PPL
					cout << "Node " << ownData.getID() << " succesfully sent LAYOUT to Node " << clients[i]->getReceiverData().getID() << endl;
					delete clients[i];								//Destroy client
					clients.erase(clients.begin() + i);				//Remove client from list
				}
				else {
					cout << "Node " << ownData.getID() << " tryied to send LAYOUT to Node " << clients[i]->getReceiverData().getID() << " but was refused!" << endl;
					postLayout(clients[i]->getReceiverData());			//Post layout again
					delete clients[i];								//Destroy failed client
					clients.erase(clients.begin() + i);				//Remove failed client from list
				}
			}
		}
		if (clients.empty()) {										//If all clients were destroyed
			nodeState = NETWORK_CREATED;							//All nodes received layout, and network is created
			clock = chrono::system_clock::now();
		}
		break;
	case NETWORK_CREATED:
		keepListening();
		keepSending();
		break;
	default:
		cout << "You fucked up with the FSM, Morty!" << endl;
		break;
	}
}

/***********************************************************************************
		METHODS USED BY CONTROLLER
***********************************************************************************/
errorType FULLNode::makeTX(const vector<Vout>& receivers, longN fee) {
	errorType ret;
	Transaction newTx;
	if (makeSmartTX(fee, receivers, newTx)) {
		ret.error = false;
		ret.datos = "Valid transaction. Have a nice day, beach";
		txs.push_back(newTx);
	}
	else {
		ret.error = true;
		ret.datos = "INVALID TRANSACTION DETECTED, NICE TRY BEACH";
	}
	for (int i = 0; i < neighbourhood.size(); i++)
		postTransaction(i, newTx);
	notifyAllObservers(this);
	return ret;
}

errorType FULLNode::makeBlock() {
	errorType ret;
	Block block = blockChain.back();
	//DO SOMETHING TO BLOCK
	block.setHeight(blockChain.size() + 1);
	block.setBlockId((blockChain.back().getBlockID()) + "A");
	//END OF BLOCK CONFIG
	blockChain.push_back(block);
	for (int i = 0; i < neighbourhood.size(); i++)
		postBlock(i, blockChain.size());									//Post Block to all neighbours
	return ret;
}
errorType FULLNode::addNeighbour(NodeData neighbour) {
	errorType ret;
	neighbourhood.push_back(neighbour);
	return ret;
}

/***********************************************************************************
		METHODS USED BY VIEWER
***********************************************************************************/
NodeData FULLNode::getData() {
	return ownData;
}
const vector<NodeData>* FULLNode::getNeighbours() {
	return &neighbourhood;
}
const vector<Transaction>* FULLNode::getPendingTX() {
	return &txs;
}

const BlockChain* FULLNode::getBlockChain() {
	return &blockChain;
}

const Layout* FULLNode::getLayout()
{
	return &layout;
}

fullNodeStates FULLNode::getState()
{
	return nodeState;
}


/***********************************************************************************
	INNER EDACoin VARIABLES
***********************************************************************************/
void FULLNode::addBlock(Block block) {
	blockChain.push_back(block);
	notifyAllObservers(this);
}

void FULLNode::addTx(string trans) {
	JSONHandler.saveTx(trans, txs);
	notifyAllObservers(this);
}
void FULLNode::updateMyMoney() //llamar UNA VEZ Q SE HAYA MODIFICADO MI DINERO (ingrese una tx, bloque, etc)
{
	myMoney = utxohandler.balance(ownData.getID());
	notifyAllObservers(this);
}
//void FULLNode::saveMerkleBlock(string merkleBlock) {}

/***********************************************************************************
	NETWORKING SH*T
***********************************************************************************/
void FULLNode::makeLayout()
{
	layoutGeneratorHandler layoutGen(ownData, neighbourhood);
	layoutGen.generateLayout();
	layout = layoutGen.getLayout();
}
void FULLNode::keepListening() {
	vector<vector<Server*>::iterator> deleteThis;
	vector<Server*> doneServers;
	servers.back()->listening();

	if ((*(servers.end() - 1))->getDoneListening()) {
		cout << "Latest Server picked up something!" << endl;
		Server* newServer = new Server(port);
		newServer->startConnection();
		servers.push_back(newServer);
		cout << "New Server created and pushed!" << endl;
	}
	auto i = servers.begin();
	for (; i != servers.end() - 1; i++) {
		if (!(*i)->getDoneDownloading())
			(*i)->receiveMessage();
		else if (!(*i)->getDoneSending())
			(*i)->sendMessage(serverResponse((*i)->getState(), (*i)->getMessage()));
		if ((*i)->getDoneSending()) {
			cout << "Server done servering" << endl;
			doneServers.push_back(*i);
			deleteThis.push_back(i);
		}
	}
	//Handle finished servers
	auto j = doneServers.begin();
	for (; j != doneServers.end(); j++) {
		Block blck;
		bool found = false;
		switch ((*j)->getState()) {
		case BLOCK:			//Done
			blck.saveBlock((*j)->getMessage());
			handleReceivedBlock(blck);
			/*found = false;
			for (int i = 0; i < blockChain.size(); i++) {
				if (blck.getBlockID() == blockChain[i].getBlockID())			//If received block is already in chain, it gets ignored
					found = true;
			}
			if (!found) {														//If it's a new block
				blockChain.push_back(blck);										//Save block into blockchain
				checkForFilter(blck);											//Inform possible suscripted SPVNodes
				floodBlock(blck, (*j)->getSender());							//And flood the block
			}*/
			break;
		case TX:			//Done
			handleReceivedTx((*j)->getMessage());
			break;
		case MERKLE:														//FULL NODES DONT CARE ABOUT RECEIVING MERKLE BLOCKS
			break;
		case FILTER:
			filters.push_back(JSONHandler.saveFilter((*j)->getMessage()));
			found = false;
			for (int i = 0; i < neighbourhood.size(); i++) {
				if (neighbourhood[i] == (*j)->getSender())
					found = true;
			}
			if (!found)
				neighbourhood.push_back((*j)->getSender());					//If Filter sender is not a neighbour, we add it
			askedForFilter.push_back((*j)->getSender());
			break;
		}
	}
	if (!deleteThis.empty())
		notifyAllObservers(this);
	i = servers.begin();
	for (; i != servers.end() - 1; i++) {
		if ((*i)->getDoneSending())
			delete* i;
	}
	auto k = deleteThis.begin();
	for (; k != deleteThis.end(); k++) {
		servers.erase(*k);
	}
}

void FULLNode::keepSending() {
	for (int i = 0; i < clients.size(); i++) {
		clients[i]->sendRequest();
		if (clients[i]->getRunning() == 0) {
			//FULLNODES DONT EXPECT ANSWER TO POSTS IN NETWORK_READY STATE (?)
			delete clients[i];								//Destroy client
			clients.erase(clients.begin() + i);				//Remove client from list
			notifyAllObservers(this);
		}
	}
}

/***********************************************************************************
	POSTING / GETTING METHODS
***********************************************************************************/
errorType FULLNode::postTransaction(unsigned int neighbourPos, Transaction tx)
{
	Client* client = new Client(neighbourhood[neighbourPos]);
	string tx_ = JSONHandler.createJsonTx(tx);
	client->POST("/eda_coin/send_tx", tx_);
	errorType err = client->sendRequest();
	clients.push_back(client);

	notifyAllObservers(this);
	return err;
}

errorType FULLNode::postBlock(unsigned int neighbourPos, unsigned int height)
{
	Block bl0ck;
	for (int i = 0; i < blockChain.size(); i++)
	{
		if (blockChain[i].getHeight() == height)
		{
			bl0ck = blockChain[i];
		}
	}

	Client* client = new Client(neighbourhood[neighbourPos]);
	string blck = JSONHandler.createJsonBlock(bl0ck);
	client->POST("/eda_coin/send_block", blck);
	errorType err = client->sendRequest();
	clients.push_back(client);

	notifyAllObservers(this);
	return err;
}

errorType FULLNode::postMerkleBlock(Block blck, Transaction tx, unsigned int neighbourPos)
{
	Client* client = new Client(neighbourhood[neighbourPos]);
	string merkle = JSONHandler.createJsonMerkle(blck, tx);
	client->POST("/eda_coin/send_merkle_block", merkle);
	errorType err = client->sendRequest();
	clients.push_back(client);

	notifyAllObservers(this);
	return err;
}

errorType FULLNode::postMerkleBlock(Block blck, Transaction tx, NodeData data)
{
	Client* client = new Client(data);
	string merkle = JSONHandler.createJsonMerkle(blck, tx);
	client->POST("/eda_coin/send_merkle_block", merkle);
	errorType err = client->sendRequest();
	clients.push_back(client);

	notifyAllObservers(this);
	return err;
}


errorType FULLNode::postLayout(NodeData sock)
{
	Client* client = new Client(sock);
	client->POST("/eda_coin/NETWORK_LAYOUT", layoutMsg);
	errorType err = client->sendRequest();
	clients.push_back(client);

	notifyAllObservers(this);
	return err;
}



errorType FULLNode::postPing(NodeData sock)
{
	Client* client = new Client(sock);
	client->POST("/eda_coin/PING");
	errorType err = client->sendRequest();
	clients.push_back(client);

	notifyAllObservers(this);
	return err;
}

/***********************************************************************************
	SEREVR REPONSE
***********************************************************************************/
string FULLNode::serverResponse(STATE rta, string msg)
{
	string message;

	switch (rta)
	{
	case HEADER:
		if (checkForId(msg))
		{
			message = createServerHeader("/eda_coin/get_block_header", msg);
		}

		else
		{
			message = createServerErrRsp();
		}
		break;

	case TX:
		message = createServerOkRsp("/eda_coin/send_tx");
		break;

	case BLOCK:
		message = createServerOkRsp("/eda_coin/send_block");
		break;

	case MERKLE:
		message = createServerOkRsp("/eda_coin/send_merkle_block");
		break;

	case FILTER:
		message = createServerOkRsp("/eda_coin/send_filter");
		break;

	case LAYOUT:
		message = createServerOkRsp("/eda_coin/NETWORK_LAYOUT");
		break;

	case PING:
		if (nodeState == NETWORK_CREATED)	//DUDOSISIMO ESPERAR ALEJO
		{
			message = createServerReadyRsp();
		}

		else
		{
			message = createServerNotReadyRsp();
		}
		break;

	case ERR:
		message = createServerErrRsp();
		break;
	}

	return message;

}

string FULLNode::createServerNotReadyRsp()
{
	string message;
	char dateLine[100];
	char expiresLine[100];
	createDates(dateLine, expiresLine);

	string content = JSONHandler.createJsonNotReady();

	message += "HTTP/1.1 200 OK";
	message += CRLF;
	message += dateLine;
	message += CRLF;
	message += "Location: 127.0.0.1/eda_coin/PING";
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

string FULLNode::createServerReadyRsp()
{
	string message;
	char dateLine[100];
	char expiresLine[100];
	createDates(dateLine, expiresLine);
	string content = JSONHandler.createJsonReady(layout, blockChain);

	message += "HTTP/1.1 200 OK";
	message += CRLF;
	message += dateLine;
	message += CRLF;
	message += "Location: 127.0.0.1/eda_coin/PING";
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

string FULLNode::createServerErrRsp()
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


string FULLNode::createServerHeader(string path, string id)
{
	string message;
	char dateLine[100];
	char expiresLine[100];
	createDates(dateLine, expiresLine);
	string content = JSONHandler.createJsonBlockHeader(blockChain, id);

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

string FULLNode::createServerOkRsp(string path)
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


void FULLNode::createDates(char* c1, char* c2)
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

bool FULLNode::makeSmartTX(longN fee, const vector<Vout>& receivers, Transaction& tx)
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
		FLOODING / VERIFICATION
***********************************************************************************/
void FULLNode::checkForFilter(Block blck)
{
	for (int j = 0; j < filters.size(); j++)
	{
		for (int i = 0; i < blck.getTransactions().size(); i++)
		{
			for (int k = 0; k < blck.getTransactions()[i].vIn.size(); k++)
			{
				if (blck.getTransactions()[i].vIn[k].blockId == filters[j].publicID)					//NOT ACTUALLY CORRECT
				{																						//SHOULD ACTUALLY SEARCH FOR A BLOCK WITH THE SAME ID
					NodeData d("Dummy", filters[j].port, filters[j].ip);								//AND FIND THE TX WITH THE SAME TXID, AND CHECK ((ITS)) VOUT FOR PUBLIC ID
					postMerkleBlock(blck, blck.getTransactions()[i], d);								//MAPS PERHAPS?
					break;
				}
			}

			for (int k = 0; k < blck.getTransactions()[i].vOut.size(); k++)
			{
				if (blck.getTransactions()[i].vOut[k].publicId == filters[j].publicID)
				{
					NodeData d("Dummy", filters[j].port, filters[j].ip);
					postMerkleBlock(blck, blck.getTransactions()[i], d);
					break;
				}
			}

		}

	}
}

bool FULLNode::checkForId(string id)
{
	bool ret = false;
	for (int i = 0; i < blockChain.size(); i++)
	{
		if (blockChain[i].getBlockID() == id)
		{
			ret = true;
		}
	}

	return ret;
}


void FULLNode::floodBlock(Block blck, NodeData sender) {
	for (int i = 0; i < neighbourhood.size(); i++) {
		if (sender.getID() != neighbourhood[i].getID())					//If neighbour is not the one who sent the block
			postBlock(i, blockChain.size());							//Send neighbour the block ()
	}
}
void FULLNode::floodTx(Transaction tx, NodeData sender) {
	for (int i = 0; i < neighbourhood.size(); i++) {
		if (sender.getID() != neighbourhood[i].getID())					//If neighbour is not the one who sent the tx
			postTransaction(i, tx);										//Send neighbour the tx
	}
}


void FULLNode::manageNetworkReady(string rta)
{
	json rt = json::parse(rta);
	string blckchain = rt["blockchain"].dump();
	BlockChain laNewBlckChain;
	JSONHandler.saveBlockChain(laNewBlckChain, blckchain);
	blockChain = laNewBlckChain;


}

void FULLNode::handleReceivedTx(string txString) {
	JSONHandler.saveTx(txString, txs);
	Transaction newTx = txs.back();
	txs.pop_back();
	if (!utxohandler.TxExistAlready(newTx) && cryptohandler.verifyTXHash(newTx) && cryptohandler.verifyTXSign(newTx, &utxohandler) && utxohandler.validateTX(newTx).error) {
		utxohandler.insertTX(newTx);
		floodTx(newTx, ownData);
		notifyAllObservers(this);
	}
}
void FULLNode::handleReceivedBlock(Block& block) {
	if (verifyChallenge(block) && verifyPrevID(block) && !utxohandler.BlockExistAlready(block) && utxohandler.validateBlock(block).error && cryptohandler.verifyBlockHash(block) && cryptohandler.verifyBlockSign(block, &utxohandler)) {
		blockChain.push_back(block);
		floodBlock(block, ownData);
		handleChallengeRating();
		notifyAllObservers(this);
	}
}

void FULLNode::handleChallengeRating() {
	newBlocks++;
	miningAverage += chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - clock);	//Clock has timepoint from last mined/received block
	if (newBlocks == BLOCKS_FOR_AVERAGE) {
		auto ms = miningAverage.count()/BLOCKS_FOR_AVERAGE;				//ms holds the average time for one block
		if (ms < MINING_LOWER)											//Adjust challenge rating accordingly
			challenge++;
		else if (ms > MINING_UPPER)
			challenge--;
		newBlocks = 0;													//Reset the counter for next average fase
		chrono::duration<int, milli> dur(0);
		miningAverage = dur;											//Reset time counter
	}
	clock = chrono::system_clock::now();								//Reset clock
}

bool FULLNode::verifyChallenge(Block& block) {
	bool ret = false;
	string ID = block.getBlockID();
	int i = 0;
	unsigned int zeros = 0;
	while (i < ID.size() && ID[i] == '0') {
		zeros++;
		i++;
	}
	if (zeros >= challenge)
		ret = true;
	return ret;
}

bool FULLNode::verifyPrevID(Block& block) {
	bool ret = false;
	if (block.getPreviousBlockID() == blockChain.back().getBlockID())
		ret = true;
	return ret;
}