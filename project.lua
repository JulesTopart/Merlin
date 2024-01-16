function newProject(name)
	project(name)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	debugdir ("../bin/" .. outputdir .. "/%{prj.name}")
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders/**.*"
	}

	vpaths {
	   ["Headers/*"] = "**.h",
	   ["Sources/*"] = {"**.c", "**.cpp"},
	   ["Docs"] = "**.md",
	   ["Assets/*"] = "assets/**.*"
	}
	
	includedirs { "../merlin.core/vendor/glfw/include" }
	
	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt" }

	includedirs
	{
		"../merlin.core/vendor/spdlog/include",
		"../merlin.core/src",
		"../merlin.core/vendor",
		"../merlin.core/%{IncludeDir.glm}",
		"../merlin.core/%{IncludeDir.glad}",
		"../merlin.core/%{IncludeDir.imgui}"
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
		  "{COPYDIR} %[../merlin.core/assets] %[%{!cfg.targetdir}/assets/common]"
		}

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
		optimize "on"
		postbuildcommands {
		  "{COPYDIR} %[assets] %[%{!cfg.targetdir}/assets]",
		  "{COPYDIR} %[../merlin.core/assets] %[%{!cfg.targetdir}/assets/common]"
		}
end