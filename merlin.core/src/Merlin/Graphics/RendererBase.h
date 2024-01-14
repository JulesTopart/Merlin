#pragma once
#include "Merlin/Core/Core.h"

namespace Merlin::Graphics {
	class RendererBase {
	public:
		RendererBase();
		~RendererBase();

		virtual void Clear();
		void SetBackgroundColor(float r, float g, float b, float a);

		void EnableTransparency();
		void DisableTransparency();

		void EnableSampleShading();
		void DisableSampleShading();

		void EnableMultisampling();
		void DisableMultisampling();

		void EnableFaceCulling();
		void DisableFaceCulling();

		void EnableDepthTest();
		void DisableDepthTest();

		void EnableCubeMap();
		void DisableCubeMap();


	private:

		glm::vec4 backgroundColor;
	};
}
