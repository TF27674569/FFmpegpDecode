## ffmpegp 解码YUV420P

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
