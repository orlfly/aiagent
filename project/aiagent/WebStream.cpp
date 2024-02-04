#include "WebStream.hpp"
#include <stdint.h>

namespace {
    WebStream* s_instance = NULL;
};

WebStream* WebStream::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new WebStream();
    }

    return s_instance;
}

void WebStream::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}
int WebStream::OpenVideo(AVCodec *codec){
    AVCodecContext *c = _st->codec;

    /* open the codec */
    int ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not open video codec. %s\n", avcodec_get_name(c->codec_id));
    }
    else {

        /* allocate and init a re-usable frame */
        _frame = av_frame_alloc();
        if (!_frame) {
            av_log(NULL, AV_LOG_ERROR, "Could not allocate video frame.\n");
            ret = -1;
        }
        else {
            _frame->format = c->pix_fmt;
            _frame->width = c->width;
            _frame->height = c->height;
	    ret = av_image_alloc(_frame->data, _frame->linesize, c->width, c->height, c->pix_fmt, 32);
	    if (ret < 0) {
	      av_log(NULL, AV_LOG_ERROR, "Could not allocate raw picture buffer\n");
	      return -1;
	    }
        }
    }

    return ret;
}
AVStream* WebStream::CreateStream(AVCodec **codec,int width, int height)
{
    AVCodecContext *c;
    AVStream *st=NULL;

    /* find the encoder */
    *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!(*codec)) {
        av_log(NULL, AV_LOG_ERROR, "Could not find encoder for '%s'.\n", avcodec_get_name(AV_CODEC_ID_H264));
    }
    else {
        st = avformat_new_stream(_c, *codec);
        if (!st) {
            av_log(NULL, AV_LOG_ERROR, "Could not allocate stream.\n");
        }
        else {
            st->id = _c->nb_streams - 1;
            st->time_base.den = 90000;
            st->time_base.num = 1;

            c = st->codec;
            c->codec_id = AV_CODEC_ID_H264;
            c->bit_rate = 400000;
            c->width = width;
            c->height = height;
            c->time_base.den = 25;
            c->time_base.num = 1;
            c->gop_size = 12; /* emit one intra frame every twelve frames at most */
            c->pix_fmt = AV_PIX_FMT_YUV420P;
	    /*remove b frames*/
	    c->max_b_frames = 0;
        }
    }

    return st;
  
}
void WebStream::Initialize(const char *url, int fps, int width, int height) {

    AVCodec *codec;
    av_register_all();
    avformat_network_init();

    avformat_alloc_output_context2(&_c, NULL, "rtsp", url);

    if (!_c) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate an output context for '%s'.\n", url);
        return;
    }
    
    if (!_c->oformat) {
        av_log(NULL, AV_LOG_FATAL, "Could not create the output format for '%s'.\n", url);
        return;
    }
    _st = CreateStream(&codec, width, height);

    if (_st) {
        av_log(NULL, AV_LOG_DEBUG, "Video stream codec %s.\n ", avcodec_get_name(_st->codec->codec_id));

        int ret = OpenVideo(codec);
        if (ret < 0) {
            av_log(NULL, AV_LOG_FATAL, "Open video stream failed.\n");
            return;
        }
    }
    else {
        av_log(NULL, AV_LOG_FATAL, "Add video stream for the codec '%s' failed.\n", avcodec_get_name(AV_CODEC_ID_H264));
        return;
    }
    av_dump_format(_c, 0, url, 1);

    int ret = avformat_write_header(_c, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to connect to RTSP server for '%s'.\n", url);
        return;
    }
}


void WebStream::Release() {
    if (_st) {
        avcodec_close(_st->codec);
        av_frame_free(&_frame);
    }

    avformat_free_context(_c);
}

void WebStream::Encoder(uint8_t *rgb) {
    AVCodecContext *c = _st->codec;
    const int in_linesize[1] = { 4 * c->width };
    _sws_context = sws_getCachedContext(_sws_context,
            c->width, c->height, AV_PIX_FMT_RGB32,
            c->width, c->height, AV_PIX_FMT_YUV420P,
            0, NULL, NULL, NULL);
    sws_scale(_sws_context, (const uint8_t * const *)&rgb, in_linesize, 0,
            c->height, _frame->data, _frame->linesize);
}

void WebStream::Write(uint8_t *rgb) {
    AVCodecContext *c = _st->codec;
    _frame->pts = _nframes;
    Encoder(rgb);
    AVPacket pkt = { 0 };
    av_init_packet(&pkt);
    
    int got_output;
    int ret = avcodec_encode_video2(c, &pkt, _frame, &got_output);
    _nframes++;
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error encoding frame\n");
        return;
    }
    if (got_output) {
        pkt.stream_index = _st->index;
	pkt.pts = av_rescale_q_rnd(pkt.pts, c->time_base, _st->time_base, AVRounding(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	ret = av_write_frame(_c, &pkt);

	if (ret < 0) {
	  av_log(NULL, AV_LOG_ERROR, "Error while writing video frame.\n");
	}
    }
}

WebStream::WebStream():
  _c(NULL),
  _nframes(0),
  _sws_context(NULL)
{
}

WebStream::~WebStream()
{

}

    

