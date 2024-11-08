#pragma once

#include "blockChain.h"

class TreeWindow
{
public:
	TreeWindow(const TreeWindow& c);
	TreeWindow(const Block& b,const string& nodeId);
	void replaceBlock(const Block& b, const string& nodeId);

	void draw();
	bool isOpen();
private:
	string windowName;
	bool open;
	Block block;
	MerkleTree tree;

	bool validRoot;
	void validateRoot(void); //solo se fija coincidencia entre el valor del Root especificado en el bloque, y en el especificado dentro del arreglo del tree

	void drawTree(void);
	void drawFloor(const longN totalHeight, const longN floor, const longN uniques, vector<newIDstr>::iterator& i);
};

