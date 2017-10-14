add_executable( test test_pcl.cpp )
target_link_libraries( test pose_estimation_3d2d )

TARGET_LINK_LIBRARIES(test
                        ${ZED_LIBRARIES}
                        ${OpenCV_LIBRARIES}
                        ${PCL_LIBRARIES}
                        ${SPECIAL_OS_LIBS}#这个不加编译不了
                        ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_npp_LIBRARY}
                        ${CSPARSE_LIBRARY}
                        g2o_core g2o_stuff g2o_types_sba g2o_csparse_extension )
