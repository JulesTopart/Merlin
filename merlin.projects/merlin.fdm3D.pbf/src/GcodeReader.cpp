#include "GcodeReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "merlin/core/console.h"

using namespace Merlin;

GcodeSimulator::GcodeSimulator() : m_current_position(0.0f), m_current_target(0.0f) {
    m_commands.push_back({ glm::vec4(0, 0, 2, 0) - glm::vec4(m_origin_offset,0), 180 });
    m_commands.push_back({ glm::vec4(40, 0, 2, 800) - glm::vec4(m_origin_offset,0), 30 });
}

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
            static Command command = { glm::vec4(0), 0 };
            if (commandNum == 1) {  // We handle G1 commands
                
                char coord;
                float value;
                while (iss >> coord >> value) {
                    switch (coord) {
                    case 'X': command.position.x = value; break;
                    case 'Y': command.position.y = value; break;
                    case 'Z': command.position.z = value; break;
                    case 'E': command.position.w = value; break;
                    case 'F': command.speed = value/60.0; break;
                    }
                }
                m_commands.push_back(command);
            } else if (commandNum == 92) {  // We handle G1 commands
                char coord;
                float value;
                while (iss >> coord >> value) {
                    switch (coord) {
                    case 'E': command.position.w = value; break;
                    }
                }
                m_commands.push_back(command);
            }
        }
    }

    file.close();
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
    }
}

void GcodeSimulator::reset() {
    m_current_position = glm::vec4(0.0f);
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
        currentIndex = 0;
    }
}

void GcodeSimulator::update(float dt) {
    glm::vec3 delta = glm::vec3(m_current_target) - glm::vec3(m_current_position);
    glm::vec3 direction = glm::normalize(delta);
    glm::vec3 movement = direction * m_current_speed * dt;

    if (glm::length(glm::vec3(delta)) < 0.1) {
        m_current_position = m_current_target;
        // Move to the next target if available
        
        if (++currentIndex < m_commands.size()) {
            m_current_target = m_commands[currentIndex].position;
            m_current_speed = m_commands[currentIndex].speed; // Set the first target
            Console::info() << "Current target : " << m_current_target << Console::endl;
            Console::info() << "Current speed  : " << m_current_speed << Console::endl;
        }
    }
    else {
        m_current_position += glm::vec4(movement,1.0);
    }
}

glm::vec3 GcodeSimulator::getNozzlePosition() {
    return glm::vec3(m_current_position) + m_origin_offset;
}

float GcodeSimulator::getExtruderDistance() {
    if (m_current_target.w != m_current_position.w && m_current_target.w != 0) return m_current_speed * m_current_target.w/30;
       return 0.0;
}