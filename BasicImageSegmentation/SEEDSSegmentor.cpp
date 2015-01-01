#include "SEEDSSegmentor.h"

//IMPLEMENT_DYNCRT_CLASS(SEEDSSegmentor);

SEEDSSegmentor::SEEDSSegmentor(void)
{
	m_Name = "SEEDS";

	m_NumRegions = 266;

	m_argNum = 1;
}

SEEDSSegmentor::~SEEDSSegmentor(void)
{
}

void SEEDSSegmentor::SetArgs(const vector<float> _args)
{
	cout<<"["<<m_Name<<"] Getting arguments..."<<endl;

	float argu[] = {m_NumRegions};
	string argNames[] = {"NumRegions"};
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

	m_NumRegions = argu[0];

	stringstream ss;
	ss<<m_Name<<"_"<<m_NumRegions<<".txt";
	m_ResultName = ss.str();
}

void SEEDSSegmentor::Run()
{
	cout<<"====="<<m_Name<<" Runing..."<<endl;

	IplImage* img = new IplImage(m_Img);

	int width = img->width;
	int height = img->height;
	int sz = height*width;

	UINT* ubuff = new UINT[sz];
	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			cv::Vec3b bgr = m_Img.at<cv::Vec3b>(i, j);
			ubuff[i*img->width+j] = (bgr[2]<<16) | (bgr[1]<<8) | (bgr[0]);
		}
	}	

	int NR_BINS = 5; // Number of bins in each histogram channel

	SEEDS seeds(width, height, 3, NR_BINS);

	int nr_superpixels = m_NumRegions;
	// NOTE: the following values are defined for images from the BSD300 or BSD500 data set.
	// If the input image size differs from 480x320, the following values might no longer be 
	// accurate.
	// For more info on how to select the superpixel sizes, please refer to README.TXT.
	int seed_width = 3; int seed_height = 4; int nr_levels = 4;
	if (width >= height)
	{
		if (nr_superpixels == 600) {seed_width = 2; seed_height = 2; nr_levels = 4;}
		if (nr_superpixels == 400) {seed_width = 3; seed_height = 2; nr_levels = 4;}
		if (nr_superpixels == 266) {seed_width = 3; seed_height = 3; nr_levels = 4;}
		if (nr_superpixels == 200) {seed_width = 3; seed_height = 4; nr_levels = 4;}
		if (nr_superpixels == 150) {seed_width = 2; seed_height = 2; nr_levels = 5;}
		if (nr_superpixels == 100) {seed_width = 3; seed_height = 2; nr_levels = 5;}
		if (nr_superpixels == 50)  {seed_width = 3; seed_height = 4; nr_levels = 5;}
		if (nr_superpixels == 25)  {seed_width = 3; seed_height = 2; nr_levels = 6;}
		if (nr_superpixels == 17)  {seed_width = 3; seed_height = 3; nr_levels = 6;}
		if (nr_superpixels == 12)  {seed_width = 3; seed_height = 4; nr_levels = 6;}
		if (nr_superpixels == 9)  {seed_width = 2; seed_height = 2; nr_levels = 7;}
		if (nr_superpixels == 6)  {seed_width = 3; seed_height = 2; nr_levels = 7;}
	}
	else
	{
		if (nr_superpixels == 600) {seed_width = 2; seed_height = 2; nr_levels = 4;}
		if (nr_superpixels == 400) {seed_width = 2; seed_height = 3; nr_levels = 4;}
		if (nr_superpixels == 266) {seed_width = 3; seed_height = 3; nr_levels = 4;}
		if (nr_superpixels == 200) {seed_width = 4; seed_height = 3; nr_levels = 4;}
		if (nr_superpixels == 150) {seed_width = 2; seed_height = 2; nr_levels = 5;}
		if (nr_superpixels == 100) {seed_width = 2; seed_height = 3; nr_levels = 5;}
		if (nr_superpixels == 50)  {seed_width = 4; seed_height = 3; nr_levels = 5;}
		if (nr_superpixels == 25)  {seed_width = 2; seed_height = 3; nr_levels = 6;}
		if (nr_superpixels == 17)  {seed_width = 3; seed_height = 3; nr_levels = 6;}
		if (nr_superpixels == 12)  {seed_width = 4; seed_height = 3; nr_levels = 6;}
		if (nr_superpixels == 9)  {seed_width = 2; seed_height = 2; nr_levels = 7;}
		if (nr_superpixels == 6)  {seed_width = 2; seed_height = 3; nr_levels = 7;}
	}
	seeds.initialize(seed_width, seed_height, nr_levels);
	seeds.update_image_ycbcr(ubuff);
	seeds.iterate();
	//int n_Seeds = seeds.count_superpixels();

	int* labels = (int*)seeds.get_labels();

	for (int i = 0; i < img->height; i++)
	{
		int* ptr = m_Result.ptr<int>(i);
		for (int j = 0; j < img->width; j++)
		{
			ptr[j] = labels[i*img->width+j];
		}
	}

	delete[] ubuff;

	Segmentor::Run();
}