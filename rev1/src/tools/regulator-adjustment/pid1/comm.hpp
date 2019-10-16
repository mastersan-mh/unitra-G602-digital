#ifndef COMM_HPP
#define COMM_HPP

#include "GClasses/GCommBase.hpp"

#include <QObject>
#include <QByteArray>
#include <QVector>

class Comm: public QObject, public GCommBase
{
    Q_OBJECT
public:
    Comm();
    virtual ~Comm();
public slots:
    void appendInputStream(const QByteArray &data);
    bool readFrame(QByteArray &frame);
    void writeFrame(const QByteArray &frame);
signals:
    void readyFrameToRead();
    void readyOutputStream(const QByteArray &data);

private:

    virtual unsigned bytesRawAvailable();
    virtual int  byteReadRaw();
    virtual void byteWriteRaw(uint8_t byte);

    QByteArray m_input;
    QByteArray m_output;
    QVector<QByteArray> m_frames;
};

#endif // COMM_HPP

