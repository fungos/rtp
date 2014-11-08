#ifndef ALGORITHMSORT_H
#define ALGORITHMSORT_H

#include "ialgorithm.h"
#include "pixel.h"
#include <QList>
#include <QVector>
#include <QColor>
#include <QFuture>

struct PixelPos
{
	public:
		PixelPos()
			: p(0)
			, x(0)
			, y(0)
			, fD(0)
		{
		}

		PixelPos(Pixel pc, int px, int py, DistanceFunction func = rtm_distance)
			: p(pc)
			, x(px)
			, y(py)
			, fD(func(pc, empty))
		{
		}

		Pixel p;
		int x;
		int y;
		double fD;
};

class AlgorithmSortBase : public IAlgorithm
{
	public:
		AlgorithmSortBase();
		virtual ~AlgorithmSortBase();

		virtual bool setup(QImage *input, QImage *palette, DistanceFunction func) override;

		QList<PixelPos> vInput;
		QList<PixelPos> vPalette;

		int iCurPos;
};

class AlgorithmIndexedReplace : public AlgorithmSortBase
{
	public:
		virtual bool update() override;
		virtual QString name() override
		{
			return "Indexed Replace";
		}
};

class AlgorithmBisectDistance : public AlgorithmSortBase
{
	public:
		virtual bool update() override;
		virtual QString name() override
		{
			return "Single Bisect";
		}
};

class AlgorithmBisectDistanceQt : public AlgorithmSortBase
{
	public:
		virtual bool update() override;
		virtual QString name() override
		{
			return "Single Bisect (Qt)";
		}
};

class AlgorithmBisectDistanceThreaded : public AlgorithmSortBase
{
	public:
		virtual bool setup(QImage *input, QImage *palette, DistanceFunction func) override;
		virtual bool update() override;
		virtual QString name() override
		{
			return "Threaded Bisect";
		}

		void doWork(QList<PixelPos> palette, int id);

		int iWorkers;
		QVector<QList<PixelPos>> mThreadPalette;
		QVector<QFuture<void>> mThreadWorker;
};

#endif // ALGORITHMSORT_H
