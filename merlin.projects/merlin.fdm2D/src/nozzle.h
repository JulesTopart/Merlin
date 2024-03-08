#pragma once
#include "Merlin.h"
#include <vector>
#include <limits>


std::vector<glm::vec2> createNozzleChamberGeom(float w, float h, float o) {
	std::vector<glm::vec2> geom = {
		glm::vec2(o / 2.0, h/4),
		glm::vec2((o / 2.0) + w / 4, h/2),
		glm::vec2((o / 2.0) + w / 4, h),
		glm::vec2(-(o / 2.0) - w / 4, h),
		glm::vec2(-(o / 2.0) - w / 4, h / 2),
		glm::vec2(-o / 2.0, h / 4)
	};
	return geom;
}

std::vector<glm::vec2> createRightNozzleGeom(float w, float h, float o) {
	std::vector<glm::vec2> geom = {
		glm::vec2(o / 2.0, 0),
		glm::vec2((o / 2.0) + w / 4, 0),
		glm::vec2((o / 2.0) + w / 2, h / 4),
		glm::vec2((o / 2.0) + w / 2, h),
		glm::vec2((o / 2.0) + w / 4, h),
		glm::vec2((o / 2.0) + w / 4, h / 2),
		glm::vec2((o / 2.0), h / 4),
		glm::vec2((o / 2.0), 0)
	};
	return geom;
}

std::vector<glm::vec2> createLeftNozzleGeom(float w, float h, float o) {
	std::vector<glm::vec2> geom = {
		glm::vec2( -o / 2.0, 0),
		glm::vec2(-(o / 2.0) - w / 4, 0),
		glm::vec2(-(o / 2.0) - w / 2, h / 4),
		glm::vec2(-(o / 2.0) - w / 2, h),
		glm::vec2(-(o / 2.0) - w / 4, h),
		glm::vec2(-(o / 2.0) - w / 4, h / 2),
		glm::vec2(-(o / 2.0), h / 4),
		glm::vec2(-(o / 2.0), 0)
	};
	return geom;
}


