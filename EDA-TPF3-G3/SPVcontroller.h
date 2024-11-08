#pragma once
#include "observer.h"

#include "SPVNode.h"
#include "WarningWindowHandler.h"
#include "drawTXHandler.h"

enum SPVcontrolState { SPV_MENU, SPV_MTX, SPV_PFILTER, SPV_CHANGEFN, SPV_CHANGEHN };

class SPVcontroller :
	public Observer
{
public:
	SPVcontroller(SPVNode* model) : snode(model), windowID(model->getData().getID()),
		warningHandler(model->getData().getID()) {
		cstate = SPV_MENU;
		//currTX = 1;
	}
	virtual void update(void*);
	void cycle();

private:
	void drawWindow(); 
	void drawMTX();
	//bool drawV(Vout&, int);
	void drawPFilter();
	void drawChangeFN();
	void drawChangeHN();
	void newPortSelect();
	void newIpSelect();
	void returnButton();
	//bool isAllDigits(string);


	//Mismas indicaciones q drawWindow de FULLcontroller, pero en este caso accede a:
	/*
	errorType makeTX(const vector<Vout>& receivers);
	errorType postFilter();
	errorType changeFilterNode(NodeData FilterNode);
	errorType changeHeaderNode(NodeData HeaderNode);
	*/
	int cstate;
	//int currTX;
	//vector<string> auxstr;
	//vector<Vout> txVout;
	//string feeStr;
	//bool errfee;
	string newID;
	int newPort;
	int newIP[4];
	SPVNode* snode;
	string windowID;
	WarningWindowHandler warningHandler;
	drawTXHandler TXHandler;

};

