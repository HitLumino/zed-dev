#ifndef POSE_ESTIMATION_3D2D_H
#define POSE_ESTIMATION_3D2D_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <chrono>

#include <g2o/core/base_vertex.h>
#include <g2o/core/base_unary_edge.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/solvers/csparse/linear_solver_csparse.h>
#include <g2o/types/sba/types_six_dof_expmap.h>

using namespace std;
using namespace cv;

struct RESULT_OF_PNP
{
    cv::Affine3d::Mat3 R;
    cv::Affine3d::Vec3 tvec;
    Mat inliers;
};

void find_feature_matches (
    const Mat& img_1, const Mat& img_2,
    std::vector<KeyPoint>& keypoints_1,
    std::vector<KeyPoint>& keypoints_2,
    std::vector< DMatch >& matches );

// 像素坐标转相机归一化坐标
Point2d pixel2cam ( const Point2d& p, const Mat& K );
void pnp(Mat &rgb_image1, Mat &rgb_image2, Mat &depth_image, RESULT_OF_PNP &result);
void bundleAdjustment (const vector<Point3f> points_3d,
    const vector<Point2f> points_2d,
    const Mat& K,
    cv::Affine3d::Mat3& R,
    cv::Affine3d::Vec3& t
);

#endif

