#在这里生成共享库而不是可执行文件
#即使是生成可执行文件也是在这里add_....
add_library( pose_estimation_2d2d SHARED
    pose_estimation_2d2d.cpp
    feature_extraction.cpp
)

target_link_libraries( pose_estimation_2d2d ${OpenCV_LIBS} )
