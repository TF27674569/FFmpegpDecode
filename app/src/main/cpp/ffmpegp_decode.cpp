#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>


#define TAG "FFmpegp"

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,TAG,FORMAT,##__VA_ARGS__);


extern "C"{

//封装视频格式
#include "libavformat/avformat.h"
// 解码库
#include "libavcodec/avcodec.h"
// 缩放
#include "libswscale/swscale.h"

}


extern "C"
JNIEXPORT void JNICALL
Java_com_sample_decode_ffmpegp_VideoUtils_decodeYUV420P(JNIEnv *env, jclass type, jstring inPath_,
                                                        jstring outPath_) {
    const char *inPath = env->GetStringUTFChars(inPath_, 0);
    const char *outPath = env->GetStringUTFChars(outPath_, 0);

    /**
     * 思路：
     *  1 注册组建 av_register_all
     *  2 打开输入视频文件 avformat_open_input
     *  3 获取视频信息 avformat_find_stream_info
     *  4 获取视频解码器 avcodec_find_decoder (在此之前需要先获取到视频流信息)
     *  5 打开视频解码器 avcodec_open2
     *  6 一帧一帧读取压缩视频数据AVPacket 一帧一帧的视频包
     *   6.1 压缩视频包转YUV420P的包
     *   6.2 写入文件
     *  7 关闭释放资源
     */


    // 1 注册组建
    av_register_all();


    // 2 打开输入视频文件
    //  需要参数 1 AVFormatContext 封装格式上下文
    //          2 const char * 路径
    //          3 AVInputFormat 指定输入的封装格式。一般传NULL，由FFmpeg自行探测
    //          4 AVDictionary 其它参数设置。它是一个字典，用于参数传递，不传则写NULL。
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    if((avformat_open_input(&pFormatCtx,inPath,NULL,NULL))!=0){
        LOGE("打开文件失败  %s \n",inPath);
        return;
    }

    // 3 获取视频信息
    if (avformat_find_stream_info(pFormatCtx,NULL)<0){
        LOGE("获取视频信息失败\n");
        return;
    }

    // 4 获取视频解码器 for 循环找到视频流
    int videoStreamId = -1;

    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        // 判断流的类型是否是视频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStreamId = i;
            goto CHECK;
        }
    }

    CHECK:
    // 判断是否获取到了视频流
    if (videoStreamId == -1){
        LOGE("没有获取到视频流\n");
        return;
    }


    // 视频编码器上下文 包含了众多编解码器需要的参数信息
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStreamId]->codec;

    // 拿到流获取解码器
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);


    if (pCodec == NULL){
        LOGE("无法解码，视频是否加密\n");
        return;
    }


    // 5 打开解码器
    if (avcodec_open2(pCodecCtx,pCodec,NULL)<0){
        LOGE("解码器无法打开\n");
        return;
    }

    // 6 一帧一帧读取压缩视频数据AVPacket 一帧一帧的视频包
    // 需要一个 avpacket
    AVPacket *avPacket = (AVPacket*)(av_malloc(sizeof(AVPacket)));


    AVFrame *srcFrame = av_frame_alloc();

    // yuv为输出文件frame
    AVFrame *yuvFrame = av_frame_alloc();
    // 需要指定格式 画面大小才能 真正分配内存
    uint8_t *out_buff = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height));
    // 初始化缓冲区
    avpicture_fill((AVPicture *)(yuvFrame), out_buff, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    // 用于判断是否结束
    int got_picture_ptr = -1,frameCount = 0;

    // 像素格式转换上下文
    SwsContext *swsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,/*原宽高，原格式*/
                                        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,/*转码后的宽高，转码成YUV420P格式*/
                                        SWS_BILINEAR, //转换算法
                                        NULL,NULL,NULL);

    // 输出
    FILE *outFile = fopen(outPath,"wb");

    while (av_read_frame(pFormatCtx,avPacket)>=0){
        // AVPacket - > AVFrame
        // AVCodecContext 编码器上下文
        // AVFrame 编码后一帧的数据
        // int *got_picture_ptr 为0 表示没有数据了
        // AVPacket  读取到的数据包
        avcodec_decode_video2(pCodecCtx,srcFrame,&got_picture_ptr,avPacket);

        // 不为0 正在解码
        if (got_picture_ptr){
            // 将AVFrame -> YUV420P 像素格式转换
            // SwsContext 像素转换 缩放上下文
            // const uint8_t *const srcSlice 指向平面的指针的数组
            // int srcStride
            sws_scale(swsCtx,
                      srcFrame->data,srcFrame->linesize,0,srcFrame->height,/*目标资源*/
                      yuvFrame->data,yuvFrame->linesize/*yuv资源*/);

            // 一帧的大小
            size_t size = pCodecCtx->width*pCodecCtx->height;
            // 保存yuv Y：U：V =  4 ：1 ：1
            fwrite(yuvFrame->data[0],1,size,outFile);
            fwrite(yuvFrame->data[1],1,size/4,outFile);
            fwrite(yuvFrame->data[2],1,size/4,outFile);

            LOGI("解码第%d帧",frameCount++);
        }

        av_free_packet(avPacket);
    }


    fclose(outFile);
    av_frame_free(&srcFrame);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);



    env->ReleaseStringUTFChars(inPath_, inPath);
    env->ReleaseStringUTFChars(outPath_, outPath);
}