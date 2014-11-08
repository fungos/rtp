#ifndef ALGORITHMCOPY_H
#define ALGORITHMCOPY_H

#include "ialgorithm.h"

class AlgorithmCopy : public IAlgorithm
{
	public:
		virtual bool setup(QImage *input, QImage *palette, DistanceFunction func) override;
		virtual bool update() override;
		virtual QString name() override
		{
			return "Copy";
		}
};

#endif // ALGORITHMCOPY_H
