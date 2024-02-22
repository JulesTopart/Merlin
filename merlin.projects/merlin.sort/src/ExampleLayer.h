#pragma once

#include <Merlin.h>

const GLuint n = 512 * 512 * 512; //Number of data
const GLuint wgSize = 512; //WorkGroup size
const GLuint wgCount = (n + wgSize - 1) / wgSize; //WorkGroup size

const GLuint blockSize = floor(log2f(n));
const GLuint blocks = (n + blockSize - 1) / blockSize;

const GLuint bwgSize = 64; //WorkGroup size
const GLuint bwgCount = (blocks + bwgSize - 1) / bwgSize; //WorkGroup size

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
	SSBO<GLuint> inDataBuffer;
	SSBO<GLuint> outDataBuffer;
	SSBO<GLuint> compactSumBuffer;
	SSBO<GLuint> prefixSumBuffer;

	std::vector<GLuint> data;

	Shared<StagedComputeShader> prefixSum;
	Shared<StagedComputeShader> countingCount;
};