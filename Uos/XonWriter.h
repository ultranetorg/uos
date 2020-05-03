#pragma once
#include "Xon.h"

namespace uos
{
	class UOS_LINKING CXonTextWriter : public IXonWriter
	{
		public:
			bool										IsWriteTypes = false;
			CStream *									OutStream = null;

			void										Write(CXon * s) override;

			CXonTextWriter();
			CXonTextWriter(CStream * s)
			{
				OutStream = s;
			}
			CXonTextWriter(CStream * s, bool writeTypes) : IsWriteTypes(writeTypes)
			{
				OutStream = s;
			}
//			CXonTextWriter(bool writeTypes) : IsWriteTypes(writeTypes){}
			~CXonTextWriter(){}

		private:
			void										Write(std::wostringstream & s, CXon * n, int d);
	};

	class UOS_LINKING CXonBinaryWriter : public IXonWriter
	{
		public:
			CStream * Stream;
	
			virtual void								Write(CXon * s) override;

			CXonBinaryWriter(CStream * s);
			~CXonBinaryWriter(){}

		private:

	};
}
