#pragma once

#include "segmentor.h"
#include "EfficientGraphBased/segment-image.h"

class GraphBasedSegmentor :
	public Segmentor
{
	DECLARE_DYNCRT_CLASS(GraphBasedSegmentor, Segmentor);

public:
	GraphBasedSegmentor(void);
	~GraphBasedSegmentor(void);

	virtual void SetArgs(const vector<float> _args);

	virtual void Run();

private:
	float m_Sigma;
	float m_Threshold;
	int m_MinSize;
};

