#include "XDecode.h"
extern "C"
{
    #include<libavcodec/avcodec.h>
}
#include <iostream>

using namespace std;

void XDecode::Close()
{
    mux.lock();
    if (codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mux.unlock();
}

void XDecode::Clear()
{
    mux.lock();
    //clear decoding buffer
    if (codec)
        avcodec_flush_buffers(codec);
    mux.unlock();
}

bool XDecode::Open(AVCodecParameters* para)
{
    Close();
	if (!para) return false;

    //find video decoder
    AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
    if (!vcodec)
    {
        avcodec_parameters_free(&para);
        cout << "cannot find the codec id: " << para->codec_id << endl;
        return false;
    }

    cout << "find the av codec id: " << para->codec_id << endl;

    mux.lock();
    //create decoder context
    codec = avcodec_alloc_context3(vcodec);

    //modify context parameters
    avcodec_parameters_to_context(codec, para);
    //8 thread decode
    codec->thread_count = 8;

    //open decoder context
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0)
    {
        avcodec_parameters_free(&para);
        avcodec_free_context(&codec);
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "avcodec_open2 failed! " << buf << endl;
        return false;
    }
    cout << "video avcodec_open2 success! " << endl;
    mux.unlock();
    avcodec_parameters_free(&para);

	return true;
}

//send to decode thread, clear pkt memory (object and media content)
bool XDecode::Send(AVPacket* pkt)
{
    //fault-tolerance
    if (!pkt || pkt->size <= 0 || !pkt->data) return false;
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if (re != 0) return false;
    return true;
}

//get decoded data, one send might have mutiple receive
//get copy each time, free by user, av_frame_free
AVFrame* XDecode::Recv()
{
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return NULL;
    }
    AVFrame* frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    mux.unlock();
    if (re != 0)
    {
        av_frame_free(&frame);
        return NULL;
    }
    cout << "lineSize" << frame->linesize[0] << " " << endl;
    return frame;
}