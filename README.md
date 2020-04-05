# TPDPHook
Unofficial patch for TPDP: SoD 1.103  
Allows modders to create new skills and tweak the game in ways that are not possible just by editing the data files.

## Usage
Grab a release zip from the [releases](https://github.com/php42/TPDPHook/releases/latest) page and extract dxgi.dll and TPDPHook.ini into the game folder.  
TPDPHook.ini contains various settings that can be configured to your liking. The one provided in release packages
is heavily commented with examples.

Its primary use for the moment is setting the animations for new skills since there is no way to do this from the data files.

## Programming Skills
Support for programming *entirely* new skills is currently quite limited and requires rebuilding TPDPHook from source.  
See [custom_skills.cpp](https://github.com/php42/TPDPHook/blob/master/TPDPHook/tpdp/custom_skills.cpp) for example skill implementaions.  
See [tpdp_data.h](https://github.com/php42/TPDPHook/blob/master/TPDPHook/tpdp/tpdp_data.h) for game data and functions that are accessible from skill functions.

## Compiling
Prerequisites:  
Visual Studio 2019 or newer.

No dependencies, just open the MSVC project file and click build.
