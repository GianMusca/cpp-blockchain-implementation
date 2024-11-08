/******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "jsonHandler.h"
#include "json.hpp"
#include <fstream>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BLOCK_FIELDS 7
#define TRANS_FIELDS 5
#define MRKL_FIELDS 4

/*******************************************************************************
 * NAMESPACES
 ******************************************************************************/
using namespace std;
using json = nlohmann::json;


/***********************************************************************************
	SAVING METHODS
***********************************************************************************/
void jsonHandler::saveBlockChain(BlockChain& blockchain, string path)
{
	json j;
	std::ifstream i(path.c_str()); //Se puede cambiar, no se como recibo el JSON;

	if (i.is_open())
		i >> j;

	else 
		j = json::parse(path);

	for (auto& blocks : j)
	{
		//Block 
		Block block;

		auto height = blocks["height"];
		block.setHeight(height);

		auto nonce = blocks["nonce"];
		block.setNonce(nonce);

		auto blockId = blocks["blockid"];
		block.setBlockId(blockId.get<string>());

		auto prevBlockId = blocks["previousblockid"];
		block.setPrevBlockId(prevBlockId.get<string>());

		auto root = blocks["merkleroot"];
		block.setMerkleRoot(root.get<string>());

		auto nTx = blocks["nTx"];
		block.setNTx(nTx);

		//Transactions
		auto arrayTrans = blocks["tx"];
		vector <Transaction> auxTrans;
		for (unsigned i = 0; i < arrayTrans.size(); i++)
		{
			saveTx(arrayTrans[i].dump(), auxTrans);
		}

		block.setTransactions(auxTrans);

		blockchain.push_back(block);
	}
}


void jsonHandler::saveTx(string _trans, vector<Transaction>& txs)
{
	json trans;
	Transaction tx;
	trans = json::parse(_trans);
	auto txId = trans["txid"];
	tx.txId = txId.get<string>();
	auto nTxIn = trans["nTxin"];
	tx.nTxIn = nTxIn;
	auto vIn = trans["vin"];
	for (auto& elsi : vIn) {
		Vin auxVin;
		auto tBlockId = elsi["blockid"];
		auxVin.blockId = tBlockId.get<string>();
		auto tTxId = elsi["txid"];
		auxVin.txId = tTxId.get<string>();
		auto sign = elsi["signature"];
		auxVin.signature = sign.get<string>();
		auto nut = elsi["nutxo"];
		auxVin.nutxo = nut;
		tx.vIn.push_back(auxVin);
	}
	auto nTxOut = trans["nTxout"];
	tx.nTxOut = nTxOut;
	auto vOut = trans["vout"];
	for (auto& elso : vOut) {
		Vout auxVout;
		auto publicId = elso["publicid"];
		auxVout.publicId = publicId.get<string>();
		auto amount = elso["amount"];
		auxVout.amount = amount;
		tx.vOut.push_back(auxVout);
	}
	txs.push_back(tx);
}

void jsonHandler::saveMerkleBlock(string _merkleBlock, vector<MerkleBlock>& mrkl)
{
	MerkleBlock mBlock;
	json merkleBlock = json::parse(_merkleBlock);

	auto blockId = merkleBlock["blockid"];
	mBlock.blockId = blockId.get<string>();

	auto mPath = merkleBlock["merklePath"];
	for (auto& elsi : mPath) {
		auto Id = elsi["Id"];
		mBlock.merklePath.push_back(Id.get<string>());
	}
	//Transactions
	auto trans = merkleBlock["tx"];
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

	mBlock.tx.push_back(auxTrans);

	auto txPos = merkleBlock["txPos"];
	mBlock.txPos = txPos;


	mrkl.push_back(mBlock);
}


Filter_tt jsonHandler::saveFilter(string filter)
{
	Filter_tt aux;
	json filt = json::parse(filter);
	aux.publicID = filt["Id"].get<string>();
	aux.ip = crackIp(filt["ip"].get<string>());
	aux.port = filt["port"];

	return aux;

}

Block jsonHandler::saveBlockHeader(string header)
{
	Block blck;
	json blocks = json::parse(header);

	auto height = blocks["height"];
	blck.setHeight(height);

	auto nonce = blocks["nonce"];
	blck.setNonce(nonce);

	auto blockId = blocks["blockid"];
	blck.setBlockId(blockId.get<string>());

	auto prevBlockId = blocks["previousblockid"];
	blck.setPrevBlockId(prevBlockId.get<string>());

	auto root = blocks["merkleroot"];
	blck.setMerkleRoot(root.get<string>());

	auto nTx = blocks["nTx"];
	blck.setNTx(nTx);

	return blck;
}

/***********************************************************************************
	JSONS's CREATION
***********************************************************************************/
string jsonHandler::createJsonBlockHeader(BlockChain blockChain, string id)
{
	json blck;
	for (int i = 0; i < blockChain.size(); i++)
	{
		blck = json::parse(createJsonBlock(blockChain[i]));
		blck.erase("tx");
	}

	return blck.dump();
}

string jsonHandler::createJsonBlockchain(BlockChain blckchain)
{
	json chain = json::array();
	for (int i = 0; i < blckchain.size(); i++)
	{
		chain[i] = json::parse(createJsonBlock(blckchain[i]));
	}

	return chain.dump();
}

string jsonHandler::createJsonBlock(Block block)
{
	json blck;
	auto tx = json::array();
	for (int i = 0; i < block.getNTx(); i++)
	{
		tx += json::parse(createJsonTx(block.getTransactions()[i]));
	}
	blck["tx"] = tx;
	blck["height"] = block.getHeight();
	blck["nonce"] = block.getNonce();
	blck["blockid"] = block.getBlockID();
	blck["previousblockid"] = block.getPreviousBlockID();
	blck["merkleroot"] = block.getMerkleRoot();
	blck["nTx"] = block.getNTx();

	return blck.dump();
}


string jsonHandler::createJsonTx(Transaction trans)
{
	json tx;
	tx["txid"] = trans.txId;

	tx["nTxin"] = trans.nTxIn;
	auto vin = json::array();
	for (auto i = 0; i < trans.nTxIn; i++)
	{
		vin.push_back(json::object({ {"txid",trans.vIn[i].txId}, {"blockid", trans.vIn[i].blockId},
			{"signature",trans.vIn[i].signature}, {"nutxo", trans.vIn[i].nutxo} }));
	}
	tx["vin"] = vin;

	tx["nTxout"] = trans.nTxOut;
	auto vout = json::array();
	for (auto i = 0; i < trans.nTxOut; i++)
	{
		vout.push_back(json::object({ {"amount",trans.vOut[i].amount}, {"publicid", trans.vOut[i].publicId} }));
	}
	tx["vout"] = vout;

	return tx.dump();
}

string jsonHandler::createJsonMerkle(Block block, Transaction trans)
{
	json merkle;
	merkle["blockid"] = block.getBlockID();
	merkle["tx"] = json::parse(createJsonTx(trans));
	for (int i = 0; i < block.getNTx(); i++)
	{
		if (block.getTransactions()[i].txId == trans.txId)
		{
			merkle["txPos"] = i;
		}
	}
	json mpath = json::array();
	MerkleTree path = block.getMerklePath(trans);
	for (int i = 0; i < path.size(); i++)
	{
		mpath.push_back(json::object({ { "Id",path[i] } }));
	}
	merkle["merklePath"] = mpath;

	return merkle.dump();
}

string jsonHandler::createJsonFilter(Filter_tt filt)
{
	json filter;
	filter["Id"] = filt.publicID;
	filter["port"] = filt.port;
	filter["ip"] = filt.getIPString();

	return filter.dump();
}

string jsonHandler::createJsonFilter(NodeData data)	
{
	json filter;
	filter["Id"] = data.getID();
	filter["port"] = data.getSocket().getPort();
	filter["ip"] = data.getSocket().getIPString();

	return filter.dump();
}

string jsonHandler::createHeader(string id)
{
	string head = "Header:'block_id':" + id;
	return head;
}

string jsonHandler::createJsonOk()
{
	json content = { {"result",true},{"errorCode",nullptr} };
	return content.dump();
}

string jsonHandler::createJsonErr()
{
	json content = { {"result",false},{"errorCode","ERROR"} };
	return content.dump();

}

string jsonHandler::createJsonNotReady()
{
	json cont = { {"status", false} };
	return cont.dump();
}

string jsonHandler::createJsonReady(Layout layout, BlockChain blockchain)
{
	json message;
	message["status"] = true;
	message["layout"] = json::parse(createJsonLayout(layout));
	message["blockchain"] = json::parse(createJsonBlockchain(blockchain));

	return message.dump();

}

/***********************************************************************************
	JSONS's VALIDATION
***********************************************************************************/
errorType jsonHandler::validateBlock(string blck)
{
	errorType err = { true, "Wrong amount of fields/Wrong format" };

	try
	{
		json block = json::parse(blck);

		//Block
		if (block.size() == BLOCK_FIELDS) //Si son 7 elementos
		{
			block.at("height");
			block.at("nonce");
			block.at("blockid");	//Se fija que sean los correspondientes
			block.at("previousblockid");
			block.at("merkleroot");
			int ntx = block.at("nTx");
			block.at("height");
			block.at("tx");

			//Transactions
			auto arrayTrans = block["tx"];

			for (unsigned i = 0; i < arrayTrans.size(); i++)
			{
				err = validateTx(arrayTrans[i].dump());
			}
		}
	}

	catch (std::exception& e)
	{
		err = { true, "Invalid fields" };
	}

	return err;
}


errorType jsonHandler::validateTx(string tx)
{
	errorType err = { true, "Wrong amount of fields/Wrong format" };

	try
	{
		json trans = json::parse(tx);

		if (trans.size() == TRANS_FIELDS)	//Si son 5 elementos
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

				err = { false, "OK Transaction" };
			}
		}
	}

	catch (std::exception& e)
	{
		err = { true, "Invalid fields" };
	}

	return err;
}

errorType jsonHandler::validateFilter(string filter)
{
	errorType err = { true, "Missing field Id/Wrong format" }; //Falta verificar cant de campos

	try
	{
		json fltr = json::parse(filter);
		fltr.at("Id");
		fltr.at("port");
		fltr.at("ip");

		err = { false, "OK Filter" };
	}

	catch (std::exception& e)
	{
		err = { true, "Missing field Id/Wrong format" };
	}

	return err;
}

errorType jsonHandler::validateMerkle(string merkle)
{
	errorType err = { true, "Missing field Id/Wrong format" };

	try
	{
		json mrkl = json::parse(merkle);

		if (mrkl.size() == MRKL_FIELDS)
		{
			mrkl.at("blockid");
			mrkl.at("txPos");
			mrkl.at("tx");
			auto path = mrkl["merklePath"];

			if (!validateTx(mrkl["tx"].dump()).error)
			{
				err = { false, "OK Merkle Block" };
			}

			for (auto& id : path)
			{
				id.at("Id");
			}
		}


	}

	catch (std::exception& e)
	{
		err = { true, "Missing field Id/Wrong format" };
	}

	return err;
}

/***********************************************************************************
	LAYOUT CONTROLLERS
***********************************************************************************/
string jsonHandler::createJsonLayout(Layout& layout)
{
	json lays = json::object();
	for (int i = 0; i < layout.size(); i++)
	{
		lays["nodes"][i] = layout[i].ownData.getID();
		auto kcyo = json::object();
		for (int j = 0; j < layout[i].myNeighbours.size(); j++)
		{
			kcyo += {"target" + to_string(j + 1), layout[i].myNeighbours[j].getID()};
		}
		lays["edges"][i] = kcyo;
	}

	return lays.dump();
}

void jsonHandler::readLayout(string layout, NodeData mySocket, vector <NodeData>& neighbourhood)
{
	json lay = json::parse(layout);
	auto nodes = lay["nodes"];
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].get<string>() == mySocket.getID())
		{
			auto arr = lay["edges"][i];
			for (auto& trjt : arr)
			{
				NodeData n(trjt.get<string>());
				neighbourhood.push_back(n);
			}
		}
	}

}


errorType jsonHandler::validateLayout(string layout)		//Es imposible hacerlo, todos los campos son variables.
{
	errorType err = { false, "OK Filter" };
	return err;
}

void jsonHandler::getNodesInLayout(string path, NodeData ownData, vector<NodeData>& nodes)
{
	ifstream i(path.c_str());
	json j;
	i >> j;

	for (auto& nods : j)
	{
		if (nods["id"].get<string>() != ownData.getID())
		{
			NodeData dat(nods["id"].get<string>(), nods["port"], crackIp(nods["ip"].get<string>()));
			nodes.push_back(dat);
		}
	}
}

/***********************************************************************************
	HACKING METHODS
***********************************************************************************/
ip_t jsonHandler::crackIp(string ip)
{
	ip_t _ip;
	size_t pos1 = ip.find('.');
	_ip.b1 = atoi(ip.substr(0, pos1).c_str());

	size_t pos2 = ip.find('.', pos1 + 1);
	_ip.b2 = atoi(ip.substr(pos1 + 1, pos2).c_str());

	size_t pos3 = ip.find('.', pos2 + 1);
	_ip.b3 = atoi(ip.substr(pos2 + 1, pos3).c_str());

	_ip.b4 = atoi(ip.substr(pos3 + 1).c_str());

	return _ip;
}

string jsonHandler::decipherId(string code)
{
	json _code = json::parse(code);
	auto encryptedId = _code["Id"];
	string decryptedId = encryptedId.get<string>();
	return decryptedId;
}