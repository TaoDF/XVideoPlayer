#pragma once
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include <mutex>

class XDecode
{
public:
	bool isAudio = false;
	//open Decoder and release para memory no matter if success
	virtual bool Open(AVCodecParameters* para);

	//send to decode thread, clear pkt memory (object and media content)
	virtual bool Send(AVPacket *pkt);

	//get decoded data, one send might have mutiple receive
	//get copy each time, free by user, av_frame_free
	virtual AVFrame* Recv();

	virtual void Close();
	virtual void Clear();

protected:
	AVCodecContext* codec = 0;
	std::mutex mux;
};

