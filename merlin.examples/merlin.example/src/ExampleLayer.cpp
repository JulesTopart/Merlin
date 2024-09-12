#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

const float radius = 3;

ExampleLayer::ExampleLayer() {
	camera().setPosition(glm::vec3(0.7, -10, 2.4));
	camera().setRotation(glm::vec3(30, 0, +90));
}


namespace R_extract {
	using namespace glm;
	// Normalize a quaternion
	vec4 normalizeQuaternion(vec4 q) {
		float length = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		if (length > 1.0e-9) { // Avoid division by zero
			return vec4(q.x / length, q.y / length, q.z / length, q.w / length);
		}
		return vec4(0.0, 0.0, 0.0, 1.0); // Return identity quaternion if length is zero
	}

	vec4 multiplyQuaternions(vec4 a, vec4 b)
	{
		vec4 ans;
		ans.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;  // i
		ans.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;  // j
		ans.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;  // k
		ans.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;  // 1
		return ans;
	}


	// Convert angle-axis to quaternion
	vec4 angleAxisToQuaternion(float angle, vec3 axis) {
		axis = normalize(axis);
		float halfAngle = angle * 0.5;
		float s = sin(halfAngle);
		return vec4(axis * s, cos(halfAngle));
	}

	// Convert quaternion to rotation matrix

	mat3 quaternionToMatrix(vec4 quat)
	{
		vec4 quat2 = quat * quat;
		mat3 out;

		out[0].x = 1 - 2 * quat2.y - 2 * quat2.z;
		out[0].y = 2 * quat.x * quat.y + 2 * quat.w * quat.z;
		out[0].z = 2 * quat.x * quat.z - 2 * quat.w * quat.y;

		out[1].x = 2 * quat.x * quat.y - 2 * quat.w * quat.z;
		out[1].y = 1 - 2 * quat2.x - 2 * quat2.z;
		out[1].z = 2 * quat.y * quat.z + 2 * quat.w * quat.x;

		out[2].x = 2 * quat.x * quat.z + 2 * quat.w * quat.y;
		out[2].y = 2 * quat.y * quat.z - 2 * quat.w * quat.x;
		out[2].z = 1 - 2 * quat2.x - 2 * quat2.y;

		return out;
	}

	// Main function to extract rotation
	mat3 extractRotation(mat3 A, vec4& q,  int maxIter = 20) {
		if (length(q) == 0) q = vec4(0, 0, 0, 1);
		for (int iter = 0; iter < maxIter; iter++) {
			mat3 R = quaternionToMatrix(q);
			float denom = fabs(dot(R[0], A[0]) + dot(R[1], A[1]) + dot(R[2], A[2])) + 1.0e-9f;
			vec3 omega = cross(R[0], A[0]) + cross(R[1], A[1]) + cross(R[2], A[2]) / denom;

			float w = length(omega);
			if (w < 1.0e-9f)
				break;

			vec4 deltaQ = angleAxisToQuaternion(w, omega / w);
			q = multiplyQuaternions(deltaQ, q); // Apply deltaQ to current q
			q = normalize(q); // Normalize to maintain numerical stability
		}
		return quaternionToMatrix(q);
	}

}


void ExampleLayer::onAttach(){
	Layer3D::onAttach();

	renderer.initialize();
	renderer.setEnvironmentGradientColor(0.903, 0.803, 0.703);
	renderer.showLights();

	Shared<Model> bunny = ModelLoader::loadModel("./assets/common/models/bunny.stl");
	bunny->meshes()[0]->smoothNormals();
	bunny->setMaterial("pearl");
	bunny->scale(0.2);
	bunny->translate(glm::vec3(0,0,-0.5));
	//bunny->setShader("debug.normals");
	scene.add(bunny);

	/**/
	light = createShared<PointLight>("light0");
	light->translate(glm::vec3(radius, radius, 3));
	light->setAttenuation(glm::vec3(0.6, 0.08, 0.008));
	light->setAmbient(0.09, 0.05, 0.05);
	light->setDiffuse(1, 1, 1);
	scene.add(light);
	/**/

	Shared<DirectionalLight>  dirlight;

	/**/
	dirlight = createShared<DirectionalLight>("light1", glm::vec3(-0.5f, 0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(0.2, 0, 0));
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light2", glm::vec3(0.5f, 0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(0.0, 0.2, 0));
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light3", glm::vec3(0.0f, -0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(0.0,0,0.2));
	scene.add(dirlight);
	/**/

	/**/
	Shared<AmbientLight> amLight = createShared<AmbientLight>("light4");
	amLight->setAmbient(glm::vec3(0.1));
	scene.add(amLight);
	/**/

	scene.add(Primitives::createFloor(50, 0.5));



	//scaling of 2 in x and 3 in y, and translation by (1, 1)
	glm::mat3 T = glm::mat3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);

	glm::mat3 prevR = glm::mat3(1);


	static float target = 90;
	glm::vec4 q = glm::vec4(0);

	for (int i = 0; i <= target; i++) {

		float angle = i * DEG_TO_RAD;
		glm::mat3 R = glm::mat3(
			glm::cos(angle), -glm::sin(angle), 0.0,
			glm::sin(angle), glm::cos(angle), 0.0,
			0.0, 0.0, 1.0
		);


		glm::mat3 A = R * T;

		Console::print() << "Matrix R is : " << Console::endl;
		Console::print() << "[ " << R[0][0] << ", " << R[0][1] << ", " << R[0][2] << "]" << Console::endl;
		Console::print() << "[ " << R[1][0] << ", " << R[1][1] << ", " << R[1][2] << "]" << Console::endl;
		Console::print() << "[ " << R[2][0] << ", " << R[2][1] << ", " << R[2][2] << "]" << Console::endl;
	
		glm::mat3 eR = R_extract::extractRotation(A, q,  100);

		Console::print() << "Matrix exR is : " << Console::endl;
		Console::print() << "[ " << eR[0][0] << ", " << eR[0][1] << ", " << eR[0][2] << "]" << Console::endl;
		Console::print() << "[ " << eR[1][0] << ", " << eR[1][1] << ", " << eR[1][2] << "]" << Console::endl;
		Console::print() << "[ " << eR[2][0] << ", " << eR[2][1] << ", " << eR[2][2] << "]" << Console::endl;



	}
	//scene.add(Primitives::createRectangle(10, 10));
}

void ExampleLayer::onDetach(){}

void ExampleLayer::onEvent(Event& event){
	Layer3D::onEvent(event);
}

float t = 0.0;







void ExampleLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);
	const float hpi = 3.14159265358;
	t += ts;

	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t) * radius - x, sin(t) * radius - y, 0.0));

	renderer.clear();
	renderer.renderScene(scene, camera());
}

void ExampleLayer::onImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera().getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);

	}
	ImGui::End();

	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<Shared<RenderableObject>>&)> traverseNodes = [&](const std::list<Shared<RenderableObject>>& nodes){
		for (auto& node : nodes){
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open){
				
				if (node != nullptr){
					ImGui::Text(node->name().c_str());
				}

				traverseNodes(node->children());
				ImGui::TreePop();
			}
		}
	};

	// draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene.nodes());
	ImGui::End();
	


}
