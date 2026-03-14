#pragma once

#include <memory>

class MatchQueue;
class SessionManager;
class PlayerRepository;
class Session;

class MatchController {
private:
    std::shared_ptr<SessionManager> sessionManager;
    std::shared_ptr<PlayerRepository> playerRepository;
    std::shared_ptr<MatchQueue> matchQueue;

    void handleReqMatch(Session* s, const char* buf, size_t size);

public:
    MatchController(
        std::shared_ptr<SessionManager> sessionManager,
        std::shared_ptr<PlayerRepository> playerRepository,
        std::shared_ptr<MatchQueue> matchQueue
    );

    void registerHandlers();

};