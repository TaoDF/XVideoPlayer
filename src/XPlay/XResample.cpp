#include "XResample.h"
#include <iostream>
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "swresample.lib")

using namespace std;

//return size after resample, release indata no matter if success
int XResample::Resample(AVFrame* indata, unsigned char* d)
{
    if (!indata) return 0;
    if (!d)
    {
        av_frame_free(&indata);
        return 0;
    }

    uint8_t* data[2] = { 0 };
    data[0] = d;
    int re = swr_convert(actx,
        data, indata->nb_samples, //output
        (const uint8_t**)indata->data, indata->nb_samples //input
    );
    if (re <= 0) return re;
    int outsize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    return outsize;
}

void XResample::Close()
{
    mux.lock();
    if (actx) swr_free(&actx);
    mux.unlock();
}

//only sample format is different, output is S16
bool XResample::Open(AVCodecParameters* para)
{
    mux.lock();
    if (!para) return false;

    //audio resample
    //if(!actx) actx = swr_alloc();

    //if actx is null, will be allocate memory
    actx = swr_alloc_set_opts(actx,
        av_get_default_channel_layout(2), //output format
        (AVSampleFormat)outFormat, //out sample format 16bits, 1=AV_SAMPLE_FMT_S16
        para->sample_rate, //out
        av_get_default_channel_layout(para->channels), //input
        (AVSampleFormat)para->format,
        para->sample_rate,
        0, 0
    );
    avcodec_parameters_free(&para);

    int re = swr_init(actx);
    mux.unlock();

    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init failed! " << buf << endl;
        getchar();
        return -1;
    }

    //unsigned char* pcm = NULL;

	return true;
}