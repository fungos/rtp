#include "ialgorithm.h"
#include <QImage>
#include <assert.h>

IAlgorithm::IAlgorithm()
	: pDistance(rtm_distance)
	, pInput(nullptr)
	, pPalette(nullptr)
	, pCurrent(nullptr)
	, iCount(0)
	, iInputHeight(0)
	, iInputWidth(0)
	, iPaletteHeight(0)
	, iPaletteWidth(0)
	, bFinished(false)
{}

IAlgorithm::~IAlgorithm()
{
	delete pInput;
	delete pPalette;
	delete pCurrent;

	pInput   = nullptr;
	pPalette = nullptr;
	pCurrent = nullptr;
}

bool IAlgorithm::setup(QImage *input, QImage *palette, DistanceFunction func)
{
	pDistance = func;

	if (!input)
		return false;

	if (!palette)
		return false;

	pInput = new QImage(*input);
	pPalette = new QImage(*palette);

	iInputWidth = pInput->width();
	iInputHeight = pInput->height();
	iPaletteWidth = pPalette->width();
	iPaletteHeight = pPalette->height();
	iCount = iInputWidth * iInputHeight;
	assert(iCount == iPaletteHeight * iPaletteWidth);

	delete pCurrent;
	pCurrent = new QImage(pPalette->bits(), iInputWidth, iInputHeight, pInput->bytesPerLine(), pPalette->format());


	bFinished = !(iCount == iPaletteHeight * iPaletteWidth);

	return !bFinished;
}

bool IAlgorithm::process()
{
	if (bFinished)
		return true;

	return this->update();
}
