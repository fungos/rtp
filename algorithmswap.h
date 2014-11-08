#ifndef ALGORITHMSWAP_H
#define ALGORITHMSWAP_H

#include "ialgorithm.h"

class AlgorithmSwapDistance : public IAlgorithm
{
	public:
		virtual bool update() override;
		virtual QString name() override
		{
			return "Random Pixel Swap";
		}

		void doStep();
};

#endif // ALGORITHMSWAP_H
