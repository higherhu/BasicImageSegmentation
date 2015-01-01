#pragma once

#include "segmentor.h"
#include "SLIC/SLIC.h"

typedef unsigned int uint;

class SLICSegmentor :
	public Segmentor
{
	DECLARE_DYNCRT_CLASS(SLICSegmentor, Segmentor);

public:
	SLICSegmentor(void);
	~SLICSegmentor(void);

	virtual void SetArgs(const vector<float> _args);

	virtual void Run();

private:
	int m_Step;
};
