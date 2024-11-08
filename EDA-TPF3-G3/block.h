#pragma once
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <string>
#include <vector>
#include "Socket.h"
#include <cstdint>

/*******************************************************************************
 * NAMESPACES
 ******************************************************************************/
using namespace std;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 *****************************************************************************/
typedef std::string newIDstr;
typedef unsigned long int longN;
typedef vector<newIDstr> MerkleTree;


/********
 * VIN	*
********/
struct Vin {
	string blockId;
	string txId;
	int nutxo;
	string signature;
};

/********
 * VOUT	*
********/
struct Vout {
	string publicId;
	unsigned long int amount;
};

/****************
 * TRANSACTION	*
****************/
struct Transaction {
	string txId;
	unsigned long int nTxIn;
	vector<Vin> vIn;
	unsigned long int nTxOut;
	vector<Vout> vOut;
};

/****************
 * MERKLE BLOCK	*
****************/
struct MerkleBlock {
	string blockId;
	vector<Transaction> tx;
	int txPos;
	vector<newIDstr> merklePath;
};

/****************
 *    FILTER   	*
****************/
struct Filter_tt{
	string publicID;
	ip_t ip;
	unsigned int port;

	string getIPString()
	{
		return (to_string(ip.b1) + '.' + to_string(ip.b2) + '.' + to_string(ip.b3) + '.' + to_string(ip.b4));
	}

};



class Block {
public:
	/***********************************************************************************
		BLOCK RELATED METHODS
	***********************************************************************************/
	bool validateBlock(string block);
	void saveBlock(string blck);
	
	/***********************************************************************************
		GETTERS
	***********************************************************************************/
	const vector<Transaction>& getTransactions();
	const Transaction getTx(vector<Transaction>::iterator it);
	const Transaction getTx(unsigned int it);
	const unsigned long int getHeight() const;
	uint32_t getNonce();
	const unsigned long int getNTx();
	const long int getBlockPos(vector<Block>* BlockChain);
	const string getBlockID() const;
	const string getPreviousBlockID();
	const string getMerkleRoot();
	const string getTxId(Transaction tx);

	/***********************************************************************************
		MERKLE TREE & MERKLE PATH
	***********************************************************************************/
	vector<newIDstr> getMerkleTree();
	vector<string> getMerklePath(Transaction trx);
	newIDstr getRootFromPath(vector<newIDstr> path);

	/***********************************************************************************
		SETTERS
	***********************************************************************************/
	void addTx(Transaction _tx) { tx.push_back(_tx); };
	void setHeight(unsigned long int h) { height = h; };
	void setNonce(uint32_t h) { nonce = h; };
	void setBlockId(string s) { blockId = s; };
	void setPrevBlockId(string s) { previousBlockId = s; };
	void setMerkleRoot(string s) { merkleRoot = s; };
	void setNTx(unsigned long int n) { nTx = n; };
	void setTransactions(vector <Transaction> txs) { tx = txs; };


private:
	/***********************************************************************************
		MERKLE TREE & MERKLE PATH
	***********************************************************************************/
	void fillLevel(int level, int* nearestPow, vector<newIDstr>::iterator it, vector<newIDstr>* tree);
	void fillMerklePath(vector<newIDstr>* path, vector<newIDstr>* tree, int j);

	/***********************************************************************************
		BLOCK STRUCTURE
	***********************************************************************************/
	vector<Transaction> tx;			//utxo-
	unsigned long int height;		//utxo-
	uint32_t nonce;					//crypto-
	string blockId;					//crypto-
	string previousBlockId;			//utxo-
	string merkleRoot;				//?
	vector<string> merkleTree;		//?
	unsigned long int nTx;			//utxo-
};