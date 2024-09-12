#pragma once

namespace Merlin {

	class Timestep
	{
	public:
		Timestep(double time = 0.0f)
			: m_Time(time)
		{
		}

		operator float() const { return m_Time; }

		double getSeconds() const { return m_Time; }
		double getMilliseconds() const { return m_Time * 1000.0f; }
	private:
		double m_Time;
	};

}