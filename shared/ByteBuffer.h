#pragma once
#include "Defines.h"
#include "Opcodes.h"
#include <cstring>

class ByteBuffer
{
    friend class MainSocket;
public:
    ByteBuffer(size_t size = 100) : m_rpos(0), m_wpos(0), m_storageSize(size)
    {
        m_storage = new uint8[size];
    }

    ~ByteBuffer()
    {
        delete[] m_storage;
    }

    void resize(size_t newsize)
    {
        delete[] m_storage;
        m_storageSize = newsize;
        m_storage = new uint8[m_storageSize];
        m_rpos = m_wpos = 0;
    }

    size_t getSize()
    {
        return m_storageSize;
    }

    ByteBuffer& operator>>(uint8& val)
    {
        read(&val, sizeof(uint8));
        return *this;
    }

    ByteBuffer& operator>>(uint16& val)
    {
        read(&val, sizeof(uint16));
        return *this;
    }

    ByteBuffer& operator>>(int16& val)
    {
        read(&val, sizeof(int16));
        return *this;
    }

    ByteBuffer& operator>>(uint32& val)
    {
        read(&val, sizeof(uint32));
        return *this;
    }

    ByteBuffer& operator>>(float& val)
    {
        read(&val, sizeof(float));
        return *this;
    }

    ByteBuffer& operator>>(std::string& val)
    {
        val.clear();
        char c;
        while (m_rpos < m_storageSize)
        {
            c = m_storage[m_rpos++];
            if (c == 0)
                break;
            val += c;
        }
        return *this;
    }

    ByteBuffer& operator<<(const uint8 val)
    {
        append(&val, sizeof(uint8));
        return *this;
    }

    ByteBuffer& operator<<(const uint16 val)
    {
        append(&val, sizeof(uint16));
        return *this;
    }

    ByteBuffer& operator<<(const int16 val)
    {
        append(&val, sizeof(int16));
        return *this;
    }

    ByteBuffer& operator<<(const uint32 val)
    {
        append(&val, sizeof(uint32));
        return *this;
    }

    ByteBuffer& operator<<(const float val)
    {
        append(&val, sizeof(float));
        return *this;
    }

    ByteBuffer& operator<<(const std::string& val)
    {
        append(val.c_str(), val.size());
        *this << uint8(0);
        return *this;
    }

    void append(const void* src, size_t cnt)
    {
        if (!cnt)
            return;

        if (m_storageSize < m_wpos + cnt)
        {
            m_storageSize = m_wpos + cnt;
            uint8* newStorage = new uint8[m_storageSize];
            memcpy(newStorage, m_storage, m_wpos);
            delete[] m_storage;
            m_storage = newStorage;
        }
        memcpy(&m_storage[m_wpos], src, cnt);
        m_wpos += cnt;
    }

    void read(void* dst, size_t cnt)
    {
        if (!dst)
            return;

        if (m_storageSize < m_rpos + cnt)
            return;

        memcpy(dst, &m_storage[m_rpos], cnt);
        m_rpos += cnt;
    }

    void put(uint32 val, size_t pos)
    {
        if (pos + 4 > m_storageSize)
            return;

        memcpy(&m_storage[pos], &val, 4);
    }

protected:
    size_t m_rpos, m_wpos, m_storageSize;
    uint8* m_storage;
};

class IOPacket : public ByteBuffer
{
public:
    IOPacket() : ByteBuffer(0) {};
    IOPacket(size_t size, uint16 op) : ByteBuffer(size), opcode(op) {};
    void reset(size_t size, uint16 op) { resize(size); opcode = op; }
    uint16 getOpcode() { return opcode; };
private:
    void resize(size_t size) { ByteBuffer::resize(size); };
    uint16 opcode;
};
