#include "segmentor.h"

//IMPLEMENT_DYNCRT_BASE(Segmentor);

//map<string, Segmentor::ClassGen>& Segmentor::class_set()
//{
//	static map<string, Segmentor::ClassGen> sMap;
//	return sMap;
//}


Segmentor::Segmentor()
{
	
}

Segmentor::~Segmentor(void)
{
}

void Segmentor::SetImage(const Mat& _img)
{
	m_Img = _img.clone(); 
	m_Result.create(m_Img.size(), CV_32SC1);
}

void Segmentor::ShowResult(const Vec3b& _color)
{
	int w = m_Img.cols, h = m_Img.rows;
	int label, top, bot, left, right;//, topl, topr, botl, botr;
	Mat showImg = m_Img.clone();
	Mat mask(m_Img.size(), CV_8UC1);
	mask = 0;

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			label = m_Result.at<int>(i, j);
			top = m_Result.at<int>( (i-1 > -1 ? i-1 : 0), j );
			bot = m_Result.at<int>( (i+1 < h ? i+1 : h-1), j );
			left = m_Result.at<int>( i, (j-1>-1 ? j-1 : 0) );
			right = m_Result.at<int>( i, (j+1<w ? j+1 : w-1) );
			if (label!=top || label!=left || label!=right || label!=bot)
				mask.at<uchar>(i, j) = 255;
		}
	}
	showImg.setTo(_color, mask);
	int end = m_ResultName.find_last_of('.');
	string winName = m_ResultName.substr(0, end);
	namedWindow(winName);
	imshow(winName, showImg);
}

void Segmentor::SaveResult()
{
	// save to txt file
	fstream f(m_ResultName, ios::out);
	f<<m_Result.rows<<" "<<m_Result.cols<<endl;
	for (int i = 0; i < m_Result.rows; i++)
	{
		for (int j = 0; j < m_Result.cols; j++)
		{
			f<<m_Result.at<int>(i, j)<<" ";
		}
		f<<endl;
	}
	f.close();
}

void Segmentor::Run()
{
	fixResult();
}

void Segmentor::fixResult()
{
// 调整超像素编号，使得编号从0开始，且连续。返回超像素个数(最大编号+1）
// 并不保证编号一定按从小到大排序
	Mat _fixedSp(m_Result.size(), CV_32SC1);

	int regionNum=0;
	int* lookUpTable = new int[m_Result.cols*m_Result.rows];	// look up table,初始为-1,
	memset(lookUpTable, -1, m_Result.cols*m_Result.rows);
	for (int i = 0; i < m_Result.rows; i++)
	{
		const int* ptr = m_Result.ptr<int>(i);
		for (int j = 0; j < m_Result.cols; j++)
		{
			int spId = ptr[j];
			if (lookUpTable[spId] < 0)	// 当前Id在查找表中不存在，则加入查找表
			{
				lookUpTable[spId] = regionNum;
				regionNum ++;
			}
		}
	}

	for (int i = 0; i < m_Result.rows; i++)
	{
		const int* ptrSp = m_Result.ptr<int>(i);
		int* ptrFix = _fixedSp.ptr<int>(i);
		for (int j = 0; j < m_Result.cols; j++)
		{
			int spId = ptrSp[j];
			if (lookUpTable[spId] >= 0)
			{
				ptrFix[j] = lookUpTable[spId];
			}
		}
	}
	m_Result.release();
	m_Result = _fixedSp.clone();
}

//void Segmentor::Run()
//{
//	namedWindow("Interactive");
//	setMouseCallback("Interactive", onMouse, 0);
//	imshow("Interactive", m_Img);
//	waitKey(0);
//}

//void Segmentor::onMouse(int event, int x, int y, int flags, void* param)
//{
//	switch( event )
//    {
//	case CV_EVENT_LBUTTONDOWN: 
//		cout<<"Left Button Down"<<endl;
//        break;
//    case CV_EVENT_RBUTTONDOWN: 
//        cout<<"Right Button Down"<<endl;
//        break;
//    case CV_EVENT_LBUTTONUP:
//        cout<<"Left Button Up"<<endl;
//        break;
//    case CV_EVENT_RBUTTONUP:
//        cout<<"Right Button Up"<<endl;
//        break;
//    case CV_EVENT_MOUSEMOVE:
//        cout<<"Mouse Move"<<endl;
//        break;
//    }
//}