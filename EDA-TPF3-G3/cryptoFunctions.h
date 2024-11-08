#pragma once

//       \\\\\           \\\\\           \\\\\           \\\\\    
//      \\\\\\\__o      \\\\\\\__o      \\\\\\\__o      \\\\\\\__o
//______\\\\\\\'/_______\\\\\\\'/_______\\\\\\\'/_______\\\\\\\'/_
//toda la clase

//#include "dll.h"

#include "cryptopp/cryptlib.h"
#include "cryptopp/osrng.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/oids.h"
#include "cryptopp/hex.h"
#include "cryptopp/sha3.h"
#include <string>

#define PRIVATE_KEY_CHARS 32
#define PUBLIC_KEY_CHARS 64
#define HEADER_CHARS 24

using namespace std;
using namespace CryptoPP;

ECDSA<ECP, SHA256>::PrivateKey generatePrivKey();
vector<byte> getSignature(ECDSA<ECP, SHA256>::PrivateKey& privKey, string& data);
bool verifySignature(ECDSA<ECP, SHA256>::PublicKey& pubKey, string& data, vector<byte>& signature);
bool verifySignatureString(byte header[HEADER_CHARS],string &pubKey, string &data, string &signature); //(WIP)

vector<byte> privateKeyToByte(ECDSA<ECP, SHA256>::PrivateKey& privKey);
vector<byte> publicKeyToByte(ECDSA<ECP, SHA256>::PublicKey& pubKey);
string number32ToString(uint32_t number);
string byteToString(vector<byte>& dataToPrint);
void stringToByte(string& dataToPrint, byte output[],int size);

void copyPublicKeys(ECDSA<ECP, SHA256>::PublicKey& original, ECDSA<ECP, SHA256>::PublicKey& copy); //be careful!! (might explode)

string hashMessage(string& message);

//ALE, USA ESTE PARA EL MERKLE TREE Y ROOT
string hash2nodes(string txid1, string txid2);