#include "NETcontroller.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#define FIRST	0
#define LAST	2

typedef enum errSPVStates{NONE, SAME_NODE, NO_ID};

#define WINDOW_NAME "EDA Coins Net"

#define CLEAN(vector)	for(int i=0;i<(vector).size();i++) delete (vector)[i]
#define CYCLE(vector)	for(int i=0;i<(vector).size();i++) (vector)[i]->cycle()

NETcontroller::NETcontroller(EDAcoinsNet* model) :
	netmodel(model), myWindowName(WINDOW_NAME),
	netviewer(WINDOW_NAME), running(true),
	guiHandler(), warningHandler(WINDOW_NAME),
	FcontrolList(), FviewList(),
	ScontrolList(), SviewList(),
	McontrolList(), MviewList(),
	network_created(false)
{
	netmodel->attach(netviewer);
	netmodel->attach(*this);
	findFullNames();
	for (int i = 0; i < 4; i++)
		IParr[i] = 0;
	type = 0;
	nodePort = 0;

	makeFirstFULL();
}

NETcontroller::~NETcontroller(){
	CLEAN(FcontrolList);
	CLEAN(FviewList);
	CLEAN(ScontrolList);
	CLEAN(SviewList);
	CLEAN(McontrolList);
	CLEAN(MviewList);
}

void NETcontroller::update(void*)
{
	size_t index = FcontrolList.size();
	if (netmodel->getFULLamount() > index) {
		FULLNode* newNode = netmodel->getFULLnode(index);

		FULLcontroller* newController = new FULLcontroller(newNode);
		FULLviewer* newViewer = new FULLviewer();

		newNode->attach(*newController);
		newNode->attach(*newViewer);

		FcontrolList.emplace_back(newController);
		FviewList.emplace_back(newViewer);

		newViewer->update(newNode);
	}

	index = ScontrolList.size();
	if (netmodel->getSPVamount() > index) {
		SPVNode* newNode = netmodel->getSPVnode(index);

		SPVcontroller* newController = new SPVcontroller(newNode);
		SPVviewer* newViewer = new SPVviewer();

		newNode->attach(*newController);
		newNode->attach(*newViewer);

		ScontrolList.emplace_back(newController);
		SviewList.emplace_back(newViewer);

		newViewer->update(newNode);
	}

	index = McontrolList.size();
	if (netmodel->getMINERamount() > index) {
		MINERNode* newNode = netmodel->getMINERnode(index);

		FULLcontroller* newController = new FULLcontroller(newNode);
		FULLviewer* newViewer = new FULLviewer();

		newNode->attach(*newController);
		newNode->attach(*newViewer);

		McontrolList.emplace_back(newController);
		MviewList.emplace_back(newViewer);

		newViewer->update(newNode);
	}
}


bool NETcontroller::isRunning()
{
	return running;
}

void NETcontroller::cycle()
{

	guiHandler.start_frame(running);

	//ALEX.EXE
	netmodel->cycle();

	if (network_created == false) {
		checkIfDoneConnecting();
		waitWindow();
	}
	else {

		drawControlWindow();
		netviewer.cycle();

		CYCLE(FcontrolList);
		CYCLE(FviewList);
		CYCLE(ScontrolList);
		CYCLE(SviewList);
		CYCLE(McontrolList);
		CYCLE(MviewList);

		warningHandler.draw();
	}

	guiHandler.end_frame();
}

//Actualizate Oli, ponte a la onda!!!
void NETcontroller::drawControlWindow() {
	ImGui::Begin(myWindowName.c_str());
	ImGui::Text("Elija el tipo de nodo que desee crear");
	
	ImGui::RadioButton("FULL Node", &type, 1); ImGui::SameLine();
	ImGui::RadioButton("MINER Node", &type, 2); ImGui::SameLine();
	ImGui::RadioButton("SPV Node", &type, 0); ImGui::SameLine();

	//ImGui::SetNextItemWidth(250);
	//ImGui::InputText("Node ID", &IDname);

	ImGui::SetNextItemWidth(50);
	ImGui::DragInt("Port", &nodePort, 0.5);

	ImGui::PushItemWidth(25);
	ImGui::DragInt(".##1", IParr, 0.5);
	ImGui::SameLine();
	ImGui::DragInt(".##2", IParr + 1, 0.5);
	ImGui::SameLine();
	ImGui::DragInt(".##3", IParr + 2, 0.5);
	ImGui::SameLine();
	ImGui::DragInt("Node IP##4", IParr + 3, 0.5);
	ImGui::PopItemWidth();

	if (type) {
		if (type == 1) {
			if (ImGui::Button("Create Full Node"))
				warningHandler.check(netmodel->createFULLNode(Socket((uint)nodePort, (uint)IParr[0], (uint)IParr[1], (uint)IParr[2], (uint)IParr[3])));
		}
		else {
			if (ImGui::Button("Create Miner Node"))
				warningHandler.check(netmodel->createMINERNode(Socket((uint)nodePort, (uint)IParr[0], (uint)IParr[1], (uint)IParr[2], (uint)IParr[3])));
		}
	}
	else {
		findFullNames();
		ImGui::Combo("Seleccione el nodo FULL vecino al que se le postearan los filters", &currFilter, FullNames.c_str());
		ImGui::Combo("Seleccione el nodo FULL vecino al que se le pediran los merkle blocks", &currHeader, FullNames.c_str());
		if (ImGui::Button("Create SPV Node")) {
			errSPV = NONE;
			//if (IDname.size() != 0) {
				if (currFilter != currHeader)
					warningHandler.check(netmodel->createSPVNode(Socket(nodePort, IParr[0], IParr[1], IParr[2], IParr[3]), netmodel->getFULLnode(currFilter)->getData(), netmodel->getFULLnode(currHeader)->getData()));
				else
					errSPV = SAME_NODE;
			//}
			//else
			//	errSPV = NO_ID;
		}
		if(errSPV == SAME_NODE)
			ImGui::Text("No puede elegir el mismo nodo para ambos campos");
		//else if(errSPV == NO_ID)
		//	ImGui::Text("Por favor complete el campo Node ID");

	}

	ImGui::End();
}

void NETcontroller::findFullNames() {
	FullNames.clear();
	for (int i = 0; i < netmodel->getFULLamount(); i++) {
		FullNames += netmodel->getFULLnode(i)->getData().getID();
		FullNames += '\0';
	}
	FullNames += '\0';
}

void NETcontroller::makeFirstFULL()
{
	vector<NodeData> genesisNodes = {};
	NodeData dummyNode("NO_DATA");

	jsonhandler.getNodesInLayout("manifest.json", dummyNode, genesisNodes);

	for (size_t i = FIRST; i <= LAST; i++) {
		netmodel->createFULLNode(genesisNodes[i].getSocket());
	}

	for (size_t i = 0; i < genesisNodes.size(); i++) {
		netmodel->insertKnownFULLdata(genesisNodes[i]);
	}
}

void NETcontroller::checkIfDoneConnecting()
{
	if (netmodel->checkIfConnectionMade())
		network_created = true;
}

void NETcontroller::waitWindow()
{
	ImGui::Begin("CREATING INITIAL LAYOUT...");

	ImGui::Text("Please, wait until the initial connection has been made");
	ImGui::Text("Oh... viste ese manejo del english, haha, beri biutiful");
	ImGui::Text("Haha... igual, si llegaste hasta aca y no se rompio, tremendo, no? Porque, segun yo, ni a palos funciona esto antes de las 10 de la noche");
	ImGui::Text("EN EDA ANDA TODO!!!");
	ImGui::Text("che, vamos a quitar este mensaje antes de la entrega, no?");
	ImGui::End();
}




