#pragma once
#include "merlin/shaders/shaderBase.h"
#include <vector>
namespace Merlin {

	template <class T>
	class Uniform {
	public:
		Uniform(const std::string& name, T initValue) : m_name(name), m_data(initValue){}

		inline T& value() {
			return m_data;
		}

		void sync(ShaderBase& shader);

	private:
		T m_data;
		std::string m_name;
	};


}