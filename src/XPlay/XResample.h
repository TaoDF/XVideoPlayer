#pragma once
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
#include <mutex>;
class XResample
{
public:
	//only sample format is different, output is S16, release para
	virtual bool Open(AVCodecParameters *para);
	virtual void Close();

	//return size after resample, release indata no matter if success
	virtual int Resample(AVFrame *indata, unsigned char *data);

	//AV_SAMPLE_FMT_S16
	int outFormat = 1;

protected:
	std::mutex mux;
	SwrContext* actx = 0;
};

