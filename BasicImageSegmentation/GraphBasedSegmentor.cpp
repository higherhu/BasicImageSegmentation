#include "GraphBasedSegmentor.h"


GraphBasedSegmentor::GraphBasedSegmentor(void)
{
	m_Name = "GraphBased";

	m_Sigma=0.5;
	m_Threshold=30;
	m_MinSize=30;

	m_argNum = 3;
}

GraphBasedSegmentor::~GraphBasedSegmentor(void)
{
}

void GraphBasedSegmentor::SetArgs(const vector<float> _args)
{
	cout<<"["<<m_Name<<"] Getting arguments..."<<endl;

	float argu[] = {m_Sigma, m_Threshold, m_MinSize};
	string argNames[] = {"Sigma", "Threshold", "MinSize"};
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

	m_Sigma = argu[0]; m_Threshold = argu[1]; m_MinSize = argu[2];

	stringstream ss;
	ss<<m_Name<<"_"<<m_Sigma<<"_"<<m_Threshold<<"_"<<m_MinSize<<".txt";
	m_ResultName = ss.str();
}

void GraphBasedSegmentor::Run()
{
	cout<<"====="<<m_Name<<" Runing..."<<endl;

	Mat img3u(m_Img);
	Mat img3f;
	img3u.convertTo(img3f, CV_32FC3, 1.0/255);
	cvtColor(img3f, img3f, COLOR_BGR2Lab);
	int regionNum = SegmentImage(img3f, m_Result, m_Sigma, m_Threshold, m_MinSize);

	Segmentor::Run();
}