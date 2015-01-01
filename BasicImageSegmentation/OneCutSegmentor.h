#pragma once

#include "segmentor.h"
#include "multi-labelGraphCut/graph.h"

const float INT32_CONST = 1000;
const float HARD_CONSTRAINT_CONST = 1000;

#define NEIGHBORHOOD_8_TYPE 1;
#define NEIGHBORHOOD_25_TYPE 2;

const int NEIGHBORHOOD = NEIGHBORHOOD_8_TYPE;

class OneCutSegmentor :
	public Segmentor
{
	DECLARE_DYNCRT_CLASS(OneCutSegmentor, Segmentor);

public:
	OneCutSegmentor(void);
	~OneCutSegmentor(void);

	virtual void SetArgs(const vector<float> _args);

	virtual void Run();

private:
	static void onMouse(int event, int x, int y, int flags, void* param);

	void showImage();

	string m_WinName;
	string m_ResultWinName;
	void init();
	void getBinPerPixel();
	void getEdgeVariance();
	void doSegmente();
	void releaseAll();


	Mat showImg, binPerPixelImg, segMask, segShowImg;
	Mat scribbleMask;

	bool m_isDrawing;
	bool m_isForeground;
	Point m_PrevPoint;

	int numUsedBins;
	float varianceSquared;

	// default arguments
	float bha_slope;
	int numBinsPerChannel;

	GraphType *myGraph; 
};

