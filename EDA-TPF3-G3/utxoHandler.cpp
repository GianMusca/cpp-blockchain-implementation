#include "utxoHandler.h"
#include "cryptoHandler.h"
#include <iostream>

#define BONUS 50 //cuanto gana el minero por minar (ademas del resto de cada tx)

utxoHandler::utxoHandler(tipo_de_nodo tipo,BlockChain* blockChain, vector<Transaction>* txs)
	: blockChain(blockChain), txs(txs), utxoList(), processingTxList(),
	tipo(tipo), miningBlock(nullptr)
{
}

void utxoHandler::initializeUtxo()
{
	if (tipo != NODO_SPV) {
		for (size_t blockIndex = 0; blockIndex < blockChain->size(); blockIndex++) { //por cada bloque

			Block& currBlock = (*blockChain)[blockIndex];

			for (size_t txIndex = 0; txIndex < currBlock.getNTx(); txIndex++) { //Por cada TX

				Transaction currTx = currBlock.getTx(txIndex);

				for (size_t vinIndex = 0; vinIndex < currTx.nTxIn; vinIndex++) { //por cada Vin ...
					eraseUtxo(currTx.vIn[vinIndex]);								//... elimina un utxo
				}
				for (size_t voutIndex = 0; voutIndex < currTx.nTxOut; voutIndex++) { //y por cada Vout...
					addUtxo(currBlock, currTx, voutIndex);								//... agrega un utxo
				}

			}

		}
	}
}

longN utxoHandler::balance(string publicKey)
{
	longN money = 0;
	for (size_t i = 0; i < utxoList.size(); i++) {
		if (utxoList[i].ownerID == publicKey) {
			money += utxoList[i].amount;
		}
	}
	return money;
}

bool utxoHandler::createTX(string myPublicKey, const vector<Vout>& receivers, Transaction& tx, longN fee)
{
	bool validez = true;

	tx.vOut = receivers;
	tx.nTxOut = receivers.size();

	//Dinero que se destina a los vouts
	longN neededMoney = 0;
	for (size_t i = 0; i < tx.nTxOut; i++)
		neededMoney += tx.vOut[i].amount;
	neededMoney += fee;

	//Dinero disponible
	longN availableMoney = balance(myPublicKey);

	if (availableMoney < neededMoney) {
		validez = false;
	}

	//busco los indices de todas las utxo que tengo disponibles
	vector<size_t> myUtxoIndex = {};
	for (size_t i = 0; i < utxoList.size(); i++) {
		if (utxoList[i].ownerID == myPublicKey) {
			myUtxoIndex.push_back(i);
		}
	}

	//coloco los vins necesarios para llegar al monto
	if (validez == true) {
		longN insertedMoney = 0;

		while (insertedMoney < neededMoney && myUtxoIndex.size()>0) {
			tx.vIn.emplace_back(utxo2vin(myUtxoIndex.back()));
			insertedMoney += utxoList[myUtxoIndex.back()].amount;
			myUtxoIndex.pop_back();
		}//NOTA!!! Si exploto aca el programa, avisarle a Alex

		if (insertedMoney < neededMoney) {
			validez = false;
			std::cout << endl << endl
				<< "***********************************************" << endl
				<< "*          PROBLEMAS:                         *" << endl
				<< "*                                             *" << endl
				<< "* Aunque dije que si, la cantidad de utxo que *" << endl
				<< "* estan disponibles para esta ID NO ALCANZA   *" << endl
				<< "* para pagar a los destinatarios. Lo siento.  *" << endl
				<< "* FAVOR DE DESPERTAR A ALEX                   *" << endl
				<< "***********************************************" << endl
				<< endl << endl;
		}
		else if (insertedMoney > neededMoney) {
			Vout diferencia;
			diferencia.publicId = myPublicKey;
			diferencia.amount = insertedMoney - neededMoney;
			tx.vOut.emplace_back(diferencia);
		}


	}
	else {
		while (myUtxoIndex.size() > 0) {
			tx.vIn.emplace_back(utxo2vin(myUtxoIndex.back()));
			myUtxoIndex.pop_back();
		}
	}

	return validez;
}

//se fija en el txid
bool utxoHandler::TxExistAlready(Transaction& tx)
{
	for (size_t i = 0; i < txs->size(); i++) {
		if ((*txs)[i].txId == tx.txId) {
			return true;
		}
	}
	return false;
}

errorType utxoHandler::validateTX(Transaction& tx)
{
	errorType rta;
	rta.datos = "";
	rta.error = false;
	size_t index = 0;
	vector<size_t> utxoIndex = {};
	

	//Verifica que todos los miembros de Vin referencien a una UTXO
	for (size_t i = 0;rta.error==false && i < tx.nTxIn; i++) {
		if (vinRefersToUtxo(tx.vIn[i],index) == false) {
			utxoIndex.clear();
			rta.error = true;
			rta.datos = "TX invalida:\nElemento de Vin no referencia a UTXO";
		}
		else {
			utxoIndex.push_back(index);
		}
	}

	/*{
	//AQUI CORROBORAR Q NO SE USE UNA UTXO 2 VECES!!!
	}*/

	//Verifica que no se exceda el dinero disponible de los Vins
	if (rta.error == false) {
		longN income = 0;
		longN outcome = 0;

		//Suma de los montos del vin
		while (utxoIndex.size() > 0) {
			income += utxoList[utxoIndex.back()].amount;
			utxoIndex.pop_back();
		}
		for (size_t i = 0; i < tx.nTxOut; i++) {
			outcome += tx.vOut[i].amount;
		}

		if (income < outcome) {
			rta.error = true;
			rta.datos = "TX invalida:\nEl dinero entregado a los Vout excede el disponible en los Vin";
		}

	}

	return rta;
}

errorType utxoHandler::insertTX(Transaction& tx)
{
	errorType err;
	err.error = false;
	err.datos = "";
	size_t index = 0;
	for (size_t i = 0; err.error==false&& i < tx.nTxIn; i++) {
		if (vinRefersToUtxo(tx.vIn[i], index)) {
			processingTxList.emplace_back(utxoList[index]);
			utxoList.erase(utxoList.begin() + index);
		}
		else {
			err.error = true;
			err.datos = "FATAL ERROR:\nSe intento ingresar una TX que no cumple con la prueba de validacion\nUno de nosotros 4 no esta haciendo su trabajo...";
		}
	}

	if (tipo != NODO_SPV && err.error == false) {
		txs->emplace_back(tx);
	}

	return err;
}

bool utxoHandler::BlockExistAlready(Block& block)
{
	if (blockChain->size() >= block.getHeight()) {
		return true;
	}
	return false;
}

errorType utxoHandler::validateBlock(Block& block)
{
	errorType err;
	err.datos = "";
	err.error = false;

	size_t aux = 0;

	vector<size_t> utxoIndex = {};
	vector<size_t> procIndex = {};


	//verifica el height adecuado
	if (block.getHeight() != 1 + blockChain->size()) {
		err.error = true;
		err.datos = "Bloque invalido:\nEl height del bloque no corresponde con el esperado";
	}

	//Verifica q en cada TX, los Vins refieran a UTXOs o a PROCESSINGTX
	for (size_t txIndex = 0; err.error == false && txIndex < block.getNTx(); txIndex++) {
		Transaction currTx = block.getTx(txIndex);
		for (size_t vinIndex = 0; err.error == false && vinIndex < currTx.nTxIn; vinIndex++) {
			if (vinRefersToProcessing(currTx.vIn[vinIndex], aux)) {
				procIndex.push_back(aux);
			}
			else if (vinRefersToUtxo(currTx.vIn[vinIndex], aux)) {
				utxoIndex.push_back(aux);
			}
			else {
				err.error = true;
				err.datos = "Bloque invalido:\nUn miembro de Vin refiere a una TX no presente en la lista de UTXO ni en la lista de Processing TX";
			}
		}
	}

	longN income = 0;
	longN outcome = 0;

	if (err.error == false) {
		while (procIndex.size() > 0) {
			income += processingTxList[procIndex.back()].amount;
			procIndex.pop_back();
		}
		while (utxoIndex.size() > 0) {
			income += utxoList[utxoIndex.back()].amount;
			utxoIndex.pop_back();
		}

		for (size_t txIndex = 0; txIndex < block.getNTx(); txIndex++) {
			Transaction currTx = block.getTx(txIndex);
			for (size_t voutIndex = 0; voutIndex < currTx.nTxOut; voutIndex++) {
				outcome += currTx.vOut[voutIndex].amount;
			}
		}

		if (income + BONUS != outcome) {
			err.error = true;
			err.datos = "Bloque invalido:\nEl dinero proveniente de los Vins (+ 50) no coincide con el destinado a los Vouts";
		}
	}

	return err;
}

errorType utxoHandler::insertBlock(Block& block)
{
	errorType err;
	err.error = false;
	err.datos = "";

	size_t aux = 0;

	//ACTUALIZO MI LISTA INTERNA DE UTXO Y PROCESSING
	for (size_t txIndex = 0;
		err.error==false && txIndex < block.getNTx();
		txIndex++) {

		Transaction currTx = block.getTx(txIndex);

		for (size_t vinIndex = 0;
			err.error == false && vinIndex < currTx.nTxIn;
			vinIndex++) {

			if (vinRefersToProcessing(currTx.vIn[vinIndex], aux)) {
				processingTxList.erase(processingTxList.begin() + aux);
			}
			else if (vinRefersToUtxo(currTx.vIn[vinIndex], aux)) {
				utxoList.erase(utxoList.begin() + aux);
			}
			else {
				err.error = true;
				err.datos = "FATAL ERROR:\nSe intento ingresar un Bloque con un TX sin respaldo en utxo o processing tx\nSon las 2:30 AM de la mañana, y mi computadora parece tener mas ganas de dormir que yo";
			}

		}
	}

	//AGREGO NUEVAS UTXO
	for (size_t txIndex = 0;
		err.error == false && txIndex < block.getNTx();
		txIndex++) {

		Transaction currTx = block.getTx(txIndex);

		for (size_t voutIndex = 0;
			voutIndex < currTx.nTxOut;
			voutIndex++){
			
			addUtxo(block, currTx, voutIndex);
		}
	}

	//ELIMINO TX QUE FUERON INGRESADAS DE LA LISTA TXS
	if (err.error == false) {
		size_t index = 0;
		for (int i = 0; i < block.getNTx(); i++) {
			Transaction currTx = block.getTx(i);
			if (foundInTXs(currTx, index)) {
				txs->erase(txs->begin() + index);
			}
		}
	}


	if (err.error == false)
		blockChain->emplace_back(block);


	return err;
}

string utxoHandler::getOwner(Vin& vin)
{
	string owner;
	if (tipo != NODO_SPV) {
		size_t aux = 0;

		Vout original;
		if (foundInBlockChain(vin, original)) {
			owner = original.publicId;
		}
	}
	return owner;
}

void utxoHandler::setMiningBlock(Block* miningBlock)
{
	this->miningBlock = miningBlock;
}

void utxoHandler::startNewMiningBlock(string myPublicId, cryptoHandler& cryptohandler)
{
	if (checkMiner() == false)
		return;

	//ORDENAR LAS TX Y METERLAS

	vector<Transaction> enterTX = *txs;
	longN fee = 50 + getDifference(enterTX);

	Vout myFee;
	myFee.publicId = myPublicId;
	myFee.amount = fee;

	Transaction firstOne;
	firstOne.nTxIn = 0;
	firstOne.vIn.clear();
	firstOne.nTxOut = 1;
	firstOne.vOut.clear();
	firstOne.vOut.emplace_back(myFee);
	cryptohandler.hashTx(firstOne);

	enterTX.emplace(enterTX.begin(), firstOne);


	miningBlock->setTransactions(enterTX);
	miningBlock->setHeight(blockChain->size() + 1);
	miningBlock->setNTx(txs->size());
	miningBlock->setPrevBlockId((*blockChain).back().getBlockID());

}

void utxoHandler::processHeader(MerkleBlock& merkle, unsigned long int height, string myPublicId)
{
	if (tipo == NODO_SPV) {
		Transaction tx = merkle.tx[0];

		//elimina
		size_t aux = 0;
		for (int i = 0; i < tx.vIn.size(); i++) {
			if (vinRefersToProcessing(tx.vIn[i], aux)) {
				processingTxList.erase(processingTxList.begin() + aux);
			}
		}
		for (int i = 0; i < tx.vOut.size(); i++) {
			if (tx.vOut[i].publicId == myPublicId) {
				addUtxo(merkle.blockId, height, merkle.tx[0], i);
			}
		}

	}
}

bool utxoHandler::checkMiner()
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

bool utxoHandler::vinRefersToUtxo(Vin& vin,size_t& index)
{
	return findUtxo(vin.txId, vin.nutxo, index);
}

bool utxoHandler::findUtxo(const string& id,int nutxo, size_t& indexInList)
{
	for (size_t i = 0; i < utxoList.size(); i++) {
		if (utxoList[i].txID == id && utxoList[i].nutxo == nutxo) {
			indexInList = i;
			return true;
		}
	}
	return false;
}

bool utxoHandler::vinRefersToProcessing(Vin& vin, size_t& index)
{
	return findProcessing(vin.txId, vin.nutxo, index);
}

bool utxoHandler::findProcessing(const string& id, int nutxo, size_t& indexInList)
{
	for (size_t i = 0; i < processingTxList.size(); i++) {
		if (processingTxList[i].txID == id && processingTxList[i].nutxo == nutxo) {
			indexInList = i;
			return true;
		}
	}
	return false;
}

void utxoHandler::addUtxo(string& blockID, longN height, Transaction& tx, size_t voutIndex)
{
	utxo newUtxo;
	newUtxo.blockID = blockID;
	newUtxo.blockHeight =height;
	newUtxo.txID = tx.txId;
	newUtxo.nutxo = voutIndex + 1;
	newUtxo.ownerID = tx.vOut[voutIndex].publicId;
	newUtxo.amount = tx.vOut[voutIndex].amount;

	utxoList.emplace_back(newUtxo);
}

void utxoHandler::addUtxo(Block& block, Transaction& tx,size_t voutIndex)
{
	string blockID = block.getBlockID();
	longN height = block.getHeight();
	addUtxo(blockID,height,tx,voutIndex);
}

void utxoHandler::eraseUtxo(Vin& vin)
{
	size_t index;
	if (vinRefersToUtxo(vin, index)) {
		utxoList.erase(utxoList.begin() + index);
	}
	else {
		std::cout << endl<<endl
			<<"***********************************************" <<endl
			<<"*          PROBLEMAS:                         *" <<endl
			<<"*                                             *" << endl
			<<"* El nodo recibio un blockChain no valido     *" << endl
			<<"* (los Vins no corresponden con utxos)        *" << endl
			<<"***********************************************" << endl
			<< endl << endl;
	}
}

Vin utxoHandler::utxo2vin(size_t index)
{
	Vin vin;
	if (index >= utxoList.size()) {
		std::cout << endl << endl
			<< "***********************************************" << endl
			<< "*          PROBLEMAS:                         *" << endl
			<< "*                                             *" << endl
			<< "* Se intento transformar un utxo con un indice*" << endl
			<< "* que excede el tamaño de la lista. Woops     *" << endl
			<< "* Comunicarse con Alex, aunque sean las 5 AM  *" << endl
			<< "***********************************************" << endl
			<< endl << endl;
	}
	else {
		utxo& currUtxo = utxoList[index];
		vin.blockId = currUtxo.blockID;
		vin.txId = currUtxo.txID;
		vin.nutxo = currUtxo.nutxo;
	}

	return vin;
}

bool utxoHandler::foundInBlockChain(Vin& vin, Vout& answer)
{
	bool rta = false;
	for (int i = blockChain->size() - 1;rta == false && i >= 0; i--) {//por cada bloque
		if (vin.blockId == (*blockChain)[i].getBlockID()) {
			Block& block = (*blockChain)[i];
			for (int j = 0; rta == false && j < block.getNTx(); j++) {//por cada tx
				if (vin.txId == block.getTx(j).txId) {
					Transaction tx = block.getTx(j);
					if (vin.nutxo > tx.vOut.size()) {
						cout << "ERROR: busque el vout que dio origen a esta vin, encontre el bloque y la tx, pero esta ultima no tiene tantos miembros como exige el nutxo" << endl;
						break;
					}
					else {
						rta = true;
						answer = tx.vOut[vin.nutxo - 1];
					}
				}
			}
			if (rta == false) {
				cout << "ERROR: busque el vout que dio origen a esta vin, encontre el bloque pero no la tx" << endl;
				break; //encontro el bloque, pero no existia esa tx
			}
		}
	}
	return rta;
}

bool utxoHandler::foundInTXs(Transaction& tx, size_t& index)
{
	bool found = false;

	for (int i = 0; found == false && i < txs->size(); i++) {
		if (tx.txId == (*txs)[i].txId) {
			found = true;
			index = i;
		}
	}
	return found;
}

longN utxoHandler::getMoneyFromVin(Vin& vin)
{
	size_t aux = 0;
	longN money = 0;

	if (vinRefersToProcessing(vin, aux)) {
		money = processingTxList[aux].amount;
	}
	else if (vinRefersToUtxo(vin, aux)) {
		money = utxoList[aux].amount;
	}
	else {
		Vout source;
		if (foundInBlockChain(vin, source)) {
			money = source.amount;
		}
		else {
			cout << "WARNING!! Esto no debio haber pasado la prueba de validacion..." << endl;
		}
	}


	return money;
}

longN utxoHandler::getDifference(vector<Transaction>& enterTX)
{
	longN income = 0;
	longN outcome = 0;

	for (int i = 0; i < enterTX.size(); i++) {
		for (int j = 0; j < enterTX[i].vOut.size(); j++) {
			outcome += enterTX[i].vOut[j].amount;
		}
		for (int k = 0; k < enterTX[i].vIn.size(); k++) {
			income += getMoneyFromVin(enterTX[i].vIn[k]);
		}

	}

	return income - outcome;
}
