#include <fstream>
#include "msImageProcessor.h"
using namespace std;
using namespace SEG;

//分割图像
void M_SegImage(IplImage *m_Im)
{
	bool bfuse=true;

	msImageProcessor *iProc = new msImageProcessor();//图像输入

	imageType gtype= COLOR;
	int m_height, m_width;
	m_height	=	m_Im->height;
	m_width		=	m_Im->width;

	iProc->DefineImage((uchar*)m_Im->imageData, gtype, m_height, m_width);	

	float speedUpThreshold_=0.1f;//进行图像过滤
	iProc->SetSpeedThreshold(speedUpThreshold_);
	int sigmaS=11;
	float sigmaR=6.5;
	SpeedUpLevel speedUpLevel_=MED_SPEEDUP;
	iProc->Filter(sigmaS, sigmaR, speedUpLevel_);

	if (bfuse) {//进行图像融合
		int minRegion=20; 
		iProc->FuseRegions(sigmaR, minRegion);
	}
	int L=m_width*m_height;
	int *labels2=(int*)malloc(L*sizeof(int));
	iProc->GetLabels(labels2);
	
	int m_RegionNum=iProc->regionCount;

	delete iProc;
}