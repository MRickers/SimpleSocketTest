//
// Created by max on 07.01.24.
//

#ifndef SIMPLESOCKETTEST_UTILS_HPP
#define SIMPLESOCKETTEST_UTILS_HPP

#include <string>
#include "raylib.h"

namespace myapp {
    struct Entity {
        Vector2 position;
        Color color;
    };

    using Map = std::unordered_map<std::string, Entity>;

    inline std::string makeProtocol(Entity const &e, std::string_view const id, std::string_view const color) {
        std::string payload;

        payload = "id:";
        payload += id;
        payload += ";x:" + std::to_string(static_cast<int>(e.position.x));
        payload += ";y:" + std::to_string(static_cast<int>(e.position.y));
        payload += ";color:";
        payload += color;
        return payload;
    }

    inline Color getColorByName(std::string_view t_color) {
        static const std::unordered_map<std::string, Color> colorMap = {
                {"red", MAROON},
                {"green", DARKGREEN},
                {"blue", SKYBLUE},
                {"purple", DARKPURPLE},
                // Add more color mappings as needed
        };
        auto it = colorMap.find(std::string{t_color});
        if(it != colorMap.end()) {
            return it->second;
        }
        return LIGHTGRAY;
    }

    inline std::vector<std::string_view> splitStringView(std::string_view str, char delimiter) {
        std::vector<std::string_view> result;

        size_t start = 0;
        size_t end = str.find(delimiter);

        while (end != std::string_view::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        // Add the last substring (or the only substring if there's no delimiter)
        result.push_back(str.substr(start));

        return result;
    }
}
#endif //SIMPLESOCKETTEST_UTILS_HPP
