#include "glpch.h"
#include "rendererBase.h"
#include "merlin/core/core.h"

namespace Merlin {

	RendererBase::RendererBase() {
		backgroundColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	}

	RendererBase::~RendererBase() {
	}

	void RendererBase::disableMultisampling() {
		glDisable(GL_MULTISAMPLE);
	}

	void RendererBase::disableTransparency() {
		glDisable(GL_BLEND);
	}

	void RendererBase::enableTransparency() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	void RendererBase::disableSampleShading() {
		glDisable(GL_SAMPLE_SHADING);
	}

	void RendererBase::enableSampleShading() {
		glEnable(GL_SAMPLE_SHADING);
		glMinSampleShading(8);
	}

	void RendererBase::enableMultisampling() {
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);
	}

	void RendererBase::enableFaceCulling() {
		// Enables Cull Facing
		glEnable(GL_CULL_FACE);
		// Keeps front faces
		glCullFace(GL_BACK);
		// Uses counter clock-wise standard
		glFrontFace(GL_CCW);

		use_culling = true;
	}

	void RendererBase::disableFaceCulling() {
		// Enables Cull Facing
		glDisable(GL_CULL_FACE);
		use_culling = false;
	}

	void RendererBase::enableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}	
	
	void RendererBase::disableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}	
	
	void RendererBase::enableCubeMap() {
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}	
	
	void RendererBase::disableCubeMap() {
		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void RendererBase::clear() {
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererBase::setBackgroundColor(float r, float g, float b, float a) {
		backgroundColor = glm::vec4(r, g, b, a);
	}



}