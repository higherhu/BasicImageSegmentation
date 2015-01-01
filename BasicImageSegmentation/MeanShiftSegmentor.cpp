#include "MeanShiftSegmentor.h"

//IMPLEMENT_DYNCRT_CLASS(MeanShiftSegmentor);

MeanShiftSegmentor::MeanShiftSegmentor(void)
{
	m_Name = "MeanShift";

	m_SigmaS = 11;
	m_SigmaR = 6.5;
	m_MinRegion = 20;
	m_SpeedUpLevel = (SpeedUpLevel) 1;

	m_argNum = 4;
}

MeanShiftSegmentor::~MeanShiftSegmentor(void)
{
}

void MeanShiftSegmentor::SetArgs(const vector<float> _args)
{
	cout<<"["<<m_Name<<"] Getting arguments..."<<endl;

	float argu[] = {m_SigmaS, m_SigmaR, m_MinRegion, m_SpeedUpLevel};
	string argNames[] = {"SigmaS", "SigmaR", "MinRegion", "SpeedUpLevel"};
	cout<<"--Given "<<(_args.size()>m_argNum ? m_argNum : _args.size())<<" argument(s)"; 
	int i = 0;
	for ( ; i < _args.size(); i++)
	{
		cout<<", "<<"setting "<<argNames[i]<<": "<<_args[i];
		argu[i] = _args[i];
	}
	for (; i < m_argNum; i++)
	{
		cout<<", "<<"using default "<<argNames[i]<<": "<<argu[i];
	}
	cout<<endl;

	m_SigmaS = argu[0]; m_SigmaR = argu[1]; m_MinRegion = argu[2]; m_SpeedUpLevel = (SpeedUpLevel)(int)argu[3];

	stringstream ss;
	ss<<m_Name<<"_"<<m_SigmaS<<"_"<<m_SigmaR<<"_"<<m_MinRegion<<"_"<<m_SpeedUpLevel<<".txt";
	m_ResultName = ss.str();
}

void MeanShiftSegmentor::Run()
{
	cout<<"====="<<m_Name<<" Runing..."<<endl;

	IplImage* image = new IplImage(m_Img);
	msImageProcessor *iProc = new msImageProcessor();

	imageType gtype= COLOR;
	int m_height, m_width;
	m_height	=	image->height;
	m_width		=	image->width;

	iProc->DefineImage((uchar*)image->imageData, gtype, m_height, m_width);	

	float speedUpThreshold_=0.1f;
	iProc->SetSpeedThreshold(speedUpThreshold_);	//进行图像过滤
	iProc->Filter(m_SigmaS, m_SigmaR, m_SpeedUpLevel);
	iProc->FuseRegions(m_SigmaR, m_MinRegion);			//进行图像融合


	int L=m_width*m_height;
	int *labels2=(int*)malloc(L*sizeof(int));
	iProc->GetLabels(labels2);
	
	//int m_RegionNum=iProc->regionCount;

	for (int i = 0; i < m_height; i++)
	{
		int* ptr = m_Result.ptr<int>(i);
		for (int j = 0; j < m_width; j++)
		{
			ptr[j] = labels2[i*m_width+j];
		}
	}

	delete iProc;

	Segmentor::Run();
}