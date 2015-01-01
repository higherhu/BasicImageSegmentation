#pragma once

#include "segmentor.h"
#include "MeanShift/msImageProcessor.h"



class MeanShiftSegmentor :
	public Segmentor
{
	DECLARE_DYNCRT_CLASS(MeanShiftSegmentor, Segmentor);

public:
	MeanShiftSegmentor(void);
	~MeanShiftSegmentor(void);

	virtual void SetArgs(const vector<float> _args);

	virtual void Run();

private:
	int m_SigmaS;
	float m_SigmaR;
	int m_MinRegion;
	SpeedUpLevel m_SpeedUpLevel;
};

