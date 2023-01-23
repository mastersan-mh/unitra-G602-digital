
#include "comm.hpp"

unsigned Comm::bytesRawAvailable()
{
    return m_input.size();
}

int Comm::byteReadRaw()
{
    uint8_t value = m_input[0];
    m_input.remove(0, 1);
    return value;
}

void Comm::byteWriteRaw(uint8_t byte)
{
    switch(byte)
    {
        case ':':
        {
            m_output.clear();
            m_output.append(byte);
            break;
        }
        case '\n':
        {
            m_output.append(byte);
            emit readyOutputStream(m_output);
            break;
        }
        default:
        {
            m_output.append(byte);
        }
    }
}

void Comm::appendInputStream(const QByteArray &data)
{
    m_input.append(data);

#define COMM_CAP 32

    uint8_t frame[COMM_CAP];
    unsigned size;
    while(m_input.size() > 0)
    {
        GCommBase::Error error = GCommBase::readFrame(frame, COMM_CAP, &size);
        if(error == GCommBase::Error::OK)
        {
            m_frames.push_back(QByteArray((char *)frame, size));
            emit readyFrameToRead();
        }
    }
}

bool Comm::readFrame(QByteArray &frame)
{
    if(m_frames.isEmpty()) return false;
    frame = m_frames.takeFirst();
    return true;
}

void Comm::writeFrame(const QByteArray &frame)
{
    GCommBase::writeFrame((const uint8_t*)frame.data(), frame.size());
}

