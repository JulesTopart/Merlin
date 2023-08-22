#include "Solver.h"

Solver::Solver(GLsizeiptr instances, GLuint thread) : _instances(instances), _threadPattern(thread) {}

void Solver::AddComputeShader(Shared<ComputeShader> step) {
	_shaders.push_back(step);
}

void Solver::AddStorageBuffer(Shared<SSBO> buffer) {
	_buffers.push_back(buffer);
}

void Solver::Execute(Shared<ComputeShader> shader, std::vector<Shared<SSBO>> buffers) {

	for(Shared<SSBO> buffer : buffers) {
		//buffer->Attach(shader);
		buffer->Bind();
	}

	shader->Use();
	// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
	shader->Dispatch(_instances / _threadPattern, 1, 1);

	// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	
}

void Solver::Execute(Shared<ComputeShader> shader) {

	for (Shared<SSBO> buffer : _buffers) {
		//buffer->Attach(shader);
		buffer->Bind();
	}

	shader->Use();
	// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
	shader->Dispatch(_instances / _threadPattern, 1, 1);

	// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void Solver::Execute() {

	for (Shared<ComputeShader> shader : _shaders) {
		for (Shared<SSBO> buffer : _buffers) {
			buffer->Bind();
			//buffer->Attach(shader);
		}

		shader->Use();
		// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
		shader->Dispatch(_instances / _threadPattern, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

}