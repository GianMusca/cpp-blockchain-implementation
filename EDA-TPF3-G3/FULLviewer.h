#pragma once
#include "observer.h"
#include "NODEviewer.h"
#include "FULLNode.h"
#include "TreeWindowHandler.h"
#include "layoutWindowHandler.h"
#include "typeEnums.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class FULLviewer :	public NODEviewer
{
public:
	FULLviewer();
	virtual void update(void*);
	void cycle();
	~FULLviewer();

private:
	//NodeData nodedata;
	//string windowName;
	//tipo_de_nodo type;
	//unsigned long int money;
	const vector<NodeData>* neighbours;
	const Layout* layout;
	const vector<Transaction>* pendingTX;
	const BlockChain* blockchain; //fijarse si no volo todo aca...
	TreeWindowHandler treeHandler;
	layoutWindowHandler layoutHandler;
	//ImFont* numberFont;
	ALLEGRO_BITMAP* nbutton;
	ALLEGRO_BITMAP* pbutton;
	bool redButton;

	void drawWindow();
	//void showNodeType();

						
	/*

	1/3

	Info del FULL para imprimir
	-Data (con la funcion ya hecha printNodeData)
	-Neighbours (usar printNeighbours)
	-TX pendientes (printPendingTX)

	para la ultima, se usa una funcion llamada printTx, de printTXroutine, tambien es tu deber modificarla si algo de ahi no te gusta
	O sea, por lo pronto, es solo llamar a esas 3 funciones, salvo q no te guste el estilo de las ventanas (q es el mismo q en el TP anterior)

	2/3
	ahora, agregarle a drawWindow una llamada a la funcion printBlockList
	esta no la hice, asi q es tu mision hacerla.
	esto es lo q tengo en mente:
		en primer lugar, q tenga una "solapa", al estilo de las otras 3 funciones q mencione mas arriba
		al abrirla, q apareza una lista (por ID o height, o ambas, como:
			1) blablabla
			2) nyehehe
			...
		listando todos los bloques presentes en la blockchain. La idea es q se pueda seleccionar cualquier
		bloque, y al pulsarlo, simplemente tenes q llamar a
			treeHandler.createWindow(bloque en cuestion)
		y la magia del MVC se encarga del resto
		o sea, q sea como botones. no te molestes en preguntarte cuales bloques ya fueron pulsados o no
		indistintamente si ya estan abiertas o no las ventanas de los trees, vos llama a esa funcion, el handler se encarga del resto
	
	Ah, cierto, los bloques se encuentran (asumi ya cargados, aunque pueden ser 0) en
	"blockchain"

	Buena suerte. Cualquier cosa consultame.

	3/3
	Me complace anunciar que los handlers de FULLviewer estan practicamente terminados
	drawWindow ahora deberia hacer una llamada a drawBigRedButton.
	Este deberia tener un boton super copado (solo estoy presumiendo) tal que,
	cuando lo presiones, se llame al layout handler asi:

	layoutHandler.createWindow(nodedata,*layout,*neighbours);

	como siempre, no te tienes q preocupar sobre como funciona exactamente, con tal de q le pases esos 3 parametros (actualizados en el update)
	acaso no molan un monton los handlers? Si pudiera, me casaria con uno...

	bueno ya, creo q no tengo por el momento ninguna otra mision para darte.
	Como te ha estado yendo? no me fije en los avances q hiciste, pero tampoco
	me preguntaste demasiado.
	Bueno, no habia demasiado para preguntar, era cuestion de hacer ventanitas, no?
	Como sea, ya se me hizo tarde, deberia ir volviendo a casa antes de q Mama se enoje conmigo
	Nos vemos temprano mañana!
	(van a tener q definir "temprano" rigurosamente...)

	*/

	//void printNodeData();
	void printNeighbours();
	void printPendingTX();	
	void printBlockList(); 
	void drawBigRedButton(); 

//	void printVin(int i);	//DO NOT ERASE (YET)
//	void printVout(int i);
};

