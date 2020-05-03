#include "StdAfx.h"
#include "Fdn.h"

using namespace uos;

CFdn::CFdn(CLevel2 * l)
{
	Level =l;
	Ipfs = new ipfs::Client("localhost", 5001);
}
	
CFdn::~CFdn()
{
	delete Ipfs;
}

void CFdn::Load(const CString & addr, std::function<void(CArray<char> *)> & ok)
{
	CArray<char> * a = null;

	Level->Core->RunThread(addr,[this, &ok, a, addr]() mutable
								{
									std::stringstream s;

									try
									{
										Ipfs->FilesGet(addr.ToAnsi(), &s);
									}
									catch(std::exception & e)
									{
										Level->Log->ReportError(this, L"Load failed: %s", CString::FromAnsi(e.what()));
										return;
									}

									auto eos = std::istreambuf_iterator<char>();
									a = new CArray<char>(std::istreambuf_iterator<char>(s), eos);
								}, 
								[a, ok]
								{
									ok(a);
									delete a;
								});
}

