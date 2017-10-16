#ifndef PCL_H
#define PCL_H

#include <vector>
#include <boost/timer.hpp>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//PCL
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include <pcl/common/transforms.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/filters/voxel_grid.h>

// Eigen !
#include <Eigen/Core>
#include <Eigen/Geometry>

// 类型定义
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;

// 相机内参
using namespace std;
const double camera_factor = 1000;
const double camera_cx = 325.1;
const double camera_cy = 249.7;
const double camera_fx = 520.9;
const double camera_fy = 521.0;

// 函数接口
// image2PonitCloud 将rgb图转换为点云
PointCloud::Ptr image2PointCloud(cv::Mat& rgb, cv::Mat& depth);

PointCloud::Ptr joinPointCloud(PointCloud::Ptr lastcloud, PointCloud::Ptr newCloud, Eigen::Isometry3d T);

PointCloud::Ptr joinPointCloud2( PointCloud::Ptr lastcloud, PointCloud::Ptr newCloud, Eigen::Isometry3d T);
#endif
