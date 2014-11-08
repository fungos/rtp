#include "algorithmcopy.h"
#include <QImage>

bool AlgorithmCopy::setup(QImage *input, QImage *, DistanceFunction)
{
	delete pCurrent;
	pCurrent = new QImage(*input);

	emit finished(pCurrent);

	return true;
}

bool AlgorithmCopy::update()
{
	return bFinished = true;
}
