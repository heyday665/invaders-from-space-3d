from typing import List, Tuple, Dict
from pathlib import Path
from conan import ConanFile
from conan.tools.files import get, replace_in_file

class Link1(ConanFile): 
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    binary_hashes : Dict[str, str] = {
            "Windows" : "b79f9df1412da1bb23c96ce15b9b8fe1", 
            "Linux" : "688213060139b26a0a1021bac6b82a17", 
            "MacOS" : "a892611ebd1e8da2308449bb3c5a7fa2"
        }
    binary_trailing : Dict[str, str] = {
            "Windows" : "windows", 
            "Linux" : "linux", 
            "MacOS" : "macos"
        }
    binary_url_format_string : str = \
            "https://eliemichel.github.io/LearnWebGPU/_downloads/{}/wgpu-native-for-{}.zip"

    def generate(self): 
        os : str = str(self.settings.os)
        get(
                self, 
                Link1.binary_url_format_string.format(
                        Link1.binary_hashes[os], 
                        Link1.binary_trailing[os]
                    )
            )
        webgpu_cmake_path = Path(self.build_folder) / "webgpu" / "CMakeLists.txt"
        print("Patching WebGPU CMakeLists.txt")
        replace_in_file(self, webgpu_cmake_path, "PARENT_SCOPE", "")
        replace_in_file(
                self, 
                webgpu_cmake_path, 
                'INTERFACE_INCLUDE_DIRECTORIES "${WGPU}"', 
                'INTERFACE_INCLUDE_DIRECTORIES "${WGPU}/.."'
            )


    def requirements(self): 
        self.requires("glfw/3.3.8")

