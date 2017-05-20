#pragma once

#include "Singleton.h"
#include "Log.h"

// Logging Makros
#define INIT_LOG() Eruption::Utility::Singleton::Get<Eruption::Utility::Log>()->Initialize()
#define LOG_INFO(engineSystem, message) Eruption::Utility::Singleton::Get<Eruption::Utility::Log>()->LogInfo(engineSystem, message)
#define LOG_SUCCESS(engineSystem, message) Eruption::Utility::Singleton::Get<Eruption::Utility::Log>()->LogSuccess(engineSystem, message)
#define LOG_WARNING(engineSystem, message) Eruption::Utility::Singleton::Get<Eruption::Utility::Log>()->LogWarning(engineSystem, message)
#define LOG_ERROR(engineSystem, message) Eruption::Utility::Singleton::Get<Eruption::Utility::Log>()->LogError(engineSystem, message)
#define LOG_NEW_LINE() Eruption::Utility::Singleton::Get<Eruption::Utility::Log>()->NewLine()

// Vulkan Helper Makros
#define VULKAN_FUNC_CALL(result, successMessage, errorMessage)	if(result != VK_SUCCESS) {	\
																	LOG_ERROR("VulkanFunction", errorMessage); \
																} else {	\
																	LOG_SUCCESS("VulkanFunction", successMessage); \
																}