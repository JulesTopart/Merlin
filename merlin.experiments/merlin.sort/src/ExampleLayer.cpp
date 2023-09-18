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
	radixSort = ComputeShader::Create( "radixSort", "./assets/shaders/radix.sort.comp");
	radixSort->Use();

	dataBuffer = SSBO::Create("dataBuffer");
	dataBuffer->Bind();
	dataBuffer->SetBindingPoint(0);
	dataBuffer->Attach(*radixSort);
	dataBuffer->Allocate<GLuint>(data);

	prefixSumBuffer = SSBO::Create("prefixSumBuffer");
	prefixSumBuffer->Bind();
	prefixSumBuffer->SetBindingPoint(1);
	prefixSumBuffer->Attach(*radixSort);
	prefixSumBuffer->Allocate<GLuint>(10);

	Console::info("Data") << "Sorting data..." << Console::endl;
	for (GLuint i = 0; i < 4; ++i) {
		// Set uniforms or push constants for the radix divisor
		uint32_t radixDivisor = pow(10, i);
		radixSort->SetUInt("radixDivisor", radixDivisor);

		prefixSumBuffer->Bind();
		prefixSumBuffer->Clear();

		// Dispatch the compute shader
		radixSort->Dispatch(wgCount, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		debugBuffer<GLuint>(*prefixSumBuffer);


	}

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
