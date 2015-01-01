#include "GrabCutSegmentor.h"

//IMPLEMENT_DYNCRT_CLASS(GrabCutSegmentor);

int GrabCutSegmentor::BGD_KEY = CV_EVENT_FLAG_CTRLKEY;
int GrabCutSegmentor::FGD_KEY = CV_EVENT_FLAG_SHIFTKEY;

GrabCutSegmentor::GrabCutSegmentor(void)
{
	m_Name = "GrabCut";

	m_WinName = "GrabCut Interactive";
	isInitialized = false;

	m_argNum = 0;
}


GrabCutSegmentor::~GrabCutSegmentor(void)
{
}

void GrabCutSegmentor::SetArgs(const vector<float> _args)
{
	//cout<<"["<<m_Name<<"] Getting arguments..."<<endl;

	stringstream ss;
	ss<<m_Name<<".txt";
	m_ResultName = ss.str();
}

void GrabCutSegmentor::SetImage(const Mat& _img)
{
	Segmentor::SetImage(_img);

	m_Mask.create( m_Img.size(), CV_8UC1);
	reset();
}

void GrabCutSegmentor::Run()
{
	cout<<"====="<<m_Name<<" Runing..."<<endl;

	namedWindow(m_WinName);
	setMouseCallback(m_WinName, onMouse, this);
	showImage();

	cout << "\nThe Grabcut Segmentation\n"
        "\nSelect a rectangular area around the object you want to segment\n" <<
        "\tESC - Finished\n"
        "\tr - restore the original image\n"
        "\ts - do segmentation\n"
        "\tleft mouse button - set rectangle\n"
        "\tCTRL+left mouse button - set GC_BGD pixels\n"
        "\tSHIFT+left mouse button - set CG_FGD pixels\n"
        "\tCTRL+right mouse button - set GC_PR_BGD pixels\n"
        "\tSHIFT+right mouse button - set CG_PR_FGD pixels\n" << endl;

	for(;;)
    {
        int c = waitKey(0);
        switch( (char) c )
        {
        case '\x1b':
            cout << "Exiting ..." << endl;
            goto exit;
        case 'r':
            cout << endl;
            reset();
            showImage();
            break;
        case 's':
			int iter = iterCount;
            //cout << "<" << iterCount << "... ";
            int newIterCount = nextIter();
            if( newIterCount > iter )
            {
                showImage();
                cout << "Finished!" << endl;
            }
            else
                cout << "rect must be determined!" << endl;
            break;
        }
    }

exit:
	m_Mask = m_Mask & 1;
	m_Mask.convertTo(m_Result, CV_32SC1);

	destroyWindow( m_WinName );	

	Segmentor::Run();
}

void GrabCutSegmentor::onMouse(int event, int x, int y, int flags, void* param)
{
//left mouse button - set rectangle
//CTRL+left mouse button - set GC_BGD pixels
//SHIFT+left mouse button - set CG_FGD pixels
//CTRL+right mouse button - set GC_PR_BGD pixels
//SHIFT+right mouse button - set CG_PR_FGD pixels

	GrabCutSegmentor* s = (GrabCutSegmentor*)param;
	if (s == NULL)
	{
		cout<<"--Error: Failed to get GrabCut Entity!"<<endl;
		return;
	}
	switch( event )
    {
    case CV_EVENT_LBUTTONDOWN: // set rect or GC_BGD(GC_FGD) labels
        {
            bool isb = (flags & BGD_KEY) != 0,
                 isf = (flags & FGD_KEY) != 0;
            if( s->rectState == NOT_SET && !isb && !isf )
            {
                s->rectState = IN_PROCESS;
                s->rect = Rect( x, y, 1, 1 );
            }
            if ( (isb || isf) && s->rectState == SET )
                s->lblsState = IN_PROCESS;
        }
        break;
    case CV_EVENT_RBUTTONDOWN: // set GC_PR_BGD(GC_PR_FGD) labels
        {
            bool isb = (flags & BGD_KEY) != 0,
                 isf = (flags & FGD_KEY) != 0;
            if ( (isb || isf) && s->rectState == SET )
                s->prLblsState = IN_PROCESS;
        }
        break;
    case CV_EVENT_LBUTTONUP:
        if( s->rectState == IN_PROCESS )
        {
            s->rect = Rect( Point(s->rect.x, s->rect.y), Point(x,y) );
            s->rectState = SET;
            s->setRectInMask();
            assert( s->bgdPxls.empty() && s->fgdPxls.empty() && s->prBgdPxls.empty() && s->prFgdPxls.empty() );
            s->showImage();
        }
        if( s->lblsState == IN_PROCESS )
        {
            s->setLblsInMask(flags, Point(x,y), false);
            s->lblsState = SET;
            s->showImage();
        }
        break;
    case CV_EVENT_RBUTTONUP:
        if( s->prLblsState == IN_PROCESS )
        {
            s->setLblsInMask(flags, Point(x,y), true);
            s->prLblsState = SET;
            s->showImage();
        }
        break;
    case CV_EVENT_MOUSEMOVE:
        if( s->rectState == IN_PROCESS )
        {
            s->rect = Rect( Point(s->rect.x, s->rect.y), Point(x,y) );
            assert( s->bgdPxls.empty() && s->fgdPxls.empty() && s->prBgdPxls.empty() && s->prFgdPxls.empty() );
            s->showImage();
        }
        else if( s->lblsState == IN_PROCESS )
        {
            s->setLblsInMask(flags, Point(x,y), false);
            s->showImage();
        }
        else if( s->prLblsState == IN_PROCESS )
        {
            s->setLblsInMask(flags, Point(x,y), true);
            s->showImage();
        }
        break;
    }
}

void GrabCutSegmentor::doSegment()
{

}

void GrabCutSegmentor::setRectInMask()
{
    assert( !m_Mask.empty() );
    m_Mask.setTo( GC_BGD );
    rect.x = max(0, rect.x);
    rect.y = max(0, rect.y);
    rect.width = min(rect.width, m_Img.cols-rect.x);
    rect.height = min(rect.height, m_Img.rows-rect.y);
    (m_Mask(rect)).setTo( Scalar(GC_PR_FGD) );
}

void GrabCutSegmentor::setLblsInMask( int flags, Point p, bool isPr )
{
    vector<Point> *bpxls, *fpxls;
    uchar bvalue, fvalue;
    if( !isPr )
    {
        bpxls = &bgdPxls;
        fpxls = &fgdPxls;
        bvalue = GC_BGD;
        fvalue = GC_FGD;
    }
    else
    {
        bpxls = &prBgdPxls;
        fpxls = &prFgdPxls;
        bvalue = GC_PR_BGD;
        fvalue = GC_PR_FGD;
    }
    if( flags & BGD_KEY )
    {
        bpxls->push_back(p);
        circle( m_Mask, p, 2, bvalue, -1 );
    }
    if( flags & FGD_KEY )
    {
        fpxls->push_back(p);
        circle( m_Mask, p, 2, fvalue, -1 );
    }
}

const Scalar RED = Scalar(0,0,255);
const Scalar PINK = Scalar(230,130,255);
const Scalar BLUE = Scalar(255,0,0);
const Scalar LIGHTBLUE = Scalar(255,255,160);
const Scalar GREEN = Scalar(0,255,0);
void GrabCutSegmentor::showImage()
{
    if( m_Img.empty())
        return;

    Mat res;
    Mat binMask;
    if( !isInitialized )
        m_Img.copyTo( res );
    else
    {
		if( m_Mask.empty() || m_Mask.type()!=CV_8UC1 )
			CV_Error( CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)" );
		if( binMask.empty() || binMask.rows!=m_Mask.rows || binMask.cols!=m_Mask.cols )
			binMask.create( m_Mask.size(), CV_8UC1 );
		binMask = m_Mask & 1;
        m_Img.copyTo( res, binMask );
    }

    vector<Point>::const_iterator it;
    for( it = bgdPxls.begin(); it != bgdPxls.end(); ++it )
        circle( res, *it, 2, BLUE, -1 );
    for( it = fgdPxls.begin(); it != fgdPxls.end(); ++it )
        circle( res, *it, 2, RED, -1 );
    for( it = prBgdPxls.begin(); it != prBgdPxls.end(); ++it )
        circle( res, *it, 2, LIGHTBLUE, -1 );
    for( it = prFgdPxls.begin(); it != prFgdPxls.end(); ++it )
        circle( res, *it, 2, PINK, -1 );

    if( rectState == IN_PROCESS || rectState == SET )
        rectangle( res, Point( rect.x, rect.y ), Point(rect.x + rect.width, rect.y + rect.height ), GREEN, 2);

    imshow( m_WinName, res );
}

int GrabCutSegmentor::nextIter()
{
    if( isInitialized )
        grabCut( m_Img, m_Mask, rect, m_BgdModel, m_FgdModel, 1 );
    else
    {
        if( rectState != SET )
            return iterCount;

        if( lblsState == SET || prLblsState == SET )
            grabCut( m_Img, m_Mask, rect, m_BgdModel, m_FgdModel, 1, GC_INIT_WITH_MASK );
        else
            grabCut( m_Img, m_Mask, rect, m_BgdModel, m_FgdModel, 1 , GC_INIT_WITH_RECT );

        isInitialized = true;
    }
    iterCount++;

    bgdPxls.clear(); fgdPxls.clear();
    prBgdPxls.clear(); prFgdPxls.clear();

    return iterCount;
}

void GrabCutSegmentor::reset()
{
    if( !m_Mask.empty() )
        m_Mask.setTo(Scalar::all(GC_BGD));
    bgdPxls.clear(); fgdPxls.clear();
    prBgdPxls.clear();  prFgdPxls.clear();

    isInitialized = false;
    rectState = NOT_SET;
    lblsState = NOT_SET;
    prLblsState = NOT_SET;
    iterCount = 0;
}