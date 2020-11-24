#include "XDemux.h"
#include <iostream>
using namespace std;

extern "C"
{
    #include "libavformat/avformat.h"
}

#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "avcodec.lib")

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

XDemux::XDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;

    dmux.lock();
    if (isFirst)
    {
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

bool XDemux::Open(const char* url)
{
    Close();
    AVDictionary* opts = NULL;

    //set rtsp stream open as TCP
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    //network delay
    av_dict_set(&opts, "max_delay", "500", 0);

    mux.lock();
    int re = avformat_open_input(&ic, url,
        0, //auto choose demuxer
        &opts //parameter for ex. rtsp delay 
    );

    if (re != 0)
    {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open" << url << " failed! " << buf << endl;
        return false;
    }

    cout << "open" << url << " success! " << endl;

    re = avformat_find_stream_info(ic, 0);

    totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "total ms = " << totalMs << endl;

    av_dump_format(ic, 0, url, 0);

    //get video stream
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream* as = ic->streams[videoStream];

    width = as->codecpar->width;
    height = as->codecpar->height;
    cout << videoStream << ": video information" << endl;
    cout << "width: " << as->codecpar->width << endl;
    cout << "height: " << as->codecpar->height << endl;
    cout << "codec id: " << as->codecpar->codec_id << endl;
    cout << "sample format: " << as->codecpar->format << endl;
    //framerate
    cout << "fps = " << r2d(as->avg_frame_rate) << endl;
    cout << "frame size = " << as->codecpar->frame_size << endl;


    //get audio stream
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    as = ic->streams[audioStream];

    cout << audioStream << ": audio information" << endl;
    cout << "sample rate: " << as->codecpar->sample_rate << endl;
    cout << "sample format: " << as->codecpar->format << endl;
    cout << "channels: " << as->codecpar->channels << endl;
    cout << "codec id: " << as->codecpar->codec_id << endl;
    cout << "fps = " << r2d(as->avg_frame_rate) << endl;
    //single channel num of samples
    cout << "frame size = " << as->codecpar->frame_size << endl;

    mux.unlock();

    return true;
}

void XDemux::Clear()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }

    //clear read buffer
    avformat_flush(ic);
    mux.unlock();
}

void XDemux::Close()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }

    avformat_close_input(&ic);
    //video total length
    totalMs = 0;
    mux.unlock();
}

bool XDemux::Seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }

    //clear read buffer
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    //int ms = 3000;
    //long long pos = (ms / 1000) / r2d(ic->streams[pkt->stream_index]->time_base);
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();

    if (re < 0) return false;
    return true;
}

//get video parameters, need to clean return mem, avcodec_parameters_free
AVCodecParameters* XDemux::CopyVPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return NULL;
    }

    AVCodecParameters* pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mux.unlock();
    return pa;
}

//get audio parameters
AVCodecParameters* XDemux::CopyAPara()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return NULL;
    }

    AVCodecParameters* pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
    mux.unlock();
    return pa;
}

bool XDemux::IsAudio(AVPacket* pkt)
{
    if (!pkt) return false;
    if (pkt->stream_index == videoStream) return false;
    return true;
}

AVPacket* XDemux::Read()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return 0;
    }
    AVPacket* pkt = av_packet_alloc();

    //read 1 frame and assign space
    int re = av_read_frame(ic, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return 0;
    }
    //pts to ms
    pkt->pts = pkt->pts * r2d(ic->streams[pkt->stream_index]->time_base) * 1000;
    mux.unlock();
    cout << " pts " << pkt->pts << " " << flush;

    return pkt;
}
