#include "EDAcoinsNet.h"
#include "NETcontroller.h"

//Turbio
bool FULLNode::isLedaderNode = false;

int main(void) {
	srand(time(NULL));
	EDAcoinsNet network;
	NETcontroller controller(&network);


	while (controller.isRunning())
		controller.cycle();

	return 0;
}