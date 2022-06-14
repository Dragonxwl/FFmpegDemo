#include <jni.h>
#include <string>

extern "C"
{


#include <libavcodec/codec.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>

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


}