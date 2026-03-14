#pragma once

#include <cstdint>
#include <unordered_map>
#include <format>
#include <iostream>
#include <functional>

class Session;

#pragma pack(push, 1)

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
	C2S_REQ_MATCH = 1003, // 클라 -> 서버: 매치 요청
	S2C_RES_MATCH = 1004, // 서버 -> 클라: 매치 결과
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

struct PKT_C2S_ReqMatch {
    PacketHeader header;
    uint32_t userId;
};

struct PKT_S2C_ResMatch {
	PacketHeader header;
    StatusCode status;
    uint8_t matchFlag;
	uint32_t player1Id;
    uint32_t player2Id;
};

#pragma pack(pop)

class PacketHandler
{
private:
    using HandlerFunc = std::function<void(Session*, const char*, size_t)>;
    static std::unordered_map<PacketID, HandlerFunc> handlers;

public:
    static void initRoutes();
    static void addHandler(PacketID id, HandlerFunc handler);
	static void dispatch(Session* session, const char* buffer, size_t size);
};

