#include "StdAfx.h"
/*#include "AltTab.h"
#include "Actor.h"

namespace Mightywill
{
	CAltTab::CAltTab(CActor * pl, IParameter * p) : CEntity3World(pl)
	{
		Parameter	= p;
		Actor		= pl;
		IsActive	= false;

		AltTabHotKeyIdNext	= Level1->RegisterGlobalHotKey(MOD_ALT, VK_TAB, this);
		AltTabHotKeyIdPrev	= Level1->RegisterGlobalHotKey(MOD_ALT|MOD_SHIFT, VK_TAB, this);
		
		LoadParameteres();
	
		OptionBlock		= null;
		OptionLocation	= null;
	}
	
	CAltTab::~CAltTab()
	{
	}

	void CAltTab::CreateOptions(IOptioner * o, IOptionLocation * root)
	{
		Optioner = o;
		OptionBlock = o->CreateBlock(L"AltTab", Level2->GetPathToModule(ModuleDescriptor, L"AltTabOptionsPanel.xrc"), L"WorldAltTabOptionsPanel");
		OptionBlock->Inited			+= EventHandler(CAltTab::OnOptionBlockInited);
		OptionBlock->DataCommited	+= EventHandler(CAltTab::OnOptionBlockCommited);
		Optioner->AddBlock(OptionBlock);

		OptionLocation	= o->CreateLocation(root, L"AltTab", OptionBlock);
	}

	void CAltTab::DeleteOptions()
	{
		delete OptionLocation;

		Optioner->RemoveBlock(OptionBlock);
		delete OptionBlock;
	}
	
	void CAltTab::OnOptionBlockInited()
	{
		OptionBlock->SetSource(Parameter);
		OptionBlock->Assign(L"AnimateTime",				L"AnimateTime");
		OptionBlock->Assign(L"Curvature",				L"Curvature");
		OptionBlock->Assign(L"XSpacing",				L"XSpacing");
		OptionBlock->Assign(L"YSpacing",				L"YSpacing");
		OptionBlock->Assign(L"SelectionAreaMinWidth",	L"SelectionAreaMinWidth");
		OptionBlock->Assign(L"SelectionScale",			L"SelectionScale");
	}

	void CAltTab::OnOptionBlockCommited()
	{
		Config->Save();
		LoadParameteres();
	}
	
	void CAltTab::LoadParameteres()
	{
		AnimateTime				= Parameter->GetFloat32(L"AnimateTime");
		Curvature				= Parameter->GetFloat32(L"Curvature");
		XSpacing				= Parameter->GetFloat32(L"XSpacing");
		YSpacing				= Parameter->GetFloat32(L"YSpacing");
		SelectionAreaMinWidth	= Parameter->GetFloat32(L"SelectionAreaMinWidth");
		SelectionScale			= Parameter->GetFloat32(L"SelectionScale");
	}
		
	bool CAltTab::SortOperator(CWorldNode * l, CWorldNode * r)
	{
		if(l->SortIndex != r->SortIndex)
			return l->SortIndex < r->SortIndex;
		else
			return false;
	}	

	void CAltTab::ProcessHotKey(int id)
	{
		if(!IsActive)
		{
			Actor->ActivateNavigator(this);
		}
		else
		{
			if(TryCollectObjects())
			{
				if(id == AltTabHotKeyIdNext)
				{
					Index++;
				}
				if(id == AltTabHotKeyIdPrev)
				{
					Index--;
				}
				Build();
			}
		}
	}

	void CAltTab::Update(bool camera, bool layout)
	{
		if(IsActive)
		{
			if((GetAsyncKeyState(VK_LMENU) & 0x8000) == 0)
			{
				Actor->ActivateDefaultNavigator();
				
				int n = (int)Nodes.size();
				if(Index < n)
				{
					//Objects[Index]->ZoomRequired();
					Nodes[Index]->MoveToZoom(true, null, L"CAltTab::Update");
				}
				return;
			}
		}
		if(camera || layout)
		{
			if(TryCollectObjects())
			{
				Build();
			}
		}
	}
	
	bool CAltTab::CanActivate()
	{
		return TryCollectObjects();
	}

	void CAltTab::Activate(CEye * eye)
	{
		if(TryCollectObjects())
		{
			IsActive	= true;
			Eye			= eye;
			Index		= 1;

			Actor->WNode->EnableTitles(true);

			Actor->WNode->NodeAdded	+= EventHandler(CAltTab::OnObjectAddedRemoved);
			Actor->WNode->NodeRemoved	+= EventHandler(CAltTab::OnObjectAddedRemoved);

			Build();
		}
	}

	void CAltTab::Deactivate()
	{

		Actor->WNode->NodeAdded	-= EventHandler(CAltTab::OnObjectAddedRemoved);
		Actor->WNode->NodeRemoved	-= EventHandler(CAltTab::OnObjectAddedRemoved);

		Actor->WNode->EnableTitles(false);
		IsActive = false;
	}

	bool CAltTab::IsTotal()
	{
		return true;
	}
	
	void CAltTab::OnObjectAddedRemoved(CWorldNode *)
	{
		if(TryCollectObjects())
		{
			Build();
		}
	}
	
	bool CAltTab::TryCollectObjects()
	{
		Nodes.clear();
		foreach(CArray<CWorldNode *>, i, Actor->WNode->Nodes)
		{
	//		if((*i)->Navigatable)
			{
				Nodes.push_back(*i);
			}
		}
		return Nodes.size() > 1;
	}

	void CAltTab::Build()
	{

		if(Index >= (int)Nodes.size())
		{
			Index = 0;
		}
		if(Index < 0)
		{
			Index = Nodes.size()-1;
		}
			
		std::sort(Nodes.begin(), Nodes.end(), &CAltTab::SortOperator);

		float	zoffset	= 4000.f;
		float	yoffset	= 2500.f;
		int		n = (int)Nodes.size();
		float	w2;
		float	h2;
		
		float	selectedWidth	= Nodes[Index]->GNode->GetOBB().x;
		float	selectedHeight	= Nodes[Index]->GNode->GetOBB().y;
		if(selectedWidth < SelectionAreaMinWidth)
		{
			selectedWidth = SelectionAreaMinWidth;
		}

		CVector3 p = Actor->WNode->GetPosition();
		p.x = 0.f;
		p.y = yoffset;
		p.z = zoffset;
				
		Nodes[Index]->Configurate(false, ETitleMode::TargetTopLeft, false, true);
		Nodes[Index]->MoveToFree(Eye, p, CVector3(0.f, -yoffset, -zoffset), Eye->WView->GetCamera()->GetRotation(), SelectionScale, AnimateTime);

		float x;
		float y;
		
		if(Index < (int)Nodes.size()-1) // selected is no last
		{
			x = (selectedWidth/2)*SelectionScale + 2*XSpacing;
			y = (selectedHeight/2)*SelectionScale + yoffset + YSpacing*2;

			for(int i=Index+1; i<n; i++)
			{
				w2 = Nodes[i]->GNode->GetOBB().x/2.f;
				h2 = Nodes[i]->GNode->GetOBB().y/2.f;
				
				x += w2;
				float y0 = (x*x)/Curvature;
				
				if(y0 - h2 < y) 
				{
					y = y0;
				}
				else
				{
					y += h2;		// too low
					x = sqrt(y*Curvature);
				}
				x += XSpacing;
				y += YSpacing;

				//CVector3 p = c->GetPosition();
				p.x = x;
				p.y = -(y-yoffset);
				p.z = zoffset;
				Nodes[i]->Configurate(false, ETitleMode::TargetTopLeft, false, false);
				Nodes[i]->MoveToFree(Eye, p, CVector3(-x, y-yoffset, -zoffset), Eye->WView->GetCamera()->GetRotation(), 1.f, AnimateTime);
				
				x += w2;
				y += h2;
			}
		}

		if(Index > 0)
		{
			x = (selectedWidth/2)*SelectionScale + 2*XSpacing;
			y = (selectedHeight/2)*SelectionScale + yoffset + YSpacing*2;
	
			for(int i=Index-1; i>=0; i--)
			{
				w2 = Nodes[i]->GNode->GetOBB().x/2.f;
				h2 = Nodes[i]->GNode->GetOBB().y/2.f;
				
				x += w2;
				float y0 = (x*x)/Curvature;
				
				if(y0 - h2 < y) 
				{
					y = y0;
				}
				else
				{
					y += h2;		// too low
					x = sqrt(y*Curvature);
				}
				x += XSpacing;
				y += YSpacing;

				//CVector3 p = c->GetPosition();
				p.x = -x;
				p.y = -(y-yoffset);
				p.z = zoffset;
				Nodes[i]->Configurate(false, ETitleMode::TargetTopRight, false, false);
				Nodes[i]->MoveToFree(Eye, p, CVector3(x, y-yoffset, -zoffset), Eye->WView->GetCamera()->GetRotation(), 1.f, AnimateTime);
				
				x += w2;
				y += h2;
			}
		}
	}
}*/