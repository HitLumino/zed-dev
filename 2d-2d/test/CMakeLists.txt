add_executable( test 2d-2d.cpp )
target_link_libraries( test pose_estimation_2d2d )

TARGET_LINK_LIBRARIES(test
                        ${ZED_LIBRARIES}
                        #${OpenCV_LIBRARIES}
                        ${SPECIAL_OS_LIBS}#这个不加编译不了
                        ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_npp_LIBRARY} )
