#include "cryptoHandler.h"
#include "cryptoFunctions.h"

#include <cstring>
#include <iostream>

#include <cstdlib>
#include <ctime>

using namespace std;

cryptoHandler::cryptoHandler(tipo_de_nodo tipo) : tipo(tipo), miningBlock(nullptr)
{
	//srand(time(NULL));
	ECDSA<ECP, SHA256>::PrivateKey myprivateKey2;
	ECDSA<ECP, SHA256>::PublicKey mypublicKey2;
	myprivateKey2 = generatePrivKey();
	myprivateKey2.MakePublicKey(mypublicKey);

	byte auxBuf[HEADER_CHARS + PUBLIC_KEY_CHARS];
	ArraySink sink(auxBuf, HEADER_CHARS + PUBLIC_KEY_CHARS);
	mypublicKey.Save(sink);

	memcpy(header, auxBuf, HEADER_CHARS);
	
}

string cryptoHandler::getMyPrivateKey()
{
	vector<byte> b = privateKeyToByte(myprivateKey);
	return byteToString(b);
}

string cryptoHandler::getMyPublicKey()
{
	vector<byte> b = publicKeyToByte(mypublicKey);
	return byteToString(b);
}

void cryptoHandler::signAllVinsInTx(Transaction& tx)
{
	string lastPartOfMessage = concatenateVout(tx);

	for (size_t i = 0; i < tx.vIn.size(); i++) {
		Vin& currVin = tx.vIn[i];
		string message = "";
		message += currVin.blockId;
		message += currVin.txId;
		message += to_string(currVin.nutxo);
		message += lastPartOfMessage;

		string signature = signMessage(message);

		currVin.signature = signature;
	}
}

void cryptoHandler::hashTx(Transaction& tx)
{	
	tx.txId = makeHashFromTx(tx);
}

bool cryptoHandler::verifyTXHash(Transaction& tx)
{
	return (tx.txId == makeHashFromTx(tx));
}

bool cryptoHandler::verifyTXSign(Transaction& tx,utxoHandler* handler)
{
	bool rta = true;

	string lastPartOfMessage = concatenateVout(tx);

	for (size_t i = 0; rta == true && i < tx.vIn.size(); i++) {
		Vin& currVin = tx.vIn[i];
		string message = "";
		message += currVin.blockId;
		message += currVin.txId;
		message += to_string(currVin.nutxo);
		message += lastPartOfMessage;

		string owner = handler->getOwner(currVin);

		rta = isSignValid(message, owner, currVin.signature);
	}

	return rta;
}

bool cryptoHandler::verifyBlockHash(Block& block)
{
	return (block.getBlockID() == makeHashFromBlock(block));
}

bool cryptoHandler::verifyBlockSign(Block& block, utxoHandler* handler)
{
	bool rta = true;

	for (int i = 0; rta == true && i < block.getNTx(); i++) {
		Transaction tx = block.getTx(i);
		rta = verifyTXSign(tx, handler);
	}

	return rta;
}

string cryptoHandler::signMessage(string& message)
{
	vector<byte> b = getSignature(myprivateKey,message);
	return byteToString(b);
}

bool cryptoHandler::isSignValid(string& message, string& pubKey, string& sign)
{
	return verifySignatureString(header,pubKey,message,sign);
}

string cryptoHandler::hashMessage(string& message)
{
	return hashMessage(message);
}

bool cryptoHandler::isHashValid(string& message, string& hash)
{
	return (hash == hashMessage(message));
}

void cryptoHandler::setMiningBlock(Block* miningBlock)
{
	this->miningBlock = miningBlock;
}

void cryptoHandler::tryNewNonce()
{
	if (checkMiner() == false)
		return;

	uint32_t random = 0;
	for (int i = 0; i < 4; i++)
		random += ((rand() % 256) << (8 * i));

	miningBlock->setNonce(random);
	hashBlock(*miningBlock);
}

void cryptoHandler::hashBlock(Block& block)
{
	block.setBlockId( makeHashFromBlock(block) );
}

bool cryptoHandler::checkMiner()
{
	if (tipo != NODO_MINERO) {
		cout << "ILEGAL: un nodo no minero no puede minar" << endl;
		return false;
	}
	if (miningBlock == nullptr) {
		cout << "ILEGAL: se trato de minar, pero aun no se enlazo con el mining block" << endl;
		return false;
	}

	return true;
}

string cryptoHandler::makeHashFromTx(Transaction& tx)
{
	string message = "";

	message += to_string(tx.nTxIn);

	for (size_t i = 0; i < tx.vIn.size(); i++)
		message += tx.vIn[i].blockId;

	for (size_t i = 0; i < tx.vIn.size(); i++)
		message += tx.vIn[i].txId;

	for (size_t i = 0; i < tx.vIn.size(); i++)
		message += to_string(tx.vIn[i].nutxo);

	for (size_t i = 0; i < tx.vIn.size(); i++)
		message += tx.vIn[i].signature;

	for (size_t i = 0; i < tx.vOut.size(); i++)
		message += tx.vOut[i].publicId;

	for (size_t i = 0; i < tx.vOut.size(); i++)
		message += to_string(tx.vOut[i].amount);

	return hashMessage(message);
}

string cryptoHandler::concatenateVout(Transaction& tx)
{
	string rta = "";

	for (size_t i = 0; i < tx.vOut.size(); i++) {
		rta += tx.vOut[i].publicId;
	}
	for (size_t i = 0; i < tx.vOut.size(); i++) {
		rta += to_string(tx.vOut[i].amount);
	}
	return rta;
}

string cryptoHandler::makeHashFromBlock(Block& block)
{
	string message = "";
	message += block.getPreviousBlockID();
	message += to_string(block.getHeight());
	message += block.getMerkleRoot();
	message += number32ToString(block.getNonce());
	return hashMessage(message);
}
