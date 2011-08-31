solution "SaturnKi"
  configurations { "Debug", "Release" }
  flags { "Symbols" }
  platforms { "native", "x64" }
  targetdir ("bin")
  location ("projects")

  if os.get() == "windows" then
    defines { "WIN32" }
  end

  if os.get() == "linux" then
    buildoptions "-w"--turn off warnings
  end

  configuration "Debug"
    defines { "DEBUG" }
 
  configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize" }

  project "GuiSdl"
    if os.get() == "windows" then
      includedirs "../../build/vs/**"
      links "../../../build/vs/libs/sdl/**"
    end

    if os.get() == "linux" then
      libdirs { "/usr/lib/" }
      libdirs { os.findlib("SDL")}
      links { "SDL", "SDLmain" }
    end

    kind "ConsoleApp"
    links { "SaturnCore", "CdCore" }
    language "C"
    files { 
      "../../src/sdl/**"
      }

  project "GuiC#"
    language "C#"
	kind "WindowedApp"
	flags "Unsafe"
    links     { 
	"System", 
	"System.Windows.Forms", 
	"System.Drawing", 
	"SaturnCore",
	"CdCore", 
	"SlimDX" }
    files { 
      "../../../lib/c#/**",
      "../../src/c#/main.cs"
      }

  project "GuiWin32"
    flags {
      "WinMain",
      "Unicode"
      }
    links "../../../build/vs/libs/dx/**"

    kind "WindowedApp"
    links { "SaturnCore", "CdCore" }
    language "C++"
    files { 
      "../../src/win32/**"
      }

  project "SaturnCore"
    kind "SharedLib"
    language "C++"
    files { 
      "../../../lib/c/**",
      "../../../src/**.h" 
      }

  project "CdCore"
    kind "SharedLib"
    language "C"
    files { 
      "../../src/common/**",
      }
      
  project "HardwareTests"
    kind "SharedLib"
    language "C++"
	includedirs "../../../tests/hardware_level/"
    files {
      "../../../tests/hardware_level/**",
      }