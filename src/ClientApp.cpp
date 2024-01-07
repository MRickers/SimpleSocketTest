//
// Created by max on 07.01.24.
//

#include "ClientApp.hpp"

namespace myapp {
    ClientApp::ClientApp() {
        const int screenWidth = 800;
        const int screenHeight = 450;

        InitWindow(screenWidth, screenHeight, "SimpleSocketClient test");
        SetTargetFPS(60);
    }

    ClientApp::~ClientApp() {
        CloseWindow();
    }

    void ClientApp::run() {
        m_map["kenobi"] = Entity{Vector2{200,200},DARKGREEN};
        while (!WindowShouldClose()) {
            if (!m_socket.isOpen()) {
                try {
                    m_socket.connect("localhost", "5000");
                    m_socket.setReceiveCallback([this](std::vector<char> const & data) -> std::vector<char> {
                        std::string_view data_view{data.data(), data.size()};
                        const auto tokens = splitStringView(data_view, ';');
                        if(tokens.size() != 4) {
                            fmt::print("protocol miss match {}\n", data_view);
                            return {};
                        }
                        Entity entity{};
                        std::string name{};

                        try {
                            for(auto const & token : tokens) {
                                const auto attribute = splitStringView(token, ':');
                                if(attribute.at(0).contains("id")) {
                                    name = attribute.at(1);
                                } else if(attribute.at(0).contains("x")) {
                                    try {
                                        entity.position.x = static_cast<float>(std::stoi(std::string{attribute.at(1)}));
                                    } catch(std::invalid_argument const & e) {
                                        fmt::print("x {} value could not be converted to int: {}", attribute.at(1), e.what());
                                        return {};
                                    } catch(std::out_of_range const & e) {
                                        fmt::print("x {} value out of range: {}", attribute.at(1), e.what());
                                        return {};
                                    }
                                } else if(attribute.at(0).contains("y")) {
                                    try {
                                        entity.position.y = static_cast<float>(std::stoi(std::string{attribute.at(1)}));
                                    } catch(std::invalid_argument const & e) {
                                        fmt::print("y {} value could not be converted to int: {}", attribute.at(1), e.what());
                                        return {};
                                    } catch(std::out_of_range const & e) {
                                        fmt::print("y {} value out of range: {}", attribute.at(1), e.what());
                                        return {};
                                    }
                                } else if(attribute.at(0).contains("color")) {
                                    entity.color = getColorByName(attribute.at(1));
                                }
                            }
                            std::lock_guard lock{m_mutex};
                            m_map[name] = entity;
                        } catch(std::out_of_range const & e) {
                            fmt::print("could not parse incoming message: {}", data.data());
                        }
                        return {};
                    });
                    m_socket.run();
                } catch (simple::SocketError const &) {
                }
            }

            if (m_socket.isOpen()) {
                try {
                    std::lock_guard lock{m_mutex};
                    m_socket.send(makeProtocol(m_map["kenobi"], "Kenobi", "green"));
                } catch(simple::SocketShutdownError const & e) {
                    if(m_map.size() > 1) {
                        auto const kenobi = m_map["kenobi"];
                        m_map.clear();
                        m_map["kenobi"] = kenobi;
                    }
                } catch (simple::SocketError const &e) {
                    fmt::print("sending data failed: {}\n", e.what());
                }
            } else {
                if(m_map.size() > 1) {
                    auto const kenobi = m_map["kenobi"];
                    m_map.clear();
                    m_map["kenobi"] = kenobi;
                }
            }
            update();
            draw();
        }
    }

    void ClientApp::update() {
        std::lock_guard lock{m_mutex};

        if (IsKeyDown(KEY_RIGHT)) m_map["kenobi"].position.x += 2.0f;
        if (IsKeyDown(KEY_LEFT)) m_map["kenobi"].position.x -= 2.0f;
        if (IsKeyDown(KEY_UP)) m_map["kenobi"].position.y -= 2.0f;
        if (IsKeyDown(KEY_DOWN)) m_map["kenobi"].position.y += 2.0f;
    }

    void ClientApp::draw() {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        {
            std::lock_guard lock{m_mutex};
            for(auto const & entity : m_map) {
                DrawCircleV(entity.second.position, 25, entity.second.color);
                DrawText(entity.first.c_str(), static_cast<int>(entity.second.position.x)- 12,
                         static_cast<int>(entity.second.position.y)-5, 15, LIGHTGRAY);
            }

        }

        EndDrawing();
    }
} // myapp