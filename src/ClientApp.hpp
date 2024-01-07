//
// Created by max on 07.01.24.
//

#ifndef SIMPLESOCKETTEST_CLIENTAPP_HPP
#define SIMPLESOCKETTEST_CLIENTAPP_HPP
#include "raylib.h"
#include "simple_socket.hpp"
#include "utils.hpp"

namespace myapp {
    class ClientApp {
    public:
        ClientApp();

        virtual ~ClientApp();
        void run();
    private:
        void update();
        void draw();
    private:
        simple::ClientSocket m_socket;
        std::mutex m_mutex;
        Map m_map;
    };

} // myapp

#endif //SIMPLESOCKETTEST_CLIENTAPP_HPP
