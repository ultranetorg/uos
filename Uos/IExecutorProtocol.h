#pragma once

namespace uos
{
	auto constexpr EXECUTOR_PROTOCOL = L"Uos.Executor";

	class IExecutorProtocol
	{
		public:
			void virtual								Execute(const CUrq & o, CExecutionParameters * ep)=0;
			bool virtual 								CanExecute(const CUrq & o)=0;

			virtual ~IExecutorProtocol(){}
	};
}