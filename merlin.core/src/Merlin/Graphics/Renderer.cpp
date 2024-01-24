#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Renderer.h"

namespace Merlin::Graphics {

	Renderer::Renderer() : currentTransform(glm::mat4(1.0)) {}

	Renderer::~Renderer() {
	}


	void Renderer::Initialize() {
		EnableMultisampling();
		EnableDepthTest();
		EnableCubeMap();
	}

	void Renderer::PushMatrix() {
		matrixStack.push(currentTransform);
	}

	void Renderer::PopMatrix() {
		if (!matrixStack.empty()) {
			currentTransform = matrixStack.top();
			matrixStack.pop();
		}
	}

	void Renderer::ResetMatrix() {
		while(!matrixStack.empty()) {
			matrixStack.pop();
		}
		currentTransform = glm::mat4(1);
	}


	void Renderer::RenderScene(const Scene& scene, const Camera& camera) {
		for (const auto& node : scene.nodes()) {
			if(!node->IsHidden()) Render(node, camera);
		}
	}

	void Renderer::RenderModel(const Model& mdl, const Camera& camera) {
		for (const auto& mesh : mdl.meshes()) {
			RenderMesh(*mesh, camera);
		}
	}


	void Renderer::RenderParticleSystem(const deprecated_GenericParticleSystem& ps, const Camera& camera) {
		if (ps.GetDisplayMode() == deprecated_ParticleSystemDisplayMode::POINT_SPRITE) {
			const Shader* shader;
			//glPointSize(10);
			glEnable(GL_PROGRAM_POINT_SIZE);
			if (ps.GetMesh()->HasShader())
				shader = &ps.GetMesh()->GetShader();
			else
				shader = &m_shaderLibrary.Get(ps.GetMesh()->GetShaderName());

			shader->Use();
			shader->SetMat4("model", currentTransform); //Sync model matrix with GPU
			shader->SetMat4("view", camera.GetViewMatrix()); //Sync model matrix with GPU
			shader->SetMat4("projection", camera.GetProjectionMatrix()); //Sync model matrix with GPU
			shader->SetVec2("WindowSize", glm::vec2(camera.Width(), camera.Height())); //Sync model matrix with GPU
			ps.Draw(*shader);

			glDisable(GL_PROGRAM_POINT_SIZE);
		}else if(ps.GetDisplayMode() == deprecated_ParticleSystemDisplayMode::POINT_SPRITE_SHADED) {
			const Shader* shader;
			//glPointSize(10);
			glEnable(GL_PROGRAM_POINT_SIZE);
			glEnable(0x8861);//WTF
			//glDisable(GL_DEPTH_TEST);

			if (ps.GetMesh()->HasShader())
				shader = &ps.GetMesh()->GetShader();
			else
				shader = &m_shaderLibrary.Get(ps.GetMesh()->GetShaderName());


			shader->Use();
			shader->SetMat4("model", currentTransform); //Sync model matrix with GPU
			shader->SetMat4("view", camera.GetViewMatrix()); //Sync model matrix with GPU
			shader->SetMat4("projection", camera.GetProjectionMatrix()); //Sync model matrix with GPU
			shader->SetVec3("viewPos", camera.GetPosition()); //Sync model matrix with GPU
			shader->SetVec2("WindowSize", glm::vec2(camera.Width(), camera.Height())); //Sync model matrix with GPU
			ps.Draw(*shader);

			//glEnable(GL_DEPTH_TEST);
			glDisable(GL_PROGRAM_POINT_SIZE);
			glDisable(0x8861);//WTF
		}else if (ps.GetDisplayMode() == deprecated_ParticleSystemDisplayMode::MESH) {
			const Shader* shader;
			const Material* mat;

			if (ps.GetMesh()->HasShader())
				shader = &ps.GetMesh()->GetShader();
			else
				shader = &m_shaderLibrary.Get(ps.GetMesh()->GetShaderName());


			if (ps.GetMesh()->HasMaterial())
				mat = &ps.GetMesh()->GetMaterial();
			else {

				mat = &m_materialLibrary.Get(ps.GetMesh()->GetMaterialName());
			}


			shader->Use();
			if (shader->SupportMaterial()) {
				/*
				if (mat->usePBR() && shader->SupportPBR()) {//todo remove this -> Replace by proper UBO management
					shader->SetVec3("albedo", mat->albedo());
					shader->SetFloat("metallic", mat->metallic());
					shader->SetFloat("roughness", mat->roughness());
					shader->SetFloat("ao", mat->ao());
					shader->SetVec3("viewPos", camera.GetPosition()); //Sync model matrix with GPU
				}
				else {

				}
				*/

				shader->SetVec3("ambient", mat->ambient());
				shader->SetVec3("diffuse", mat->diffuse());
				shader->SetVec3("specular", mat->specular());
				shader->SetFloat("shininess", mat->shininess());
				shader->SetVec3("viewPos", camera.GetPosition()); //Sync model matrix with GPU

			}

			shader->SetMat4("model", currentTransform); //Sync model matrix with GPU
			shader->SetMat4("view", camera.GetViewMatrix()); //Sync model matrix with GPU
			shader->SetMat4("projection", camera.GetProjectionMatrix()); //Sync model matrix with GPU

			if (shader->SupportTexture()) {
				Texture* tex = &mat->GetTexture(TextureType::COLOR);

				//WARNING This should be done once...
				tex->SetUnit(1); //Skybox is 0...
				tex->SyncTextureUnit(*shader, (tex->typeToString()) + "0");


				tex->Bind();
				shader->SetInt("hasColorTex", !tex->IsDefault());
			}
			ps.Draw(*shader);
		}
	}

	void Renderer::RenderTransformObject(const TransformObject& obj, const Camera& camera) {
		//TODO Render axis
		Render(obj.GetXAxisMesh(), camera);
		Render(obj.GetYAxisMesh(), camera);
		Render(obj.GetZAxisMesh(), camera);
	}

	void Renderer::RenderSkyBox(const SkyBox& sky, const Camera& camera) {
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);
		
		const Shader* shader;

		if (sky.HasShader())
			shader = &sky.GetShader();
		else
			shader = &m_shaderLibrary.Get(sky.GetShaderName());
		

		shader->Use();

		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = camera.GetProjectionMatrix();

		//if (sky.HasCubeMap()) shader->SetInt("isSkyboxBound", 1);
		shader->SetMat4("view", view); //Sync model matrix with GPU
		shader->SetMat4("projection", projection); //Sync model matrix with GPU


		sky.Draw();

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);
	}

	void Renderer::RenderMesh(const Mesh& mesh, const Camera& camera) {

		const Shader* shader;
		const Material* mat;

		if (mesh.HasShader())
			shader = &mesh.GetShader();
		else
			shader = &m_shaderLibrary.Get(mesh.GetShaderName());
		

		if (mesh.HasMaterial()) 
			mat = &mesh.GetMaterial();
		else {

			mat = &m_materialLibrary.Get(mesh.GetMaterialName());
		}
				
		shader->Use();
		if (shader->SupportMaterial()) {
			/*
			if (mat->usePBR() && shader->SupportPBR()) {//todo remove this -> Replace by proper UBO management
				shader->SetVec3("albedo", mat->albedo());
				shader->SetFloat("metallic", mat->metallic());
				shader->SetFloat("roughness", mat->roughness());
				shader->SetFloat("ao", mat->ao());
				shader->SetVec3("viewPos", camera.GetPosition()); //Sync model matrix with GPU
			}
			else {

			}
			*/

			shader->SetVec3("ambient", mat->ambient());
			shader->SetVec3("diffuse", mat->diffuse());
			shader->SetVec3("specular", mat->specular());
			shader->SetFloat("shininess", mat->shininess());
			shader->SetVec3("viewPos", camera.GetPosition()); //Sync model matrix with GPU

		}

		shader->SetMat4("model", currentTransform); //Sync model matrix with GPU
		shader->SetMat4("view", camera.GetViewMatrix()); //Sync model matrix with GPU
		shader->SetMat4("projection", camera.GetProjectionMatrix()); //Sync model matrix with GPU

		if (shader->SupportTexture()) {
			Texture*  tex;

			tex = &mat->GetTexture(TextureType::COLOR);

			if(tex){
				shader->SetInt("skybox", 0);
				shader->SetInt("hasColorTex", !tex->IsDefault());

				if (!tex->IsDefault()) {
					tex->SetUnit(1); //Skybox is 0...
					tex->SyncTextureUnit(*shader, (tex->typeToString()) + "0");
					tex->Bind();
				}
			}
			
			tex = &mat->GetTexture(TextureType::NORMAL);

			if (tex) {
				shader->SetInt("hasNormalTex", !tex->IsDefault());

				if (!tex->IsDefault()) {
					tex->SetUnit(2); //Skybox is 0...
					tex->SyncTextureUnit(*shader, (tex->typeToString()) + "0");
					tex->Bind();
				}
			}
		}
		
		mesh.Draw();
	}

	void Renderer::Render(const Shared<RenderableObject>& object, const Camera& camera) {
		PushMatrix();
		currentTransform *= object->transform();

		if (object->IsWireFrame()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			RenderMesh(*mesh, camera);
		}//The object is a model
		else if (const auto model = std::dynamic_pointer_cast<Model>(object)) {
			RenderModel(*model, camera);
		}//The object is a scene
		else if (const auto scene = std::dynamic_pointer_cast<Scene>(object)) {
			RenderScene(*scene, camera); //Propagate to childrens
		}//The object is a scene node
		else if (const auto sky = std::dynamic_pointer_cast<SkyBox>(object)) {
			RenderSkyBox(*sky, camera); //Propagate to childrens
		}//The object is a skybox node
		else if (const auto ps = std::dynamic_pointer_cast<TransformObject>(object)) {
			RenderTransformObject(*ps, camera); //Propagate to childrens
		}
		else if (const auto ps = std::dynamic_pointer_cast<deprecated_GenericParticleSystem>(object)) {
			RenderParticleSystem(*ps, camera); //Propagate to childrens
		}//The object is a particleSystem node
		
		for (auto node : object->children()) {
			Render(node, camera);//Propagate to childrens
		}

		PopMatrix();
	}


	const Shared<Shader>& Renderer::ShareShader(std::string n) {
		return m_shaderLibrary.Share(n);
	}


	const Material& Renderer::GetMaterial(std::string n) {
		return m_materialLibrary.Get(n);
	}

	const Shader& Renderer::GetShader(std::string n) {
		return m_shaderLibrary.Get(n);
	}

	void Renderer::LoadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath) {
		Shared<Shader> shader = Shader::Create(name, vertexShaderPath, fragmentShaderPath, geomShaderPath);
		m_shaderLibrary.Add(shader);
	}

	void Renderer::CreateMaterial(MaterialProperty matProps){
		std::string name = "material";
		std::stringstream ss;
		ss << name << m_materialLibrary.size();
		Shared<Material> mat = CreateShared<Material>(ss.str());
		m_materialLibrary.Add(mat);
	}

	void Renderer::AddMaterial(Shared<Material> material) {
		m_materialLibrary.Add(material);
	}

	void Renderer::AddShader(Shared<Shader> shader) {
		if (!shader->IsCompiled()) Console::error("Renderer") << "Shader is not compiled. Compile the shader before adding them to the ShaderLibrary" << Console::endl;
		m_shaderLibrary.Add(shader);
	}

	void Renderer::Clear() {
		RendererBase::Clear();
		ResetMatrix();
	}




}