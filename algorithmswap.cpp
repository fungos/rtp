#include "algorithmswap.h"

#include <QImage>
#include <QColor>
#include <QtConcurrent/QtConcurrent>

bool AlgorithmSwapDistance::update()
{
	for (int i = 0; i < kUpdateSize; i++)
		doStep();

	emit step();

	return false;
}

void AlgorithmSwapDistance::doStep()
{
	QPoint a(qrand() % iInputWidth, qrand() % iInputHeight);
	QPoint b(qrand() % iPaletteWidth, qrand() % iPaletteHeight);

	auto inputA = Pixel(pInput->pixel(a));
	auto inputB = Pixel(pInput->pixel(b));
	auto resultA = Pixel(pCurrent->pixel(a));
	auto resultB = Pixel(pCurrent->pixel(b));

	auto dAA = pDistance(inputA, resultA);
	auto dBB = pDistance(inputB, resultB);
	auto dAB = pDistance(inputA, resultB);
	auto dBA = pDistance(inputB, resultA);
	if (dAA + dBB > dAB + dBA)
	{
		pCurrent->setPixel(a, resultB.c);
		pCurrent->setPixel(b, resultA.c);
	}
}
