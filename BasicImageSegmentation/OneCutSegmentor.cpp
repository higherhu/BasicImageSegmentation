#include "OneCutSegmentor.h"


OneCutSegmentor::OneCutSegmentor(void)
{
	m_Name = "OneCut";

	m_WinName = "OneCut Interactive";
	m_ResultWinName = "OneCut Result";
	numUsedBins = 0;
	varianceSquared = 0;
	m_isDrawing = false;
	m_isForeground = false;

	// default arguments
	bha_slope = 0.1f;
	numBinsPerChannel = 64;
	myGraph = NULL;

	m_argNum = 0;
}

OneCutSegmentor::~OneCutSegmentor(void)
{
	if (myGraph)
	{
		delete myGraph;
	}
}

void OneCutSegmentor::SetArgs(const vector<float> _args)
{
	//cout<<"["<<m_Name<<"] Getting arguments..."<<endl;

	stringstream ss;
	ss<<m_Name<<".txt";
	m_ResultName = ss.str();
}

void OneCutSegmentor::Run()
{
	cout<<"====="<<m_Name<<" Runing..."<<endl;

	init();
	namedWindow(m_WinName);
	setMouseCallback(m_WinName, onMouse, this);
	showImage();

	cout << "\nThe OneCut Segmentation\n"
        "\tESC - Finished\n"
        "\tr - restore the original image\n"
        "\ts - do segmentation\n"
        "\tleft mouse button - set Foreground pixels\n"
		"\tright mouse button - set Background pixels\n" << endl;

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
            releaseAll();
			init();
            showImage();
            break;
        case 's':
			doSegmente();
            break;
        }
    }

exit:
	segMask.convertTo(m_Result, CV_32SC1);

	cv::destroyWindow( m_WinName );	
	cv::destroyWindow( m_ResultWinName );

	Segmentor::Run();
}

void OneCutSegmentor::onMouse(int event, int x, int y, int flags, void* param)
{
//left mouse button - set Foreground pixels
//right mouse button - set Background pixels

	OneCutSegmentor* s = (OneCutSegmentor*)param;
	if (s == NULL)
	{
		cout<<"--Error: Failed to get OneCut Entity!"<<endl;
		return;
	}

	if (x < 0 || x > 32767)
		x = 0;
	if (y < 0 || y > 32767)
		y = 0;

	if (x >= s->m_Img.cols)
		x = s->m_Img.cols - 1;
	if (y >= s->m_Img.rows)
		y = s->m_Img.rows - 1;

	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		{
			s->m_isDrawing = true;	
			s->m_isForeground = true;
			s->m_PrevPoint.x = x;
			s->m_PrevPoint.y = y;
			cv::line(s->showImg, s->m_PrevPoint, cv::Point(x,y), Scalar(0,0,255), 3);
			cv::line(s->scribbleMask, s->m_PrevPoint, Point(x,y), Scalar(255,0,0), 3);
		}
		break;
	case CV_EVENT_MOUSEMOVE:
		{
			if(s->m_isDrawing)
			{
				if (s->m_isForeground)
				{
					cv::line(s->showImg, s->m_PrevPoint, cv::Point(x,y), Scalar(0,0,255), 3);	// 前景叠加红色
					cv::line(s->scribbleMask, s->m_PrevPoint, Point(x,y), Scalar(255,0,0), 3);	//前景mask为255
				} else {
					cv::line(s->showImg, s->m_PrevPoint, cv::Point(x,y), Scalar(255,0,0), 3);	// 背景叠加蓝色
					cv::line(s->scribbleMask, s->m_PrevPoint, Point(x,y), Scalar(128,0,0), 3);	// 背景mask为128
				}
				s->m_PrevPoint.x=x;
				s->m_PrevPoint.y=y;
			}
		}
		break;
	case CV_EVENT_LBUTTONUP:
		{			
			s->m_isDrawing=false;
			circle(s->showImg, Point(x,y), 1, Scalar(0,0,255));
			circle(s->scribbleMask, Point(x,y), 1, Scalar(255,0,0));
		}
		break;
	case CV_EVENT_RBUTTONDOWN:
		{
			s->m_isForeground = false;
			s->m_isDrawing = true;				
			s->m_PrevPoint.x = x;
			s->m_PrevPoint.y = y;
			cv::line(s->showImg, s->m_PrevPoint, cv::Point(x,y), Scalar(255,0,0), 3);
			cv::line(s->scribbleMask, s->m_PrevPoint, Point(x,y), Scalar(128,0,0), 3);
		}
		break;
	case CV_EVENT_RBUTTONUP:
		{			
			s->m_isDrawing=false;
			circle(s->showImg, Point(x,y), 1, Scalar(255,0,0));
			circle(s->scribbleMask, Point(x,y), 1, Scalar(128,0,0));
		}
		break;
	}
	s->showImage();
}

void OneCutSegmentor::doSegmente()
{
	for(int i=0; i<m_Img.rows; i++)
	{
		for(int j=0; j<m_Img.cols; j++) 
		{
			GraphType::node_id currNodeId = i * m_Img.cols + j;
	
			if (scribbleMask.at<uchar>(i,j) == 255)
				myGraph->add_tweights(currNodeId,(int)ceil(INT32_CONST * HARD_CONSTRAINT_CONST + 0.5),0);
			else if (scribbleMask.at<uchar>(i,j) == 128)
				myGraph->add_tweights(currNodeId,0,(int)ceil(INT32_CONST * HARD_CONSTRAINT_CONST + 0.5));
		}
	}
	cout << "====OneCut:maxflow..." << endl;
	int flow = myGraph -> maxflow();

	// this is where we store the results
	segMask = 0;
	m_Img.copyTo(segShowImg);

	// empty scribble masks are ready to record additional scribbles for additional hard constraints
	// to be used next time
	scribbleMask = 0;

	// copy the segmentation results on to the result images
	for (int i = 0; i<m_Img.rows * m_Img.cols; i++)
	{
		// if it is foreground - color blue
		if (myGraph->what_segment((GraphType::node_id)i ) == GraphType::SOURCE)
		{
			segMask.at<uchar>(i/m_Img.cols, i%m_Img.cols) = 255;
			(uchar)segShowImg.at<Vec3b>(i/m_Img.cols, i%m_Img.cols)[2] =  200;
		}
		// if it is background - color red
		else
		{
			segMask.at<uchar>(i/m_Img.cols, i%m_Img.cols) = 0;
			(uchar)segShowImg.at<Vec3b>(i/m_Img.cols, i%m_Img.cols)[0] =  200;
		}
	}

	imshow(m_ResultWinName, segShowImg);

	cout << "====OneCut:done maxflow!" << endl;
}

void OneCutSegmentor::releaseAll()
{
	// clear all data
	scribbleMask.release();
	showImg.release();
	binPerPixelImg.release();
	segMask.release();
	segShowImg.release();

	delete myGraph;
}

// init all images/vars
void OneCutSegmentor::init()
{
	showImg = m_Img.clone();
	segShowImg = m_Img.clone();

	// this is the mask to keep the user scribbles
	scribbleMask.create(2,m_Img.size,CV_8UC1);
	scribbleMask = 0;
	segMask.create(2,m_Img.size,CV_8UC1);
	segMask = 0;
	binPerPixelImg.create(2, m_Img.size,CV_32F);

	// get bin index for each image pixel, store it in binPerPixelImg
	getBinPerPixel();

	// compute the variance of image edges between neighbors
	getEdgeVariance();
	
	myGraph = new GraphType(/*estimated # of nodes*/ m_Img.rows * m_Img.cols + numUsedBins, 
		/*estimated # of edges=11 spatial neighbors and one link to auxiliary*/ 12 * m_Img.rows * m_Img.cols); 
	GraphType::node_id currNodeId = myGraph -> add_node((int)m_Img.cols * m_Img.rows + numUsedBins); 

	for(int i=0; i<m_Img.rows; i++)
	{
		for(int j=0; j<m_Img.cols; j++) 
		{
			// this is the node id for the current pixel
			GraphType::node_id currNodeId = i * m_Img.cols + j;

			// add hard constraints based on scribbles
			if (scribbleMask.at<uchar>(i,j) == 255)
				myGraph->add_tweights(currNodeId,(int)ceil(INT32_CONST * HARD_CONSTRAINT_CONST + 0.5),0);
			else if (scribbleMask.at<uchar>(i,j) == 128)
				myGraph->add_tweights(currNodeId,0,(int)ceil(INT32_CONST * HARD_CONSTRAINT_CONST + 0.5));
				
			// You can now access the pixel value with cv::Vec3b
			float b = (float)m_Img.at<Vec3b>(i,j)[0];
			float g = (float)m_Img.at<Vec3b>(i,j)[1];
			float r = (float)m_Img.at<Vec3b>(i,j)[2];

			// go over the neighbors
			for (int si = -NEIGHBORHOOD; si <= NEIGHBORHOOD && si + i < m_Img.rows && si + i >= 0 ; si++)
			{
				for (int sj = 0; sj <= NEIGHBORHOOD && sj + j < m_Img.cols; sj++)
				{
					if ((si == 0 && sj == 0) ||
						(si == 1 && sj == 0) || 
						(si == NEIGHBORHOOD && sj == 0))
						continue;

					// this is the node id for the neighbor
					GraphType::node_id nNodeId = (i+si) * m_Img.cols + (j + sj);
					
					float nb = (float)m_Img.at<Vec3b>(i+si,j+sj)[0];
					float ng = (float)m_Img.at<Vec3b>(i+si,j+sj)[1];
					float nr = (float)m_Img.at<Vec3b>(i+si,j+sj)[2];

					//   ||I_p - I_q||^2  /   2 * sigma^2
					float currEdgeStrength = exp(-((b-nb)*(b-nb) + (g-ng)*(g-ng) + (r-nr)*(r-nr))/(2*varianceSquared));
					float currDist = sqrt((float)si*(float)si + (float)sj*(float)sj);

					// this is the edge between the current two pixels (i,j) and (i+si, j+sj)
					currEdgeStrength = ((float)0.95 * currEdgeStrength + (float)0.05) /currDist;
					myGraph -> add_edge(currNodeId, nNodeId,    /* capacities */ (int) ceil(INT32_CONST*currEdgeStrength + 0.5), (int)ceil(INT32_CONST*currEdgeStrength + 0.5));
				}
			}
			// add the adge to the auxiliary node
			int currBin =  (int)binPerPixelImg.at<float>(i,j);

			myGraph -> add_edge(currNodeId, (GraphType::node_id)(currBin + m_Img.rows * m_Img.cols),
				/* capacities */ (int) ceil(INT32_CONST*bha_slope+ 0.5), (int)ceil(INT32_CONST*bha_slope + 0.5));
		}

	}
	
	return ;
}

// get bin index for each image pixel, store it in binPerPixelImg
void OneCutSegmentor::getBinPerPixel()
{
	// this vector is used to through away bins that were not used
	vector<int> occupiedBinNewIdx((int)pow((double)numBinsPerChannel,(double)3),-1);

	// go over the image
	int newBinIdx = 0;
	for(int i=0; i<m_Img.rows; i++)
		for(int j=0; j<m_Img.cols; j++) 
		{
			// You can now access the pixel value with cv::Vec3b
			float b = (float)m_Img.at<Vec3b>(i,j)[0];
			float g = (float)m_Img.at<Vec3b>(i,j)[1];
			float r = (float)m_Img.at<Vec3b>(i,j)[2];

			// this is the bin assuming all bins are present
			int bin = (int)(floor(b/256.0 *(float)numBinsPerChannel) + (float)numBinsPerChannel * floor(g/256.0*(float)numBinsPerChannel) 
				+ (float)numBinsPerChannel * (float)numBinsPerChannel * floor(r/256.0*(float)numBinsPerChannel)); 

			
			// if we haven't seen this bin yet
			if (occupiedBinNewIdx[bin]==-1)
			{
				// mark it seen and assign it a new index
				occupiedBinNewIdx[bin] = newBinIdx;
				newBinIdx ++;
			}
			// if we saw this bin already, it has the new index
			binPerPixelImg.at<float>(i,j) = (float)occupiedBinNewIdx[bin];
		}

		double maxBin;
		minMaxLoc(binPerPixelImg,NULL,&maxBin);
		numUsedBins = (int) maxBin + 1;

		occupiedBinNewIdx.clear();
}

// compute the variance of image edges between neighbors
void OneCutSegmentor::getEdgeVariance()
{
	varianceSquared = 0;
	int counter = 0;
	for(int i=0; i<m_Img.rows; i++)
	{
		for(int j=0; j<m_Img.cols; j++) 
		{
			
			// You can now access the pixel value with cv::Vec3b
			float b = (float)m_Img.at<Vec3b>(i,j)[0];
			float g = (float)m_Img.at<Vec3b>(i,j)[1];
			float r = (float)m_Img.at<Vec3b>(i,j)[2];
			for (int si = -NEIGHBORHOOD; si <= NEIGHBORHOOD && si + i < m_Img.rows && si + i >= 0 ; si++)
			{
				for (int sj = 0; sj <= NEIGHBORHOOD && sj + j < m_Img.cols ; sj++)

				{
					if ((si == 0 && sj == 0) ||
						(si == 1 && sj == 0) || 
						(si == NEIGHBORHOOD && sj == 0))
						continue;

					float nb = (float)m_Img.at<Vec3b>(i+si,j+sj)[0];
					float ng = (float)m_Img.at<Vec3b>(i+si,j+sj)[1];
					float nr = (float)m_Img.at<Vec3b>(i+si,j+sj)[2];

					varianceSquared+= (b-nb)*(b-nb) + (g-ng)*(g-ng) + (r-nr)*(r-nr); 
					counter ++;
				}
			}
		}
	}
	varianceSquared/=counter;
}

void OneCutSegmentor::showImage()
{
	imshow(m_WinName, showImg);
}