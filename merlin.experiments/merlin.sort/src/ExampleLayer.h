#pragma once

#include <Merlin.h>
using namespace Merlin::Graphics;

const GLuint n = 1000; //Number of data
const GLuint wgSize = 32; //WorkGroup size
const GLuint wgCount = (n + wgSize - 1) / wgSize; //WorkGroup size

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

	Shared<SSBO> dataBuffer;
	Shared<SSBO> prefixSumBuffer;

	std::vector<GLuint> data;

	Shared<ComputeShader> radixSort;

};