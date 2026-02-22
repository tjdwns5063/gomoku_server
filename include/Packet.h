#pragma once

#include <cstdint>
#include <unordered_map>
#include <format>
#include <iostream>

#pragma pack(push, 1)

class Session;

struct PacketHeader {
    uint16_t size;
    uint16_t id;
};

enum class StoneColor : uint8_t {
    NONE = 0,
    BLACK = 1,
    WHITE = 2
};

enum class StatusCode : int16_t {
    SUCCESS = 0,
    NOT_YOUR_TURN = 1,
    INVALID_POSITION = 2,
    // ...
};

enum PacketID : uint16_t {
    C2S_REQ_PLACE_STONE = 1001, // 클라 -> 서버: 돌 두기 요청
    S2C_RES_PLACE_STONE = 1002, // 서버 -> 클라: 돌 두기 결과
    // ...
};

struct PKT_C2S_ReqPlaceStone {
    PacketHeader header;
    int8_t x;
    int8_t y;
    StoneColor color;
};

struct PKT_S2C_RES_PLACE_STONE {
    PacketHeader header;
	StatusCode status;
};

#pragma pack(pop)

class PacketHandler
{
private:
    static std::unordered_map<PacketID, void(*)(Session* session, const char* buffer, size_t size)> handlers;
    static void addHandler(PacketID id, void(*handler)(Session* session, const char* buffer, size_t size));

public:
    static void initRoutes();
	static void dispatch(Session* session, const char* buffer, size_t size);
};

