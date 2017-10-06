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
#include <opencv2/imgproc/imgproc.hpp>
#include <feature_extraction.h>

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
    // Set configuration parameters
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION_VGA; // cost time: 0.04647
    //init_params.camera_resolution = RESOLUTION_HD720;//cost time: 0.086479
    init_params.camera_fps = 30; // Set fps at 60

    // Open the camera
    ERROR_CODE err = zed.open(init_params);
    if (err != SUCCESS){
        std::cout << sl::errorCode2str(err) << std::endl;
        zed.close();
        return 1; // Quit if an error occurred
    }
startZED();

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
    int i = 0;
    sl::Mat sl_l_image,sl_r_image;//zed格式图像
    cv::Mat cv_l_image,cv_r_image;//opencv格式图像

    while (!quit) {
        // Grab an image
        if (zed.grab() == SUCCESS) {
            // A new image is available if grab() returns SUCCESS
            zed.retrieveImage(sl_l_image, VIEW_LEFT); // Get the left image
            zed.retrieveImage(sl_r_image, VIEW_RIGHT); //Get the right image
            unsigned long long timestamp = zed.getCameraTimestamp(); // Get the timestamp at the time the image was captured
            cv_l_image=slMat2cvMat(sl_l_image);
            cv_r_image=slMat2cvMat(sl_r_image);

            cvtColor(cv_l_image,cv_l_image,CV_BGRA2BGR);
            cvtColor(cv_r_image,cv_r_image,CV_BGRA2BGR);

            feature_extraction(cv_l_image,cv_r_image);


           // cv::imshow("VIEW_LEFT",cv_l_image);
           // cv::imshow("VIEW_RIGHT",cv_r_image);
            //filename1 << "/home/lumino/Workspace/zed-dev/data/" << "left" << setfill('0') << setw(6) << i << ".png";
            //filename2 << "/home/lumino/Workspace/zed-dev/data/" << "right" << setfill('0') << setw(6) << i << ".png";
            //cv::imwrite(filename1.str(), cv_l_image);//imwrite()特别慢,程序运行的时候一般不要用
            //cv::imwrite(filename2.str(), cv_r_image);//imwrite()特别慢,程序运行的时候一般不要用
            //cv::imwrite("Left_image.png",cv_l_image);
            //cv::imwrite("Right_image.png",cv_r_image);
            cv::waitKey(5);
            //printf("Image resolution: %d x %d  || Image timestamp: %llu\n", zed_image.getWidth(), zed_image.getHeight(), timestamp);
            std::cout<<i<<endl;
            i++;
        }
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
