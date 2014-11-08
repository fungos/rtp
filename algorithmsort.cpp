#include "algorithmsort.h"

#include <qalgorithms.h>
#include <QImage>
#include <QColor>
#include <QtConcurrent/QtConcurrent>

QList<PixelPos> createPixelList(const QImage *img, DistanceFunction func)
{
	auto h = img->height();
	auto w = img->width();

	QList<PixelPos> p;
	p.reserve(h * w);

	for (int i = 0, y = 0 ; y < h; y++)
	{
		for (int x = 0 ; x < w; x++, i++)
		{
			auto px = Pixel(img->pixel(x, y));
			PixelPos e(px, x, y, func);
			p.append(e);
		}
	}

	qSort(p.begin(), p.end(), [](const PixelPos a, const PixelPos b)
	{
		return a.fD < b.fD;
	});

	return std::move(p);
}


PixelPos findNearest(QList<PixelPos> &list, double value)
{
	QList<PixelPos>::iterator it = qLowerBound(list.begin(), list.end(), value, [](const PixelPos px, const double d)
	{
		return px.fD < d;
	});

	auto ret = *it;
	list.erase(it);
	return ret;
}

PixelPos binarySearch(QList<PixelPos> &list, double value, int lower, int higher)
{
	if (lower == higher || higher < 0)
	{
		auto px = list[lower];
		list.removeAt(lower);
		return px;
	}
	else if (lower > higher)
	{
		PixelPos pl;
		double vl = 0;

		auto ph = list[lower];
		double vh = ph.fD;

		if (lower > 0)
		{
			pl = list[lower - 1];
			vl = pl.fD;
		}

		double diffH = std::abs(value - vh);
		double diffL = value - vl;

		if (diffH < diffL)
		{
			list.removeAt(lower);
			return ph;
		}

		list.removeAt(lower - 1);
		return pl;
	}

	int mid = (higher + lower) >> 1;
	auto p = list[mid];

	if (p.fD > value)
		return binarySearch(list, value, lower, mid - 1);
	else if (p.fD < value)
		return binarySearch(list, value, mid + 1, higher);
	else
	{
		list.removeAt(mid);
		return p;
	}
}

AlgorithmSortBase::AlgorithmSortBase()
	: vInput()
	, vPalette()
	, iCurPos(0)
{
}

AlgorithmSortBase::~AlgorithmSortBase()
{
	vInput.clear();
	vPalette.clear();
}

bool AlgorithmSortBase::setup(QImage *input, QImage *palette, DistanceFunction func)
{
	if (!IAlgorithm::setup(input, palette, func))
		return false;

	vInput = createPixelList(pInput, func);
	vPalette = createPixelList(pPalette, func);
	iCurPos = 0;

	return true;
}

bool AlgorithmIndexedReplace::update()
{
	for (int i = 0; i < kUpdateSize && iCurPos < iCount; i++, iCurPos++)
	{
		auto ori = vInput.at(iCurPos);
		auto pal = vPalette.at(iCurPos);
		pCurrent->setPixel(ori.x, ori.y, pal.p.c);
	}

	bFinished = (iCurPos == iCount);

	if (bFinished)
		emit finished(pCurrent);
	else
		emit step();

	return bFinished;
}

bool AlgorithmBisectDistance::update()
{
	for (int i = 0; i < 10 && iCurPos < iCount; i++, iCurPos++)
	{
		auto ori = vInput.at(iCurPos);
		auto pal = binarySearch(vPalette, ori.fD, 0, vPalette.size() - 1);
		pCurrent->setPixel(ori.x, ori.y, pal.p.c);
	}

	bFinished = (iCurPos == iCount);

	if (bFinished)
		emit finished(pCurrent);
	else
		emit step();

	return bFinished;
}

bool AlgorithmBisectDistanceQt::update()
{
	for (int i = 0; i < 10 && iCurPos < iCount; i++, iCurPos++)
	{
		auto ori = vInput.at(iCurPos);
		auto pal = findNearest(vPalette, ori.fD);
		pCurrent->setPixel(ori.x, ori.y, pal.p.c);
	}

	bFinished = (iCurPos == iCount);

	if (bFinished)
		emit finished(pCurrent);
	else
		emit step();

	return bFinished;
}

void AlgorithmBisectDistanceThreaded::doWork(QList<PixelPos> palette, int id)
{
//	auto myid = id;
	auto len = palette.size();
	auto start = id * len;
	auto end = len - 1;
	for (int i = 0; i < len; i++)
	{
		auto ori = vInput.at(start + i);
		//auto pal = findNearest(palette, ori.fD);
		auto pal = binarySearch(palette, ori.fD, 0, end--);
		pCurrent->setPixel(ori.x, ori.y, pal.p.c);
	}
}

bool AlgorithmBisectDistanceThreaded::setup(QImage *input, QImage *palette, DistanceFunction func)
{
	if (!AlgorithmSortBase::setup(input, palette, func))
		return false;

	auto px = pCurrent->pixel(0, 0);
	pCurrent->setPixel(0, 0, px); // copy on write

	iWorkers = QThread::idealThreadCount();
	auto workerSize = iCount / iWorkers;

	mThreadPalette.clear();
	mThreadWorker.clear();
	for (int i = 0; i < iWorkers; i++)
		mThreadPalette.append(vPalette.mid(i * workerSize, workerSize));

	for (int i = 0; i < iWorkers; i++)
		mThreadWorker.append(QtConcurrent::run(this, &AlgorithmBisectDistanceThreaded::doWork, mThreadPalette[i], i));

	return true;
}

bool AlgorithmBisectDistanceThreaded::update()
{
	int count = 0;
	for (int i = 0; i < iWorkers; i++)
	{
		count += mThreadWorker[i].isFinished();
	}

	bFinished = (count == iWorkers);

	if (bFinished)
		emit finished(pCurrent);
	else
		emit step();

	return bFinished;
}
