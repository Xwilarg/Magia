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
		unsigned short layerCount = layers.size();
		fs.write(reinterpret_cast<const char*>(&layerCount), sizeof layerCount);
		for (auto& l : layers)
		{
			char pxCount = 0;
			uint32_t lastPixel;
			for (int i = 0; i < width * height; i++)
			{
				auto value = l->Get(i);
				if (i == 0)
				{
					lastPixel = value;
					pxCount++;
				}
				else if (value == lastPixel)
				{
					pxCount++;
				}
				else
				{
					fs.write(reinterpret_cast<const char*>(&pxCount), sizeof pxCount);
					fs.write(reinterpret_cast<const char*>(&lastPixel), sizeof value);
					pxCount = 1;
					lastPixel = value;
				}
			}
			fs.write(reinterpret_cast<const char*>(&pxCount), sizeof pxCount);
			fs.write(reinterpret_cast<const char*>(&lastPixel), sizeof lastPixel);
		}
		fs.close();
	}
}