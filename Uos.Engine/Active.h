#pragma once
#include "Mesh.h"
#include "Geometry.h"
#include "Camera.h"
#include "ScreenEngine.h"
#include "Mouse.h"
#include "TouchScreen.h"

namespace uos
{
	class CActiveSpace;
	class CActiveGraph;
	class CActiveEvent;
	class CActive;

	enum class EActiveState
	{
		Null=0, Normal=1, Active=2
	};

	static wchar_t * ToString(EActiveState e)
	{
		static wchar_t * name[] = {L"", L"Normal", L"Active"};
		return name[int(e)];
	}

	class UOS_ENGINE_LINKING CPick : public CMeshIntersection
	{
		public:
			CActiveSpace *	Space	= null;
			CActive *		Active	= null;
			CMesh *			Mesh	= null;
			CCamera *		Camera	= null;
			CFloat2			Vpp		= {NAN, NAN};
			CRay			Ray		= CRay(CFloat3(NAN), CFloat3(NAN));
			float			Z		= FLT_MAX;

			CPick(){}
			CPick(const CPick & a);
			CPick(CPick && a);
			~CPick();

			CFloat3			GetFinalPosition();
			CFloat3			GetWorldPosition();
			CPick &			operator = (const CPick & a);
			bool			operator != (const CPick & a);
	};
	
	class CNodeCapture
	{
		public:
			CPick 			Pick;
			CInputMessage 	Message;
			int				Status = 0;
			
			bool IsCaptured()
			{
				return Pick.Active != null;
			}
			CPick *	GetIntersection()
			{
				return &Pick;
			}
			CInputMessage * GetInputMessage()
			{
				return &Message;
			}
	};	

	enum class EActiveEvent : unsigned short
	{
		Null			= 0,
		State			= 0b0000'0000'0000'0001,
		Mouse			= 0b0000'0000'0000'0010,
		Keyboard		= 0b0000'0000'0000'0100,
		Touch			= 0b0000'0000'0000'1000,
	};
	
	enum class EGraphEvent
	{
		Null, 
		Input,
		Enter, Leave, Captured, Released, GuestHover, Hover, Roaming, 
		Feedback,
		Click
	};

	struct CActiveArgs : CShared,  virtual public IType
	{
		CActiveArgs *	Parent = null;
		bool			StopPropagation = false;
		bool			StopRelatedPropagation = false;

		UOS_RTTI
		CActiveArgs(){}
		virtual ~CActiveArgs(){}
	}; 

	class CActiveStateArgs : public CActiveArgs
	{
		public:
			EActiveState	Old;
			EActiveState	New;
	};

	struct CInputArgs : public CActiveArgs
	{
		int					Id;
		EInputClass			Class;
		EControl			Control;
		EInputAction		Action;
		CScreen *			Screen;
		CInputDevice *		Device;

		CInputArgs(CInputMessage & m)
		{			
			Id			= m.Id;
			Class		= m.Class;
			Control		= m.Control;
			Action		= m.Action;
			Screen		= m.Screen;
			Device		= m.Device;
		}
	}; 

	class CMouseArgs : public CInputArgs
	{
		public:
			CPick				Pick;
			CNodeCapture 		Capture;
			EGraphEvent			Event = EGraphEvent::Null;
			CMouseInput * 		Input;
			
			CMouseArgs(CInputMessage & m) : CInputArgs(m)
			{
				Input = m.ValuesAs<CMouseInput>();
				Input->Take();
			}
			
			~CMouseArgs()
			{
				Input->Free();
			}
	};

	class CKeyboardArgs : public CInputArgs
	{
		public:
			CKeyboardInput *	Input;

			CKeyboardArgs(CInputMessage & m) : CInputArgs(m)
			{
				Input = m.ValuesAs<CKeyboardInput>();
				Input->Take();
			}

			~CKeyboardArgs()
			{
				Input->Free();
			}
	};
	
	class CTouchArgs : public CInputArgs
	{
		public:
			EGraphEvent				Event = EGraphEvent::Null;
			CNodeCapture 			Capture;
			CTouchInput	*			Input;
			CMap<CTouch *, CPick>	Picks;

			CTouchArgs(CInputMessage & m) : CInputArgs(m)
			{
				Input = m.ValuesAs<CTouchInput>();
				Input->Take();
			}

			~CTouchArgs()
			{
				Input->Free();
			}

			CTouch * GetPimaryTouch()
			{
				return Input->Touches->Find([](auto i){ return i->Primary; });
			}

			CPick & GetPick()
			{
				return Picks(Input->Touch);
			}

	};
	
	class UOS_ENGINE_LINKING CActive : public CEngineEntity, public CShared
	{	
		public:
			CString												Name;
			bool												Enabled = true;
			ETransformating										Transformating = ETransformating::Inherit;
			CMatrix												Matrix;
			CMatrix												FinalMatrix = CMatrix::Nan;
			CMesh *												Mesh = null;
			CRefList<CActive *>									Nodes;
			CActive *											Parent = null;
			CActive *											HoverChild = null;
			EActiveState										State = EActiveState::Normal;
			bool												IsListener = false;
			bool												IsPropagator=true;
			IType *												Owner = null;
			CActiveGraph *										Graph = null;
			int													Index = -1;
			bool												ActivatePassOn = false;
			EClipping											Clipping = EClipping::Inherit;
			CMesh *												ClippingMesh = null;

			CEvent<CActive *, CActive *, CActiveStateArgs *>	StateChanged;

			CEvent<CActive *, CActive *, CMouseArgs *>			MouseInput;
			CEvent<CActive *, CActive *, CKeyboardArgs *>		KeyboardInput;
			CEvent<CActive *, CActive *, CTouchArgs *>			TouchInput;

			CEvent<CActive *, CActive *, CMouseArgs *>			MouseFilter;
			CEvent<CActive *, CActive *, CKeyboardArgs *>		KeyboardFilter;
			CEvent<CActive *, CActive *, CTouchArgs *>			TouchFilter;

			#ifdef _DEBUG
			CTransformation										_Decomposed;
			CString												FullName;
			#endif

			UOS_RTTI
			CActive(CEngineLevel * l, const CString & name);
			virtual ~CActive();

			void										SetName(const CString & name);
			void										SetMesh(CMesh * m);
			void										SetMatrix(CMatrix const & m);
			CMatrix &									GetMatrix();
			EActiveState								GetState();
			CMesh *										GetMesh();
			bool										IsReady();
			void										Enable(bool e);
			void										Listen(bool e){ IsListener = e; }
			void										TransformMatrix(CMatrix &  m);

			template<class T> T *						AncestorOwnerOf()
														{
															auto p = this;
															while(p && !dynamic_cast<T *>(p->Owner))
															{
																p = p->Parent;
															}
															return p ? dynamic_cast<T *>(p->Owner) : null;
														}

			template<class T> T *						HighestOwnerOf()
														{
															auto p = this;
															T * a = null; 

															while(p)
															{
																auto o = p->GetOwnerAs<T>();
																if(o)
																{
																	a = o;
																}

																p = p->Parent;
															}
															return a;
														}

			template<class T> T *						GetOwnerAs() { return dynamic_cast<T *>(Owner); }
														
			CString										GetStatus();

			void										Attach();
			void										Detach();
			
			void										AddNode(CActive * c);
			void										RemoveNode(CActive * c);
			CActive *									GetParent();
			CActive *									GetAncestor(const CString & name);
			CActive *									GetRoot();
			bool										HasAncestor(CActive * n);
			CActive *									GetElderEnabled();
			CActive *									FindFirstDisabledAncestor();
			bool										IsFinallyEnabled();
			CActive *									FindCommonAncestor(CActive * n);
			CActive *									FindChildContaning(CActive * a);

			CAABB										GetFinalBBox2D();
			CAABB										GetClippingBBox2D();

			CFloat3										Transit(CActive * owner, CFloat3 & p);
			CMeshIntersection							GetIntersection(CRay & r);
			CFloat3										GetXyPlaneIntersectionPoint(CMatrix & sm, CCamera * vp, CFloat2 & vpp);
		
			void										SetClipping(CMesh * mesh);
			CMesh *										GetClipping();
			CActive *									GetActualClipper();
			EClipping									GetActualClipping();

			void										SetMeta(const CString & k, const void * v);
			void										SetMeta(const CString & k, const CString & v);
			template<class T> void						SetMeta(T * v)
														{
															auto s = dynamic_cast<CShared *>(v);
													
															if(s)
															{
																s->Take();
																Metas.push_back(CMetaItem(T::GetClassName(), s));
															}
															else
																Metas.push_back(CMetaItem(T::GetClassName(), static_cast<void *>(s)));
														}
			CString &									GetMetaString(const CString & k);
			void *										GetMetaPointer(const CString & k);
			template<class T> T *						GetMetaPointer()
														{	
															auto & m = Metas.Find([](auto & i){ return i.Key == T::GetClassName(); });
															return m.Pointer ? static_cast<T *>(m.Pointer) : dynamic_cast<T *>(m.Shared); 
														}
			template<class T> bool						HasMetaPointer()
														{ 
															return Metas.Has([](auto & i){ return i.Key == T::GetClassName(); }); 
														}
			bool										HasMeta(const CString & key)
														{ 
															return Metas.Has([&key](auto & i){ return i.Key == key; }); 
														}
		private:
			CList<CMetaItem>							Metas;
	};
}