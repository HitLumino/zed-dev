#在这里生成共享库而不是可执行文件
#即使是生成可执行文件也是在这里add_....
add_library( slam_base SHARED
    pose_estimation_3d2d.cpp
    pcl.cpp
    
)
target_link_libraries( slam_base 
   ${OpenCV_LIBS}
   ${CSPARSE_LIBRARY}
   ${PCL_LIBRARIES}
   g2o_core g2o_stuff g2o_types_sba g2o_csparse_extension
)
