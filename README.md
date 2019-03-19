##ffmpegp 解码YUV420P

### 整体流程图

![解码流程图](https://github.com/TF27674569/FFmpegpDecode/blob/master/image/ffmpegp_decode.jpg)

### 按流程编
&nbsp;　　1 注册组建
&nbsp;　　2 打开输入视频文件
&nbsp;　　3 获取视频信息
&nbsp;　　4 获取视频解码器  (在此之前需要先获取到视频流信息)
&nbsp;　　5 打开视频解码器
&nbsp;　　6 一帧一帧读取压缩视频数据AVPacket 一帧一帧的视频包
&nbsp;　　&nbsp;　　6.1 压缩视频包转YUV420P的包
&nbsp;　　&nbsp;　　6.2 写入文件
&nbsp;　　7 关闭释放资源