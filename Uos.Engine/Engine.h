#pragma once

#include "VulkanEngine.h"
#include "ScreenEngine.h"
#include "InputSystem.h"

#include "Renderer.h"
#include "Interactor.h"

#include "ActiveSpace.h"
#include "TreeVisualGraph.h"
#include "MaterialFactory.h"
#include "TextureFactory.h"
#include "FontFactory.h"

#include "Geometry.h"
#include "QuadragonMesh.h"
#include "FrameMesh.h"
#include "BoxMesh.h"
#include "SphereMesh.h"

#include "DirectionalLight.h"

namespace uos
{
	#undef AddJob

	class UOS_ENGINE_LINKING CEngine : public IType, public IIdleWorker
	{
		public:
			CLevel2 *									Level;
			CEngineLevel								EngineLevel;

			CDiagnostic *								Diagnostic;
			IPerformanceCounter *						PcUpdate;

			CInputSystem  *								InputSystem;
			CDirectSystem *								DisplaySystem;
			CScreenEngine *								ScreenEngine;
			CRenderer *									Renderer;
			CInteractor *								Interactor;

			CDirectPipelineFactory *					PipelineFactory;
			CMaterialFactory *							MaterialFactory;
			CTextureFactory *							TextureFactory;
			CFontFactory *								FontFactory;

			UOS_RTTI
			CEngine(CLevel2 * l, CServer * una, CConfig * c);
			~CEngine();

			void										Start();
			void										Stop();
			bool										IsRunning();
			void										Update();

			CMesh *										CreateMesh();
			CVisual *									CreateVisual(const CString & name, CMesh * mesh, CMaterial * mtl, CMatrix const & m);
			CActive *									CreateActive(const CString & name, CMesh * mesh, CMatrix const & m);
			CTreeVisualGraph *							CreateTreeVisualGraph(const CString & name);
			CVisualSpace *								CreateVisualSpace(const CString & n);
			CActiveSpace *								CreateActiveSpace(const CString & n);
	
			void										OnInteractorMessageProcessed(CInputMessage & m);
			void										OnDiagnosticsUpdate(CDiagnosticUpdate & a);

			virtual void								DoIdle() override;
	};
}
