// ****************************************************************************** 
// SEEDS Superpixels
// ******************************************************************************
// Author: Beat Kueng based on Michael Van den Bergh's code
// Contact: vamichae@vision.ee.ethz.ch
//
// This code implements the superpixel method described in:
// M. Van den Bergh, X. Boix, G. Roig, B. de Capitani and L. Van Gool, 
// "SEEDS: Superpixels Extracted via Energy-Driven Sampling",
// ECCV 2012
// 
// Copyright (c) 2012 Michael Van den Bergh (ETH Zurich). All rights reserved.
// ******************************************************************************

#if !defined(_SEEDS_H_INCLUDED_)
#define _SEEDS_H_INCLUDED_

#include <string>

using namespace std;

typedef unsigned int UINT;


class SEEDS  
{
public:
	SEEDS(int width, int height, int nr_channels, int nr_bins);
	~SEEDS();

	// initialize. this needs to be called only once
	//the number of superpixels is:
	// (image_width/seeds_w/(2^(nr_levels-1))) * (image_height/seeds_h/(2^(nr_levels-1)))
	void initialize(int seeds_w, int seeds_h, int nr_levels);
	
	//set a new image in YCbCr format
	//image must have the same size as in the constructor was given
	void update_image_ycbcr(UINT* image);
	
	// go through iterations
	void iterate();

	UINT* get_labels() { return labels[seeds_top_level]; }
	// output labels
	UINT** labels;	 //[level][y * width + x]

	// evaluation
	int count_superpixels();
	void SaveLabels_Text(string filename);

	// mean colors
	void compute_mean_map();
	UINT* means;

private:
	void deinitialize();
	
	bool initialized;

	// seeds	
	int seeds_w;
	int seeds_h;
	int seeds_nr_levels;
	int seeds_top_level; // == seeds_nr_levels-1 (const)
	int seeds_current_level; //start with level seeds_top_level-1, then go down

   	// image processing
	float* image_l; //input image
	float* image_a;
	float* image_b;
	UINT* image_bins; //bin index (histogram) of each image pixel [y*width + x]
	float* bin_cutoff1; //for each bin: define upper limit of L value used to calc bin index from color value
	float* bin_cutoff2; //for each bin: define upper limit of a value
	float* bin_cutoff3; //for each bin: define upper limit of b value

	// keep one labeling for each level
	UINT* nr_labels; //[level]
	UINT** parent; //[level][label] = corresponding label of block with level+1
	UINT** nr_partitions; //[level][label] how many partitions label has on level-1
	int** T; //[level][label] how many pixels with this label
	int go_down_one_level();

	// initialization
	void assign_labels();
	void compute_histograms(int until_level = -1);
	void compute_means();
	//void lab_get_histogram_cutoff_values(const Image& image);
	
	// color conversion and histograms
	int RGB2HSV(const int& r, const int& g, const int& b, float* hval, float* sval, float* vval);
	int RGB2LAB(const int& r, const int& g, const int& b, float* lval, float* aval, float* bval);
	int LAB2bin(float l, float a, float b);
	int RGB2LAB_special(int r, int g, int b, float* lval, float* aval, float* bval);
	int RGB2LAB_special(int r, int g, int b, int* bin_l, int* bin_a, int* bin_b);
	void LAB2RGB(float L, float a, float b, int* R, int* G, int* B);

	int histogram_size; //= nr_bins ^ 3 (3 channels)
	int*** histogram; //[level][label][j < histogram_size]
	

  void update(int level, int label_new, int x, int y);
	void add_pixel(int level, int label, int x, int y);
	void add_pixel_m(int level, int label, int x, int y);
	void delete_pixel(int level, int label, int x, int y);
	void delete_pixel_m(int level, int label, int x, int y);
	void add_block(int level, int label, int sublevel, int sublabel);
	void delete_block(int level, int label, int sublevel, int sublabel);
	void update_labels(int level);


	// probability computation
	bool probability(int color, int label1, int label2, int prior1, int prior2);
	bool probability_means(float L, float a, float b, int label1, int label2, int prior1, int prior2);

	int threebythree(int x, int y, UINT label);
	int threebyfour(int x, int y, UINT label);
	int fourbythree(int x, int y, UINT label);


	// block updating
	void update_blocks(int level, float req_confidence = 0.0);
	float intersection(int level1, int label1, int level2, int label2);

	// border updating
	void update_pixels();
	void update_pixels_means();
	bool forwardbackward;
	int threebythree_upperbound;
	int threebythree_lowerbound;

	inline bool check_split(int a11, int a12, int a13, int a21, int a22, 
			int a23, int a31, int a32, int a33, bool horizontal, bool forward);

	int* nr_w; //[level] number of seeds in x-direction
	int* nr_h;

	float* L_channel;
	float* A_channel;
	float* B_channel;
	int width, height, nr_channels, nr_bins;
};




#endif // !defined(_SEEDS_H_INCLUDED_)
