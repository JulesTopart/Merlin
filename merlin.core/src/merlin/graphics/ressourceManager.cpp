#include "glpch.h"
#include "ressourceManager.h"
#include "merlin/utils/util.h"

namespace Merlin {

    




	ShaderLibrary::ShaderLibrary() {
		LoadDefaultShaders();
	}

	void ShaderLibrary::LoadDefaultShaders() {

		Shared<Shader> defaultShader = createShared<Shader>("default");

		std::string vertexSrc =
			"#version 330 core\n"
			"layout(location = 0) in vec3 _position;\n"
			"layout(location = 1) in vec3 _normal;\n"
			"layout(location = 2) in vec3 _color;\n"
			"layout(location = 3) in vec2 _texCoord;\n"
			"out vec3 position;\n"
			"out vec3 normal;\n"
			"out vec3 color;\n"
			"out vec2 texCoord;\n"
			"uniform vec3 viewPos;\n"
			"uniform mat4 view;\n"
			"uniform mat4 model;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"position = vec3(model * vec4(_position, 1.0f));\n"
			"color = _color;\n"
			"normal = _normal;\n"
			"texCoord = _texCoord;\n"
			"gl_Position = projection * view * vec4(position, 1.0f);\n"
			"}";

		std::string fragmentSrc =
			"#version 330 core\n"
			"in vec3 position;\n"
			"in vec3 normal;\n"
			"in vec3 color;\n"
			"in vec2 texCoord;\n"
			"uniform vec3 viewPos;\n"
			"uniform vec3 ambient;\n"
			"uniform vec3 diffuse;\n"
			"uniform vec3 specular;\n"
			"uniform float shininess;\n"
			"out vec4 FragColor;\n"
			"void main() {\n"
			"vec3 lightPos = vec3(100);\n"
			"vec3 n = normalize(normal);\n"
			"vec3 lightDir = normalize(lightPos - position);\n"
			"vec3 viewDir = normalize(viewPos - position);\n"
			"vec3 reflectDir = reflect(-lightDir, n);\n"
			"vec3 _ambient = ambient *color;\n"
			"vec3 _diffuse = diffuse * max(dot(n, lightDir), 0.0f);\n"
			"vec3 _specular = specular * pow(max(dot(viewDir, reflectDir), 0.0f), max(shininess, 8));\n"
			"FragColor = vec4(_ambient + _diffuse + _specular, 1.0f);\n"
			"}";

		defaultShader->compileFromSrc(vertexSrc, fragmentSrc);
		defaultShader->noTexture();
		//defaultShader->noMaterial();
		add(defaultShader);

		/*
		Shared<Shader> debugShader = Shader::create("debug.normals", "assets/common/shaders/debug.normals.vert", "assets/common/shaders/debug.normals.frag", "assets/common/shaders/debug.normals.geom");
		debugShader->noTexture();
		debugShader->noMaterial();
		Add(debugShader);*/
	}

	MaterialLibrary::MaterialLibrary() {
		LoadDefaultMaterials();
	}

	void MaterialLibrary::LoadDefaultMaterials() {

		Material_Ptr defaultMaterial = createShared<Material>("default");
		defaultMaterial->setAmbient(glm::vec3(0.6, 0.6, 0.6));
		defaultMaterial->setDiffuse(glm::vec3(0.4, 0.4, 0.4));
		defaultMaterial->setSpecular(glm::vec3(0.2, 0.2, 0.2));
		defaultMaterial->setShininess(0.5);

		Material_Ptr aluminum = createShared<Material>("aluminum");
		aluminum->setAlbedo(glm::vec3(0.912, 0.914, 0.920));
		aluminum->setMetallic(1);
		aluminum->setRoughness(0.0);
		aluminum->setAmbientOcclusion(0.6);

		Shared<Material> emerald = createShared<Material>("emerald");
		emerald->setAmbient(glm::vec3(0.0215, 0.1745, 0.0215));
		emerald->setDiffuse(glm::vec3(0.07568, 0.61424, 0.07568));
		emerald->setSpecular(glm::vec3(0.633, 0.727811, 0.633));
		emerald->setShininess(0.6);

		Shared<Material> jade = createShared<Material>("jade");
		jade->setAmbient(glm::vec3(0.135, 0.2225, 0.1575));
		jade->setDiffuse(glm::vec3(0.54, 0.89, 0.63));
		jade->setSpecular(glm::vec3(0.316228, 0.316228, 0.316228));
		jade->setShininess(0.1);

		Shared<Material> obsidian = createShared<Material>("obsidian");
		obsidian->setAmbient(glm::vec3(0.05375, 0.05, 0.06625));
		obsidian->setDiffuse(glm::vec3(0.18275, 0.17, 0.22525));
		obsidian->setSpecular(glm::vec3(0.332741, 0.328634, 0.346435));
		obsidian->setShininess(0.3);

		Shared<Material> pearl = createShared<Material>("pearl");
		pearl->setAmbient(glm::vec3(0.25, 0.20725, 0.20725));
		pearl->setDiffuse(glm::vec3(1, 0.829, 0.829));
		pearl->setSpecular(glm::vec3(0.296648, 0.296648, 0.296648));
		pearl->setShininess(0.088);

		Shared<Material> ruby = createShared<Material>("ruby");
		ruby->setAmbient(glm::vec3(0.1745, 0.01175, 0.01175));
		ruby->setDiffuse(glm::vec3(0.61424, 0.04136, 0.04136));
		ruby->setSpecular(glm::vec3(0.727811, 0.626959, 0.626959));
		ruby->setShininess(0.6);

		Shared<Material> turquoise = createShared<Material>("turquoise");
		turquoise->setAmbient(glm::vec3(0.1, 0.18725, 0.1745));
		turquoise->setDiffuse(glm::vec3(0.396, 0.74151, 0.69102));
		turquoise->setSpecular(glm::vec3(0.297254, 0.30829, 0.306678));
		turquoise->setShininess(0.1);

		Shared<Material> brass = createShared<Material>("brass");
		brass->setAmbient(glm::vec3(0.329412, 0.223529, 0.027451));
		brass->setDiffuse(glm::vec3(0.780392, 0.568627, 0.113725));
		brass->setSpecular(glm::vec3(0.992157, 0.941176, 0.807843));
		brass->setShininess(0.21794872);

		Shared<Material> bronze = createShared<Material>("bronze");
		bronze->setAmbient(glm::vec3(0.2125, 0.1275, 0.054));
		bronze->setDiffuse(glm::vec3(0.714, 0.4284, 0.18144));
		bronze->setSpecular(glm::vec3(0.393548, 0.271906, 0.166721));
		bronze->setShininess(0.2);

		Shared<Material> chrome = createShared<Material>("chrome");
		chrome->setAmbient(glm::vec3(0.25, 0.25, 0.25));
		chrome->setDiffuse(glm::vec3(0.4, 0.4, 0.4));
		chrome->setSpecular(glm::vec3(0.774597, 0.774597, 0.774597));
		chrome->setShininess(0.6);

		Shared<Material> copper = createShared<Material>("copper");
		copper->setAmbient(glm::vec3(0.19125, 0.0735, 0.0225));
		copper->setDiffuse(glm::vec3(0.7038, 0.27048, 0.0828));
		copper->setSpecular(glm::vec3(0.256777, 0.137622, 0.086014));
		copper->setShininess(0.1);

		Shared<Material> gold = createShared<Material>("gold");
		gold->setAmbient(glm::vec3(0.24725, 0.1995, 0.0745));
		gold->setDiffuse(glm::vec3(0.75164, 0.60648, 0.22648));
		gold->setSpecular(glm::vec3(0.628281, 0.555802, 0.366065));
		gold->setShininess(0.4);

		Shared<Material> silver = createShared<Material>("silver");
		silver->setAmbient(glm::vec3(0.19225, 0.19225, 0.19225));
		silver->setDiffuse(glm::vec3(0.50754, 0.50754, 0.50754));
		silver->setSpecular(glm::vec3(0.508273, 0.508273, 0.508273));
		silver->setShininess(0.4);

		Shared<Material> black_plastic = createShared<Material>("black plastic");
		black_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		black_plastic->setDiffuse(glm::vec3(0.01, 0.01, 0.01));
		black_plastic->setSpecular(glm::vec3(0.50, 0.50, 0.50));
		black_plastic->setShininess(0.25);

		Shared<Material> cyan_plastic = createShared<Material>("cyan plastic");
		cyan_plastic->setAmbient(glm::vec3(0.0, 0.1, 0.06));
		cyan_plastic->setDiffuse(glm::vec3(0.0, 0.50980392, 0.50980392));
		cyan_plastic->setSpecular(glm::vec3(0.50196078, 0.50196078, 0.50196078));
		cyan_plastic->setShininess(0.25);

		Shared<Material> green_plastic = createShared<Material>("green plastic");
		green_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		green_plastic->setDiffuse(glm::vec3(0.1, 0.35, 0.1));
		green_plastic->setSpecular(glm::vec3(0.45, 0.55, 0.45));
		green_plastic->setShininess(0.25);

		Shared<Material> red_plastic = createShared<Material>("red plastic");
		red_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		red_plastic->setDiffuse(glm::vec3(0.5, 0.0, 0.0));
		red_plastic->setSpecular(glm::vec3(0.7, 0.6, 0.6));
		red_plastic->setShininess(0.25);

		Shared<Material> white_plastic = createShared<Material>("white plastic");
		white_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		white_plastic->setDiffuse(glm::vec3(0.55, 0.55, 0.55));
		white_plastic->setSpecular(glm::vec3(0.70, 0.70, 0.70));
		white_plastic->setShininess(0.25);

		Shared<Material> yellow_plastic = createShared<Material>("yellow plastic");
		yellow_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		yellow_plastic->setDiffuse(glm::vec3(0.5, 0.5, 0.0));
		yellow_plastic->setSpecular(glm::vec3(0.60, 0.60, 0.50));
		yellow_plastic->setShininess(0.25);

		Shared<Material> black_rubber = createShared<Material>("black rubber");
		black_rubber->setAmbient(glm::vec3(0.02, 0.02, 0.02));
		black_rubber->setDiffuse(glm::vec3(0.01, 0.01, 0.01));
		black_rubber->setSpecular(glm::vec3(0.4, 0.4, 0.4));
		black_rubber->setShininess(0.078125);

		Shared<Material> cyan_rubber = createShared<Material>("cyan rubber");
		cyan_rubber->setAmbient(glm::vec3(0.0, 0.05, 0.05));
		cyan_rubber->setDiffuse(glm::vec3(0.4, 0.5, 0.5));
		cyan_rubber->setSpecular(glm::vec3(0.04, 0.7, 0.7));
		cyan_rubber->setShininess(0.078125);

		Shared<Material> blue_rubber = createShared<Material>("blue rubber");
		blue_rubber->setAmbient(glm::vec3(0.0, 0.00, 0.05));
		blue_rubber->setDiffuse(glm::vec3(0.3, 0.3, 0.5));
		blue_rubber->setSpecular(glm::vec3(0.04, 0.04, 0.7));
		blue_rubber->setShininess(0.078125);

		Shared<Material> green_rubber = createShared<Material>("green rubber");
		green_rubber->setAmbient(glm::vec3(0.0, 0.05, 0.0));
		green_rubber->setDiffuse(glm::vec3(0.4, 0.5, 0.4));
		green_rubber->setSpecular(glm::vec3(0.04, 0.7, 0.04));
		green_rubber->setShininess(0.078125);

		Shared<Material> red_rubber = createShared<Material>("red rubber");
		red_rubber->setAmbient(glm::vec3(0.05, 0.0, 0.0));
		red_rubber->setDiffuse(glm::vec3(0.5, 0.4, 0.4));
		red_rubber->setSpecular(glm::vec3(0.7, 0.04, 0.04));
		red_rubber->setShininess(0.078125);

		Shared<Material> white_rubber = createShared<Material>("white rubber");
		white_rubber->setAmbient(glm::vec3(0.05, 0.05, 0.05));
		white_rubber->setDiffuse(glm::vec3(0.5, 0.5, 0.5));
		white_rubber->setSpecular(glm::vec3(0.7, 0.7, 0.7));
		white_rubber->setShininess(0.078125);

		Shared<Material> yellow_rubber = createShared<Material>("yellow rubber");
		yellow_rubber->setAmbient(glm::vec3(0.05, 0.05, 0.0));
		yellow_rubber->setDiffuse(glm::vec3(0.5, 0.5, 0.4));
		yellow_rubber->setSpecular(glm::vec3(0.7, 0.7, 0.04));
		yellow_rubber->setShininess(0.078125);

		add(defaultMaterial);
		add(aluminum);
		add(emerald);
		add(jade);
		add(obsidian);
		add(pearl);
		add(ruby);
		add(turquoise);
		add(brass);
		add(bronze);
		add(chrome);
		add(copper);
		add(gold);
		add(silver);
		add(black_plastic);
		add(cyan_plastic);
		add(green_plastic);
		add(red_plastic);
		add(white_plastic);
		add(yellow_plastic);
		add(black_rubber);
		add(cyan_rubber);
		add(blue_rubber);
		add(green_rubber);
		add(red_rubber);
		add(white_rubber);
		add(yellow_rubber);


	}


}