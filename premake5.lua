-- OpenGL-Sandbox
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

-- Include directories relative to OpenGL-Core
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
group ""

include "merlin.core"
include "merlin.example"
