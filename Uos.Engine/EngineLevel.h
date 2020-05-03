#pragma once

namespace uos
{
	class CEngineLevel : public CLevel2
	{
		public:
			CConfig *									Config;
			CServer *									Server;

			CEngineLevel(CLevel2 & l) : CLevel2(l)
			{
			}
			
			static void VerifyCritical(const wchar_t * m, int l, HRESULT hr)
			{
				if(hr != S_OK)
				{
					if(hr == E_OUTOFMEMORY)
					{
						throw CAttentionException(m, l, L"Not enough system memory");
					}

					throw CException(m, l, GetErrorDescription(hr).c_str());
				}
			}
			
			static CString GetErrorDescription(HRESULT hr)
			{
				/*LPWSTR pBuffer = NULL;
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pBuffer, 0, NULL);
				CString a = pBuffer;
				LocalFree(pBuffer);*/

				wchar_t b[1024];
				DXGetErrorDescription(hr, b, _countof(b));
				return b;
			}
	};

	class CEngineEntity : public virtual IType
	{
		public:
			CEngineLevel *								Level;

			CEngineEntity(CEngineLevel * l);
			~CEngineEntity();
		
			void										VerifyWithWarning(HRESULT hr);
	};
}