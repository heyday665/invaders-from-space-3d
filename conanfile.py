from typing import List, Tuple, Dict
from pathlib import Path
from conan import ConanFile
from conan.tools.files import get, replace_in_file

class Link1(ConanFile): 
    settings : Tuple[str, str] = "os", "compiler", "build_type", "arch"
    generators : Tuple[str, str] = "CMakeToolchain", "CMakeDeps"
    latest_web_gpu_url : str = "https://github.com/eliemichel/WebGPU-distribution/archive/refs/heads/wgpu.zip"
    latest_glfw_web_gpu_url : str = "https://github.com/eliemichel/glfw3webgpu/archive/refs/heads/main.zip"
    def generate(self): 
        os : str = str(self.settings.os)
        get(self, Link1.latest_web_gpu_url)
        get(self, Link1.latest_glfw_web_gpu_url)
        webgpu_cmake_path : Path = Path(self.build_folder) / "WebGPU-distribution-wgpu" / "CMakeLists.txt"
        glfw3_webgpu_cmake_path : Path = Path(self.build_folder) / "glfw3webgpu-main" / "CMakeLists.txt"
        #print("Patching WebGPU CMakeLists.txt")
        #replace_in_file(self, webgpu_cmake_path, "PARENT_SCOPE", "")
        #replace_in_file(
        #        self, 
        #        webgpu_cmake_path, 
        #        'INTERFACE_INCLUDE_DIRECTORIES "${WGPU}"', 
        #        'INTERFACE_INCLUDE_DIRECTORIES "${WGPU}/.."'
        #    )


    def requirements(self): 
        self.requires("glfw/3.3.8")
