#pragma once
#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
	XDemux::XDemux();
	//open file or stream media rtmp, http, rstp
	virtual bool Open(const char *url);

	//memory need to be released by user, release obj space and data space av_packet_free
	virtual AVPacket* Read();

	virtual bool IsAudio(AVPacket *pkt);

	//get video parameters, need to clean return mem, avcodec_parameters_free
	virtual AVCodecParameters* CopyVPara();

	//get audio parameters
	virtual AVCodecParameters* CopyAPara();

	//seek position 0.0 ~1.0
	virtual bool Seek(double pos);

	//clear read buffer
	virtual void Clear();
	virtual void Close();

    //media length
	int totalMs = 0;
	int width = 0;
	int height = 0;

protected:
	std::mutex mux;
	//demux context
	AVFormatContext* ic = NULL;
	//video, audio index
	int videoStream = 0;
	int audioStream = 1;
};

