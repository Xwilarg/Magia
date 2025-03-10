#include <stdexcept>
#include <fstream>

#include "McfExporter.hpp"
#include "config.hpp"

namespace Magia
{
	void McfExporter::Export(const std::string& path, int width, int height, std::vector<std::shared_ptr<DrawLayer>>& layers)
	{
		std::ofstream fs(path, std::ios::out | std::ios::binary | std::ios::trunc);

		fs.write(reinterpret_cast<const char*>(&width), sizeof(width));
		fs.write(reinterpret_cast<const char*>(&height), sizeof(height));
		unsigned short layerCount = layers.size();
		fs.write(reinterpret_cast<const char*>(&layerCount), sizeof layerCount);
		for (auto& l : layers)
		{
			unsigned char pxCount = 0;
			uint32_t lastPixel;
			for (int i = 0; i < width * height; i++)
			{
				auto value = l->Get(i);
				if (i == 0)
				{
					lastPixel = value;
					pxCount = 1;
				}
				else if (value == lastPixel && pxCount < UCHAR_MAX)
				{
					pxCount++;
				}
				else
				{
					fs.write(reinterpret_cast<const char*>(&pxCount), sizeof(pxCount));
					fs.write(reinterpret_cast<const char*>(&lastPixel), sizeof(value));
					pxCount = 1;
					lastPixel = value;
				}
			}
			fs.write(reinterpret_cast<const char*>(&pxCount), sizeof(pxCount));
			fs.write(reinterpret_cast<const char*>(&lastPixel), sizeof(lastPixel));
		}
		fs.close();
	}

	void McfExporter::Import(const std::string& path, std::vector<std::shared_ptr<DrawLayer>>& layers)
	{
		std::ifstream fs(path, std::ios::in | std::ios::binary);

		int width, height;
		fs.read(reinterpret_cast<char*>(&width), sizeof(width));
		fs.read(reinterpret_cast<char*>(&height), sizeof(height));
		unsigned short layerCount;
		fs.read(reinterpret_cast<char*>(&layerCount), sizeof(layerCount));

		layers.clear();

		int max = width * height;

		for (int l = 0; l < layerCount; l++)
		{
			std::shared_ptr<DrawLayer> layer = std::make_shared<DrawLayer>(width, height);

			int i = 0;
			while (i < max)
			{
				unsigned char adv;
				uint32_t pixel;
				fs.read(reinterpret_cast<char*>(&adv), sizeof(adv));
				fs.read(reinterpret_cast<char*>(&pixel), sizeof(pixel));

				if (adv == 0)
				{
					std::runtime_error("Invalid size 0");
				}

				for (int px = 0; px < adv; px++)
				{
					if (i + px >= width * height)
					{
						std::runtime_error("Out of bounds");
					}
					layer->Set(i + px, pixel);
				}

				i += adv;
			}

			layers.push_back(std::move(layer));
		}

		fs.close();
	}
}