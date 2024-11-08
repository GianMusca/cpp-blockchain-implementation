#pragma once
/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <vector>
#include "observer.h"

using namespace std;


/*******************************************************************************
 * CLASS PROTOTYPE
 ******************************************************************************/
class Subject {
public:
	void attach(Observer& o) { observers.push_back(&o); };
	void detach(vector<Observer*>::iterator pos) { observers.erase(pos); };
protected:
	void notifyAllObservers(void* esta) {
		for (int i=0;i<observers.size();i++)
			observers[i]->update(esta);
	}
	vector<Observer*> observers;
};