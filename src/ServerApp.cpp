//
// Created by max on 06.01.24.
//

#include "ServerApp.hpp"
#include <string>
#include <thread>

using namespace std::chrono_literals;

myapp::ServerApp::ServerApp() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "SimpleSocketServer test");
    SetTargetFPS(60);
}

myapp::ServerApp::~ServerApp() {
    CloseWindow();
}

void myapp::ServerApp::run() {
    m_map["luke"] = Entity{Vector2{50, 50}, DARKPURPLE};

    try {
        m_server.bindAndListen("5000", 0, true);
    } catch (simple::SocketError const &e) {
        fmt::print("could not bind socket: {}", e.what());
        return;
    }

    while (!WindowShouldClose() && m_server.isOpen()) {
        try {
            auto new_client = m_server.accept(1ms);
            fmt::print("new client connected: {}:{}", new_client->getPeer().service, new_client->getPeer().host);
            // id:e1;x:123;y:123;color:green
            new_client->setReceiveCallback([this](std::vector<char> const &data) -> std::vector<char> {
                std::string_view data_view{data.data(), data.size()};
                const auto tokens = splitStringView(data_view, ';');
                if (tokens.size() != 4) {
                    fmt::print("protocol miss match {}\n", data_view);
                    return {};
                }
                Entity entity{};
                std::string name{};

                try {
                    for (auto const &token: tokens) {
                        const auto attribute = splitStringView(token, ':');
                        if (attribute.at(0).contains("id")) {
                            name = attribute.at(1);
                        } else if (attribute.at(0).contains("x")) {
                            try {
                                entity.position.x = static_cast<float>(std::stoi(std::string{attribute.at(1)}));
                            } catch (std::invalid_argument const &e) {
                                fmt::print("x {} value could not be converted to int: {}", attribute.at(1), e.what());
                                return {};
                            } catch (std::out_of_range const &e) {
                                fmt::print("x {} value out of range: {}", attribute.at(1), e.what());
                                return {};
                            }
                        } else if (attribute.at(0).contains("y")) {
                            try {
                                entity.position.y = static_cast<float>(std::stoi(std::string{attribute.at(1)}));
                            } catch (std::invalid_argument const &e) {
                                fmt::print("y {} value could not be converted to int: {}", attribute.at(1), e.what());
                                return {};
                            } catch (std::out_of_range const &e) {
                                fmt::print("y {} value out of range: {}", attribute.at(1), e.what());
                                return {};
                            }
                        } else if (attribute.at(0).contains("color")) {
                            entity.color = getColorByName(attribute.at(1));
                        }
                    }
                    std::lock_guard lock{m_mutex};
                    m_map[name] = entity;
                } catch (std::out_of_range const &e) {
                    fmt::print("could not parse incoming message: {}", data.data());
                }
                return {};
            });
            new_client->run();
            m_clients.push_back(std::move(new_client));

        } catch (simple::SocketTimeoutError const &) { // do nothing
        } catch (simple::SocketError const &e) {
            fmt::print("{}", e.what());
        }
        update();
        draw();

        // update peer
        std::lock_guard lock{m_mutex};
        if (m_clients.empty() && m_map.size() > 1) {
            auto const luke = m_map["luke"];
            m_map.clear();
            m_map["luke"] = luke;
        }
        for (auto client = m_clients.begin(); client != m_clients.end();) {
            if (client->get()->isOpen()) {
                try {
                    client->get()->send(makeProtocol(m_map["luke"], "Luke", "purple"));
                    client++;
                } catch (simple::SocketShutdownError const &) {
                    client = m_clients.erase(client);
                    continue;
                }
            } else {
                client = m_clients.erase(client);
            }
        }
    }

}

void myapp::ServerApp::update() {
    std::lock_guard lock{m_mutex};

    if (IsKeyDown(KEY_D)) m_map["luke"].position.x += 2.0f;
    if (IsKeyDown(KEY_A)) m_map["luke"].position.x -= 2.0f;
    if (IsKeyDown(KEY_W)) m_map["luke"].position.y -= 2.0f;
    if (IsKeyDown(KEY_S)) m_map["luke"].position.y += 2.0f;
}

void myapp::ServerApp::draw() {

    BeginDrawing();

    ClearBackground(RAYWHITE);
    {
        std::lock_guard lock{m_mutex};
        for (auto const &entity: m_map) {
            DrawCircleV(entity.second.position, 25, entity.second.color);
            DrawText(entity.first.c_str(), static_cast<int>(entity.second.position.x)- 12,
                     static_cast<int>(entity.second.position.y)-5, 15, LIGHTGRAY);
        }
    }

    EndDrawing();
}


