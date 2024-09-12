#pragma once
#include "merlin/core/core.h"

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
		inline bool useFaceCulling() const { return use_culling; }

		void enableDepthTest();
		void disableDepthTest();

		void enableCubeMap();
		void disableCubeMap();


	private:
		bool use_culling = true;
		glm::vec4 backgroundColor;
	};
}
