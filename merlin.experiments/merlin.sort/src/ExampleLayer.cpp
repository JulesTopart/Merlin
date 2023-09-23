#include "ExampleLayer.h"

using namespace Merlin;
using namespace Merlin::Utils;
using namespace Merlin::Memory;
using namespace Merlin::Graphics;

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>

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
	debugVector(data);

	Console::print() << "Shuffling data..." << Console::endl;
	std::shuffle(data.begin(), data.end(), std::default_random_engine());
	debugVector(data);

	//Init buffers
	countingCount = ComputeShader::Create( "counting.count", "./assets/shaders/counting.count.comp");
	prefixSum = ComputeShader::Create( "prefix.sum", "./assets/shaders/prefix.sum.comp");

	dataBuffer = SSBO::Create("inDataBuffer");
	dataBuffer->Bind();
	dataBuffer->Attach(*prefixSum, 0);
	dataBuffer->Attach(*countingCount, 0);
	dataBuffer->Allocate<GLuint>(data);

	compactSumBuffer = SSBO::Create("compactSumBuffer");
	compactSumBuffer->Bind();
	compactSumBuffer->Attach(*prefixSum, 1);
	compactSumBuffer->Attach(*countingCount, 1);
	compactSumBuffer->Allocate<GLuint>(blocks);

	prefixSumBuffer = SSBO::Create("prefixSumBuffer");
	prefixSumBuffer->Bind();
	prefixSumBuffer->Attach(*prefixSum, 2);
	prefixSumBuffer->Attach(*countingCount, 2);
	prefixSumBuffer->Allocate<GLuint>(data.size());

	countingCount->Use();
	countingCount->Dispatch(data.size());
	glFinish();

	debugBuffer<GLuint>(*prefixSumBuffer);
	
	Console::print() << "Parallelizing prefixSum over " << blocks << " blocks ( " << blockSize << " values per blocks)" << Console::endl;

	prefixSum->Use();
	prefixSum->SetUInt("stage", 0);
	prefixSum->Dispatch(blocks);
	glFinish();
	debugBuffer<GLuint>(*prefixSumBuffer);
	
	//Binary tree on rightmost element of blocks
	GLuint steps = blockSize;
	GLuint space = 1;

	prefixSum->SetUInt("stage", 1);
	for (GLuint step = 0; step < blocks; step++) {
		// Calls the parallel operation
		
		prefixSum->SetUInt("space", space);
		prefixSum->Dispatch(blocks);
		glFinish();

		debugBuffer<GLuint>(*compactSumBuffer);

		space *= 2;
	}

	prefixSum->SetUInt("stage", 2);
	prefixSum->Dispatch(blocks);
	
	debugBuffer<GLuint>(*prefixSumBuffer);
	debugBuffer<GLuint>(*dataBuffer);
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
