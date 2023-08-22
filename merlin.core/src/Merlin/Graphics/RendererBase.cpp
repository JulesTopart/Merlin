#include "glpch.h"
#include "RendererBase.h"
#include "Merlin/Core/Core.h"

namespace Merlin::Graphics {

	RendererBase::RendererBase() {
		backgroundColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	}

	RendererBase::~RendererBase() {
	}

	void RendererBase::DisableMultisampling() {
		glDisable(GL_MULTISAMPLE);
	}

	void RendererBase::DisableTransparency() {
		glDisable(GL_BLEND);
	}

	void RendererBase::EnableTransparency() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	void RendererBase::DisableSampleShading() {
		glDisable(GL_SAMPLE_SHADING);
	}

	void RendererBase::EnableSampleShading() {
		glEnable(GL_SAMPLE_SHADING);
		glMinSampleShading(8);
	}

	void RendererBase::EnableMultisampling() {
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);
	}

	void RendererBase::EnableFaceCulling() {
		// Enables Cull Facing
		glEnable(GL_CULL_FACE);
		// Keeps front faces
		glCullFace(GL_FRONT);
		// Uses counter clock-wise standard
		glFrontFace(GL_CCW);
	}

	void RendererBase::DisableFaceCulling() {
		// Enables Cull Facing
		glDisable(GL_CULL_FACE);
	}

	void RendererBase::EnableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}	
	
	void RendererBase::DisableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}	
	
	void RendererBase::EnableCubeMap() {
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}	
	
	void RendererBase::DisableCubeMap() {
		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void RendererBase::Clear() {
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererBase::SetBackgroundColor(float r, float g, float b, float a) {
		backgroundColor = glm::vec4(r, g, b, a);
	}



}