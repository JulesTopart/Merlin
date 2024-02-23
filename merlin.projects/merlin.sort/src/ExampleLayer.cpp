#include "ExampleLayer.h"

using namespace Merlin;

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
	for (GLuint i = 0; i < n; i++) {
		data.push_back(i);
	}

	//debugVector(data);
	std::vector<GLuint> unsorted = data;
	Console::print() << "Shuffling data..." << Console::endl;
	std::shuffle(unsorted.begin(), unsorted.end(), std::default_random_engine());
	//debugVector(data);

	//Init buffers
	prefixSum = CreateShared<StagedComputeShader>( "prefix.sum", "./assets/shaders/prefix.sum.comp", 4);
	countingCount = CreateShared<StagedComputeShader>("counting.count", "./assets/shaders/counting.count.comp", 2);
	prefixSum->SetWorkgroupLayout(bwgCount);
	countingCount->SetWorkgroupLayout(wgCount);

	inDataBuffer.rename("inDataBuffer");
	inDataBuffer.write(unsorted);

	outDataBuffer.rename("outDataBuffer");
	outDataBuffer.write(unsorted);

	prefixSumBuffer.rename("prefixSumBuffer");
	prefixSumBuffer.reserve(data.size());

	compactSumBuffer.rename("compactSumBuffer");
	compactSumBuffer.reserve(blocks);

	inDataBuffer.setBindingPoint(0);
	outDataBuffer.setBindingPoint(1);
	prefixSumBuffer.setBindingPoint(2);
	compactSumBuffer.setBindingPoint(3);

	prefixSum->Use();
	prefixSum->Attach(inDataBuffer);
	prefixSum->Attach(outDataBuffer);
	prefixSum->Attach(prefixSumBuffer);
	prefixSum->Attach(compactSumBuffer);

	countingCount->Use();
	countingCount->Attach(inDataBuffer);
	countingCount->Attach(outDataBuffer);
	countingCount->Attach(prefixSumBuffer);

	inDataBuffer.print();

	Console::info("Sorting") << "Starting..." << Console::endl;
	double time = (double) glfwGetTime();

	countingCount->Use();
	countingCount->Execute(0);
	
	prefixSumBuffer.print();

	Console::print() << "Data : " << unsorted.size() << " uint values" << Console::endl;
	Console::print() << "Parallelizing counting sort over " << blocks << " blocks ( " << blockSize << " values per blocks)" << Console::endl;

	prefixSum->Use();
	prefixSum->Execute(0);
	
	//Binary tree on rightmost element of blocks
	GLuint steps = blockSize;
	UniformObject<GLuint> space("space");
	space.value = 1;
	
	for (GLuint step = 0; step < blockSize; step++) {
		// Calls the parallel operation
		
		space.Sync(*prefixSum);
		prefixSum->Execute(1);
		prefixSum->Execute(2);

		space.value *= 2;
	}

	prefixSum->Execute(3);

	countingCount->Use();
	countingCount->Execute(1);

	glFinish();
	prefixSumBuffer.print();
	double detla = (double)glfwGetTime() - time;
	Console::success("Sorting") << "Computation finished in " << detla << "s (" << detla * 1000.0 << " ms)" << Console::endl;
	outDataBuffer.print();

	std::vector<GLuint> buf = outDataBuffer.read();
	GLuint check = 0;
	GLuint error = data.size() - buf.size();
	for (GLuint i : buf) {
		if (i != data[check]) error++;
		check++;
	}
	Console::success("Checking result") << "got" << error <<  " errors " << Console::endl;

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
