#include <jni.h>
#include <string>
#include<android/log.h>
#include <exception>
#include <iostream>
#include <sstream>

//定义日志宏变量
#define logw(content)   __android_log_write(ANDROID_LOG_WARN,"eric",content)
#define loge(content)   __android_log_write(ANDROID_LOG_ERROR,"eric",content)
#define logd(content)   __android_log_write(ANDROID_LOG_DEBUG,"eric",content)

extern "C"
{


#include <libavcodec/codec.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>


using namespace std;

JNIEXPORT jstring JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_avcodecinfo(JNIEnv *env, jobject instance) {

    char info[40000] = {0};

    void *item = nullptr;
    for (;;) {
        const AVCodec *cur = av_codec_iterate(&item);
        if (!cur)
            break;
        if (av_codec_is_decoder(cur) != 0) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (cur->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, cur->name);
    }

    return env->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_urlprotocolinfo(JNIEnv *env, jobject instance) {
    char info[40000] = {0};
    struct URLProtocol *pup = nullptr;
    struct URLProtocol **p_temp = &pup;
    avio_enum_protocols((void **) p_temp, 0);
    while ((*p_temp) != nullptr) {
        sprintf(info, "%sInput: %s\n", info, avio_enum_protocols((void **) p_temp, 0));
    }
    pup = nullptr;
    avio_enum_protocols((void **) p_temp, 1);
    while ((*p_temp) != nullptr) {
        sprintf(info, "%sInput: %s\n", info, avio_enum_protocols((void **) p_temp, 1));
    }

    return env->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_avformatinfo(JNIEnv *env, jobject instance) {
    char info[40000] = {0};
    void *inputItem = nullptr;
    for (;;) {
        const AVInputFormat *inputCur = av_demuxer_iterate(&inputItem);
        if (!inputCur)
            break;
        sprintf(info, "%sInput: %s\n", info, inputCur->name);
    }

    void *outputItem = nullptr;
    for (;;) {
        const AVOutputFormat *outputCur = av_muxer_iterate(&outputItem);
        if (!outputCur)
            break;
        sprintf(info, "%sOutput: %s\n", info, outputCur->name);
    }
    return env->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_avfilterinfo(JNIEnv *env, jobject instance) {
    char info[40000] = {0};
    void *item = nullptr;
    for (;;) {
        const AVFilter *cur = av_filter_iterate(&item);
        if (!cur)
            break;
        sprintf(info, "%s%s\n", info, cur->name);
    }
    return env->NewStringUTF(info);
}

int avError(int errNum) {
    char buf[1024];
    //获取错误信息
    av_strerror(errNum, buf, sizeof(buf));
    loge(string().append("发生异常：").append(buf).c_str());
    return -1;
}

jobject pushCallback = NULL;
jclass cls = NULL;
jmethodID mid = NULL;
int callback(JNIEnv *env, int64_t pts, int64_t dts, int64_t duration, long long index) {
//    logw("=================")
    if (pushCallback == NULL) {
        return -3;
    }
    if (cls == NULL) {
        return -1;
    }
    if (mid == NULL) {
        return -2;
    }
    env->CallVoidMethod(pushCallback, mid, (jlong) pts, (jlong) dts, (jlong) duration,
                        (jlong) index);
    return 0;
}

/**
 * 设置回到对象
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_setCallback(JNIEnv *env, jobject instance,
                                                            jobject pushCallback1) {
    //转换为全局变量
    pushCallback = env->NewGlobalRef(pushCallback1);
    if (pushCallback == NULL) {
        return -3;
    }
    cls = env->GetObjectClass(pushCallback);
    if (cls == NULL) {
        return -1;
    }
    mid = env->GetMethodID(cls, "videoCallback", "(JJJJ)V");
    if (mid == NULL) {
        return -2;
    }
    env->CallVoidMethod(pushCallback, mid, (jlong) 0, (jlong) 0, (jlong) 0, (jlong) 0);
    return 0;
}

JNIEXPORT int JNICALL
Java_com_xwl_ffmpegdemo_MainActivity_pushRtspFile(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    logw(path);
    int videoindex = -1;
    avformat_network_init();//初始化网络库
    const char *inUrl = path;
    //输出的地址
    const char *outUrl = "rtmp://192.168.3.9:554/live";

    //////////////////////////////////////////////////////////////////
    //                   输入流处理部分
    /////////////////////////////////////////////////////////////////
    //打开文件，解封装 avformat_open_input
    //AVFormatContext **ps  输入封装的上下文。包含所有的格式内容和所有的IO。如果是文件就是文件IO，网络就对应网络IO
    //const char *url  路径
    //AVInputFormt * fmt 封装器
    //AVDictionary ** options 参数设置
    AVFormatContext *ictx = NULL;

    AVFormatContext *octx = NULL;
    AVPacket pkt;
    int ret = 0;
    try {
        //打开文件，解封文件头
        ret = avformat_open_input(&ictx, inUrl, 0, NULL);
        if (ret < 0) {
            avError(ret);
            throw ret;
        }
        //打印视频视频信息
        //0打印所有  inUrl 打印时候显示，
        av_dump_format(ictx, 0, inUrl, 0);
        //////////////////////////////////////////////////////////////////
        //                   输出流处理部分
        /////////////////////////////////////////////////////////////////
        //如果是输入文件 flv可以不传，可以从文件中判断。如果是流则必须传
        //创建输出上下文
        ret = avformat_alloc_output_context2(&octx, NULL, "flv", outUrl);
        if (ret < 0) {
            avError(ret);
            throw ret;
        }
        logd("avformat_alloc_output_context2 success!");

        int i;

        for (i = 0; i < ictx->nb_streams; i++) {

            //获取输入视频流
            AVStream *in_stream = ictx->streams[i];
            //为输出上下文添加音视频流（初始化一个音视频流容器）
            const AVCodec *in_codec = avcodec_find_decoder(in_stream->codecpar->codec_id);
            AVStream *out_stream = avformat_new_stream(octx, in_codec);
            if (!out_stream) {
                printf("未能成功添加音视频流\n");
                ret = AVERROR_UNKNOWN;
            }
            if (octx->oformat->flags & AVFMT_GLOBALHEADER) {
                const AVCodec *out_codec = avcodec_find_decoder(out_stream->codecpar->codec_id);
                avcodec_alloc_context3(out_codec)->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                printf("copy 编解码器上下文失败\n");
            }
            out_stream->codecpar->codec_tag = 0;
//        out_stream->codec->codec_tag = 0;
        }
        //找到视频流的位置
        for (i = 0; i < ictx->nb_streams; i++) {
            if (ictx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoindex = i;
                break;
            }
        }

        av_dump_format(octx, 0, outUrl, 1);
        //////////////////////////////////////////////////////////////////
        //                   准备推流
        /////////////////////////////////////////////////////////////////

        //打开IO
        ret = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
        if (ret < 0) {
            avError(ret);
            throw ret;
        }
        logd("avio_open success!");
        //写入头部信息
        ret = avformat_write_header(octx, 0);
        if (ret < 0) {
            avError(ret);
            throw ret;
        }
        logd("avformat_write_header Success!");
        //推流每一帧数据
        //int64_t pts  [ pts*(num/den)  第几秒显示]
        //int64_t dts  解码时间 [P帧(相对于上一帧的变化) I帧(关键帧，完整的数据) B帧(上一帧和下一帧的变化)]  有了B帧压缩率更高。
        //获取当前的时间戳  微妙
        long long start_time = av_gettime();
        int frame_index = 0;
        logd("start push >>>>>>>>>>>>>>>");
        while (1) {
            //输入输出视频流
            AVStream *in_stream, *out_stream;
            //获取解码前数据
            ret = av_read_frame(ictx, &pkt);
            if (ret < 0) {
                break;
            }
            /*
            PTS（Presentation Time Stamp）显示播放时间
            DTS（Decoding Time Stamp）解码时间
            */
            //没有显示时间（比如未解码的 H.264 ）
            if (pkt.pts == AV_NOPTS_VALUE) {
                //AVRational time_base：时基。通过该值可以把PTS，DTS转化为真正的时间。
                AVRational time_base1 = ictx->streams[videoindex]->time_base;

                //计算两帧之间的时间
                /*
                r_frame_rate 基流帧速率  （不是太懂）
                av_q2d 转化为double类型
                */
                int64_t calc_duration =
                        (double) AV_TIME_BASE / av_q2d(ictx->streams[videoindex]->r_frame_rate);

                //配置参数
                pkt.pts = (double) (frame_index * calc_duration) /
                          (double) (av_q2d(time_base1) * AV_TIME_BASE);
                pkt.dts = pkt.pts;
                pkt.duration =
                        (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
            }
            //延时
            if (pkt.stream_index == videoindex) {
                AVRational time_base = ictx->streams[videoindex]->time_base;
                AVRational time_base_q = {1, AV_TIME_BASE};
                //计算视频播放时间
                int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
                //计算实际视频的播放时间
                int64_t now_time = av_gettime() - start_time;

                AVRational avr = ictx->streams[videoindex]->time_base;
                cout << avr.num << " " << avr.den << "  " << pkt.dts << "  " << pkt.pts << "   "
                     << pts_time << endl;
                if (pts_time > now_time) {
                    //睡眠一段时间（目的是让当前视频记录的播放时间与实际时间同步）
                    av_usleep((unsigned int) (pts_time - now_time));
                }
            }

            in_stream = ictx->streams[pkt.stream_index];
            out_stream = octx->streams[pkt.stream_index];

            //计算延时后，重新指定时间戳
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = (int) av_rescale_q(pkt.duration, in_stream->time_base,
                                              out_stream->time_base);
//        __android_log_print(ANDROID_LOG_WARN, "eric", "duration %d", pkt.duration);
            //字节流的位置，-1 表示不知道字节流位置
            pkt.pos = -1;

            if (pkt.stream_index == videoindex) {
                printf("Send %8d video frames to output URL\n", frame_index);
                frame_index++;
            }
            //回调数据
            callback(env, pkt.pts, pkt.dts, pkt.duration, frame_index);
            //向输出上下文发送（向地址推送）
            ret = av_interleaved_write_frame(octx, &pkt);

            if (ret < 0) {
                printf("发送数据包出错\n");
                break;
            }
            //释放
            av_packet_unref(&pkt);
        }
        ret = 0;
    } catch (int errNum) {
        loge(reinterpret_cast<const char *>(errNum));
    }
    logd("finish===============");
    //关闭输出上下文，这个很关键。
    if (octx != NULL)
        avio_close(octx->pb);
    //释放输出封装上下文
    if (octx != NULL)
        avformat_free_context(octx);
    //关闭输入上下文
    if (ictx != NULL)
        avformat_close_input(&ictx);
    octx = NULL;
    ictx = NULL;
    env->ReleaseStringUTFChars(path_, path);
    return ret;
}
}