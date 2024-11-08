#pragma once

#include "TreeWindow.h"
#include "blockChain.h"
#include <map>
#include <string>

struct mapp {
	mapp(unsigned int i, const Block& b, const string& nodeId) :
		orden(i),window(b,nodeId) {}
	mapp(const mapp&m) :
		orden(m.orden), window(m.window) {}
	mapp(unsigned int i,const TreeWindow&  w) : orden(i), window(w) {}
	unsigned int orden;
	TreeWindow window;
};

class TreeWindowHandler
{
public:
	TreeWindowHandler();
	void setId(const string& id);

	void createWindow(const Block& block); // °w° -Estaria bueno hacer const a los getters de Block (no lo q devuelve, sino al metodo en si) asi puedo manejarme con const Blocks!!!! 
	void draw();
private:
	string nodeId;
	//map<unsigned int,TreeWindow> windowList;
	vector<mapp> windowList;
};

