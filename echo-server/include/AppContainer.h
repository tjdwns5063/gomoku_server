#pragma once

#include <memory>

class PlayerRepository;
class SessionManager;
class MatchQueue;
class NetworkEngine;
class IdGenerator;

class AppContainer {
private:
    std::shared_ptr<PlayerRepository> playerRepository;
    std::shared_ptr<SessionManager> sessionManager;
    std::shared_ptr<MatchQueue> matchQueue;
    std::shared_ptr<NetworkEngine> networkEngine;
    std::shared_ptr<IdGenerator> sessionIdGen;
    std::shared_ptr<IdGenerator> playerIdGen;

public:
    AppContainer();

    void run();
};