#pragma once

#include "blockChain.h"

struct utxo {
	string blockID;
	longN blockHeight; //OJO: comienza en 1
	string txID; //identifica a cada ID
	int nutxo; //OJO: comienza en 1
	
	string ownerID;
	longN amount;
};