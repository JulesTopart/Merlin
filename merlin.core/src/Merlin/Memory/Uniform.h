#pragma once
#include "Merlin/Graphics/ShaderBase.h"
#include <vector>
namespace Merlin::Memory {
	using namespace Merlin::Graphics;

	template <class T>
	class Uniform {
	public:
		Uniform(const std::string& name, T initValue) : m_name(name), m_data(initValue), m_dataCopy(initValue){}

		inline bool HasChanged() {
			return m_dataCopy != m_data;
		}

		inline T& value() {
			return m_data;
		}

		void Sync(ShaderBase& shader);

	private:
		bool needRefresh = true;
		T m_data, m_dataCopy;
		std::string m_name;
	};


}