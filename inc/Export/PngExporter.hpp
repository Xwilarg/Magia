#pragma once

#include <vector>
#include <memory>
#include "DrawLayer.hpp"

namespace Magia
{
	class PngExporter
	{
	public:
		void Export(const std::string& path, int width, int height, uint32_t* pixels);
		void Import(const std::string& path, std::vector<std::shared_ptr<DrawLayer>>& layers);
	};
}