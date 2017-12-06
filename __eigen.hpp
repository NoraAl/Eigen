#ifndef EIGEN_H
#define EIGEN_H

#include <iostream>
#include <fstream>
#include <sstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "_framerec.hpp"
#include "__colors.hpp"


using namespace cv;
using namespace std;


Mat norm_0_255(InputArray _src);
void readCsv( vector<Mat>& images, vector<int>& labels, char separator = ',');

#endif