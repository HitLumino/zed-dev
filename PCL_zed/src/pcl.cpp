#include "pcl.h"

PointCloud::Ptr image2PointCloud( cv::Mat& rgb, cv::Mat& depth )
{
    PointCloud::Ptr cloud ( new PointCloud );

    for (int m = 0; m < depth.rows; m++)
        for (int n=0; n < depth.cols; n++)
        {
            // 获取深度图中(m,n)处的值
            ushort d = depth.ptr<ushort>(m)[n];
            // d 可能没有值，若如此，跳过此点
            if (d == 0)
                continue;
            // d 存在值，则向点云增加一个点
            PointT p;

            // 计算这个点的空间坐标
            p.z = double(d) / camera_factor;
            p.x = (n - camera_cx) * p.z / camera_fx;
            p.y = (m - camera_cy) * p.z / camera_fy;

            // 从rgb图像中获取它的颜色
            // rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
            p.b = rgb.ptr<uchar>(m)[n*3];
            p.g = rgb.ptr<uchar>(m)[n*3+1];
            p.r = rgb.ptr<uchar>(m)[n*3+2];

            // 把p加入到点云中
            cloud->points.push_back( p );
        }
    // 设置并保存点云
    cloud->height = 1;
    cloud->width = cloud->points.size();
    cloud->is_dense = false;
    return cloud;
}

PointCloud::Ptr joinPointCloud( PointCloud::Ptr lastcloud, PointCloud::Ptr newCloud, Eigen::Isometry3d T)
{

    PointCloud::Ptr output (new PointCloud());
    pcl::transformPointCloud( *lastcloud, *output, T.matrix() );

    *newCloud += *output;

//     Voxel grid 滤波降采样
//    static pcl::VoxelGrid<PointT> voxel;
//    voxel.setLeafSize( 0.05, 0.05, 0.05 );
//    voxel.setInputCloud( newCloud );
//    PointCloud::Ptr tmp( new PointCloud() );
//    voxel.filter( *tmp );
//    return tmp;
    return newCloud;

}

PointCloud::Ptr joinPointCloud2( PointCloud::Ptr lastcloud, PointCloud::Ptr newCloud, Eigen::Isometry3d T)
{
    PointCloud::Ptr output (new PointCloud());
    pcl::transformPointCloud( *newCloud, *output, T.inverse().matrix() );

    *lastcloud+=*output;
    //     Voxel grid 滤波降采样
        static pcl::VoxelGrid<PointT> voxel;
        voxel.setLeafSize( 0.05, 0.05, 0.05 );
        voxel.setInputCloud( lastcloud );
        PointCloud::Ptr tmp( new PointCloud() );
        voxel.filter( *tmp );
        return tmp;
   // return lastcloud;
}

