#include "drawTXHandler.h"
#include "imgui.h"
#include "imgui_stdlib.h"

drawTXHandler::drawTXHandler(){
	errfee = false;
	erramount = false;
	currTX = 1;
}

unsigned long int drawTXHandler::getFee() {
	unsigned long int fee = _atoi64(feeStr.c_str());
	return fee;
}

vector<Vout> drawTXHandler::getVout() {
	return txVout;
}

bool drawTXHandler::drawTX() {
	bool r = false;
	ImGui::Text("\nMAKE A TRANSACTION\n\n");

	for (int i = 0; i < currTX; i++) {
		if (txVout.size() == 0) {
			Vout VoutAux;
			txVout.push_back(VoutAux);
			string aux;
			auxstr.push_back(aux);
		}
		if (drawV(txVout[i], i) && (txVout.size() != 1)) {
			currTX--;
			for (int j = i; j < currTX; j++) {
				txVout[j] = txVout[j + 1];
				auxstr[j] = auxstr[j + 1];
			}
			txVout.resize(txVout.size() - 1);
			auxstr.resize(auxstr.size() - 1);
		}
	}

	if (txVout[currTX - 1].publicId.size()) {
		Vout vout;
		string aux;
		currTX++;
		txVout.push_back(vout);
		auxstr.push_back(aux);
	}

	ImGui::Text("Amount of Vouts: %d", currTX - 1);
	ImGui::SetNextItemWidth(50);
	ImGui::InputText("Fee", &feeStr);

	if (ImGui::Button("MAKE TRANSACTION")) {
		errfee = false;
		if (isAllDigits(feeStr)) {
			txVout.pop_back();
			r = true;
			txVout.clear();
			auxstr.clear();
			feeStr.clear();
			currTX = 1;
		}
		else
			errfee = true;
	}

	if (errfee)
		ImGui::Text("Please enter a valid fee");

	return r;
}

bool drawTXHandler::drawV(Vout& vout, int i) {
	bool r = false;

	ImGui::Text("Vout");
	ImGui::InputText(("Public ID##" + to_string(i)).c_str(), &vout.publicId);
	ImGui::SetNextItemWidth(50);
	ImGui::InputText(("Monto##" + to_string(i)).c_str(), &auxstr[i]);

	if (ImGui::Button(("Delete##" + to_string(i)).c_str())) {
		if (isAllDigits(auxstr[i])) {
			vout.amount = _atoi64(auxstr[i].c_str());
			erramount = false;
			r = true;
		}
		else
			erramount = true;
	}

	if (erramount)
		ImGui::Text("Please enter a valid amount");
	ImGui::NewLine();

	return r;
}

bool drawTXHandler::isAllDigits(string s) {
	bool r = true;
	if (!s.size())
		r = false;
	else {
		for (int i = 0; i < s.size(); i++) {
			if (s[i] < '0' || s[i] > '9') {
				r = false;
				break;
			}
		}
	}

	return r;
}