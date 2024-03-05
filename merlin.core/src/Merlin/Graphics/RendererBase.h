#pragma once
#include "Merlin/Core/Core.h"

namespace Merlin {
	class RendererBase {
	public:
		RendererBase();
		~RendererBase();

		virtual void clear();
		void setBackgroundColor(float r, float g, float b, float a);

		void enableTransparency();
		void disableTransparency();

		void enableSampleShading();
		void disableSampleShading();

		void enableMultisampling();
		void disableMultisampling();

		void enableFaceCulling();
		void disableFaceCulling();

		void enableDepthTest();
		void disableDepthTest();

		void enableCubeMap();
		void disableCubeMap();


	private:

		glm::vec4 backgroundColor;
	};
}
