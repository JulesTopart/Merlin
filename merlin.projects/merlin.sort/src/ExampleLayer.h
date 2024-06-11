#pragma once

#include <Merlin.h>

const GLuint n = 512; //Number of data
const GLuint wgSize = 64; //WorkGroup size
const GLuint wgCount = (n + wgSize - 1) / wgSize; //WorkGroup size

const GLuint blockSize = floor(log2f(n));
const GLuint blocks = (n + blockSize - 1) / blockSize;

class ExampleLayer : public Merlin::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onEvent(Merlin::Event& event) override;
	virtual void onUpdate(Merlin::Timestep ts) override;
	virtual void onImGuiRender() override;
	double bench_cpu();
	double bench_gpu(int wkSize);

private:
	std::vector<GLuint> data;
};