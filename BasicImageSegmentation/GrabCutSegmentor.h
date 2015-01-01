#pragma once

#include "segmentor.h"

#define max(x,y) (x)>(y) ? (x) : (y)
#define min(x,y) (x)<(y) ? (x) : (y)

class GrabCutSegmentor :
	public Segmentor
{
	DECLARE_DYNCRT_CLASS(GrabCutSegmentor, Segmentor);

public:
	GrabCutSegmentor(void);
	~GrabCutSegmentor(void);

	virtual void SetArgs(const vector<float> _args);

	virtual void Run();

	void SetImage(const Mat& _img);

private:
	static void onMouse(int event, int x, int y, int flags, void* param);
	static int BGD_KEY;
	static int FGD_KEY;

	void doSegment();
	void setRectInMask();
	void setLblsInMask( int flags, Point p, bool isPr );
	void showImage();
	int nextIter();
	void reset();

	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };

	Mat m_Mask;
	Mat m_BgdModel, m_FgdModel;
	string m_WinName;

	int iterCount;
	uchar rectState, lblsState, prLblsState;
	Rect rect;
    vector<Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
	bool isInitialized;
};

