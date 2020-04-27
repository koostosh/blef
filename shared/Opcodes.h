#pragma once

enum Opcodes
{
    // CS(std::string text) SC(std::string username, std::string text)
    MSG_TEXT_MESSAGE        = 0x0000,
    // (uint8 info byte, ...)
    CMSG_AUTH               = 0x0001,
    // (uint8 info byte, ...)
    SMSG_AUTH               = 0x0002,
    // (std::string username)
    SMSG_USER_JOINED        = 0x0003,
    // (std::string username)
    SMSG_USER_LEFT          = 0x0004,
    // (BoardState)
    SMSG_BOARD_STATE        = 0x0005,
    // (uint8 move)
    CMSG_MOVE               = 0x0006,
    // (void)
    MSG_PING_PONG           = 0x0007,

    MAX_OPCODES
};
