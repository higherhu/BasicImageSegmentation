#pragma once

#include "segmentor.h"
#include "SEEDS/seeds2.h"

class SEEDSSegmentor :
	public Segmentor
{
	DECLARE_DYNCRT_CLASS(SEEDSSegmentor, Segmentor);

public:
	SEEDSSegmentor(void);
	~SEEDSSegmentor(void);

	virtual void SetArgs(const vector<float> _args);

	virtual void Run();

private:
	int m_NumRegions;
};

