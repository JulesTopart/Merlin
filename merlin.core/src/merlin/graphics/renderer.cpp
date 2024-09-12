#include "glpch.h"
#include "merlin/core/core.h"
#include "renderer.h"




namespace Merlin {

	Renderer::Renderer() : currentTransform(glm::mat4(1.0)) {}

	Renderer::~Renderer() {}

	void Renderer::initialize() {
		resetGlobalTransform();
		enableMultisampling();
		enableSampleShading();
		enableDepthTest();
		enableCubeMap();
		enableFaceCulling();
		m_defaultEnvironment = createShared<Environment>("defaultEnvironment", 16);
		m_defaultAmbient = createShared<AmbientLight>("defaultAmbientLight");
		m_defaultAmbient->setAmbient(glm::vec3(0.2));
		m_defaultDirLight = createShared<DirectionalLight>("defaultDirLight", glm::vec3(1,-0.6,-1)* glm::vec3(3));
		m_defaultDirLight2 = createShared<DirectionalLight>("defaultDirLight2", glm::vec3(-0.5,0.6,-1)*glm::vec3(3));
		m_defaultDirLight3 = createShared<DirectionalLight>("defaultDirLight3", glm::vec3(-1.0,-0.6,-0.6)* glm::vec3(3));
	}

	void Renderer::pushMatrix() {
		matrixStack.push(currentTransform);
	}

	void Renderer::popMatrix() {
		if (!matrixStack.empty()) {
			currentTransform = matrixStack.top();
			matrixStack.pop();
		}
	}

	void Renderer::resetMatrix() {
		while(!matrixStack.empty()) {
			matrixStack.pop();
		}
		currentTransform = m_globalTransform;
	}

	void Renderer::gatherLights(const Shared<RenderableObject>& object) {
		if (const auto light = std::dynamic_pointer_cast<Light>(object)) {
			light->applyRenderTransform(currentTransform);
			m_activeLights.push_back(light);
		}
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			BoundingBox bb = mesh->getBoundingBox();
			glm::vec3 size = bb.max - bb.min;
			float maxScale = glm::max(glm::max(size.x, size.y), size.z);
			if (maxScale > m_scene_scale)m_scene_scale = maxScale;
		}
		if (const auto model = std::dynamic_pointer_cast<Model>(object)) {
			for (const auto& mesh : model->meshes()) {
				if (!model->isHidden()) gatherLights(mesh);
			}
		}

		for (const auto& child : object->children()) {
			if (!child->isHidden()) gatherLights(child);
		}
	}

	void Renderer::renderScene(const Scene& scene, const Camera& camera) {
		if (debug)Console::info() << "Rendering scene" << Console::endl;

		//Gather lights
	

		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) gatherLights(node);
		}

		if(debug)Console::info() << "Rendering scene shadows" << Console::endl;

		if (m_activeLights.size() == 0 || use_default_light) {
			m_activeLights.push_back(m_defaultAmbient);
			m_activeLights.push_back(m_defaultDirLight);
			m_activeLights.push_back(m_defaultDirLight2);
			m_activeLights.push_back(m_defaultDirLight3);
		}

		if (use_shadows) {
			if (useFaceCulling()) glDisable(GL_CULL_FACE);
			for (const auto& light : m_activeLights) {
				if (!light->castShadow()) continue;
				castShadow(light, scene);
			}
			if (useFaceCulling()) glEnable(GL_CULL_FACE);
		}
		
		camera.restoreViewport();
		//Render the scene
		if (debug)Console::info() << "Rendering scene objects" << Console::endl;
		for (const auto& node : scene.nodes()) {
			if(!node->isHidden()) render(node, camera);
		}

		if (scene.hasEnvironment()) {
			m_currentEnvironment = scene.getEnvironment();
			renderEnvironment(*m_currentEnvironment, camera);
		}
		else {
			renderEnvironment(*m_defaultEnvironment, camera);
		}
	}


	void Renderer::renderEnvironment(const Environment& env, const Camera& camera){
		if(debug)Console::info() << "Rendering Environment" << Console::endl;
		if (!use_environment) return;
		Shared<Shader> shader = getShader("default.skybox");
		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}
		glDepthFunc(GL_LEQUAL);
		if(useFaceCulling())glDisable(GL_CULL_FACE);
		shader->use();
		Texture2D::resetTextureUnits();
		env.attach(*shader);
		shader->setVec3("environment.ambient", env.gradientColor());
		shader->setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix()))); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		env.draw();
		if (useFaceCulling())glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		env.detach();
	}


	void Renderer::renderDepth(const Shared<RenderableObject>& object, Shared<Shader> shader){
		if (debug)Console::info() << "Rendering Depth" << Console::endl;
		pushMatrix();
		currentTransform *= object->transform();

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			if (mesh->castShadow()) {
				shader->setMat4("model", currentTransform); //sync model matrix with GPU
				mesh->draw();
			}
		}else if (const auto model = std::dynamic_pointer_cast<Model>(object)) {
			for (const auto& mesh : model->meshes()) {
				renderDepth(mesh, shader);
			}
		}
		for (auto node : object->children()) {
			renderDepth(node, shader);//Propagate to childrens
		}

		popMatrix();
	}

	void Renderer::renderLight(const Light& li, const Camera& camera){
		if (debug)Console::info() << "Rendering Light" << Console::endl;
		Shared<Shader> shader = getShader("default.light");
		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}
		//Texture2D::resetTextureUnits();
		shader->use();
		shader->setVec3("light_color", li.diffuse() + li.ambient() + li.specular());

		shader->setMat4("model", currentTransform); //sync model matrix with GPU

		shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		li.draw();
	}

	void Renderer::renderMesh(const Mesh& mesh, const Camera& camera) {
		if (debug)Console::info() << "Rendering Mesh" << Console::endl;
		Material_Ptr mat = nullptr;
		Shader_Ptr shader = nullptr;

		if (mesh.hasMaterial()) mat = mesh.getMaterial();
		else mat = getMaterial(mesh.getMaterialName());

		if (mesh.hasShader())
			shader = mesh.getShader();
		else {
			if (mesh.getShaderName() == "default") {
				if (mat->type() == MaterialType::PHONG) shader = getShader("default.phong");
				else if (mat->type() == MaterialType::PBR) shader = getShader("default.pbr");
				else {
					Console::error("Renderer") << "This material has no suitable shader. Please bind it manually" << Console::endl;
				}
			}
			else shader = getShader(mesh.getShaderName());
		}

		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}

		Texture2D::resetTextureUnits();
		shader->use();

		if (shader->supportLights()) {
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->attach(i, *shader);
			}
		}

		mat->attach(*shader);

		if(m_currentEnvironment != nullptr)
			m_currentEnvironment->attach(*shader);
		else m_defaultEnvironment->attach(*shader);

		if (shader->supportLights()) shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
		shader->setMat4("model", currentTransform); //sync model matrix with GPU
		shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
		if(shader->supportLights()) shader->setInt("use_flat_shading", mesh.useFlatShading());
		if(shader->supportShadows()) shader->setInt("useShadows", use_shadows);

		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		if (shader->supportLights()) shader->setInt("numLights", m_activeLights.size());

		if (shader->supportMaterial()) shader->setInt("use_vertex_color", mesh.useVertexColors());

		mesh.draw();
		mat->detach();

		if (shader->supportEnvironment()) {
			if (m_currentEnvironment != nullptr)
				m_currentEnvironment->detach();
			else m_defaultEnvironment->detach();
		}

		if(shader->supportLights()) {
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->detach();
			}
		}

	}


	void Renderer::castShadow(Shared<Light> light, const Scene& scene) {
		if (debug)Console::info() << "Cast Shadow" << Console::endl;
		Shared<FrameBuffer> fbo;
		Shared<Shader> shader;

		if (light->type() == LightType::Directional || light->type() == LightType::Spot) shader = getShader("shadow.depth");
		else if (light->type() == LightType::Point) shader = getShader("shadow.omni");
		else return;

		fbo = light->shadowFBO();

		if (!shader || !fbo) {
			Console::error("Renderer") << "Renderer failed to gather ressoureces for shadows (shader, framebuffer or texture)" << Console::endl;
			return;
		}

		glViewport(0, 0, light->shadowResolution(), light->shadowResolution());
		fbo->bind();
		
		glClear(GL_DEPTH_BUFFER_BIT);

		Texture2D::resetTextureUnits();
		shader->use();
		light->attachShadow(*shader, m_scene_scale);
		
		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) {
				renderDepth(node, shader);
			};
		}
		fbo->unbind();
	}


	
	void Renderer::renderParticleSystem(const ParticleSystem& ps, const Camera& camera) {
		if (debug)Console::info() << "Rendering Particle System" << Console::endl;
		if (ps.getDisplayMode() != ParticleSystemDisplayMode::MESH) {
			Shader_Ptr shader = nullptr;

			if (ps.hasShader())
				shader = ps.getShader();
			else {
				if (ps.getShaderName() == "default") {
					shader = getShader("instanced.sprite");
				}
				else shader = getShader(ps.getShaderName());
			}

			if (!shader) {
				Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
				return;
			}

			shader->use();
			shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
			shader->setMat4("model", currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU

			if (ps.hasField("position")) {
				AbstractBufferObject_Ptr pos = ps.getField("position");
				shader->attach(*pos);
			}
			
			ps.draw();

		}
		else if (ps.getDisplayMode() == ParticleSystemDisplayMode::MESH) {
			Mesh& mesh = *ps.getMesh();
			Material_Ptr mat = mesh.getMaterial();
			Shader_Ptr shader;

			if (ps.hasShader()) shader = ps.getShader();
			else shader = getShader("instanced.phong");
			if (mesh.hasMaterial()) mat = mesh.getMaterial();
			else mat = getMaterial(mesh.getMaterialName());

			if (!shader) {
				Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
				return;
			}

			Texture2D::resetTextureUnits();
			shader->use();

			if(shader->supportLights())
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->attach(i, *shader);
			}

			mat->attach(*shader);

			if (shader->supportEnvironment()) {
				if (m_currentEnvironment != nullptr)
					m_currentEnvironment->attach(*shader);
				else m_defaultEnvironment->attach(*shader);
			}

			if (shader->supportLights()) shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
			shader->setMat4("model", currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			if (shader->supportShadows())shader->setInt("useShadows", use_shadows);

			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
			if (shader->supportLights()) shader->setInt("numLights", m_activeLights.size());

			ps.draw();
			mat->detach();

			if (shader->supportEnvironment()) {
				if (m_currentEnvironment != nullptr)
					m_currentEnvironment->detach();
				else m_defaultEnvironment->detach();
			}

			if (shader->supportLights())
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->detach();
			}
		}
	}

	void Renderer::renderTransformObject(const TransformObject& obj, const Camera& camera) {
		if (debug)Console::info() << "Rendering TransformObject" << Console::endl;
		render(obj.getXAxisMesh(), camera);
		render(obj.getYAxisMesh(), camera);
		render(obj.getZAxisMesh(), camera);
	}

	void Renderer::render(const Shared<RenderableObject>& object, const Camera& camera) {
		pushMatrix();
		currentTransform *= object->transform();

		if (object->isWireFrame()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			renderMesh(*mesh, camera);
		}//The object is a model
		else if (const auto ps = std::dynamic_pointer_cast<ParticleSystem>(object)) {
			renderParticleSystem(*ps, camera);
		}
		else if (const auto iso = std::dynamic_pointer_cast<IsoSurface>(object)) {
			render(iso->mesh(), camera);
		}
		else if (const auto li = std::dynamic_pointer_cast<Light>(object)) {
			if(display_lights) renderLight(*li, camera);
		}
		else if (const auto model = std::dynamic_pointer_cast<Model>(object)) {
			for (const auto& mesh : model->meshes()) {
				render(mesh, camera);
			}
		}//The object is a scene
		else if (const auto scene = std::dynamic_pointer_cast<Scene>(object)) {
			renderScene(*scene, camera); //Propagate to childrens
		}//The object is a scene node
		else if (const auto ps = std::dynamic_pointer_cast<TransformObject>(object)) {
			renderTransformObject(*ps, camera); //Propagate to childrens
		}

		for (auto node : object->children()) {
			render(node, camera);//Propagate to childrens
		}

		popMatrix();
	}

	Shared<Shader> Renderer::getShader(std::string n) {
		return ShaderLibrary::instance().get(n);
	}

	Shared<MaterialBase> Renderer::getMaterial(std::string n) {
		return MaterialLibrary::instance().get(n);
	}

	void Renderer::loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath) {
		Shared<Shader> shader = Shader::create(name, vertexShaderPath, fragmentShaderPath, geomShaderPath);
		ShaderLibrary::instance().add(shader);
	}

	void Renderer::addMaterial(Shared<MaterialBase> material) {
		MaterialLibrary::instance().add(material);
	}

	void Renderer::addShader(Shared<Shader> shader) {
		if (!shader->isCompiled()) Console::error("Renderer") << "Shader is not compiled. Compile the shader before adding them to the ShaderLibrary" << Console::endl;
		ShaderLibrary::instance().add(shader);
	}

	void Renderer::clear() {
		RendererBase::clear();
		resetMatrix();
		m_activeLights.clear();
		Texture2D::resetTextureUnits();
		//Console::success() << "Cleared screen" << Console::endl;
	}

	void Renderer::setEnvironmentGradientColor(float r, float g, float b) {
		setEnvironmentGradientColor(glm::vec3(r, g, b));
	}
	void Renderer::setEnvironmentGradientColor(glm::vec3 color) {
		if (m_currentEnvironment) m_currentEnvironment->setGradientColor(color);
		m_defaultEnvironment->setGradientColor(color);
	}
}