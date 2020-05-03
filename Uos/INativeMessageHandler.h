#pragma once

namespace uos
{
	class INativeMessageHandler
	{
		public:
			bool virtual		ProcessMessage(MSG * m) = 0;
	};
}