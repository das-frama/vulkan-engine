#include "vulkan_backend.h"

#include "vulkan_types.inl"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"

#include "core/logger.h"
#include "core/string.h"

#include "containers/darray.h"

// Static vulkan context.
static vulkan_context context;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
                                                 VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data);

i32 find_memory_index(u32 type_filter, u32 property_flags);

b8 vulkan_renderer_backend_init(renderer_backend* backend, const char* app_name, struct platform_state* plat_state) {
	// function pointers.
    context.find_memory_index = find_memory_index;
    
    // TODO(frama): Custom allocator.
    context.allocator = 0;
    
	// Setup vulkan instance.
	VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
	app_info.apiVersion = VK_API_VERSION_1_3;
	app_info.pApplicationName = app_name;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Motor Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    
	VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	create_info.pApplicationInfo = &app_info;
    
	// List of required extensions.
	const char** required_extensions = darray_create(const char*);
	darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface extension.
	platform_get_required_extension_names(&required_extensions); // Platform-specific extensions.
#if defined(_DEBUG)
	darray_push(required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	DEBUG("Required extensions:");
	u32 len = darray_length(required_extensions);
	for (u32 i = 0; i < len; ++i) {
		DEBUG(required_extensions[i]);
	}
#endif
    
	create_info.enabledExtensionCount = darray_length(required_extensions);
	create_info.ppEnabledExtensionNames = required_extensions;
    
	// Validation layers.
	const char** required_validation_layer_names = 0;
	u32 required_validation_layer_count = 0;
    
#if defined(_DEBUG)
	INFO("Validation layers enabled. Enumirating...");
	// The list of validation layers required.
	required_validation_layer_names = darray_create(const char*);
	darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
	required_validation_layer_count = darray_length(required_validation_layer_names);
    
	// List of available validation layers.
	u32 available_layer_count = 0;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
	VkLayerProperties* available_layers = darray_reserve(VkLayerProperties, available_layer_count);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));
    
	// Verify all required layers are available.
	for (u32 i = 0; i < required_validation_layer_count; ++i) {
		INFO("Searching for layer: %s...", required_validation_layer_names[i]);
		b8 found = false;
		for (u32 j = 0; j < available_layer_count; ++j) {
			if (strings_equal(required_validation_layer_names[i], available_layers[j].layerName)) {
				found = true;
				INFO("Found.");
				break;
			}
		}
        
		if (!found) {
			FATAL("Required validation layer is missing: %s", required_validation_layer_names[i]);
			return false;
		}
	}
	INFO("All required validation layers are present.");
#endif
    
	create_info.enabledLayerCount = required_validation_layer_count;
	create_info.ppEnabledLayerNames = required_validation_layer_names;
    
	VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
	INFO("Vulkan instance created.");
	// Debugger.
#if defined(_DEBUG)
	DEBUG("Creating Vulkan debugger...");
	u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
	debug_create_info.messageSeverity = log_severity;
	debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	debug_create_info.pfnUserCallback = vk_debug_callback;
	debug_create_info.pUserData = 0;
    
	PFN_vkCreateDebugUtilsMessengerEXT func = 
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
	ASSERT_MSG(func, "Failed to create debug messenger!");
	VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
	DEBUG("Vulkan debugger created.");
#endif
    
    // Surface cration.
    DEBUG("Creating vulkan surface...");
    if (!platform_create_vulkan_surface(plat_state, &context)) {
        ERROR("Failed to create platform surface.");
        return false;
    }
    DEBUG("Vulkan surface created,");
    
    // Device creation.
    if (!vulkan_device_create(&context)) {
        ERROR("Failed to create device.");
        return false;
    }
    
    // Swapchain.
    vulkan_swapchain_create(&context, context.framebuffer_width, context.framebuffer_height, &context.swapchain);
    
    // Renderpass.
    vulkan_renderpass_create(&context, &context.main_renderpass,
                             0, 0, context.framebuffer_width, context.framebuffer_height,
                             0.0f, 0.0f, 0.2f, 1.0f,
                             1.0f, 0);
    
	INFO("Vulkan renderer initialized successfully.");
	return true;
}

void vulkan_renderer_backend_shut(renderer_backend* backend) {
    DEBUG("Destroying Vulkan renderpass...");
    vulkan_renderpass_destroy(&context, &context.main_renderpass);
    
    DEBUG("Destroying Vulkan swapchain...");
	vulkan_swapchain_destroy(&context, &context.swapchain);
    
    DEBUG("Destroying Vulkan device...");
    vulkan_device_destroy(&context);
    
	DEBUG("Destroying Vulkan surface...");
    vkDestroySurfaceKHR(context.instance, context.surface, 0);
    
    DEBUG("Destroying Vulkan debugger...");
	if (context.debug_messenger) {
		PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
		func(context.instance, context.debug_messenger, context.allocator);
	}
    
	DEBUG("Destroying Vulkan instance...");
	vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_renderer_backend_on_resized(renderer_backend* backend, u16 width, u16 height) {
    
}

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 dt) {
	return true;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 dt) {
	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
                                                 VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data) {
	switch (message_severity) {
		default:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        ERROR(callback_data->pMessage);
        break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        WARN(callback_data->pMessage);
        break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        INFO(callback_data->pMessage);
        break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        TRACE(callback_data->pMessage);
        break;
	}
    
	return VK_FALSE; 
}

i32 find_memory_index(u32 type_filter, u32 property_flags) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_device, &memory_properties);
    
    for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
        // Check each memory type to see if its bit is set to 1.
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
            return i;
        }
    }
    
    WARN("Unable to find suitable memory type!");
    return -1;
}