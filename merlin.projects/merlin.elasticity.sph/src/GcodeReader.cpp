#include "GcodeReader.h"
#include <fstream>
#include <sstream>
#include <iostream>


GcodeSimulator::GcodeSimulator() : m_current_position(0.0f), m_current_target(0.0f) {}

void GcodeSimulator::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }

    while (getline(file, line)) {
        if (line[0] == 'G') {
            std::istringstream iss(line.substr(1));
            int commandNum;
            iss >> commandNum;

            if (commandNum == 1) {  // We handle G1 commands
                glm::vec4 command(0.0f);
                char coord;
                float value;
                while (iss >> coord >> value) {
                    switch (coord) {
                    case 'X': command.x = value; break;
                    case 'Y': command.y = value; break;
                    case 'Z': command.z = value; break;
                    case 'E': command.w = value; break;
                    }
                }
                m_commands.push_back(command);
            }
        }
    }

    file.close();
    if (!m_commands.empty()) {
        m_current_target = m_commands[0]; // Set the first target
    }
}

void GcodeSimulator::reset() {
    m_current_position = glm::vec4(0.0f);
    if (!m_commands.empty()) {
        m_current_target = m_commands[0];
    }
}

void GcodeSimulator::update(float dt) {
    glm::vec3 direction = glm::normalize(glm::vec3(m_current_target) - glm::vec3(m_current_position));
    float speed = 10.0f; // Speed of movement in units per second
    glm::vec3 movement = direction * speed * dt;

    if (glm::length(movement) > glm::length(glm::vec3(m_current_target) - glm::vec3(m_current_position))) {
        m_current_position = m_current_target;
        // Move to the next target if available
        static size_t currentIndex = 0;
        if (++currentIndex < m_commands.size()) {
            m_current_target = m_commands[currentIndex];
        }
    }
    else {
        m_current_position += glm::vec4(movement, 0.0f);
    }
}

glm::vec3 GcodeSimulator::getNozzlePosition() {
    return glm::vec3(m_current_position);
}

float GcodeSimulator::getExtruderSpeed() {
    // Calculate the distance traveled by the nozzle in 3D space
    glm::vec3 distanceVec = glm::vec3(m_current_target) - glm::vec3(m_current_position);
    float distance = glm::length(distanceVec);

    // Calculate the change in the extruder's 'E' value
    float deltaE = m_current_target.w - m_current_position.w;

    // Assuming 'update' was called with a time step 'dt', and 'distance' is not zero to avoid division by zero
    if (distance > 0) {
        // Extrusion speed = change in extrusion divided by the distance traveled
        float extrusionSpeed = deltaE / distance;
        return extrusionSpeed;
    }

    return 0.0f;  // If no movement, extrusion speed is zero
}