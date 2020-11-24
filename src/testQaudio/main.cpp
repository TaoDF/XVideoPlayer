#include <QtCore/QCoreApplication>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QThread>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QAudioFormat fmt;
    fmt.setSampleRate(44100);
    fmt.setSampleSize(16);
    fmt.setChannelCount(2);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);

    QAudioOutput* out = new QAudioOutput(fmt);
    QIODevice* io = out->start(); //start playing

    int size = out->periodSize();
    char* buf = new char[size];

    FILE* fp = fopen("out.pcm", "rb");
    while (!feof(fp))
    {
        if (out->bytesFree() < size)
        {
            QThread::msleep(1);
            continue;
        }
        int len = fread(buf, 1, size, fp);
        if (len <= 0) break;
        io->write(buf, len);
    }
    fclose(fp);
    delete[] buf;
    buf = 0;

    return a.exec();
}
