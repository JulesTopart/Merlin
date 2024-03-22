#include "glpch.h"
#include "ParticleSystem.h"

Shared<ParticleSystem> Merlin::ParticleSystem::Create(std::string name, GLsizeiptr count){
	return std::make_shared<ParticleSystem>(name, count);
}
