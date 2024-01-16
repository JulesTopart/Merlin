
newaction {
	trigger     = "new",
	description = "Create a new project from template using the Merlin library. example : premake5 new <name> <optional path>",
	execute = function ()
		projectName = _ARGS[1] or "newProject"
		--projectDir = solutiondir .. "/" ..(_ARGS[2] or ".") .. "/" .. projectName

		projectDir = solutiondir .. "/"

		if(_ARGS[2])
		then
			if(path.isAbsolute(_ARGS[2]))
			then
				projectDir = _ARGS[2] .. "/" .. projectName
			else
				projectDir = projectDir .. _ARGS[2] .. "/" .. projectName
			end
		end

		printf("Copying template : \"" .. solutiondir .. "/merlin.template\" > " .. "\"" .. projectDir .."\"")
		command = "\"" .. solutiondir .. "/merlin.template\" " .. "\"" .. projectDir .. "\""
		os.execute("{COPYDIR} " .. command)
		os.execute("{TOUCH} " .. "\"" .. projectDir .. "/premake5.lua\"")

		suffix = "> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""

		line = "include \"" .. solutiondir .. "/project.lua\""
		os.execute("{ECHO} " .. line .. suffix)

		suffix = ">> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""
		line = "newProject(\"" .. projectName .. "\")" 
		os.execute("{ECHO} " .. line .. suffix)
	end
}

newaction {
	trigger     = "add",
	description = "Add an existing project using the Merlin library. example : premake5 add <name> <optional path>",
	execute = function ()
		projectDir = solutiondir .. "/"
		
		projectName = ""
		if(_ARGS[1])
		then
			projectName = _ARGS[1]
		else
			if(_ARGS[2])
			then
				projectName = path.getbasename("path")
			else 
				printf("error : Please provide project name and/or path")
				return
			end
		end
		
		projectDir = solutiondir .. "/"
		if(_ARGS[2])
		then
			if(path.isAbsolute(_ARGS[2]))
			then
				projectDir = _ARGS[2] .. "/"
			else
				projectDir = projectDir .. _OPTIONS["path"]
			end
		else
			projectDir = projectDir .. "/" .. projectName
		end
	  
		
		printf("Generating project files for : " .. projectName)
		os.execute("{TOUCH} " .. "\"" .. projectDir .. "/premake5.lua\"")
		suffix = "> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""
		line = "include \"" .. solutiondir .. "/project.lua\""
		os.execute("{ECHO} " .. line .. suffix)

		suffix = ">> " .. "\"" .. projectDir .. "/premake5.lua" .. "\""
		line = "newProject(\"" .. projectName .. "\")" 
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
	include "merlin.example"
	
--group "Experiments"
	--include "merlin.experiments/merlin.dem"
	--include "merlin.experiments/merlin.euleurian"
	--include "merlin.experiments/merlin.fdm"
	--include "merlin.experiments/merlin.FDM2D"
	--include "merlin.experiments/merlin.FDM3D"
	--include "merlin.experiments/merlin.flex"
	--include "merlin.experiments/merlin.isosurface"
	--include "merlin.experiments/merlin.pbd"
	--include "merlin.experiments/merlin.pbf"
	--include "merlin.experiments/merlin.pbvef"