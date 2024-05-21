#include "glpch.h"
#include "merlin/core/core.h"
#include "renderer.h"




namespace Merlin {

	Renderer::Renderer() : currentTransform(glm::mat4(1.0)) {}

	Renderer::~Renderer() {}


	void Renderer::initialize() {
		enableMultisampling();
		enableDepthTest();
		enableCubeMap();
		enableFaceCulling();

		m_materialLibrary = MaterialLibrary::instance();
		m_shaderLibrary = ShaderLibrary::instance();
		m_defaultEnvironment = createShared<Environment>("defaultEnvironment", 16);
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
		currentTransform = glm::mat4(1);
	}

	void Renderer::renderScene(const Scene& scene, const Camera& camera) {
		if (scene.hasEnvironment()) {
			m_currentEnvironment = scene.getEnvironment();
			renderEnvironment(*m_currentEnvironment, camera);
		}
		else {
			renderEnvironment(*m_defaultEnvironment, camera);
		}

		//Gather lights
		for (const auto& node : scene.nodes()) {
			if (const auto light = std::dynamic_pointer_cast<Light>(node)) {
				light->applyRenderTransform(currentTransform);
				m_activeLights.push_back(light);
			}
		}

		if(useFaceCulling()) glDisable(GL_CULL_FACE);
		if (use_shadows) {
			for (const auto& light : m_activeLights) {
				castShadow(light, scene);
			}
		}
		if (useFaceCulling()) glEnable(GL_CULL_FACE);

		camera.restoreViewport();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Render the scene
		for (const auto& node : scene.nodes()) {
			if(!node->isHidden()) render(node, camera);
		}
	}


	void Renderer::renderEnvironment(const Environment& env, const Camera& camera){
		if (!use_environment) return;
		Shared<Shader> shader = m_shaderLibrary->get("default.skybox");
		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}
		glDepthFunc(GL_LEQUAL);
		if(useFaceCulling())glDisable(GL_CULL_FACE);
		shader->use();
		Texture2D::resetTextureUnit();
		env.attach(*shader);
		shader->setVec3("gradientColor", env.gradientColor());
		shader->setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix()))); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		env.draw();
		if (useFaceCulling())glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		env.detach();
	}


	void Renderer::renderDepth(const Shared<RenderableObject>& object, Shared<Shader> shader){
		pushMatrix();
		currentTransform *= object->transform();

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			shader->setMat4("model", currentTransform); //sync model matrix with GPU
			mesh->draw();
		}//The object is a model
		else if (const auto model = std::dynamic_pointer_cast<Model>(object)) {
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
		Shared<Shader> shader = m_shaderLibrary->get("default.light");
		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}

		shader->use();
		shader->setVec3("light_color", li.diffuse() + li.ambient() + li.specular());

		shader->setMat4("model", currentTransform); //sync model matrix with GPU
		shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		li.draw();
	}

	void Renderer::renderMesh(const Mesh& mesh, const Camera& camera) {

		Material_Ptr mat = mesh.getMaterial();
		Shader_Ptr shader = mesh.getShader();
		
		if (mesh.hasMaterial()) mat = mesh.getMaterial();
		else mat = m_materialLibrary->get(mesh.getMaterialName());

		if (mesh.hasShader())
			shader = mesh.getShader();
		else {
			if (mesh.getShaderName() == "default") {
				if(mat->type() == MaterialType::PHONG) shader = m_shaderLibrary->get("default.phong");
				else if(mat->type() == MaterialType::PBR) shader = m_shaderLibrary->get("default.pbr");
				else {
					Console::error("Renderer") << "This material has no suitable shader. Please bind it manually" << Console::endl;
				}
			}else shader = m_shaderLibrary->get(mesh.getShaderName());
		}

		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}

		shader->use();
		Texture2D::resetTextureUnit();

		for (int i = 0; i < m_activeLights.size(); i++) {
			m_activeLights[i]->attach(i, *shader);
		}

		mat->attach(*shader);
		if(m_currentEnvironment != nullptr)
			m_currentEnvironment->attach(*shader);
		else m_defaultEnvironment->attach(*shader);

		shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
		shader->setMat4("model", currentTransform); //sync model matrix with GPU
		shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		shader->setInt("numLights", m_activeLights.size());



		mesh.draw();
	}


	void Renderer::castShadow(Shared<Light> light, const Scene& scene) {

		Shared<TextureBase> tex;
		Shared<FrameBuffer> fbo;
		Shared<Shader> shader;

		if (light->type() == LightType::Directional || light->type() == LightType::Spot) shader = m_shaderLibrary->get("shadow.depth");
		else if (light->type() == LightType::Point) shader = m_shaderLibrary->get("shadow.omni");
		else return;


		tex = light->shadowMap();
		fbo = light->shadowFBO();

		if (!shader || !tex || !fbo) {
			Console::error("Renderer") << "Renderer failed to gather ressoureces for shadows (shader, framebuffer or texture)" << Console::endl;
			return;
		}

		Texture2D::resetTextureUnit();

		glViewport(0, 0, 2048, 2048);
		fbo->bind();
		
		glClear(GL_DEPTH_BUFFER_BIT);

		shader->use();
		light->attachShadow(*shader);
		
		tex->bind();
		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) {
				renderDepth(node, shader);
			};
		}
		fbo->unbind();
		tex->unbind();
	}


	/*
	void Renderer::renderParticleSystem(const ParticleSystem& ps, const Camera& camera) {
		if (ps.getDisplayMode() == ParticleSystemDisplayMode::POINT_SPRITE) {
			const Shader* shader;
			//glPointSize(10);
			glEnable(GL_PROGRAM_POINT_SIZE);
			if (ps.getMesh()->hasShader())
				shader = &ps.getMesh()->getShader();
			else
				shader = &m_shaderLibrary.get(ps.getMesh()->getShaderName());

			shader->use();
			shader->setMat4("model", currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
			shader->setVec2("WindowSize", glm::vec2(camera.width(), camera.height())); //sync model matrix with GPU
			ps.draw(*shader);

			glDisable(GL_PROGRAM_POINT_SIZE);
		}
		else if (ps.getDisplayMode() == ParticleSystemDisplayMode::POINT_SPRITE_SHADED) {
			const Shader* shader;
			//glPointSize(10);
			glEnable(GL_PROGRAM_POINT_SIZE);
			glEnable(0x8861);//WTF
			//glDisable(GL_DEPTH_TEST);

			if (ps.getMesh()->hasShader())
				shader = &ps.getMesh()->getShader();
			else
				shader = &m_shaderLibrary.get(ps.getMesh()->getShaderName());


			shader->use();
			shader->setMat4("model", currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
			shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
			shader->setVec2("WindowSize", glm::vec2(camera.width(), camera.height())); //sync model matrix with GPU
			ps.draw(*shader);

			//glEnable(GL_DEPTH_TEST);
			glDisable(GL_PROGRAM_POINT_SIZE);
			glDisable(0x8861);//WTF
		}
		else if (ps.getDisplayMode() == ParticleSystemDisplayMode::MESH) {
			const Shader* shader;
			Shared<MaterialBase> mat;

			if (ps.getMesh()->hasShader())
				shader = &ps.getMesh()->getShader();
			else
				shader = &m_shaderLibrary.get(ps.getMesh()->getShaderName());


			if (ps.getMesh()->hasMaterial())
				mat = ps.getMesh()->getMaterial();
			else {

				mat = m_materialLibrary.get(ps.getMesh()->getMaterialName());
			}


			shader->use();
			if (ps.getMesh()->hasShader())
				shader = ps.getMesh()->getShader();
			else
				shader = m_shaderLibrary.get(mesh.getShaderName());

			if (mesh.hasMaterial())
				mat = mesh.getMaterial();
			else
				mat = m_materialLibrary.get(mesh.getMaterialName());


			shader->setMat4("model", currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU

			if (shader->supportTexture()) {
				Texture2D* tex = &mat->getTexture(TextureType::ALBEDO);

				//WARNING This should be done once...
				tex->setUnit(1); //Skybox is 0...
				tex->syncTextureUnit(*shader, (tex->typeToString()) + "0");


				tex->bind();
				shader->setInt("hasColorTex", !tex->isDefault());
			}
			ps.draw(*shader);
		}
	}
	*/
	void Renderer::renderTransformObject(const TransformObject& obj, const Camera& camera) {
		//TODO Render axis
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
		return m_shaderLibrary->get(n);
	}

	void Renderer::loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath) {
		Shared<Shader> shader = Shader::create(name, vertexShaderPath, fragmentShaderPath, geomShaderPath);
		m_shaderLibrary->add(shader);
	}

	void Renderer::addMaterial(Shared<MaterialBase> material) {
		m_materialLibrary->add(material);
	}

	void Renderer::addShader(Shared<Shader> shader) {
		if (!shader->isCompiled()) Console::error("Renderer") << "Shader is not compiled. Compile the shader before adding them to the ShaderLibrary" << Console::endl;
		m_shaderLibrary->add(shader);
	}

	void Renderer::clear() {
		RendererBase::clear();
		resetMatrix();
		m_activeLights.clear();
	}

	void Renderer::setEnvironmentGradientColor(float r, float g, float b) {
		setEnvironmentGradientColor(glm::vec3(r, g, b));
	}
	void Renderer::setEnvironmentGradientColor(glm::vec3 color) {
		if (m_currentEnvironment) m_currentEnvironment->setGradientColor(color);
		m_defaultEnvironment->setGradientColor(color);
	}



}