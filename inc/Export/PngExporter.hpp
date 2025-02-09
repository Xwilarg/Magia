#pragma once

namespace Magia
{
	class PngExporter
	{
	public:
		void Export(const std::string& path, int width, int height, uint32_t* pixels);
	};
}