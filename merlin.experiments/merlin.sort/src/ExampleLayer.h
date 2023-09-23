#pragma once

#include <Merlin.h>
using namespace Merlin::Graphics;

const GLuint n = 32*32*32; //Number of data
const GLuint wgSize = 64; //WorkGroup size
const GLuint wgCount = (n + wgSize - 1) / wgSize; //WorkGroup size

const GLuint blockSize = floor(log2f(n));
const GLuint blocks = (n + blockSize - 1) / blockSize;



class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Merlin::Event& event) override;
	virtual void OnUpdate(Merlin::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:

	Shared<SSBO> inDataBuffer;
	Shared<SSBO> outDataBuffer;
	Shared<SSBO> compactSumBuffer;
	Shared<SSBO> prefixSumBuffer;

	std::vector<GLuint> data;

	Shared<ComputeShader> prefixSum;
	Shared<ComputeShader> countingCount;

};