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

	inDataBuffer.Rename("inDataBuffer");
	inDataBuffer.LoadData(data);

	outDataBuffer.Rename("outDataBuffer");
	outDataBuffer.LoadData(data);

	compactSumBuffer.Rename("compactSumBuffer");
	compactSumBuffer.Allocate(blocks);

	prefixSumBuffer.Rename("compactSumBuffer");
	prefixSumBuffer.Allocate(data.size());

	prefixSum->Attach(inDataBuffer);
	prefixSum->Attach(outDataBuffer);
	prefixSum->Attach(prefixSumBuffer);
	prefixSum->Attach(compactSumBuffer);

	countingCount->Attach(inDataBuffer);
	countingCount->Attach(prefixSumBuffer, 2);

	inDataBuffer.print();

	Console::info("Sorting") << "Starting..." << Console::endl;
	double time = (double) glfwGetTime();

	countingCount->Use();
	countingCount->SetUInt("stage", 0);
	countingCount->Dispatch(wgCount);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	prefixSumBuffer.print();

	Console::print() << "Data : " << data.size() << " uint values" << Console::endl;
	Console::print() << "Parallelizing counting sort over " << blocks << " blocks ( " << blockSize << " values per blocks)" << Console::endl;

	prefixSum->Use();
	prefixSum->SetUInt("stage", 0);
	prefixSum->Dispatch(bwgCount);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	//Binary tree on rightmost element of blocks
	GLuint steps = blockSize;
	GLuint space = 1;
	
	for (GLuint step = 0; step < blockSize; step++) {
		// Calls the parallel operation
		
		prefixSum->SetUInt("stage", 1);
		prefixSum->SetUInt("space", space);
		prefixSum->Dispatch(bwgCount);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		prefixSum->SetUInt("stage", 2);
		prefixSum->Dispatch(bwgCount);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		prefixSumBuffer.print();
		space *= 2;
	}

	prefixSum->SetUInt("stage", 3);
	prefixSum->Dispatch(bwgCount);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	prefixSumBuffer.print();

	countingCount->Use();
	countingCount->SetUInt("stage", 1);
	countingCount->Dispatch(wgCount);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glFinish();
	double detla = (double)glfwGetTime() - time;
	Console::success("Sorting") << "Computation finished in " << detla << "s (" << detla * 1000.0 << " ms)" << Console::endl;

	outDataBuffer.print();

	time = (double)glfwGetTime();
	std::sort(data.begin(), data.end());
	detla = (double)glfwGetTime() - time;
	Console::success("Sorting") << "CPU Computation finished in " << detla << "s (" << detla * 1000.0 << " ms)" << Console::endl;

	debugVector<GLuint>(data);

	
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
