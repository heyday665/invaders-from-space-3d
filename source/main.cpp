#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#define WEBGPU_BACKEND_WGPU
#ifdef WEBGPU_BACKEND_WGPU
// wgpu-native's non-standard parts are in a different header file:
#include <webgpu/wgpu.h>
#define wgpuInstanceRelease wgpuInstanceDrop
#endif


int main(int argc, char** args)
{
	if(!glfwInit()) {
		std::cerr << "ERROR: Failed to initialize GLFW\n";
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(640, 480, "Link1", nullptr, nullptr);
	WGPUInstanceDescriptor instanceDescription = {};
	instanceDescription.nextInChain = nullptr;
	WGPUInstance instance = wgpuCreateInstance(&instanceDescription);
	if(!instance) {
		std::cerr << "ERROR: Failed to initialize WebGPU instance!\n";
		return 2;
	}
	std::cout << "WebGPU Instance: " << instance << "\n";
	while(!glfwWindowShouldClose(window)) glfwPollEvents();
	wgpuInstanceRelease(instance);
	glfwDestroyWindow(window);
	return 0;
}

