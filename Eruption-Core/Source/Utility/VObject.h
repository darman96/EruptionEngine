#pragma once

/*

Wrapper class for Vulkan Objects
takes care of cleanup

*/

#include <Vulkan\vulkan.h>
#include <functional>

namespace Eruption { namespace Utility {

	template <typename T>
	class VObject {
	public:
		VObject() : VObject([](T, VkAllocationCallbacks*) {}) {}

		VObject(std::function<void(T, VkAllocationCallbacks*)> deletef) {
			this->deleter = [=](T obj) { deletef(obj, nullptr); };
		}

		VObject(const VObject<VkInstance>& instance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef) {
			this->deleter = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
		}

		VObject(const VObject<VkDevice>& device, std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef) {
			this->deleter = [&device, deletef](T obj) { deletef(device, obj, nullptr); };
		}

		~VObject() {
			cleanup();
		}

		const T* operator &() const {
			return &object;
		}

		T* replace() {
			cleanup();
			return &object;
		}

		operator T() const {
			return object;
		}

		void operator=(T rhs) {
			if (rhs != object) {
				cleanup();
				object = rhs;
			}
		}

		template<typename V>
		bool operator==(V rhs) {
			return object == T(rhs);
		}

	private:
		T object{ VK_NULL_HANDLE };
		std::function<void(T)> deleter;

		void cleanup() {
			if (object != VK_NULL_HANDLE) {
				deleter(object);
			}
			object = VK_NULL_HANDLE;
		}
	};

} }