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
	countingSort = ComputeShader::Create( "radixSort", "./assets/shaders/counting.sort.comp");
	countingCount = ComputeShader::Create( "radixSort", "./assets/shaders/counting.count.comp");

	dataBuffer = SSBO::Create("inDataBuffer");
	dataBuffer->Bind();
	dataBuffer->Attach(*countingSort, 0);
	dataBuffer->Attach(*countingCount, 0);
	dataBuffer->Allocate<GLuint>(data);

	prefixSumBuffer = SSBO::Create("prefixSumBuffer");
	prefixSumBuffer->Bind();
	prefixSumBuffer->Attach(*countingSort, 1);
	prefixSumBuffer->Attach(*countingCount, 1);
	prefixSumBuffer->Allocate<GLuint>(10);

	countingCount->Use();
	countingCount->SetUInt("size", n);



	countingSort->Use();
	countingSort->SetUInt("size", n);

	Console::info("Data") << "Sorting data..." << Console::endl;


	

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
