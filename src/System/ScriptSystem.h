#pragma once

#include "../ECS/ECS.h"

#include "../Component/ScriptComponent.h"

class ScriptSystem : public System
{
public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    void Update(double deltaTime) {
        Logger::Debug("ScriptSystem");
        for (auto entity : GetSystemEntities()) {
            const auto& script = entity.GetComponent<ScriptComponent>();
            script.func();
        }
    }
};