/******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <vector>
#include "Block.h"
#include "json.hpp"
#include <fstream>
#include "cryptoFunctions.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BLOCK_FIELDS 7
#define TRANS_FIELDS 5

 /*******************************************************************************
  * FUNCTION DECLARATIONS WITH FILE LEVEL SCOPE
  ******************************************************************************/
static unsigned int generateID(const char* str);


/*******************************************************************************
 * CLASS METHODS DEFINITIONS
 ******************************************************************************/
using namespace std;
using json = nlohmann::json;


/***********************************************************************************
	BLOCK RELATED METHODS
***********************************************************************************/
bool Block::validateBlock(string blck)
{
	bool ret = false;

	try
	{
		json block_ = json::parse(blck);

		//Block
		if (block_.size() == BLOCK_FIELDS) //Si son 7 elementos
		{
			block_.at("height");
			block_.at("nonce");
			block_.at("blockid");	//Se fija que sean los correspondientes
			block_.at("previousblockid");
			block_.at("merkleroot");
			int ntx = block_.at("nTx");
			block_.at("height");
			block_.at("tx");

			//Transactions
			auto arrayTrans = block_["tx"];
			for (auto& trans : arrayTrans)	//Parsea todas las transacciones
			{
				if (arrayTrans.size() == ntx && trans.size() == TRANS_FIELDS)	//Si son 5 elementos
				{
					trans.at("txid");
					int txin = trans.at("nTxin");
					trans.at("vin");	//Se fija que sean los correctos
					int txout = trans.at("nTxout");
					trans.at("vout");

					auto vIn = trans["vin"];
					auto vOut = trans["vout"];
					if (vIn.size() == txin && vOut.size() == txout)
					{
						for (auto& elsi : vIn)
						{
							elsi.at("blockid");
							elsi.at("txid");
							elsi.at("signature");
							elsi.at("nutxo");
						}

						for (auto& elso : vOut)
						{
							elso.at("publicid");
							elso.at("amount");
						}

						ret = true;
					}
				}
			}
		}
	}

	catch (std::exception& e)
	{
		ret = false;
	}

	return ret;
}


void Block::saveBlock(string blck)
{
	json blocks = json::parse(blck);

	auto height = blocks["height"];
	this->height = height;

	auto nonce = blocks["nonce"];
	this->nonce = nonce;

	auto blockId = blocks["blockid"];
	this->blockId = blockId.get<string>();

	auto prevBlockId = blocks["previousblockid"];
	this->previousBlockId = prevBlockId.get<string>();

	auto root = blocks["merkleroot"];
	this->merkleRoot = root.get<string>();

	auto nTx = blocks["nTx"];
	this->nTx = nTx;

	//Transactions
	auto arrayTrans = blocks["tx"];
	for (auto& trans : arrayTrans)
	{
		Transaction auxTrans;

		auto txId = trans["txid"];
		auxTrans.txId = txId.get<string>();

		auto nTxIn = trans["nTxin"];
		auxTrans.nTxIn = nTxIn;

		auto vIn = trans["vin"];
		for (auto& elsi : vIn)
		{
			Vin auxVin;

			auto tBlockId = elsi["blockid"];
			auxVin.blockId = tBlockId.get<string>();

			auto tTxId = elsi["txid"];
			auxVin.txId = tTxId.get<string>();

			auto sign = elsi["signature"];
			auxVin.signature = sign.get<string>();

			auto nut = elsi["nutxo"];
			auxVin.nutxo = nut;

			auxTrans.vIn.push_back(auxVin);
		}

		auto nTxOut = trans["nTxout"];
		auxTrans.nTxOut = nTxOut;

		auto vOut = trans["vout"];
		for (auto& elso : vOut)
		{
			Vout auxVout;

			auto publicId = elso["publicid"];
			auxVout.publicId = publicId.get<string>();

			auto amount = elso["amount"];
			auxVout.amount = amount;

			auxTrans.vOut.push_back(auxVout);
		}

		this->tx.push_back(auxTrans);
	}
}


/***********************************************************************************
	MERKLE PATH
***********************************************************************************/
vector<newIDstr> Block::getMerklePath(Transaction trx) {
	vector<newIDstr> path;
	vector<newIDstr> tree = getMerkleTree();
	path.clear();
	auto i = tx.begin();
	int j = 0;
	for (; i != tx.end(); i++, j++) {
		if (i->txId == trx.txId)
			break;
	}
	if (i != tx.end()) {	//j CONTAINS TRANSACTION POSITION IN tx ARRAY
		fillMerklePath(&path, &tree, j);
	}
	return path;
}

void Block::fillMerklePath(vector<newIDstr>* path, vector<newIDstr>* tree, int j){
	unsigned long int transactions = nTx;
	unsigned long int nearestPowerOf2 = pow(2, ceil(log(transactions) / log(2)));
	int maxLvl = (log(nearestPowerOf2) / log(2));		//Max level es el nivel del arbol menos el MerkleRoot

	if (j % 2) {										//Si el txID ocupa una posicon impar
		path->push_back((*tree)[j - 1]);				//Cargo primero el anterior
	}
	else {												//Si el txID es par
		path->push_back((*tree)[j + 1]);				//Luego el siguiente
	}

	int prevLvlAmount = nTx;
	if (nTx % 2)
		prevLvlAmount++;
	int offset = prevLvlAmount;
	j /= 2;
	while (--maxLvl) {
		if (j % 2)												//Si el txID ocupa una posicon impar
			path->push_back((*tree)[offset + j - 1]);			//Cargo el anterior
		else													//Si el txID es par
			path->push_back((*tree)[offset + j + 1]);			//Cargo el siguiente
		j /= 2;
		prevLvlAmount /= 2;										//Ahora el nivel actual tiene la mitad de elementos que el anterior
		if (prevLvlAmount % 2)
			prevLvlAmount++;
		offset += prevLvlAmount;
	}

	//path->push_back(*tree->end())								//Uncomment to push merkle root
	return;
}


/***********************************************************************************
	MERKLE TREE
***********************************************************************************/
vector<newIDstr> Block::getMerkleTree() {
	vector<string> tree;
	tree.clear();
	unsigned long int transactions = nTx;
	unsigned long int nearestPowerOf2 = pow(2, ceil(log(transactions) / log(2)));
	//Normalize floor of tree
	//Not filling floor
	for (int i = 0; i < transactions; i++) {
		/*unsigned long int inTrans = tx[i].nTxIn;
		string concatenate;
		concatenate.clear();
		for (int j = 0; j < inTrans; j++)
			concatenate += tx[i].vIn[j].txId;
		char aux[9];
		unsigned int ID = generateID(concatenate.c_str());
		sprintf_s(&aux[0], 9, "%08X", ID);*/
		newIDstr leafID(tx[i].txId);
		tree.push_back(leafID);
	}
	int prevLvlAmount = transactions;
	if (transactions % 2) {
		tree.push_back(*(tree.end() - 1));
		prevLvlAmount++;
	}
	int maxLvl = (log(nearestPowerOf2) / log(2));		//Max level es el nivel del arbol menos el MerkleRoot
	for (int i = maxLvl - 1; i > 0; i--) {					//Itera por cada nivel del arbol menos el primero que ya esta normalizado
		vector<newIDstr>::iterator it = tree.end();
		fillLevel(i, &prevLvlAmount, it, &tree);				//Rellena todos los arboles
	}

	/*newIDstr concatenate = *(tree.end() - 2) + *(tree.end() - 1);		//Concatena un par de elementos del nivel anterior
	char aux[9];
	unsigned int ID = generateID(concatenate.c_str());
	sprintf_s(&aux[0], 9, "%08X", ID);*/
	newIDstr newID(hash2nodes(*(tree.end() - 2), *(tree.end() - 1)));
	tree.push_back(newID);

	return tree;
}

void Block::fillLevel(int level, int* prevLvlAmount, vector<newIDstr>::iterator it, vector<newIDstr>* tree) {		//Fill level asume que el nivel anterior esta completo y lindo
	vector<newIDstr> lvl;
	lvl.clear();
	vector<newIDstr>::iterator newIt = it;						//it esta posicionado en el ultimo elemento del nivel anterior
	newIt -= (*prevLvlAmount);								//newIt apunta al primer elemento del nivel anterior
	for (int i = 0; i < *prevLvlAmount; i += 2) {
		/*newIDstr concatenate = *(newIt + i) + *(newIt + i + 1);		//Concatena un par de elementos del nivel anterior
		char aux[9];
		unsigned int ID = generateID(concatenate.c_str());
		sprintf_s(&aux[0], 9, "%08X", ID);*/
		newIDstr newID(hash2nodes (*(newIt + i), *(newIt + i + 1)));
		lvl.push_back(newID);										//Se pushea la concatenacion al vector del nivel actual
	}
	*prevLvlAmount /= 2;											//Ahora el nivel actual tiene la mitad de elementos que el anterior
	if ((*prevLvlAmount) % 2) {										//Si el nivel actual tiene elementos impares
		lvl.push_back(*(lvl.end() - 1));								//Se copia el ultimo elemento
		(*prevLvlAmount)++;											//El nivel actual queda con elementos pares
	}
	for (auto i = lvl.begin(); i != lvl.end(); i++) {
		tree->push_back(*i);									//Se pushea todo el nivel al arbol
	}
	return;
}


/***********************************************************************************
	GETTERS
***********************************************************************************/
const long int Block::getBlockPos(vector<Block>* BlockChain) {
	if (!BlockChain->empty()) {
		auto i = BlockChain->begin();
		for (int j = 0; i != BlockChain->end(); i++, j++) {
			if (i->getHeight() == height)
				return j;
		}
	}
	return -1;
}

const string Block::getTxId(Transaction tx) {
	/*string concatenate;
	concatenate.clear();
	for (int j = 0; j < tx.nTxIn; j++)
		concatenate += tx.vIn[j].txId;
	char aux[9];
	unsigned int ID = generateID(concatenate.c_str());
	sprintf_s(&aux[0], 9, "%08X", ID);
	newIDstr leafID(aux);*/
	return tx.txId;
}
newIDstr Block::getRootFromPath(vector<newIDstr> _path) {
	newIDstr ret;
	vector<newIDstr> path = _path;
	if (path.size() == 1)
		return path[0];
	//newIDstr concatenate = path[0] + path[1];		//Concatena un par de elementos del nivel anterior
	//char aux[9];
	//unsigned int ID = generateID(concatenate.c_str());
	//sprintf_s(&aux[0], 9, "%08X", ID);
	newIDstr newID(hash2nodes(path[0], path[1]));
	if (path.size() == 2) {
		return newID;
	}
	else {
		path.erase(path.begin());
		path.erase(path.begin() + 1);
		path.insert(path.begin(), newID);					//Now path only needs to be concatenated by pairs
		while(path.size() > 1) {
			//newIDstr concatenate = path[0] + path[1];		//Concatena un par de elementos del nivel anterior
			//char aux[9];
			//unsigned int ID = generateID(concatenate.c_str());
			//sprintf_s(&aux[0], 9, "%08X", ID);
			newIDstr newID(hash2nodes(path[0], path[1]));
			path.erase(path.begin());
			path.erase(path.begin() + 1);
			path.insert(path.begin(), newID);				//Insert concatenated pair to begining of list
		}
		return path[0];
	}
}

const vector<Transaction>& Block::getTransactions() { return tx; }
const Transaction Block::getTx(vector<Transaction>::iterator it) { for (auto i = tx.begin(); i != tx.end(); i++) { if (i == it) return *i; } }
const Transaction Block::getTx(unsigned int it) { if (it >= 0 && it < tx.size())	return tx[it]; }
const unsigned long int Block::getHeight() const { return height; }
uint32_t Block::getNonce() { return nonce; }
const unsigned long int Block::getNTx() { return nTx; }
const string Block::getBlockID() const { return blockId; }
const string Block::getPreviousBlockID() { return previousBlockId; }
const string Block::getMerkleRoot() { return merkleRoot; }

/*******************************************************************************
 * FUNCTION DEFINITIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//static unsigned int generateID(const char* str) {
//	unsigned int ID = 0;
//	int c;
//	while (c = *str++)
//		ID = c + (ID << 6) + (ID << 16) - ID;
//
//	return ID;
//}