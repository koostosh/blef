#include "MySha.h"
#include <cstring>

uint32 rotl(uint32 i, int b)
{
    return (i << b) | (i >> (32-b));
}

void MySha::Initialize()
{
    const uint32 initvalues[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    counter = 0;
    memcpy(h, initvalues, sizeof(initvalues));
    memset(data, 0, sizeof(data));
}

void MySha::UpdateData(const uint8* input, uint32 length)
{
    while (length > 0)
    {
        volatile uint32 copysize = (64 - counter%64) > length ? length : (64 - counter%64);
        memcpy(data + counter%64, input, copysize);
        length -= copysize;
        input += copysize;
        counter += copysize;
        if (counter%64 == 0)
            Process();
    }

}

void MySha::Finalize()
{
    memset(data + counter % 64, 0x80, 1);
    if (counter % 64 > 55)
        Process();

    counter *= 8;
    data[60] = (counter & 0xFF000000) >> 24;
    data[61] = (counter & 0x00FF0000) >> 16;
    data[62] = (counter & 0x0000FF00) >> 8;
    data[63] = (counter & 0x000000FF);
    Process();
    for (uint8 i = 0; i < 5; i++)
    {
        digest[4*i] = (h[i] & 0xFF000000) >> 24;
        digest[4*i +1] = (h[i] & 0x00FF0000) >> 16;
        digest[4*i +2] = (h[i] & 0x0000FF00) >> 8;
        digest[4*i +3] = (h[i] & 0x000000FF);
    }
}

void MySha::Process()
{
    volatile uint32 a[5];
    uint32 w[80];
    uint32 temp;
    uint8 i;
    memset(w, 0, 320);
    for (i = 0; i < 80; i++)
        if (i < 16)
            w[i] = data[4 * i] << 24 | data[4 * i + 1] << 16 | data[4 * i + 2] << 8 | data[4 * i + 3];
        else
            w[i] = rotl(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16],1);
    memcpy((uint32*)a, h, 20);

    for (i = 0; i < 80; i++)
    {
        temp = rotl(a[0], 5) + a[4] + w[i];
        if (i < 20)
        {
            temp += 0x5A827999 + ((a[1] & a[2]) | (~a[1] & a[3]));
        }
        else if (i < 40)
        {
            temp += 0x6ED9EBA1 + (a[1] ^ a[2] ^ a[3]);
        }
        else if (i < 60)
        {
            temp += 0x8F1BBCDC + ((a[1] & a[2]) | (a[2] & a[3]) | (a[1] & a[3]));
        }
        else
        {
            temp += 0xCA62C1D6 + (a[1] ^ a[2] ^ a[3]);
        }
        a[4] = a[3];
        a[3] = a[2];
        a[2] = rotl(a[1], 30);
        a[1] = a[0];
        a[0] = temp;
    }
    h[0] += a[0]; h[1] += a[1]; h[2] += a[2]; h[3] += a[3]; h[4] += a[4];

    memset(data, 0, sizeof(data));
}
