#pragma once
#include "blockChain.h"

#define CHILD_W	300
#define CHILD_H	330

#define CCHILD_W (CHILD_W-40)
#define CCHILD_H 200

#define VCHILD_W (CCHILD_W-40)
#define VCHILD_H 100



void printTx(const Transaction& tx, int identifier);