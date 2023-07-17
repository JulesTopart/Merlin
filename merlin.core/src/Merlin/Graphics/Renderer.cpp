#include "glpch.h"
#include "Merlin/Core/Core.h"
#include "Renderer.h"

namespace Merlin::Graphics {

	Renderer::Renderer() : currentTransform(glm::mat4(1.0)) {
	}

	Renderer::~Renderer() {
	}


	void Renderer::Initialize() {
		EnableMultisampling();
		EnableDepthTest();
		EnableCubeMap();
		
		_materialLibrary.LoadDefaultMaterials();
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
		PushMatrix();
		currentTransform *= scene.transform();

		for (const auto& node : scene.nodes()) {
			Render(node, camera);
		}

		PopMatrix();
	}

	void Renderer::RenderModel(const Model& mdl, const Camera& camera) {
		PushMatrix();
		currentTransform *= mdl.transform();

		for (const auto& mesh : mdl.meshes()) {
			RenderMesh(*mesh, camera);
		}

		PopMatrix();
	}


	void Renderer::RenderParticleSystem(const ParticleSystem& ps, const Camera& camera) {
		PushMatrix();
		currentTransform *= ps.transform();

		if (ps.GetDisplayMode() == ParticleSystemDisplayMode::POINT_SPRITE) {
			const Shader* shader;
			glPointSize(10);

			if (ps.GetMesh()->HasShader())
				shader = &ps.GetMesh()->GetShader();
			else
				shader = &_shaderLibrary.Get(ps.GetMesh()->GetShaderName());


			shader->Use();
			shader->SetMat4("model", currentTransform); //Sync model matrix with GPU
			shader->SetMat4("view", camera.GetViewMatrix()); //Sync model matrix with GPU
			shader->SetMat4("projection", camera.GetProjectionMatrix()); //Sync model matrix with GPU
			ps.Draw(*shader);

			glPointSize(1); //restore point size
		}
		else if (ps.GetDisplayMode() == ParticleSystemDisplayMode::MESH) {
			const Shader* shader;
			const Material* mat;

			if (ps.GetMesh()->HasShader())
				shader = &ps.GetMesh()->GetShader();
			else
				shader = &_shaderLibrary.Get(ps.GetMesh()->GetShaderName());


			if (ps.GetMesh()->HasMaterial())
				mat = &ps.GetMesh()->GetMaterial();
			else {

				mat = &_materialLibrary.Get(ps.GetMesh()->GetMaterialName());
			}


			shader->Use();

			if (shader->SupportMaterial()) {
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
				Shared<Texture> tex;
				tex = mat->GetTexture(TextureType::COLOR);

				//WARNING This should be done once...
				tex->SetUnit(1); //Skybox is 0...
				tex->SyncTextureUnit(*shader, (tex->typeToString()) + "0");


				tex->Bind();
				shader->SetInt("hasColorTex", !tex->IsDefault());
			}
			ps.Draw(*shader);
		}


		

		PopMatrix();

	}

	void Renderer::RenderSkyBox(const SkyBox& sky, const Camera& camera) {
		PushMatrix();
		
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);
		
		const Shader* shader;

		if (sky.HasShader())
			shader = &sky.GetShader();
		else
			shader = &_shaderLibrary.Get(sky.GetShaderName());
		

		shader->Use();

		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = camera.GetProjectionMatrix();

		shader->SetMat4("view", view); //Sync model matrix with GPU
		shader->SetMat4("projection", projection); //Sync model matrix with GPU

		sky.Draw();

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);
		PopMatrix();
	}

	void Renderer::RenderMesh(const Mesh& mesh, const Camera& camera) {

		PushMatrix();
		currentTransform *= mesh.transform();

		const Shader* shader;
		const Material* mat;

		if (mesh.HasShader())
			shader = &mesh.GetShader();
		else
			shader = &_shaderLibrary.Get(mesh.GetShaderName());
		

		if (mesh.HasMaterial()) 
			mat = &mesh.GetMaterial();
		else {

			mat = &_materialLibrary.Get(mesh.GetMaterialName());
		}
			
		
		shader->Use();

		if (shader->SupportMaterial()) {
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
			Shared<Texture> tex;
			tex = mat->GetTexture(TextureType::NORMAL);
			//WARNING This should be done once...
			tex->SetUnit(1); //Skybox is 0...
			tex->SyncTextureUnit(*shader, (tex->typeToString()) + "0");
			tex->Bind();

			tex = mat->GetTexture(TextureType::COLOR);
			//WARNING This should be done once...
			tex->SetUnit(2); //Skybox is 0...
			tex->SyncTextureUnit(*shader, (tex->typeToString()) + "0");


			tex->Bind();
			shader->SetInt("hasColorTex", !tex->IsDefault());
		}


		
		mesh.Draw();
		PopMatrix();
	}

	void Renderer::Render(const Shared<RenderableObject>& object, const Camera& camera) {

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
		else if (const auto ps = std::dynamic_pointer_cast<ParticleSystem>(object)) {
			RenderParticleSystem(*ps, camera); //Propagate to childrens
		}//The object is a particleSystem node
		else {
			for (auto node : object->children()) {
				Render(node, camera);//Propagate to childrens
			}
		}
		
	}

	const Shared<Shader>& Renderer::ShareShader(std::string n) {
		return _shaderLibrary.Share(n);
	}


	const Material& Renderer::GetMaterial(std::string n) {
		return _materialLibrary.Get(n);
	}

	const Shader& Renderer::GetShader(std::string n) {
		return _shaderLibrary.Get(n);
	}

	void Renderer::LoadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath) {
		Shared<Shader> shader = Shader::Create(name, vertexShaderPath, fragmentShaderPath, geomShaderPath);
		_shaderLibrary.Add(shader);
	}

	void Renderer::CreateMaterial(MaterialProperty matProps){
		std::string name = "material";
		std::stringstream ss;
		ss << name << _materialLibrary.size();
		Shared<Material> mat = CreateShared<Material>(ss.str());
		_materialLibrary.Add(mat);
	}

	void Renderer::AddMaterial(Shared<Material> material) {
		_materialLibrary.Add(material);
	}

	void Renderer::AddShader(Shared<Shader> shader) {
		if (!shader->IsCompiled()) Console::error("Renderer") << "Shader is not compiled. Compile the shader before adding them to the ShaderLibrary" << Console::endl;
		_shaderLibrary.Add(shader);
	}

	void Renderer::Clear() {
		RendererBase::Clear();
		ResetMatrix();
	}




}