#pragma once
#include "String.h"

namespace uos
{
	class CPipeClient
	{
		public:
			HANDLE										Pipe;

			CPipeClient(const CString & name)
			{
				BOOL   fSuccess = FALSE;

				while(1)
				{
					Pipe = CreateFile(name.data(),   // pipe name 
									   GENERIC_READ |  // read and write access 
									   GENERIC_WRITE,
									   0,              // no sharing 
									   NULL,           // default security attributes
									   OPEN_EXISTING,  // opens existing pipe 
									   0,              // default attributes 
									   NULL);          // no template file 

													   // Break if the pipe handle is valid. 

					if(Pipe != INVALID_HANDLE_VALUE)
						break;

					if(GetLastError() != ERROR_PIPE_BUSY)
					{
						return;
					}

					if(!WaitNamedPipe(name.data(), 5000))
					{
						return;
					}
				}

				// The pipe connected; change to message-read mode. 

				DWORD dwMode = PIPE_READMODE_MESSAGE;
				fSuccess = SetNamedPipeHandleState(Pipe,    // pipe handle 
												   &dwMode,  // new pipe mode 
												   NULL,     // don't set maximum bytes 
												   NULL);    // don't set maximum time 
				if(!fSuccess)
				{
					return;
				}
			}
			~CPipeClient()
			{
				CloseHandle(Pipe);
			}

			CArray<char> Send(CArray<char> & d)
			{
				DWORD  cbRead, cbWritten;
				BOOL   fSuccess = FALSE;
				
				CArray<char> r;

				fSuccess = WriteFile(	Pipe,                  // pipe handle 
										d.data(),             // message 
										(DWORD)d.size(),              // message length 
										&cbWritten,             // bytes written 
										NULL);                  // not overlapped 

				if(!fSuccess)
				{
					return r;
				}

				r.resize(1024);
				auto o = r.size();
				int read = 0;

				do
				{
					fSuccess = ReadFile(Pipe,    // pipe handle 
										r.data() + read,    // buffer to receive reply 
										DWORD(r.size() - read),  // size of buffer 
										&cbRead,  // number of bytes read 
										NULL);    // not overlapped 

					read += cbRead;

					if(!fSuccess && GetLastError() == ERROR_MORE_DATA)
					{
						r.resize(r.size() * 2);
						continue;
					}


				}while(!fSuccess);

				r.resize(read);

				return r;
			}
	};
}
