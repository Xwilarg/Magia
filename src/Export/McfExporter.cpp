#include <stdexcept>
#include <fstream>

#include "McfExporter.hpp"
#include "config.hpp"

namespace Magia
{
	void McfExporter::Export(const std::string& path, int width, int height, std::vector<std::shared_ptr<DrawLayer>>& layers)
	{
		std::ofstream fs(path, std::ios::out | std::ios::binary | std::ios::app);

		fs.write(reinterpret_cast<const char*>(&width), sizeof width);
		fs.write(reinterpret_cast<const char*>(&height), sizeof height);
		for (auto& l : layers)
		{
			for (int i = 0; i < width * height; i++)
			{
				auto value = l->Get(i);
				fs.write(reinterpret_cast<const char*>(&value), sizeof value);
			}
		}
		fs.close();
	}
}