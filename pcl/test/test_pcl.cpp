#include <fstream>
#include <boost/timer.hpp>
#include <sl/Camera.hpp>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <thread>         // std::thread
#include <mutex>
#include <opencv2/opencv.hpp>//for display
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/viz.hpp> 
#include "pose_estimation_3d2d.h"
#include "pcl.h"

using namespace std;
using namespace cv;
using namespace sl;//这个必不可少！！！

sl::Camera zed;// Create a ZED camera object
std::thread zed_callback;


bool quit = false;

void startZED();
void run();
void close();
cv::Mat slMat2cvMat(sl::Mat& input);

int main(int argc, char **argv) {

    //Set configuration parameters
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION_VGA; // cost time: 0.04647
    //init_params.camera_resolution = RESOLUTION_HD720;//cost time: 0.086479
    init_params.camera_fps = 100; // Set fps at 60
    init_params.depth_mode=DEPTH_MODE_MEDIUM;

    // Open the camera
    ERROR_CODE err = zed.open(init_params);
    if (err != SUCCESS){
        std::cout << sl::errorCode2str(err) << std::endl;
        zed.close();
        return 1; // Quit if an error occurred
    }
    startZED();

//    cv::viz::Viz3d vis("Visual Odometry");
//    cv::viz::WCoordinateSystem world_coor(1.0), camera_coor(0.5);
//    cv::Point3d cam_pos( 0, -1.0, -1.0 ), cam_focal_point(0,0,0), cam_y_dir(0,1,0);
//    cv::Affine3d cam_pose = cv::viz::makeCameraPose( cam_pos, cam_focal_point, cam_y_dir );
//    vis.setViewerPose( cam_pose );

//    world_coor.setRenderingProperty(cv::viz::LINE_WIDTH, 2.0);
//    camera_coor.setRenderingProperty(cv::viz::LINE_WIDTH, 1.0);
//    vis.showWidget( "World", world_coor );
//    vis.showWidget( "Camera", camera_coor );

    return 0;
}

void startZED() {
    quit = false;
    zed_callback = std::thread(run);//与下一句的区别，体会一下
    //std::thread zed_callback (run);//就算usleep(1000000)也没用
    zed_callback.join();//必须要加，要不然导致新的线程在主线程之后结束。
    //zed_callback.detach();//分离，会报错。

}

void run()
{
    int i=0;
    RuntimeParameters init_grab;//设置grab()参数
    init_grab.enable_depth=true;
    init_grab.enable_point_cloud=false;

    cv::Mat frame_cur_rbg;
    cv::Mat frame_last_rbg;
    cv::Mat frame_cur_depth;
    cv::Mat frame_last_depth;
    PointCloud::Ptr cloud1;



    while(!quit)
    {
        cv::Mat frame_rbg,frame_depth;
        sl::Mat image_rgb,image_depth;
        if(i==0)
        {
            if(zed.grab(init_grab)==SUCCESS);
            {
                zed.retrieveImage(image_rgb, VIEW_LEFT);
                zed.retrieveImage(image_depth, VIEW_DEPTH);
                cv::cvtColor(slMat2cvMat(image_rgb),frame_rbg,CV_BGRA2BGR);//转换成３通道图
                cv::cvtColor(slMat2cvMat(image_depth),frame_depth,CV_BGRA2BGR);
                frame_cur_rbg=frame_rbg;
                frame_cur_depth=frame_depth;
            }
        }

        else
        {
            frame_last_rbg=frame_cur_rbg;
            frame_last_depth=frame_cur_depth;
            boost::timer time1;
            if(zed.grab(init_grab)==SUCCESS);
            {
                std::mutex mutex_input;
                mutex_input.lock();//加锁
                zed.retrieveImage(image_rgb, VIEW_LEFT);
                zed.retrieveImage(image_depth, VIEW_DEPTH);
                cv::cvtColor(slMat2cvMat(image_rgb),frame_rbg,CV_BGRA2BGR);//转换成３通道图
                cv::cvtColor(slMat2cvMat(image_depth),frame_depth,CV_BGRA2BGR);
                image2PointCloud(frame_rbg,frame_depth,cloud1);
                frame_cur_rbg=frame_rbg;
                frame_cur_depth=frame_depth;
                RESULT_OF_PNP result;
                pnp(frame_last_rbg,frame_cur_rbg,frame_cur_depth,result);
                //cout<<result.R<<endl;
                //cout<<result.tvec<<endl;

                mutex_input.unlock();

            }

           // sl::sleep_ms(30);
             i++;
             cout<<time1.elapsed()<<endl;
        }

        cout<<i++<<endl;
        cout<<zed.getCurrentFPS()<<endl;

    }
}
 

void close() {
    quit = true;
    zed_callback.join();
    zed.close();

}

//Conversion function between sl::Mat and cv::Mat
cv::Mat slMat2cvMat(sl::Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
        case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
        case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
        case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
        case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
        case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
        case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
        case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
        case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}

