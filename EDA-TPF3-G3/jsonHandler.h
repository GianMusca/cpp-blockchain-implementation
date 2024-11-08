#pragma once
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <iostream>
#include "Socket.h"
#include "block.h"
#include "blockChain.h"
#include "errorType.h"
#include "layout.h"

/*******************************************************************************
 * NAMESPACES
 ******************************************************************************/
using namespace std;

class jsonHandler
{
public:
	/***********************************************************************************
		SAVING METHODS
	***********************************************************************************/
	void saveBlockChain(BlockChain& blockchain, string path);
	void saveTx(string trans, vector<Transaction>& txs);
	void saveMerkleBlock(string merkleBlock,vector <MerkleBlock>& mrkl);
	Filter_tt saveFilter(string filter);
	Block saveBlockHeader(string header);
	
	/***********************************************************************************
		JSONS's CREATION
	***********************************************************************************/
	//Blockchain related
	string createJsonBlockHeader(BlockChain blockchain, string id);
	string createJsonBlockchain(BlockChain blckchain);
	string createJsonBlock(Block block);
	string createJsonTx(Transaction tx);
	string createJsonMerkle(Block block,Transaction trans);
	string createJsonFilter(Filter_tt filt);
	string createJsonFilter(NodeData data);

	//Server response related
	string createHeader(string id);
	string createJsonOk();
	string createJsonErr();
	string createJsonReady(Layout layout, BlockChain blockchain);
	string createJsonNotReady();

	/***********************************************************************************
		JSONS's VALIDATION
	***********************************************************************************/
	errorType validateBlock(string blck);
	errorType validateTx(string tx);
	errorType validateFilter(string filter);
	errorType validateMerkle(string merkle);
	errorType validateLayout(string layout);

	/***********************************************************************************
		LAYOUT CONTROLLERS
	***********************************************************************************/
	string createJsonLayout(Layout& layout);
	void readLayout(string layout, NodeData mySockt, vector <NodeData>& neighbourhood);
	void getNodesInLayout(string path, NodeData ownData, vector<NodeData>& nodes);


	/***********************************************************************************
		HACKING METHODS
	***********************************************************************************/
	ip_t crackIp(string ip);
	string decipherId(string code);
	
};