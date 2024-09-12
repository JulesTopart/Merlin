#pragma once

#include <Merlin.h>
using namespace Merlin;

class AppLayer : public Layer3D
{
public:
	AppLayer();
	virtual ~AppLayer();

	void setupScene();
	void setupPhysics();
	void onPhysicsUpdate(Timestep ts);

	virtual void onAttach() override;
	virtual void onDetach() override;
	virtual void onEvent(Event& event) override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Scene scene;
	Renderer renderer;

	ParticleSystem_Ptr ps;
	ComputeShader_Ptr solver;
	//ParticleSystem_Ptr bs;

	glm::vec3 model_matrix_translation = { 0.8f, 0.2f, 0.3f};
};