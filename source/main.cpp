#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <bit>
#include <iterator>
#include <cstddef>
#include <GLFW/glfw3.h>
#include <glfw3webgpu.h>
#define WEBGPU_BACKEND_WGPU
#ifdef WEBGPU_BACKEND_WGPU
// wgpu-native's non-standard parts are in a different header file:
#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>
#define wgpuInstanceRelease wgpuInstanceDrop
#endif

template<typename ElementParameterType>
struct Iterator /* Help with iterator parts of this class from:
		https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp */
{
	using ElementType = ElementParameterType;
	using iterator_category = std::forward_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = ElementType;
	using pointer = ElementType*;
	using reference = ElementType&;
	Iterator(pointer elementPointer) : elementPointer(elementPointer) {}
	reference operator*() const { return *elementPointer; }
	pointer operator->() { return elementPointer; }
	Iterator& operator++() { elementPointer++; return *this; }
	Iterator operator++(int) { Iterator temporary = *this; ++(*this); return temporary; }
	friend bool operator==(const Iterator& left, const Iterator& right) {
		return left.elementPointer == right.elementPointer;
	}
	friend bool operator!=(const Iterator& left, const Iterator& right) {
		return left.elementPointer != right.elementPointer;
	}
protected:
	pointer elementPointer;
};


template<typename ElementParameterType>
struct FillBuffer
{
	using ElementType = ElementParameterType;
	using IteratorType = Iterator<ElementType>;
	using FillFunctionType = std::function<void(ElementType*)>;
	FillBuffer(size_t count) : 
			count(count), 
			buffer(allocateBuffer(count)) {}
	FillBuffer(size_t count, FillFunctionType filler) :
		count(count),
		buffer(filler(allocateBuffer(count))) {}
	IteratorType at(size_t index) {
		return &buffer.get()[index];
	}
	const IteratorType at(size_t index) const {
		return &buffer.get()[index];
	}
	IteratorType operator[](size_t index) {
		return at(index);
	}
	const IteratorType operator[](size_t index) const {
		return at(index);
	}
	void fillBuffer(FillFunctionType filler) {
		filler(buffer.get());
	}
	void fillBuffer(size_t newCount, FillFunctionType filler)
	{
		newCount = count;
		buffer = std::unique_ptr<ElementType>(allocateBuffer(count));
		filler(buffer.get());
	}
	IteratorType begin() { return &buffer.get()[0]; }
	IteratorType end() { return &buffer.get()[count]; }
	size_t getCount() const {
		return count;
	}
protected:
		size_t count;
		std::unique_ptr<ElementType> buffer;
		static ElementType* allocateBuffer(size_t count_)
		{
			return reinterpret_cast<ElementType*>(
					std::malloc(count_ * sizeof(ElementType))
				);
		}
};

struct AdapterUserData {
	WGPUAdapter adapter = nullptr;
	bool requestEnded = false;
};

std::optional<wgpu::Adapter> requestAdapter(
		wgpu::Instance instance, 
		const wgpu::RequestAdapterOptions& options
	)
{
	AdapterUserData userData;
	wgpu::RequestAdapterCallback callback = [&userData](
			wgpu::RequestAdapterStatus status,
			wgpu::Adapter adapter,
			const char* message
		)
	{
		if (status == wgpu::RequestAdapterStatus::Success)
			userData.adapter = adapter;
		else
			std::cerr << "ERROR: Could not get WebGPU adapter: " << message << "\n";
		userData.requestEnded = true;
	};
	instance.requestAdapter(options, std::move(callback));
	if (userData.requestEnded == false) {
		std::cerr << "ERROR: WebGPU Adapter Request never ended!\n";
		return std::nullopt;
	}
	return userData.adapter;
}

void inspectAdapter(wgpu::Adapter adapter)
{
	const size_t featureCount = adapter.enumerateFeatures(nullptr);
	FillBuffer<wgpu::FeatureName> features(featureCount);
	features.fillBuffer([&adapter](wgpu::FeatureName* buffer) {
			adapter.enumerateFeatures(buffer);
		});
	std::cout << "Features (" << features.getCount() << "): \n";
	for (auto feature : features) {
		std::cout << "\t-" << feature << "\n";
	}
}

int main(int argc, char** args)
{
	if(!glfwInit()) {
		std::cerr << "ERROR: Failed to initialize GLFW\n";
		return 1;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Link1", nullptr, nullptr);
	wgpu::InstanceDescriptor instanceDescription = {};
	instanceDescription.nextInChain = nullptr;
	wgpu::Instance instance = wgpu::createInstance(instanceDescription);
	if (!instance) {
		std::cerr << "ERROR: Failed to initialize WebGPU instance!\n";
		return 2;
	}
	std::cout << "WebGPU Instance: " << instance << "\n";
	wgpu::Surface surface = wgpu::Surface(glfwGetWGPUSurface(instance, window));
	std::optional<wgpu::Adapter> adapter_ = requestAdapter(instance, wgpu::RequestAdapterOptions{});
	if(adapter_.has_value() == false)
		return 3;
	wgpu::Adapter adapter = adapter_.value();
	std::cout << "Got adapter: " << adapter << "\n";
	inspectAdapter(adapter);
	while(!glfwWindowShouldClose(window)) glfwPollEvents();
	glfwDestroyWindow(window);
	return 0;
}
