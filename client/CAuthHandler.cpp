#include "CAuthHandler.h"
#include "MySha.h"
#include "MontgomeryExp.h"
#include "Util.h"

void CAuthHandler::startAuth(IOPacket* out)
{
    out->reset((1 + 4 + m_user.size() + 1), CMSG_AUTH);

    *out << uint8(AUTH_NOT_YET);
    *out << uint32(MAGIC_GAMEID);
    *out << m_user;
}

uint8 CAuthHandler::handleAuth(IOPacket* io)
{
    uint8 infoByte;
    if (io->getSize() < 1)
        return AUTH_BROKEN_DATA;

    *io >> infoByte;
    if (infoByte == AUTH_NOT_YET)
    {
        if (io->getSize() < (1 + 32 + 32))
            return AUTH_BROKEN_DATA;
        io->read(B, 32);
        io->read(s, 32);

        MagicVoid();

        io->reset((1 + 32 + 20), CMSG_AUTH);
        *io << uint8(AUTH_OK);
        io->append(A, 32);
        io->append(M1, 20);
        return AUTH_NOT_YET;
    }
    else if (infoByte == AUTH_OK)
    {
        if (io->getSize() < (1 + 20))
            return AUTH_BROKEN_DATA;
        uint8 ServerM2[20];
        io->read(ServerM2, 20);
        if (memcmp(ServerM2, M2, 20) != 0)
            return AUTH_BAD_DATA;
        return AUTH_OK;
    }
    else
        return infoByte;
}

/* WARNING! MAGIC AHEAD!
* I "user:pass" hashed
* g,N constants
* a,A client private,public key
* b,B server private,public key
* s random seed
* x=SHA(s|I) v=g^x mod N
*/
void CAuthHandler::MagicVoid()
{
    MySha sha;
    MontgomeryExp mexp;
    mexp.init(N);

    uint8       I[20], x[20], a[20], v[32], S[32], u[20];
    for (uint8 i = 0; i < 20; i++)
        a[i] = rand32() & 0xFF;
    // calc I
    sha.UpdateData(m_user);
    sha.UpdateData(":");
    sha.UpdateData(m_pass);
    sha.Finalize();
    memcpy(I, sha.GetDigest(), 20);

    // calc x and v
    sha.Initialize();
    sha.UpdateData(s, 32);
    sha.UpdateData(I, 20);
    sha.Finalize();
    memcpy(x, sha.GetDigest(), 20);
    mexp.ModExpSimple(v, g, x);

    // public key
    mexp.ModExpSimple(A, g, a);

    // calc u
    sha.Initialize();
    sha.UpdateData(A, 32);
    sha.UpdateData(B, 32);
    sha.Finalize();
    memcpy(u, sha.GetDigest(), 20);

    // do magic
    mexp.ModExpFunny(S, B, v, a, u, x);

    // calc M1
    sha.Initialize();
    sha.UpdateData(A, 32);
    sha.UpdateData(B, 32);
    sha.UpdateData(S, 32);
    sha.Finalize();
    memcpy(M1, sha.GetDigest(), 20);

    // calc M2
    sha.Initialize();
    sha.UpdateData(A, 32);
    sha.UpdateData(M1, 20);
    sha.UpdateData(S, 32);
    sha.Finalize();
    memcpy(M2, sha.GetDigest(), 20);

    sha.Initialize();
    sha.UpdateData(S, 32);
    sha.Finalize();
    memcpy(K, sha.GetDigest(), 20);
}
