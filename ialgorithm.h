#ifndef IALGORITHM_H
#define IALGORITHM_H

#include <QString>
#include <functional>
#include "pixel.h"

class QImage;
typedef std::function<double(Pixel, Pixel)> DistanceFunction;

class IAlgorithm : public QObject
{
	Q_OBJECT
	public:
		IAlgorithm();
		virtual ~IAlgorithm();

		virtual bool setup(QImage *input, QImage *palette, DistanceFunction func);
		virtual bool update() = 0;
		virtual QString name() = 0;
		virtual QImage *result()
		{
			return pCurrent;
		}
		
		bool process();

		static const int kUpdateSize = 1000;

	signals:
		void step();
		void finished(QImage *result);

	protected:
		DistanceFunction pDistance;
		QImage *pInput;
		QImage *pPalette;
		QImage *pCurrent; // result

		int iCount;
		int iInputHeight;
		int iInputWidth;
		int iPaletteHeight;
		int iPaletteWidth;

		bool bFinished;
};

#endif // IALGORITHM_H
