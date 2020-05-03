#pragma once
#include "Xon.h"

namespace uos
{
	class UOS_LINKING CXonTextReader : public IXonReader
	{
		public:
			CString						Text;
			CArray<bool>				Type;
			

			CXonTextReader(CString const & t);
			CXonTextReader(CStream * s);
			~CXonTextReader();

			EXonToken									Read() override;
			EXonToken									ReadNext() override;
			void										ReadValue(CXonSimpleValue * v) override;
			void										ReadName(CString & name, CString & type) override;

		private:
			wchar_t *				C;
			void										Next(wchar_t *& c);
	};

	class UOS_LINKING CXonBinaryReader : public IXonReader
	{
		public:
			CStream *									Stream;
			//CBinaryNode *								Read();

			CArray<CString>								Types;
			CArray<int>									Sizes;

			CList<unsigned char>						Flags;

			//int											Datasize;
			
			CXonBinaryReader(CStream * s);
			~CXonBinaryReader();

			EXonToken									Read() override;
			EXonToken									ReadNext() override;
			void										ReadValue(CXonSimpleValue * v) override;
			void										ReadName(CString & name, CString & type) override;

		private:
			//CBinaryNode * ReadNode(unsigned char * f);
	};
}
