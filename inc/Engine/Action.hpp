#pragma once

#include <vector>

namespace Magia
{
	class Action
	{
	public:
		int LayerID;
		std::vector<uint32_t> DataBefore;
		std::vector<uint32_t> DataAfter; // TODO: One of this can probably be looked instead of stored
	};
}