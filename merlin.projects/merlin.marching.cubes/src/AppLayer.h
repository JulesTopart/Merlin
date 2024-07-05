#pragma once
#include "settings.h"

using namespace Merlin;

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define GRANULAR 4
#define BOUNDARY 5

class AppLayer : public Merlin::Layer3D {
public:
	AppLayer(){}

	void onAttach() override;
	void onDetach() override;
	void onEvent(Merlin::Event& event) override;
	void onUpdate(Merlin::Timestep ts) override;
	void onImGuiRender() override;

	void InitGraphics();
	void SyncUniforms();


private:

	ComputeShader_Ptr noise;
	ComputeShader_Ptr marchingCubes;
	Texture2D_Ptr texture_debug;
	Texture3D_Ptr volume;
	Shader_Ptr mcShader;

	ImmutableSSBO_Ptr<glm::vec4> buffer_vertices;
	ImmutableSSBO_Ptr<glm::vec4> buffer_normals;

	ImmutableSSBO_Ptr<GLint> buffer_triangle_table;
	ImmutableSSBO_Ptr<GLint> buffer_configuration_table;

	Settings settings;

	Scene scene;
	Renderer renderer;

	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };

	bool paused = true;

};