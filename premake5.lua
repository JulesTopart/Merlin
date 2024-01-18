
newaction {
	trigger     = "new",
	description = "Create a new project from template using the Merlin library. example : premake5 new <name>",
	execute = function ()

		projectName = ""
		if(_ARGS[1])
		then
			projectName = _ARGS[1]
		else
			printf("error : Please provide project name")
		end

		projectDir = solutiondir .. "/merlin.projects/" .. projectName
		
		printf("Copying template : \"" .. solutiondir .. "/merlin.template\" > " .. "\"" .. projectDir .."\"")
		command = "\"" .. solutiondir .. "/merlin.template/src\" " .. "\"" .. projectDir .. "/src\""
		os.execute("{COPYDIR} " .. command)
		os.execute("{TOUCH} " .. "\"" .. projectDir .. "/premake5.lua\"")

		suffix = "> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""

		line = "include \"" .. solutiondir .. "/merlin.template/template.lua\""
		os.execute("{ECHO} " .. line .. suffix)
		
		suffix = ">> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""
		line = "newProject(\"" .. projectName .. "\")" 
		os.execute("{ECHO} " .. line .. suffix)

		suffix = ">> " .. "\"" .. solutiondir .. "/premake5.lua" .. "\""
		line = "	include \"" .. projectDir .. "\"" 
		os.execute("{ECHO} " .. line .. suffix)
	end
}

newaction {
	trigger     = "add",
	description = "Add an existing project using the Merlin library. example : premake5 add <name>",
	execute = function ()
		projectDir = solutiondir .. "/"
		
		projectName = ""
		if(_ARGS[1])
		then
			projectName = _ARGS[1]
		else
			printf("error : Please provide project name")
		end

		projectDir = solutiondir .. "/merlin.projects/" .. projectName
		
		printf("Generating project files for : " .. projectName)
		os.execute("{TOUCH} " .. "\"" .. projectDir .. "/premake5.lua\"")
		suffix = "> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""
		line = "include \"" .. solutiondir .. "/merlin.template/template.lua\""
		os.execute("{ECHO} " .. line .. suffix)

		suffix = ">> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""
		line = "newProject(\"" .. projectName .. "\")" 
		os.execute("{ECHO} " .. line .. suffix)
		
		suffix = ">> " .. "\"" .. solutiondir .. "/premake5.lua" .. "\""
		line = "	include \"" .. projectDir .. "\"" 
		os.execute("{ECHO} " .. line .. suffix)
	end
}


workspace "Merlin"
	architecture "x64"
	startproject "merlin.example"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
solutiondir = path.getabsolute("./")

IncludeDir = {}
IncludeDir["glfw"] = "vendor/glfw/include"
IncludeDir["glad"] = "vendor/glad/include"
IncludeDir["imgui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"

-- Projects
group "Dependencies"
	include "merlin.core/vendor/glfw"
	include "merlin.core/vendor/glad"
	include "merlin.core/vendor/imgui"
	include "merlin.core/vendor/stb_image"
	
group ""
	include "merlin.core"
	
group "Projects"
	include "C:/Users/jules/Git/Merlin/merlin.projects/merlin.example"
	include "C:/Users/jules/Git/Merlin/merlin.projects/merlin.dem"
	include "C:/Users/jules/Git/Merlin/merlin.projects/merlin.sort"
	
