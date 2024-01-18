function newProject(name)
	project(name)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	debugdir (solutiondir .."/merlin.projects" .. "/%{prj.name}")
	targetdir (solutiondir .."/bin/" .. outputdir .. "/%{prj.name}")
	objdir (solutiondir .."/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders/**.*"
	}
	
	removefiles { "assets/common/**.*" }

	vpaths {
	   ["Headers/*"] = "**.h",
	   ["Sources/*"] = {"**.c", "**.cpp"},
	   ["Docs"] = "**.md",
	   ["Assets/*"] = "assets/**.*"
	}
	
	includedirs { solutiondir .. "/merlin.core/vendor/glfw/include" }
	
	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt" }

	includedirs
	{
		solutiondir .. "/merlin.core/vendor/spdlog/include",
		solutiondir .. "/merlin.core/src",
		solutiondir .. "/merlin.core/vendor",
		solutiondir .. "/merlin.core/%{IncludeDir.glm}",
		solutiondir .. "/merlin.core/%{IncludeDir.glad}",
		solutiondir .. "/merlin.core/%{IncludeDir.imgui}"
	}

	links
	{
		"merlin.core",
		"glad",
		"glfw",
		"imgui",
		"opengl32"
	}
	


	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS",
			"NOMINMAX"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"
		postbuildcommands {
		  "{COPYDIR} %[assets] %[%{!cfg.targetdir}/assets]",
		  "{COPYDIR} %[" .. solutiondir .. "/merlin.core/assets] %[%{!cfg.debugdir}/assets/common]"
		}

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
		optimize "on"
		postbuildcommands {
		  "{COPYDIR} %[assets] %[%{!cfg.targetdir}/assets]",
		  "{COPYDIR} %[" .. solutiondir .. "/merlin.core/assets] %[%{!cfg.debugdir}/assets/common]"
		}
end