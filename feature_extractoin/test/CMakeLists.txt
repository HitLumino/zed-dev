add_executable( test test_feature.cpp )
target_link_libraries( test feature_extraction )
#很重要！！！
TARGET_LINK_LIBRARIES(test
                        ${ZED_LIBRARIES}
                        ${OpenCV_LIBRARIES}
                        ${SPECIAL_OS_LIBS}#这个不加编译不了
                        ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_npp_LIBRARY} )
