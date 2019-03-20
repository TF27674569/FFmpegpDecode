## ffmpegp 解码YUV420P

### 环境
&nbsp;　　1 AndroidStudio新建c++工程<br/>
&nbsp;　　2 工程main目录下新建jniLibs<br/>
&nbsp;　　3 新建cpu架构的目录arm下编译的所有arm平台都兼容<br/>
&nbsp;　　4 拷贝so到cpu架构下<br/>
&nbsp;　　5 拷贝头文件到cpp目录可以直接考入文件夹<br/>
&nbsp;　　6 配置cmakelist 注意cmakeList的路径，我的在cpp下<br/>
```java
cmake_minimum_required(VERSION 3.4.1)



add_library(
             ffmpegp_decode

             SHARED

             ffmpegp_decode.cpp )

#引入头文件
include_directories(${CMAKE_SOURCE_DIR}/include)

#引入第三方so
link_directories(${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/)




find_library(
              log-lib

              log )

# 编解码库
add_library( avcodec-56
        SHARED
        IMPORTED )
set_target_properties( avcodec-56
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavcodec-56.so)

#输出输入设备的支持
add_library( avdevice-56
        SHARED
        IMPORTED )
set_target_properties( avdevice-56
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavdevice-56.so)

#滤镜
add_library( avfilter-5
        SHARED
        IMPORTED )
set_target_properties( avfilter-5
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavfilter-5.so)


#数据封装格式的库
add_library( avformat-56
        SHARED
        IMPORTED )
set_target_properties( avformat-56
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavformat-56.so)


# 工具包
add_library( avutil-54
        SHARED
        IMPORTED )
set_target_properties( avutil-54
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavutil-54.so)


add_library( postproc-53
        SHARED
        IMPORTED )
set_target_properties( postproc-53
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libpostproc-53.so)


add_library( swresample-1
        SHARED
        IMPORTED )
set_target_properties( swresample-1
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libswresample-1.so)


#像素格式转换 缩放
add_library( swscale-3
        SHARED
        IMPORTED )
set_target_properties( swscale-3
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libswscale-3.so)


target_link_libraries( # Specifies the target library.
                       ffmpegp_decode

                       #链接第三方库
                       avcodec-56
                       avdevice-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       postproc-53
                       swresample-1
                       swscale-3

                       ${log-lib} )

```
&nbsp;　　7 修改build.gradle<br/>

```java
        cmake {
                # 根据需求添加cpu架构
                abiFilters "armeabi-v7a"
            }
```


### 整体流程图

![解码流程图](https://github.com/TF27674569/FFmpegpDecode/blob/master/image/ffmpegp_decode.jpg)

### 按流程编
&nbsp;　　1 注册组建 <br/>
&nbsp;　　2 打开输入视频文件 <br/>
&nbsp;　　3 获取视频信息 <br/>
&nbsp;　　4 获取视频解码器  (在此之前需要先获取到视频流信息) <br/>
&nbsp;　　5 打开视频解码器 <br/>
&nbsp;　　6 一帧一帧读取压缩视频数据AVPacket 一帧一帧的视频包 <br/>
&nbsp;　　&nbsp;　　6.1 压缩视频包转YUV420P的包 <br/>
&nbsp;　　&nbsp;　　6.2 写入文件 <br/>
&nbsp;　　7 关闭释放资源 <br/>

### 注意
&nbsp;　　1.CmakeLists.txt so库需要一个一个链接 <br/>
```python

# 编解码库
add_library( avcodec-56
        SHARED
        IMPORTED )
set_target_properties( avcodec-56
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavcodec-56.so)

```
&nbsp;　　2.注意权限与版本号 <br/>
```c
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>

    targetSdkVersion 21


     abiFilters "armeabi-v7a" //高版本NDK请注意cpu架构 有些不支持 armeabi

```
