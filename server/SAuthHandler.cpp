#include "SAuthHandler.h"
#include "MySha.h"
#include "MontgomeryExp.h"
#include "Util.h"

uint8 SAuthHandler::handleAuth(IOPacket* io)
{
    uint8 infoByte;
    if (io->getSize() < 1)
        return AUTH_BROKEN_DATA;

    *io >> infoByte;
    if (infoByte == AUTH_NOT_YET)
    {
        if (io->getSize() < (1 + 4  + 1))
        {
            io->reset(1, SMSG_AUTH);
            *io << uint8(AUTH_BAD_DATA);
            return AUTH_BROKEN_DATA;
        }
        uint32 gameID;
        std::string m_user;
        *io >> gameID;
        *io >> m_user;

        if (gameID != MAGIC_GAMEID)
        {
            io->reset(1, SMSG_AUTH);
            *io << uint8(AUTH_OUTDATED_CLIENT);
            return AUTH_OUTDATED_CLIENT;
        }

        ad = m_manager->getUserData(m_user);
        if (!ad)
        {
            io->reset(1, SMSG_AUTH);
            *io << uint8(AUTH_BAD_DATA);
            return AUTH_NO_ACCOUNT;
        }

        MagicVoid();

        io->reset((1 + 32 + 32), SMSG_AUTH);
        *io << uint8(AUTH_NOT_YET);
        io->append(B, 32);
        io->append(s, 32);
        return AUTH_NOT_YET;
    }
    else if (infoByte == AUTH_OK)
    {
        if (io->getSize() < (1 + 32 + 20))
        {
            io->reset(1, SMSG_AUTH);
            *io << uint8(AUTH_BAD_DATA);
            return AUTH_BROKEN_DATA;
        }

        io->read(A, 32);
        io->read(M1, 20);
        if (!MagicBool())
        {
            io->reset(1, SMSG_AUTH);
            *io << uint8(AUTH_BAD_DATA);
            return AUTH_BAD_DATA;
        }

        io->reset((1 + 20), SMSG_AUTH);
        *io << uint8(AUTH_OK);
        io->append(M2, 20);
        return AUTH_OK;
    }
    else
    {
        io->reset(1, SMSG_AUTH);
        *io << uint8(AUTH_BAD_DATA);
        return AUTH_BROKEN_DATA;
    }
}

void SAuthHandler::MagicVoid()
{
    MySha sha;
    MontgomeryExp mexp;
    mexp.init(N);

    uint8 x[20];
    uint8 bB = 0;//33rd byte
    for (uint8 i = 0; i < 32; i++)
        s[i] = rand32() & 0xFF;
    for (uint8 i = 0; i < 20; i++)
        b[i] = rand32() & 0xFF;

    // calc x and v
    sha.Initialize();
    sha.UpdateData(s, 32);
    sha.UpdateData(ad->passHash, 20);
    sha.Finalize();
    memcpy(x, sha.GetDigest(), 20);
    mexp.ModExpSimple(v, g, x);

    // calc B=(3*v+g^b) mod N
    {
        mexp.ModExpSimple(B, g, b);

        for (uint8 j = 0; j < 3; j++)
        {
            bool carry = false;
            for (uint8 i = 0; i < 32; i++)
            {
                B[i] += v[i] + carry;
                carry = (v[i] + carry > B[i]);
            }
            if (carry)
                bB++;
        }

        while (bB)
        {
            bool cut = false;
            for (uint8 i = 0; i < 32; i++)
            {
                int f = B[i] - N[i] - cut;
                if (f < 0) { f += 0x100; cut = true; }
                else cut = false;
                B[i] = f & 0xFF;
            }
            if (cut)
                bB--;
        }
    }
}

bool SAuthHandler::MagicBool()
{
    MySha sha;
    MontgomeryExp mexp;
    mexp.init(N);

    uint8 tempA[65];
    memset(tempA, 0, 65);
    memcpy(tempA, A, 32);
    mexp.modN(A, tempA);
    memset(tempA, 0, 65);
    if (memcmp(A, tempA, 32) == 0)
        return false;

    uint8 S[32], u[20];
    // calc u
    sha.Initialize();
    sha.UpdateData(A, 32);
    sha.UpdateData(B, 32);
    sha.Finalize();
    memcpy(u, sha.GetDigest(), 20);

    // calc M1
    mexp.ModExpAnother(S, A, v, u, b);
    sha.Initialize();
    sha.UpdateData(A, 32);
    sha.UpdateData(B, 32);
    sha.UpdateData(S, 32);
    sha.Finalize();

    if (memcmp(M1, sha.GetDigest(), 20) != 0)
        return false; // invalid digest

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
    return true;
}
