#pragma once
#include "observer.h"
#include "drawTXHandler.h"
#include "FULLNode.h"
#include "MINERNode.h"
#include "WarningWindowHandler.h"

#define MAX_TX_ID	280
enum FULLcontrolState { FULL_MENU, FULL_MTX, FULL_MBLOCK, FULL_ADDN };

class FULLcontroller : public Observer
{
public:
	FULLcontroller(FULLNode* model) : fnode(model), windowID(model->getData().getID()),
		warningHandler(model->getData().getID()) {
		//currTX = 1;
		cstate = FULL_MENU;
	}
	virtual void update(void*);
	void cycle();

private:
	virtual void drawWindow();
	void drawMBlock();
	void drawMTX();
	void drawAddNode();
	//bool drawV(Vout&, int);
	void newPortSelect();
	void newIpSelect();
	void returnButton();
	//bool isAllDigits(string);

	//void neighbourSelect();
	//const char* findNeighbourNames();
	//Dibuja la ventana con el nombre windowID, y llama a las siguientes funciones
	//del model, cuando corresponda:
	/*
	errorType makeTX(const vector<Vout>& receivers);
	errorType makeBlock(); //"MINAR"
	errorType addNeighbour(NodeData neighbour); //agrega fulls
	*/
	//Recordar hacer warningHandler.check() con los retornos de dichas funciones

	int newPort;
	int newIP[4];
	string newID;
	//bool errfee;
	int cstate;
	//int currTX;
	//string feeStr;
	//vector<string> auxstr;
	//int currNeighbour;
	//vector<Vout> txVout;
	FULLNode* fnode;
	string windowID;
	WarningWindowHandler warningHandler;
	drawTXHandler TXHandler;
};

