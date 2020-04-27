#include "MainSocket.h"

bool MainSocket::recv(IOPacket* in)
{
    uint16 datalength;

    if (!m_connected)
        return false;

    if (!m_hasHeader) // starting new packet
    {
        if (headerLength == 0) // starting new header
        {
            datalength = 4;
            if (!recv_raw(header, &datalength))
            {
                close_socket();
                return false;
            }
            if (datalength == 0)
                return false;
            if (datalength != 4)
            { // not whole header!
                headerLength = (uint8)datalength;
                return false;
            }
        }
        else // last header was not fully received
        {
            uint8 headertemp[4];
            datalength = 4 - headerLength;
            if (!recv_raw(headertemp, &datalength))
            {
                close_socket();
                return false;
            }

            for (uint8 i = 0; i<datalength; i++)
                header[headerLength + i] = headertemp[i];

            if (datalength != 4 - headerLength)
            { // still not whole
                headerLength += datalength;
                return false;
            }

            headerLength = 0;
        }

        // we have good new header, lets process!
        if (m_enryptHeaders)
            decrypt(header, 4);

        currentSize = (uint16)(((uint8)header[0] << 8) | (uint8)header[1]);
        uint16 opcode = (uint16)(((uint8)header[2] << 8) | (uint8)header[3]);
        in->reset(currentSize, opcode);

        if (!currentSize)
            return true;

        datalength = currentSize;
        partialPacket = new uint8[datalength];
        if (!recv_raw(partialPacket, &datalength))
        {
            close_socket();
            return false;
        }

        if (datalength < currentSize)
        { // not whole packet received
            partialLength = datalength;
            m_hasHeader = true;
            return false;
        }
        // ending packet
        memcpy(in->m_storage, partialPacket, datalength);
        delete[] partialPacket;
        if (m_enryptHeaders)
            decrypt(in->m_storage, currentSize);
        m_hasHeader = false;
        return true;
    }
    else // not whole data was received last time
    {
        datalength = currentSize - partialLength;
        if (!recv_raw(partialPacket + partialLength, &datalength))
        {
            close_socket();
            return false;
        }

        if (datalength < currentSize - partialLength)
        { // still not whole packet
            partialLength += datalength;
            return false;
        }

        memcpy(in->m_storage, partialPacket, datalength);
        delete[] partialPacket;
        if (m_enryptHeaders)
            decrypt(in->m_storage, currentSize);
        m_hasHeader = false;
        return true;
    }
}

void MainSocket::send(IOPacket* out)
{
    uint8 buffer[4];
    buffer[0] = (uint8)(out->m_wpos >> 8);
    buffer[1] = (uint8)(out->m_wpos % 256);
    buffer[2] = (uint8)(out->getOpcode() >> 8);
    buffer[3] = (uint8)(out->getOpcode() % 256);

    if (m_enryptHeaders)
        encrypt(buffer, 4);
    send_raw(buffer, 4);
    uint8* outbuf = new uint8[out->m_wpos];
    memcpy(outbuf, out->m_storage, out->m_wpos);
    if (m_enryptHeaders)
        encrypt(outbuf, out->m_wpos);
    send_raw(outbuf, (uint16)out->m_wpos);
    delete[] outbuf;
}

void MainSocket::encrypt(uint8* data, size_t len)
{
    for (size_t t = 0; t < len; t++)
    {
        index_s %= 20;
        uint8 x = (data[t] ^ headerKey[index_s]) + last_s;
        ++index_s;
        data[t] = last_s = x;
    }
}

void MainSocket::decrypt(uint8* data, size_t len)
{
    for (size_t t = 0; t < len; t++)
    {
        index_r %= 20;
        uint8 x = (data[t] - last_r) ^ headerKey[index_r];
        ++index_r;
        last_r = data[t];
        data[t] = x;
    }
}
