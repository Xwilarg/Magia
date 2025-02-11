#pragma once

#include <vector>
#include <memory>
#include <string>
#include "DrawLayer.hpp"

// Magia Compressed Format
namespace Magia
{
	class McfExporter
	{
	public:
		void Export(const std::string& path, int width, int height, std::vector<std::shared_ptr<DrawLayer>>& layers);
		void Import(const std::string& path, std::vector<std::shared_ptr<DrawLayer>>& layers);
	};
}