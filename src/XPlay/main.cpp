#include "XPlay.h"
#include <QtWidgets/QApplication>
#include "XDemux.h"
#include <iostream>
#include "XDecode.h"
#include "XResample.h"
#include <QThread>

using namespace std;

class TestThread :public QThread
{
public:
    void Init()
    {
        //char* url = "rtmp://124.139.232.61:1935/live/livestream";
        char* url = "../sample.mp4";
        //cout << "demux.open = " << demux.Open(url) << endl;
        //demux.Read();
        //demux.Clear();
        //demux.Close();
        cout << "demux.open = " << demux.Open(url) << endl;
        cout << "copyVpara = " << demux.CopyVPara() << endl;
        cout << "copyApara = " << demux.CopyAPara() << endl;
        //cout << "seek = " << demux.Seek(0.1) << endl;

        //////////////////////////
        //decode test
        cout << "vdecoder.open() = " << vdecoder.Open(demux.CopyVPara()) << endl;
        //vdecoder.Clear();
        //vdecoder.Close();

        cout << "adecoder.open() = " << adecoder.Open(demux.CopyAPara()) << endl;
        cout << "resample.open() = " << resample.Open(demux.CopyAPara()) << endl;
    }
    unsigned char* pcm = new unsigned char[1024 * 1024];
    void run()
    {
        for (;;)
        {
            AVPacket* pkt = demux.Read();
            if (demux.IsAudio(pkt))
            {
                adecoder.Send(pkt);
                AVFrame* frame = adecoder.Recv();
                cout << "resample: " << resample.Resample(frame, pcm)<<" ";
            }
            else
            {
                vdecoder.Send(pkt);
                AVFrame* frame = vdecoder.Recv();
                video->Repaint(frame);
                msleep(20);
            }
            if (!pkt) break;
        }
    }

    XDemux demux;
    XDecode vdecoder;
    XDecode adecoder;
    XResample resample;
    XVideoWidget* video;
};

int main(int argc, char *argv[])
{
    //init display

    /*
    for (;;)
    {
        AVPacket *pkt = demux.Read();
        if (demux.IsAudio(pkt))
        {
            adecoder.Send(pkt);
            AVFrame* frame = adecoder.Recv();
        }
        else
        {
            vdecoder.Send(pkt);
            AVFrame* frame = vdecoder.Recv();
        }
        if (!pkt) break;
    }
    */

    TestThread tt;
    tt.Init();

    QApplication a(argc, argv);
    XPlay w;
    w.ui.video->Init(tt.demux.width, tt.demux.height);
    tt.video = w.ui.video;
    tt.start();
    w.show();

    //init gl window
    //w.ui.video->Init(demux.width, demux.height);

    return a.exec();
}
