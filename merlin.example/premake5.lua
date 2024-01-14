project "merlin.example"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

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
		"merlin.core"
	}
	
	filter "files:../merlin.core/assets/**"
		buildaction ("Copy")

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
        optimize "on"
