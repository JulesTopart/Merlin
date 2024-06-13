#pragma once

#include <Merlin.h>

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
	double bench_cpu(int dataSize);
	double bench_gpu(int wkSize, int dataSize);

private:
};