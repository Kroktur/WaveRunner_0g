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


template<size_t count>
struct ScoreSystem
{
	using size_type = std::make_signed_t<size_t>;
	using data_type = std::pair<std::string, size_type>;
	ScoreSystem() : m_data()
	{
		m_data.fill({ "empty",0 });
	}
	void AddScore(const data_type& score)
	{
		if (score.second < m_data.back().second)
			return;

		data_type currentScore = score;
		bool newEmplace = false;
		for (size_t i = 0; i < count; ++i)
		{
			if (currentScore.second > m_data[i].second && !newEmplace)
			{
				std::swap(currentScore, m_data[i]);
				newEmplace = true;
			}
			else if (currentScore.second >= m_data[i].second && newEmplace)
				std::swap(currentScore, m_data[i]);
		}
	}
	void Save()
	{

		std::ofstream file(m_path);
		file << Tag() << std::endl;
		for (auto& it : m_data)
			if (it.first != "empty")
				file << ToStr(it) << std::endl;
		file.close();
	}
	std::array<data_type, count> Load(const std::filesystem::path& path)
	{
		m_path = path;
		std::ifstream file(path);
		std::string tag = {};
		std::getline(file, tag);
		if (tag != Tag())
			throw std::out_of_range("size incompatible");

		for (int i = 0; i < count; ++i)
		{
			std::string line = {};
			std::getline(file, line, ';');
			if (line.empty())
				return m_data;
			m_data[i].first = line;
			std::getline(file, line);
			if (line.empty())
				throw std::out_of_range("error no number");
			m_data[i].second = std::stoll(line);
		}
		file.close();
		return m_data;
	}
	std::array<data_type, count> Get()
	{
		return m_data;
	}
	void Debug()
	{
		for (auto& it : m_data)
			std::cout << it.first << " : " << it.second << std::endl;
	}
private:
	static std::string Tag()
	{
		return "Score : " + std::to_string(count);
	}
	static std::string ToStr(const data_type& data)
	{
		return data.first + ";" + std::to_string(data.second);
	}
	std::array<data_type, count> m_data;
	std::filesystem::path m_path;
};

std::unique_ptr<ScoreSystem<10>> LoadScore(const std::string& filePath);

inline std::unique_ptr<ScoreSystem<10>> LoadScore(const std::string& filePath)
{
	std::unique_ptr<ScoreSystem<10>> result = std::make_unique<ScoreSystem<10>>();
	result->Load(filePath);
	return std::move(result);
}

using ScoreLoader =
KGR::ResourceManager<ScoreSystem<10>,
	KGR::TypeWrapper<>,
	LoadScore>;