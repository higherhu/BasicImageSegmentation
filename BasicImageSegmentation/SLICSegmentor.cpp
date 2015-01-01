#include "SLICSegmentor.h"

//IMPLEMENT_DYNCRT_CLASS(SLICSegmentor);

SLICSegmentor::SLICSegmentor(void)
{
	m_Name = "SLIC";

	m_Step = 7; 

	m_argNum = 1;
}

SLICSegmentor::~SLICSegmentor(void)
{
}

void SLICSegmentor::SetArgs(const vector<float> _args)
{
	cout<<"["<<m_Name<<"] Getting arguments..."<<endl;

	float argu[] = {m_Step};
	string argNames[] = {"Step"};
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

	m_Step = argu[0];

	stringstream ss;
	ss<<m_Name<<"_"<<m_Step<<".txt";
	m_ResultName = ss.str();
}

void SLICSegmentor::Run()
{
	cout<<"====="<<m_Name<<" Runing..."<<endl;

	SLIC slicsp;
	int h = m_Img.rows, w = m_Img.cols;
	uint* imgData = new uint[h*w];

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			cv::Vec3b bgr = m_Img.at<cv::Vec3b>(i, j);
			imgData[i*w+j] = (bgr[2]<<16) | (bgr[1]<<8) | (bgr[0]);
		}
	}	

	int numLabels;
	int *klabels = new int[h*w];
	slicsp.PerformSLICO_ForGivenStepSize(imgData, w, h, klabels, numLabels, m_Step, NULL);

	for (int i = 0; i < h; i++)
	{
		int* ptr = m_Result.ptr<int>(i);
		for (int j = 0; j < w; j++)
		{
			ptr[j] = klabels[i*w+j];
		}
	}

	delete[] klabels;
	delete[] imgData;

	Segmentor::Run();
}

