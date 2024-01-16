project "merlin.core"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "glpch.h"
	pchsource "src/glpch.cpp"

	files { 
		"src/**.h",
		"src/**.c",
		"src/**.cpp",
		"vendor/imgui/backends/imgui_impl_glfw.*",
		"vendor/imgui/backends/imgui_impl_opengl3.*",
		"assets/shaders/**.*"
	}

	vpaths {
	   ["Headers/*"] = "**.h",
	   ["Sources/*"] = {"**.c", "**.cpp"},
	   ["Docs"] = "**.md",
	   ["Assets/*"] = "assets/**.*"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir. glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links 
	{ 
		"glfw",
		"glad",
		"imgui",
		"stb_image",
		"opengl32.lib"
	}

	filter { 'files:vendor/imgui/backends/imgui_impl_glfw.cpp' }
		flags { 'NoPCH' }
		
	filter { 'files:vendor/imgui/backends/imgui_impl_opengl3.cpp' }
		flags { 'NoPCH' }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
		optimize "on"