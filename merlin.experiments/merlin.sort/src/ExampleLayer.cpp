#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Memory;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <GLFW/glfw3.h>

ExampleLayer::ExampleLayer(){
	Window* w = &Application::Get().GetWindow();
	int height = w->GetHeight();
	int width = w->GetWidth();
}

ExampleLayer::~ExampleLayer(){}

template<typename T>
void debugBuffer(SSBO& buffer) {
	std::vector<T> buf;
	buffer.Bind();
	buf.resize(buffer.size() / sizeof(T));
	memcpy(buf.data(), buffer.Map(), buffer.size());
	buffer.Unmap();

	Console::info("Buffer") << buffer.name() << " : " << Console::endl << "[";
	for (GLuint i = 0; i < buf.size() -1; i++) {
		Console::print() << buf[i] << ", ";
	}
	Console::print() << buf[buf.size() -1] << "]" << Console::endl << Console::endl;
	
}


template<typename T>
void debugVector(std::vector<T>& vec) {
	Console::info("Vector") << " : " << Console::endl << "[";
	for (GLuint i = 0; i < vec.size() - 1; i++) {
		Console::print() << vec[i] << ", ";
	}
	Console::print() << vec[vec.size() - 1] << "]" << Console::endl << Console::endl;

}

void ExampleLayer::OnAttach(){
	EnableGLDebugging();
	Console::SetLevel(ConsoleLevel::_INFO);

	Console::print() << "Generating data..." << Console::endl;
	for (GLuint i = 0; i < n; i++) data.push_back(i);
	//debugVector(data);

	Console::print() << "Shuffling data..." << Console::endl;
	std::shuffle(data.begin(), data.end(), std::default_random_engine());
	//debugVector(data);

	//Init buffers
	countingCount = ComputeShader::Create( "counting.count", "./assets/shaders/counting.count.comp");
	prefixSum = ComputeShader::Create( "prefix.sum", "./assets/shaders/prefix.sum.comp");

	inDataBuffer = SSBO::Create("inDataBuffer");
	inDataBuffer->Bind();
	inDataBuffer->Attach(*prefixSum, 0);
	inDataBuffer->Attach(*countingCount, 0);
	inDataBuffer->Allocate<GLuint>(data);

	outDataBuffer = SSBO::Create("outDataBuffer");
	outDataBuffer->Bind();
	outDataBuffer->Attach(*prefixSum, 1);
	outDataBuffer->Attach(*countingCount, 1);
	outDataBuffer->Allocate<GLuint>(data);

	compactSumBuffer = SSBO::Create("compactSumBuffer");
	compactSumBuffer->Bind();
	compactSumBuffer->Attach(*prefixSum, 2);
	compactSumBuffer->Attach(*countingCount, 2);
	compactSumBuffer->Allocate<GLuint>(blocks);

	prefixSumBuffer = SSBO::Create("prefixSumBuffer");
	prefixSumBuffer->Bind();
	prefixSumBuffer->Attach(*prefixSum, 3);
	prefixSumBuffer->Attach(*countingCount, 3);
	prefixSumBuffer->Allocate<GLuint>(data.size());

	//debugBuffer<GLuint>(*inDataBuffer);

	Console::info("Sorting") << "Starting..." << Console::endl;
	double time = (double) glfwGetTime();

	countingCount->Use();
	countingCount->SetUInt("stage", 0);
	countingCount->Dispatch(data.size());
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	Console::print() << "Data : " << data.size() << " uint values" << Console::endl;
	Console::print() << "Parallelizing counting sort over " << blocks << " blocks ( " << blockSize << " values per blocks)" << Console::endl;

	prefixSum->Use();
	prefixSum->SetUInt("stage", 0);
	prefixSum->Dispatch(blocks);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	//Binary tree on rightmost element of blocks
	GLuint steps = blockSize;
	GLuint space = 1;
	
	for (GLuint step = 0; step < blocks; step++) {
		// Calls the parallel operation
		
		prefixSum->SetUInt("stage", 1);
		prefixSum->SetUInt("space", space);
		prefixSum->Dispatch(blocks);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		prefixSum->SetUInt("stage", 2);
		prefixSum->Dispatch(blocks);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		space *= 2;
	}

	prefixSum->SetUInt("stage", 3);
	prefixSum->Dispatch(blocks);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	countingCount->Use();
	countingCount->SetUInt("stage", 1);
	countingCount->Dispatch(data.size());
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	double detla = (double)glfwGetTime() - time;
	Console::success("Sorting") << "Computation finished in " << detla << "s (" << detla * 1000.0 << " ms)" << Console::endl;


	time = (double)glfwGetTime();
	std::sort(data.begin(), data.end());
	detla = (double)glfwGetTime() - time;
	Console::success("Sorting") << "CPU Computation finished in " << detla << "s (" << detla * 1000.0 << " ms)" << Console::endl;

	//debugBuffer<GLuint>(*outDataBuffer);
}

void ExampleLayer::OnDetach(){

}

void ExampleLayer::OnEvent(Event& event){

}

void ExampleLayer::OnUpdate(Timestep ts){

}

void ExampleLayer::OnImGuiRender()
{
	
}
