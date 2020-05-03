#pragma once
#include "Server.h"
#include "Protocol.h"

namespace uos
{
	struct CConnection
	{
		//CNexus *			Nexus;
		IType *				Who;
		CServer *			Server;
		IProtocol *			Protocol;
		CString				ProtocolName;

		CConnection()
		{
			Who = null;
			Server = null;
		}
		CConnection(IType * who, CServer * r, IProtocol * p, CString const & pn)
		{
			Who = who;
			Server = r;
			Protocol = p;
			ProtocolName = pn;
		}

		void Clear()
		{
			Who = null;
			Server = null;
			Protocol = null;
		}

		operator bool () const
		{
			return Server != null;
		}

		bool operator! () const
		{
			return Server == null;
		}

		template<class T> T * As()
		{
			return dynamic_cast<T *>(Protocol);
		}
	};

	template<class P> struct CProtocolConnection : public CConnection
	{
		CProtocolConnection(){}
		CProtocolConnection(CConnection & c) : CConnection(c){}

		P * operator->()
		{
			return dynamic_cast<P *>(Protocol);
		}

		operator P * () const
		{
			return dynamic_cast<P *>(Protocol);
		}
	};

}