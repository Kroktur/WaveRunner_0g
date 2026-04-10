#pragma once
#include <string>

struct ScoreAlign
{
	ScoreAlign(unsigned int stewFactor) : m_stewFactor(stewFactor){}

	std::string Compute(int number)
	{
		auto str = std::to_string(number);
		std::string fillStr;
		if (str.size() < m_stewFactor)
			fillStr.resize(m_stewFactor - str.size(), '0');
		return fillStr + str;
	}
private:
	unsigned int m_stewFactor;
};

