#pragma once

#include "se_model.h"

// std
#include <memory>

namespace se {

struct Transform2dComponent {
    glm::vec2 translation{};

    glm::mat2 mat2() { return glm::mat2{1.f}; }
};

class SeGameObject {
public:
    using id_t = unsigned int;

    static SeGameObject crateGameObject() {
        static id_t currentId = 0;
        return SeGameObject{currentId++};
    }

    SeGameObject(const SeGameObject &) = delete;
    SeGameObject& operator= (const SeGameObject &) = delete;
    SeGameObject(SeGameObject&&) = default;
    SeGameObject& operator= (SeGameObject&&) = default;

    id_t getId() { return id; }

    std::shared_ptr<SeModel> model{};
    glm::vec3 color{};
    Transform2dComponent transform2d{};

private:
    SeGameObject(id_t objId) : id{objId} {}

    id_t id;
};
}