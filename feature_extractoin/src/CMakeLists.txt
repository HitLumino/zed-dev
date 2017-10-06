#在这里生成共享库而不是可执行文件
#即使是生成可执行文件也是在这里add_....
add_library( feature_extraction SHARED
    feature_extraction.cpp
)

target_link_libraries( feature_extraction ${OpenCV_LIBS} )
