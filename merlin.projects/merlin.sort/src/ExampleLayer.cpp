#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <GLFW/glfw3.h>
#include <execution>

ExampleLayer::ExampleLayer(){}

ExampleLayer::~ExampleLayer(){}


int getwkSize(int index) {
	switch (index) {
	case 0:
		return 1;
		break;
	case 1 :
		return 32;
		break;
	case 2 :
		return 64;
		break;
	case 3 :
		return 128;
		break;
	case 4 :
		return 256;
		break;
	case 5 :
		return 512;
		break;
	case 6:
		return 1024;
		break;
	default :
		1;
	}
}

Shared<StagedComputeShader> getPrefixSumShader(int index) {
	switch (index) {
	case 1:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x1.comp", 4);
		break;
	case 32:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x32.comp", 4);
		break;
	case 64:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x64.comp", 4);
		break;
	case 128:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x128.comp", 4);
		break;
	case 256:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x256.comp", 4);
		break;
	case 512:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x512.comp", 4);
		break;
	case 1024:
		return createShared<StagedComputeShader>("prefix.sum", "./assets/shaders/prefix.sum.x1024.comp", 4);
		break;
	default:
		return nullptr;
	}
}

void ExampleLayer::onAttach(){
	enableGLDebugging();
	Console::setLevel(ConsoleLevel::_TRACE);

	std::vector<std::string> results;

	for (GLuint dataSize = 512; dataSize <= 512*512*512; dataSize*=2)
	for(GLuint wkIndex = 0; wkIndex <= 6; wkIndex++)
		for (GLuint i = 0; i < 10; i++) {
			GLuint wk = getwkSize(wkIndex);
			Console::print() << "Starting new benchmark over " << dataSize << " entry " << ", using " << wk << " thread per workgroup" << Console::endl;
			//results.push_back( std::to_string(dataSize) + ";" + std::to_string(wk) + ";" + std::to_string(i) + ";" + std::to_string(bench_cpu(dataSize)) + Console::endl);
			results.push_back( std::to_string(dataSize) + ";" + std::to_string(wk) + ";" + std::to_string(i) + ";" + std::to_string(bench_cpu(dataSize)) + ";" + std::to_string(bench_gpu(wk, dataSize)) + Console::endl);
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

double ExampleLayer::bench_cpu(int n) {

	std::vector<GLuint> data;

	Console::info() << "Generating data..." << Console::endl;
	for (GLuint i = 0; i < n; i++) {
		data.push_back(i);
	}

	std::vector<GLuint> unsorted = data;
	Console::info() << "Shuffling data..." << Console::endl;
	std::shuffle(unsorted.begin(), unsorted.end(), std::default_random_engine());

	double time = (double)glfwGetTime();
	std::sort(std::execution::par, unsorted.begin(), unsorted.end());
	double delta = (double)glfwGetTime() - time;
	Console::success("Sorting") << "CPU Computation finished in " << delta << "s (" << delta * 1000.0 << " ms)" << Console::endl;

	//debugVector<GLuint>(unsorted);
	return delta;
}

double ExampleLayer::bench_gpu(int wkSize, int n){
	SSBO<GLuint> inDataBuffer;
	SSBO<GLuint> outDataBuffer;
	SSBO<GLuint> compactSumBuffer;
	SSBO<GLuint> prefixSumBuffer;

	Shared<StagedComputeShader> prefixSum;
	Shared<StagedComputeShader> countingCount;

	std::vector<GLuint> data;

	GLuint wgSize = 64; //WorkGroup size
	GLuint wgCount = (n + wgSize - 1) / wgSize; //WorkGroup size

	GLuint blockSize = floor(log2f(n));
	GLuint blocks = (n + blockSize - 1) / blockSize;

	GLuint bwgSize = wkSize; //WorkGroup size
	GLuint bwgCount = (blocks + bwgSize - 1) / bwgSize; //WorkGroup size

	Console::info() << "Generating data..." << Console::endl;
	for (GLuint i = 0; i < n; i++) {
		data.push_back(i);
	}

	std::vector<GLuint> unsorted = data;
	Console::info() << "Shuffling data..." << Console::endl;
	std::shuffle(unsorted.begin(), unsorted.end(), std::default_random_engine());

	//init buffers
	prefixSum = getPrefixSumShader(wkSize);
	prefixSum->use();
	prefixSum->setUInt("dataSize", n);
	prefixSum->setUInt("blockSize", blockSize);
	prefixSum->setUInt("blocks", blocks);
	prefixSum->SetWorkgroupLayout(bwgCount);

	countingCount = createShared<StagedComputeShader>("counting.count", "./assets/shaders/counting.count.comp", 2);
	countingCount->use();
	countingCount->setUInt("dataSize", n);
	countingCount->setUInt("blockSize", blockSize);
	countingCount->setUInt("blocks", blocks);
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

	//inDataBuffer.print();

	Console::info("Sorting") << "Starting..." << Console::endl;
	double time = (double)glfwGetTime();

	countingCount->use();
	countingCount->execute(0);

	//prefixSumBuffer.print();

	Console::info() << "Data : " << unsorted.size() << " uint values" << Console::endl;
	Console::info() << "Parallelizing counting sort over " << blocks << " blocks ( " << blockSize << " values per blocks)" << Console::endl;

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
	//prefixSumBuffer.print();
	double delta = (double)glfwGetTime() - time;
	Console::success("Sorting") << "GPU Computation finished in " << delta << "s (" << delta * 1000.0 << " ms)" << Console::endl;
	//outDataBuffer.print();

	std::vector<GLuint> buf = outDataBuffer.read();
	GLuint check = 0;
	GLuint error = data.size() - buf.size();
	for (GLuint i : buf) {
		if (i != data[check]) error++;
		check++;
	}
	if(error == 0) Console::success("Checking result") << "got" << error << " errors " << Console::endl;
	else {
		Console::error("Checking result") << "got" << error << " errors " << Console::endl;
		throw;
	}
	return delta;
}
