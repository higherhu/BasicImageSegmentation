BasicImageSegmentation
======================

Some basic image segmentation algorithms, include mean-shift, graph-based, SLIC, SEEDS, GrabCut, OneCut, et al.

Usage
======================
1. Write the algorithms you need to the file "config.ini"

2. Use command "BasicImageSegmentation [ImageName]", or drag the image to the exe file directly. If you drag image to the exe, you need to ensure the config.ini and the image in the same directory.

3. For the interactive algorithms, such as GrabCut and OneCut, you need to scribble on the image and press 
keyboards according to the output tips.

4. After each algorithms, the program would output the segmentation result as a txt file. It's a two-dimensional matrix 
indicates the region id of each pixel.

About the "config.ini"
======================
1. The program can read this file and get the algorithm names and arguments want to use.

2. Each configuration item contains a algorithm name in a line and several arguments(can be zero) all in the following line. 
The algorithm name is surround by a bracket[].

3. This file can contain multiple algorithms. 

4. "config_example.ini" is an example. 
