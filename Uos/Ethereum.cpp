#include "StdAfx.h"
#include "Ethereum.h"

using namespace uos;
using json = nlohmann::json;
///geth.exe --cache 1024 --syncmode light --datadir c:\Ethereum\~data --keystore c:\Ethereum\~keystore

CEthereum::CEthereum(CLevel2 * l) : Http(l)
{
	Level = l;
	//Pipe = new CPipeClient(L"\\\\.\\pipe\\\\geth.ipc");


/*
	CAnsiString s = "double(int256)";



/*
	auto s = CString(L"{\"jsonrpc\":\"2.0\",\"method\":\"eth_getBalance\",\"params\":[\"0xD061eeCb93844A8cAbea06D80976d5958f48a343\", \"latest\"],\"id\":67}").ToUtf8();
	auto r = Pipe->Send(s);
	auto o = CString::FromUtf8(r.data(), r.size());
*/


}
	
CEthereum::~CEthereum()
{
	//delete Pipe;
}

CString CEthereum::Encode(CArray<CSolidityValue> & values)
{
	CString r;

	auto o = values.Sum<int>([](auto & i){ return i.GetHeaderSize(); });

	for(auto i : values)
	{
		if(i.Type != EEthType::string && i.Type != EEthType::dynamic)
		{
			r += i.Encode();
		}
		else
		{
			r += CSolidityValue::Encode(o);
			o += i.GetDataSize();
		}
	}

	for(auto i : values)
	{
		if(i.Type == EEthType::string || i.Type == EEthType::dynamic)
		{
			r += i.Encode();
		}
	}

	return r;
}

void CEthereum::Decode(CString const & d, CArray<CSolidityValue> & values)
{
	//auto c = CAbiValue::Decode(d);

	auto p = (wchar_t *)d.data();

	for(auto & i : values)
	{
		if(i.Type != EEthType::string && i.Type != EEthType::dynamic)
		{
			i.Decode(p);
		}
		else
		{
			CSolidityValue o(EEthType::integer);
			o.Decode(p);
			i.Decode((wchar_t *)d.data() + (uint64_t)o.GetInt()*2);
		}

		p += i.GetHeaderSize()*2;
	}
	//return ;
}

CString CEthereum::EncodeSignature(const CString & s)
{
	Keccak h(256);
	auto u = s.ToAnsi();
	h.addData((uint8_t *)u.data(), 0, (unsigned int)(u.size()));

	CString o;
	auto d = h.digest();

	for(int i=0; i<4; i++)
	{
		o += CString::Format(L"%02x", d[i]);
	}

	return o;
}

void CEthereum::IsRegistered(const CString & name)
{
	CArray<CSolidityValue> v;
	v.push_back(CSolidityValue(name));

	auto url = L"https://mainnet.infura.io/v3/b032fdb13aff414e9f215d53cf0a2953";
		
	auto r = R"(
				{
					"jsonrpc":"2.0", 
					"method":"eth_call", 
					"params":[{	"to": "0x53e9A469b8C3c7fBd22B9187fee4edbadB0e1700", 
								"data": "0x)" + CString(EncodeSignature(L"IsRegistered(string)") + Encode(v)).ToAnsi() + R"("}, 
								"latest"], 
					"id":1
				}
			)";


	//json j;
	//j["jsonrpc"]= "2.0";
	//j["method"]= "eth_call";
	//j["params"] = json::array({
	//							json::object({"to",  "0x0FDD079E9C770F5Bee0Aa59356e06448a3B8BB98"}),
	//							json::object({"data",  Encode(v).ToAnsi() }),
	//							json::object({"latest", ""})
	//						  });
	//j["id"] = "123";
	//
	//auto rr = j.dump(1, L'\t');
}

void CEthereum::Call(CString const & sig, CArray<CSolidityValue> * in, CArray<CSolidityValue> * out, std::function<void()> done)
{
	CString c =	
LR"({
"jsonrpc":"2.0",
"method":"eth_call",
"params":[{	"to": ")" + Contract + LR"(",
			"data": "0x)" + EncodeSignature(sig) + Encode(*in) + LR"("},
			"latest"],
"id":1
})";

	   
	auto r = new CHttpRequest(Level, Infura);
	r->Method = L"POST";
	r->Content = c.ToAnsi();
	r->Recieved =	[this, r, done, out]() mutable
					{
						auto b = r->Stream.Read();

						auto t = CString::FromUtf8((char *)b.GetData(), (int)b.GetSize()).ToAnsi();

						nlohmann::json j = nlohmann::json::parse(t);
						
						Decode(CString::FromAnsi(j["result"]).Substring(2), *out);

						done();

						delete r;
					};

	r->Failed = [r]{ delete r; };

	r->Send();
}




/*
0x08c379a0
0000000000000000000000000000000000000000000000000000000000000020
0000000000000000000000000000000000000000000000000000000000000011
50726f64756374206e6f7420666f756e64000000000000000000000000000000*/


//(string platform, uint16[4] version, string cid)
// uos, 0.1.210.0, <cid>
/*
00000000000000000000000000000000000000000000000000000000000000c0

0000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000001
00000000000000000000000000000000000000000000000000000000000000d2
0000000000000000000000000000000000000000000000000000000000000000

0000000000000000000000000000000000000000000000000000000000000100

0000000000000000000000000000000000000000000000000000000000000003
756f730000000000000000000000000000000000000000000000000000000000

000000000000000000000000000000000000000000000000000000000000002e
516d63486b77345961455745624441617a6f55626163627732514b3433436933
467469336d6d5853765732474a42000000000000000000000000000000000000
*/