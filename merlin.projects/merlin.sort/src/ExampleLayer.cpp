#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <GLFW/glfw3.h>

std::vector<GLuint> data;

ExampleLayer::ExampleLayer(){
	Window* w = &Application::get().getWindow();
	int height = w->getHeight();
	int width = w->getWidth();
}

ExampleLayer::~ExampleLayer(){}


void ExampleLayer::onAttach(){
	enableGLDebugging();
	Console::setLevel(ConsoleLevel::_INFO);

	Console::print() << "Generating data..." << Console::endl;
	for (GLuint i = 0; i < n; i++) {
		data.push_back(i);
	}

	std::vector<std::string> results;

	GLuint wk = 1;
	for (GLuint i = 0; i < 10; i++) {
		results.push_back( std::to_string(n) + ";" + std::to_string(wk) + ";" + std::to_string(i) + ";" + std::to_string(bench_cpu()) + ";" + std::to_string(bench_gpu(wk)) + Console::endl);
	}

	Console::print() << "------------- CSV -------------" << Console::endl;
	Console::print() << "Nb entry; WkGrpSize; sample; CPU std::sort; GPU counting sort" << Console::endl;
	for (auto& str : results)
		Console::print() << str;

	
}

void ExampleLayer::onDetach(){}

void ExampleLayer::onEvent(Event& event){}

void ExampleLayer::onUpdate(Timestep ts){}

void ExampleLayer::onImGuiRender(){}

double ExampleLayer::bench_cpu() {
	std::vector<GLuint> unsorted = data;
	Console::print() << "Shuffling data..." << Console::endl;
	std::shuffle(unsorted.begin(), unsorted.end(), std::default_random_engine());

	double time = (double)glfwGetTime();
	std::sort(unsorted.begin(), unsorted.end());
	double delta = (double)glfwGetTime() - time;
	Console::success("Sorting") << "CPU Computation finished in " << delta << "s (" << delta * 1000.0 << " ms)" << Console::endl;

	debugVector<GLuint>(unsorted);
	return delta;
}

double ExampleLayer::bench_gpu(int wkSize){
	SSBO<GLuint> inDataBuffer;
	SSBO<GLuint> outDataBuffer;
	SSBO<GLuint> compactSumBuffer;
	SSBO<GLuint> prefixSumBuffer;

	Shared<StagedComputeShader> prefixSum;
	Shared<StagedComputeShader> countingCount;

	GLuint bwgSize = wkSize; //WorkGroup size
	GLuint bwgCount = (blocks + bwgSize - 1) / bwgSize; //WorkGroup size

	std::vector<GLuint> unsorted = data;
	Console::print() << "Shuffling data..." << Console::endl;
	std::shuffle(unsorted.begin(), unsorted.end(), std::default_random_engine());

	//init buffers
	prefixSum = createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.comp", 4);
	countingCount = createShared<StagedComputeShader>("counting.count", "./assets/shaders/counting.count.comp", 2);
	prefixSum->SetWorkgroupLayout(bwgCount);
	countingCount->SetWorkgroupLayout(wgCount);

	inDataBuffer.bind();
	inDataBuffer.rename("inDataBuffer");
	inDataBuffer.write(unsorted);

	outDataBuffer.bind();
	outDataBuffer.rename("outDataBuffer");
	outDataBuffer.write(unsorted);

	prefixSumBuffer.bind();
	prefixSumBuffer.rename("prefixSumBuffer");
	prefixSumBuffer.reserve(data.size());

	compactSumBuffer.bind();
	compactSumBuffer.rename("compactSumBuffer");
	compactSumBuffer.reserve(blocks);

	inDataBuffer.setBindingPoint(0);
	outDataBuffer.setBindingPoint(1);
	prefixSumBuffer.setBindingPoint(2);
	compactSumBuffer.setBindingPoint(3);

	prefixSum->use();
	prefixSum->attach(compactSumBuffer);
	prefixSum->attach(prefixSumBuffer);

	countingCount->use();
	countingCount->attach(inDataBuffer);
	countingCount->attach(outDataBuffer);
	countingCount->attach(prefixSumBuffer);

	inDataBuffer.print();

	Console::info("Sorting") << "Starting..." << Console::endl;
	double time = (double)glfwGetTime();

	countingCount->use();
	countingCount->execute(0);

	prefixSumBuffer.print();

	Console::print() << "Data : " << unsorted.size() << " uint values" << Console::endl;
	Console::print() << "Parallelizing counting sort over " << blocks << " blocks ( " << blockSize << " values per blocks)" << Console::endl;

	prefixSum->use();
	prefixSum->execute(0);

	//Binary tree on rightmost element of blocks
	GLuint steps = blockSize;
	UniformObject<GLuint> space("space");
	space.value = 1;

	for (GLuint step = 0; step < blockSize; step++) {
		// Calls the parallel operation

		space.sync(*prefixSum);
		prefixSum->execute(1);
		prefixSum->execute(2);

		space.value *= 2;
	}

	prefixSum->execute(3);

	countingCount->use();
	countingCount->execute(1);

	glFinish();
	prefixSumBuffer.print();
	double delta = (double)glfwGetTime() - time;
	Console::success("Sorting") << "GPU Computation finished in " << delta << "s (" << delta * 1000.0 << " ms)" << Console::endl;
	outDataBuffer.print();

	std::vector<GLuint> buf = outDataBuffer.read();
	GLuint check = 0;
	GLuint error = data.size() - buf.size();
	for (GLuint i : buf) {
		if (i != data[check]) error++;
		check++;
	}
	Console::success("Checking result") << "got" << error << " errors " << Console::endl;

	return delta;
}
