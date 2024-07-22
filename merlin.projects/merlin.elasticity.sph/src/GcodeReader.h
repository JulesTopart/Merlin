#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class GcodeSimulator{
public:
    GcodeSimulator();
    void readFile(const std::string& filepath);

    void reset();
    void update(float dt);

    glm::vec3 getNozzlePosition();
    float getExtruderSpeed();

private:
    std::vector<glm::vec4> m_commands;

    glm::vec4 m_current_position;
    glm::vec4 m_current_target;
};

