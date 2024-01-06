#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <stdio.h>
#include <libswscale/swscale.h>
}

class WebStream
{
public:
    static WebStream* GetInstance();

    static void ReleaseInstance();

    void Initialize(const char *filename, int fps, int width, int height);

    void Release();

    void Write(uint8_t *rgb);

private:
    WebStream();

    ~WebStream();

    void Encoder(uint8_t *rgb);

    AVStream* CreateStream(AVCodec **codec,int width, int height);

    int OpenVideo(AVCodec *codec);

    AVFormatContext *_c;

    unsigned int _nframes;
    
    AVFrame *_frame;

    AVStream *_st;

    SwsContext *_sws_context;
};
