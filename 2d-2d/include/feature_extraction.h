#ifndef FEATURE_EXTRACTION_H
#define FEATURE_EXTRACTION_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/timer.hpp>

using namespace cv;
void feature_extraction(Mat &img_1,Mat &img_2);

#endif // FEATURE_EXTRACTION_H
