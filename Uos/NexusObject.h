#pragma once
#include "Shared.h"
#include "Uxx.h"
#include "XonDocument.h"
#include "FileStream.h"
#include "NativeDirectory.h"
#include "Guid.h"
#include "Protocol.h"
#include "Event.h"

namespace uos
{
	class CServer;

	class UOS_LINKING CNexusObject : public virtual CShared, public virtual IType//, public virtual IProtocol
	{
		public:
			CUol										Url;
			CServer	*									Server;
			CString										GlobalDirectory;
			CString										LocalDirectory;

			CXonDocument *								InfoDoc = null;
			CEvent<CNexusObject *>						Destroying;
			bool										Shared = false;
			
			~CNexusObject();

			virtual void								SetDirectories(CString const & path);
			void										Load();
			void										Save();
			bool										IsSaved();
			void										Delete();
			void										SaveGlobal(CTonDocument & d, CString const & path);
			void										LoadGlobal(CTonDocument & d, CString const & path);

			CString										AddGlobalReference(CUol & r);
			void										RemoveGlobalReference(CUol & l, CString const & t);

			CXon *										GetInfo(CUol & r);
			CXon *										AddInfo(CUol & r);
			
			void										LoadInfo(CStream * s);
			void										SaveInfo(CStream * s);

			CString										MapRelative(CString const & path);
			CString										MapGlobalPath(CString const & path);
			CString										MapLocalPath(CString const & path);

		protected:
			CNexusObject(CServer * s, CString const & name);

			virtual void								SaveInstance();
			virtual void								LoadInstance();
	};

	template<class T> struct CObject
	{
		CUol											Url;
		T *												Object = null;

		CObject(){}
		CObject(CUol const & o) : Url(o) {}
		CObject(CNexusObject * o) : Object(dynamic_cast<T *>(o))
		{
			if(o)
				Url = o->Url;
		}

		T * operator->()
		{
			return Object;
		}

	 	operator T * () const
		{
			return Object;
		}

		operator bool () const
		{
			return Object != null;
		}

		bool operator! () const
		{
			return Object == null;
		}

		void Clear()
		{
			Object = null;
		}
	};

	template<class T> struct CNameObject
	{
		CString											Name;
		T *												Object = null;

		CNameObject(){}
		CNameObject(T * o) : Object(o)
		{
			if(o)
				Name = o->Name;
		}

		T * operator->()
		{
			return dynamic_cast<T *>(Object);
		}

	 	operator T * () const
		{
			return dynamic_cast<T *>(Object);
		}

		operator bool () const
		{
			return Object != null;
		}

		bool operator! () const
		{
			return Object == null;
		}

		void Clear()
		{
			Object = null;
		}
	};

}