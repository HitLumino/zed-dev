///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2017, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// Standard includes
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <thread>         // std::thread
#include <mutex>

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>//for display
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/viz.hpp>
#include "pose_estimation_2d2d.h"
#include "feature_extraction.h"

using namespace sl;
using namespace std;

sl::Camera zed;// Create a ZED camera object
std::thread zed_callback;
using namespace cv;

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
    init_params.camera_fps = 60; // Set fps at 60
    init_params.depth_mode=DEPTH_MODE_NONE;

    // Open the camera
    ERROR_CODE err = zed.open(init_params);
    if (err != SUCCESS){
        std::cout << sl::errorCode2str(err) << std::endl;
        zed.close();
        return 1; // Quit if an error occurred
    }
    cv::viz::Viz3d vis("Visual Odometry");
        cv::viz::WCoordinateSystem world_coor(1.0), camera_coor(0.5);
        cv::Point3d cam_pos( 0, -1.0, -1.0 ), cam_focal_point(0,0,0), cam_y_dir(0,1,0);
        cv::Affine3d cam_pose = cv::viz::makeCameraPose( cam_pos, cam_focal_point, cam_y_dir );
        vis.setViewerPose( cam_pose );

        world_coor.setRenderingProperty(cv::viz::LINE_WIDTH, 2.0);
        camera_coor.setRenderingProperty(cv::viz::LINE_WIDTH, 1.0);
        vis.showWidget( "World", world_coor );
        vis.showWidget( "Camera", camera_coor );

    startZED();


//   while(!quit)
//   {
//       sl::Mat sl_l_image1;
//       cv::Mat cv_l_image1;
//       cv::Mat frame_cur1;
//       cv::Mat frame_last1;
//    if(zed.grab() == SUCCESS)
//    {

//        zed.retrieveImage(sl_l_image1, VIEW_LEFT); // Get the left image
//        cvtColor(slMat2cvMat(sl_l_image1),cv_l_image1,CV_BGRA2BGR);//转换成３通道图
//        frame_last1=cv_l_image1.clone();
//        //imwrite("last.png",frame_last1);
//    }

//    usleep(3000);
//    if(zed.grab() == SUCCESS)
//    {
//        zed.retrieveImage(sl_l_image1, VIEW_LEFT); // Get the left image

//        cvtColor(slMat2cvMat(sl_l_image1),cv_l_image1,CV_BGRA2BGR);//转换成３通道图

//        frame_cur1=cv_l_image1.clone();
//        //imwrite("cur.png",frame_cur1);
//    }
//    feature_extraction(frame_last1,frame_cur1);
//}


//usleep(1000000);//等待10e6微秒  报错：terminate called without an active exception

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
    init_grab.enable_depth=false;
    init_grab.enable_point_cloud=false;

    sl::Mat sl_l_image;
    cv::Mat cv_l_image;
    cv::Mat frame_cur;
    cv::Mat frame_last;

    cv::viz::Viz3d vis("Visual Odometry");
        cv::viz::WCoordinateSystem world_coor(1.0), camera_coor(0.5);
        cv::Point3d cam_pos( 0, -1.0, -1.0 ), cam_focal_point(0,0,0), cam_y_dir(0,1,0);
        cv::Affine3d cam_pose = cv::viz::makeCameraPose( cam_pos, cam_focal_point, cam_y_dir );
        vis.setViewerPose( cam_pose );

        world_coor.setRenderingProperty(cv::viz::LINE_WIDTH, 2.0);
        camera_coor.setRenderingProperty(cv::viz::LINE_WIDTH, 1.0);
        vis.showWidget( "World", world_coor );
        vis.showWidget( "Camera", camera_coor );

    while(!quit)
    {

        if(i==0)
        {
            if(zed.grab(init_grab)==SUCCESS);
            {
                cv::Mat frame;
                sl::Mat image;
                zed.retrieveImage(image, VIEW_LEFT);
                cvtColor(slMat2cvMat(image),frame,CV_BGRA2BGR);//转换成３通道图
                frame_cur=frame;
            }
        }

        else
        {
            frame_last=frame_cur;
            if(zed.grab(init_grab)==SUCCESS);
            {
                cv::Mat frame;
                sl::Mat image;
                zed.retrieveImage(image, VIEW_LEFT);
                cvtColor(slMat2cvMat(image),frame,CV_BGRA2BGR);//转换成３通道图
                frame_cur=frame;
                feature_extraction(frame_last,frame_cur);

                vector<KeyPoint> keypoints_1, keypoints_2;
                vector<DMatch> matches;
                find_feature_matches(frame_last,frame_cur,keypoints_1, keypoints_2,matches);
                cout<<"一共找到了"<<matches.size() <<"组匹配点"<<endl;


                cv::Mat R,t;
                pose_estimation_2d2d ( keypoints_1, keypoints_2, matches, R, t );
                cv::Mat t_x = ( Mat_<double> ( 3,3 ) <<
                    0,                      -t.at<double> ( 2,0 ),     t.at<double> ( 1,0 ),
                    t.at<double> ( 2,0 ),      0,                      -t.at<double> ( 0,0 ),
                    -t.at<double> ( 1.0 ),     t.at<double> ( 0,0 ),      0 );
                cv::Affine3d M(R,t);
                vis.setWidgetPose( "Camera", M);
                vis.spinOnce(1, false);

            }

            sl::sleep_ms(30);
             i++;
        }

        cout<<i++<<endl;

    }
}
 /*
         if(zed.grab(init_grab) == SUCCESS)
         {

             boost::timer timer;

             zed.retrieveImage(sl_l_image, VIEW_LEFT); // Get the left image
             cvtColor(slMat2cvMat(sl_l_image),cv_l_image,CV_BGRA2BGR);//转换成３通道图
             frame_last=cv_l_image.clone();
             imshow("last",frame_last);
             waitKey(1);
             cout<<"cost timer"<<timer.elapsed()<<endl;


         }
         usleep(30000);
         if(zed.grab() == SUCCESS)
         {
             boost::timer timer;

             zed.retrieveImage(sl_l_image, VIEW_LEFT); // Get the left image
             cvtColor(slMat2cvMat(sl_l_image),cv_l_image,CV_BGRA2BGR);//转换成３通道图
             frame_cur=cv_l_image.clone();
             imshow("cur",frame_cur);
             waitKey(1);
             cout<<"cost timer"<<timer.elapsed()<<endl;

         }

         feature_extraction(frame_last,frame_cur);

         cout<<i++<<endl;
         */



/*
            vector<KeyPoint> keypoints_1, keypoints_2;
            vector<DMatch> matches;
            find_feature_matches(frame_last,frame_cur,keypoints_1, keypoints_2,matches);
            cout<<"一共找到了"<<matches.size() <<"组匹配点"<<endl;
            
            
            cv::Mat R,t;
            pose_estimation_2d2d ( keypoints_1, keypoints_2, matches, R, t );
            cv::Mat t_x = ( Mat_<double> ( 3,3 ) <<
                0,                      -t.at<double> ( 2,0 ),     t.at<double> ( 1,0 ),
                t.at<double> ( 2,0 ),      0,                      -t.at<double> ( 0,0 ),
                -t.at<double> ( 1.0 ),     t.at<double> ( 0,0 ),      0 );
*/
            //cout<<"t^R="<<endl<<t_x*R<<endl;
            //imshow("cur",frame_cur);
           // imshow("last",frame_last);

           // feature_extraction(frame_last,frame_cur);

            
	    //cv::waitKey(5);





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
