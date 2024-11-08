#pragma once

//       \\\\\           \\\\\           \\\\\           \\\\\    
//      \\\\\\\__o      \\\\\\\__o      \\\\\\\__o      \\\\\\\__o
//______\\\\\\\'/_______\\\\\\\'/_______\\\\\\\'/_______\\\\\\\'/_
//toda la clase

#include "string"


//#include "dll.h"


#include "cryptopp/cryptlib.h"
#include "cryptopp/osrng.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/oids.h"
#include "cryptopp/hex.h"
#include "cryptopp/sha3.h"
#include <string>

#include "blockChain.h"

#include "utxoHandler.h"

#include "typeEnums.h"

using namespace std;
using namespace CryptoPP;

class utxoHandler;

class cryptoHandler
{
public:
	cryptoHandler(tipo_de_nodo tipo); //genera private y public keys
	string getMyPrivateKey();
	string getMyPublicKey();

	void signAllVinsInTx(Transaction& tx);
	void hashTx(Transaction& tx);
	
	bool verifyTXHash(Transaction& tx);
	bool verifyTXSign(Transaction& tx, utxoHandler* handler);

	bool verifyBlockHash(Block& block);
	bool verifyBlockSign(Block& block, utxoHandler* handler);

	string signMessage(string& message);
	bool isSignValid(string& message, string& pubKey, string& sign);
	string hashMessage(string& message);
	bool isHashValid(string& message, string& hash);

	//SOLO MINERO
	void setMiningBlock(Block* miningBlock);
	void tryNewNonce(); //probar una vez por cycle. Fijarse si cumple challenge con otra funcion
	
private:
	tipo_de_nodo tipo;
	Block* miningBlock;
	bool checkMiner();

	void hashBlock(Block& block);


	string makeHashFromTx(Transaction& tx);
	string concatenateVout(Transaction& tx);

	string makeHashFromBlock(Block& b);

	byte header[24];
	ECDSA<ECP, SHA256>::PrivateKey myprivateKey;
	ECDSA<ECP, SHA256>::PublicKey mypublicKey;
};

