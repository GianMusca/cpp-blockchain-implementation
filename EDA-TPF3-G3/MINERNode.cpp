#include "MINERNode.h"

MINERNode::MINERNode(Socket _socket) :
	FULLNode(_socket,NODO_MINERO), miningBlock(),
	mensajesDeVentana()
{
	cryptohandler.setMiningBlock(&miningBlock);
	utxohandler.setMiningBlock(&miningBlock);
}

/*******************************************************************************
	CYCLE
 ******************************************************************************/
void MINERNode::cycle() {
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
		utxohandler.startNewMiningBlock(ownData.getID(), cryptohandler);
		keepListening();
		keepSending();
		break;
	default:
		cout << "You fucked up with the FSM, Morty!" << endl;
		break;
	}
}

void MINERNode::handleReceivedBlock(Block& block) {
	if (verifyChallenge(block) && verifyPrevID(block) && !utxohandler.BlockExistAlready(block) && utxohandler.validateBlock(block).error && cryptohandler.verifyBlockHash(block) && cryptohandler.verifyBlockSign(block, &utxohandler)) {
		blockChain.push_back(block);
		floodBlock(block, ownData);
		handleChallengeRating();
		utxohandler.startNewMiningBlock(ownData.getID(), cryptohandler);
		notifyAllObservers(this);
	}
}


errorType MINERNode::getWindowMessage()
{
	errorType mensaje;
	if (mensajesDeVentana.size() == 0) {
		mensaje.error = false;
		mensaje.datos = "";
	}
	else {
		mensaje = mensajesDeVentana.back();
		mensajesDeVentana.pop_back();
	}
	return mensaje;
}

void MINERNode::miningActions()
{
	cryptohandler.tryNewNonce();
	if (verifyChallenge(miningBlock)) {
		//METERSE EL BLOQUE EN LA BLOCKCHAIN
		blockChain.push_back(miningBlock);
		//FLODEAR EL BLOQUE
		floodBlock(miningBlock, ownData);
		//Adjust challenge rating accordingly
		handleChallengeRating();
		errorType mensaje;
		mensaje.error = true;
		mensaje.datos = "ENHORABUENA!!!\nEste minero logro minar";
		mensajesDeVentana.emplace_back(mensaje);
		utxohandler.startNewMiningBlock(ownData.getID(),cryptohandler);

	}
}