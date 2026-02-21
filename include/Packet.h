#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct PacketHeader {
    uint16_t size;
    uint16_t id;
};

enum PacketID : uint16_t {
    C2S_REQ_PLACE_STONE = 1001, // 클라 -> 서버: 돌 두기 요청
    S2C_RES_PLACE_STONE = 1002, // 서버 -> 클라: 돌 두기 결과
    // ...
};

struct PKT_C2S_ReqPlaceStone {
    PacketHeader header;
    int16_t x;
    int16_t y;
};

#pragma pack(pop)
