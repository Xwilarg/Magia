#pragma once

namespace Magia
{
	class ARenderer
	{
	public:
		virtual void PrepareRender() noexcept = 0;
		virtual void Render() noexcept = 0;
		virtual void ProcessEvents(SDL_Event& event) noexcept = 0;
	};
}