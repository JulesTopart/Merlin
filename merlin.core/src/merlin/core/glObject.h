#pragma once
#include "merlin/core/core.h"
#include "merlin/core/console.h"

#include <cassert>
#include <string>
#include <glad/gl.h>

namespace Merlin {

	template<class GLdestructor = void (*)(unsigned int)>
	class GLObject {
	public:
		GLObject(GLuint id, GLdestructor destroy) noexcept;
		virtual ~GLObject();

		GLObject(GLObject&& mov) noexcept;
		GLObject& operator=(GLObject&& mov) noexcept;
		GLObject(const GLObject& cpy) = delete;
		GLObject& operator=(const GLObject& cpy) = delete;
		GLuint id() const noexcept;
		std::string name() const noexcept;
		inline void rename(const std::string& name) noexcept { m_name = name; }

	protected:
		inline void recreate(GLuint newID) { m_id = newID; }

	private:
		GLuint m_id = 0;
		std::string m_name;
		bool m_valid = false;
		GLdestructor m_destroy = nullptr;
		static inline int m_objectInstances = 0;
	};

	template<class GLdestructor>
	GLObject<GLdestructor>::GLObject(
		GLuint id,
		GLdestructor destroy
	) noexcept :
		m_valid(true),
		m_id(id),
		m_name("object" + std::to_string(m_objectInstances++)),
		m_destroy(destroy)
	{}

	template<class GLdestructor>
	GLObject<GLdestructor>::~GLObject() {
		if (m_valid) {
			m_destroy(m_id);
		}
	}

	template<class GLdestructor>
	GLObject<GLdestructor>::GLObject(GLObject&& mov) noexcept :
		m_valid(mov.m_valid),
		m_id(mov.m_id)
	{
		mov.m_valid = false;
	}

	template<class GLdestructor>
	GLObject<GLdestructor>& GLObject<GLdestructor>::operator=(GLObject&& mov) noexcept {
		m_valid = mov.m_valid;
		mov.m_valid = false;
		m_id = mov.m_id;
		return *this;
	}

	template<class GLdestructor>
	GLuint GLObject<GLdestructor>::id() const noexcept {
		assert(m_valid);
		return m_id;
	}

	template<class GLdestructor>
	std::string GLObject<GLdestructor>::name() const noexcept {
		assert(m_valid);
		return m_name;
	}

}

