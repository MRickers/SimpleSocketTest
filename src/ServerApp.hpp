//
// Created by max on 06.01.24.
//

#ifndef SIMPLESOCKETTEST_SERVERAPP_HPP
#define SIMPLESOCKETTEST_SERVERAPP_HPP

#include "simple_socket.hpp"
#include "utils.hpp"

namespace myapp {
    class ServerApp {
        using Clients = std::vector<std::unique_ptr<simple::ClientSocket>>;

    public:
        ServerApp();
        ~ServerApp();
        void run();

    private:
        void update();
        void draw();

    private:
        Map m_map;
        simple::ServerSocket m_server{};
        std::mutex m_mutex;
        Clients m_clients;
    };
}
#endif //SIMPLESOCKETTEST_SERVERAPP_HPP
