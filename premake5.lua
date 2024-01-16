
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
	
	
group "Experiments"
	include "merlin.experiments/merlin.dem"
	--include "merlin.experiments/merlin.euleurian"
	--include "merlin.experiments/merlin.fdm"
	--include "merlin.experiments/merlin.FDM2D"
	--include "merlin.experiments/merlin.FDM3D"
	--include "merlin.experiments/merlin.flex"
	--include "merlin.experiments/merlin.isosurface"
	--include "merlin.experiments/merlin.pbd"
	--include "merlin.experiments/merlin.pbf"
	--include "merlin.experiments/merlin.pbvef"
	include "merlin.experiments/merlin.sort"
	
group ""
	include "merlin.core"
	include "merlin.example"

