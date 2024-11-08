#include "layoutNumericConection.h"

layoutNumericConection::layoutNumericConection(index myIndex)
	: myself(myIndex), marked(false), conections()
{
}

bool layoutNumericConection::initialConection(layoutNumericConection& newConection)
{
	if ((isAlreadyConectedToMe(newConection) == false) &&
		(myself != newConection.myself)) {

		forcedConection(newConection);
		initialCheckMark();
		newConection.forcedConection(*this);
		newConection.initialCheckMark();
	}

	return marked;
}

bool layoutNumericConection::initialConection(layoutNumericConection& newConection, size_t& marks)
{
	if ((isAlreadyConectedToMe(newConection) == false) &&
		(myself != newConection.myself)) {

		forcedConection(newConection);
		initialCheckMark(marks);
		newConection.forcedConection(*this);
		newConection.initialCheckMark(marks);
	}

	return marked;
}

bool layoutNumericConection::isMarked()
{
	return marked;
}

void layoutNumericConection::mark()
{
	marked = true;
}

void layoutNumericConection::unmark()
{
	marked = false;
}

const vector<index>& layoutNumericConection::getConections()
{
	return conections;
}

void layoutNumericConection::secondaryConection(layoutNumericConection& newConection)
{
	if ((isAlreadyConectedToMe(newConection) == false) &&
		(myself != newConection.myself)) {

		initialCheckMark();
		newConection.initialCheckMark();
	}
}

void layoutNumericConection::forcedConection(layoutNumericConection& newConection)
{
	conections.emplace_back(newConection.myself);
}

void layoutNumericConection::initialCheckMark()
{
	if (conections.size() >= NUMBER_OF_CONECTIONS)
		marked = true;
}

void layoutNumericConection::initialCheckMark(size_t& marks)
{
	if ((marked == false) &&
		(conections.size() >= NUMBER_OF_CONECTIONS)) {
		marked = true;
		marks++;
	}

}

bool layoutNumericConection::isAlreadyConectedToMe(layoutNumericConection& newConection)
{
	bool answer = false;
	for (int i = 0;
		(answer == false) && (i < conections.size());
		i++) {

		if (conections[i] == newConection.myself)
			answer = true;
	}

	return answer;
}
