#include "StdAfx.h"
#include "VwmImporter.h"

using namespace uos;

CVwmImporter::CVwmImporter(CWorldLevel * l)
{
	Level = l;
	Engine	= l->Engine;
	Graph = null;
}

CVwmImporter::~CVwmImporter()
{
	for(auto i : Meshes)
	{
		i.second->Free();
	}
	for(auto i : Materials)
	{
		i.second->Free();
	}
	for(auto i : Textures)
	{
		i.second->Free();
	}
	for(auto i : Lights)
	{
		delete i.second;
	}

	delete Graph;
}

void CVwmImporter::Import(IDirectory * dir)
{
	Directory = dir;
	
	auto f = dir->OpenReadStream(L"Graph.bon");
	
	Graph = new CBonDocument(CXonBinaryReader(f));
	
	dir->Close(f);
}

//CVisual * CVwmImporter::LoadTree(CXon * p)
//{
//	std::function<CVisual * (CXon *)>	loadNode =	[this, &loadNode](auto p)
//														{
//															auto v =  new CVisual(	&Level->Engine->EngineLevel,
//																					p->AsString(),
//																					p->One(L"Mesh")  ? ImportMesh(p->Get<CString>(L"Mesh")) : null,
//																					p->One(L"Material") ? ImportMaterial(p->Get<CString>(L"Material")) : null,
//																					CMatrix(p->Get<CTransformation>(L"Transformation")));
//															//v->SetView(World->EnvView);
//															LoadVisual(p, v);
//														
//															for(auto i : p->Many(L"Node"))
//															{
//																auto c = loadNode(i);
//																v->AddNode(c);
//																c->Free();
//															}
//
//															return v;
//														};
//
//	return loadNode(p);
//}


CElement * CVwmImporter::ImportNodeTree(CXon * r)
{
	std::function<void (CXon *)> findLights =	[this, &findLights](auto p)
												{
													if(p->One(L"Light"))
													{
														ImportLight(p->Get<CString>(L"Light"));
													}

													for(auto i : p->Many(L"Node"))
													{
														findLights(i);
													}
												};

	std::function<CElement * (CXon *)> loadNode =		[this, &loadNode](auto p)
														{
															auto n = new CElement(Level, p->AsString());

															///if(p->One(L"Mesh"))
															///{
															///	n->Visual->SetMesh(ImportMesh(p->Get<CString>(L"Mesh")));
															///	n->Active->SetMesh(ImportMesh(p->Get<CString>(L"Mesh")));
															///}
															///
															///if(p->One(L"Material"))
															///{
															///	n->Visual->SetMaterial(ImportMaterial(p->Get<CString>(L"Material")));
															///}
																					
															n->Transform(p->Get<CTransformation>(L"Transformation"));
															
															if(auto v = p->One(L"Visual"))
															{
																ImportVisual(p, v, n->Visual);
															}

															if(auto v = p->One(L"Active"))
															{
																ImportActive(p, v, n->Active);
															}

														
															for(auto i : p->Many(L"Node"))
															{
																auto c = loadNode(i);
																n->AddNode(c);
																c->Free();
															}

															return n;
														};

	findLights(r);

	return loadNode(r);
}


void CVwmImporter::ImportVisual(CXon * n, CXon * vn, CVisual * v)
{
	v->SetCullMode(n->One(L"BackCull")->AsBool() ? ECullMode::CCW : ECullMode::None);

	CLight * l = null;
	
	if(Lights.size() > 0 && vn->Get<CBool>(L"IsReceiveLight"))
		l = Lights.begin()->second;
			
	if(auto mesh = vn->One(L"Mesh"))
		v->SetMesh(ImportMesh(mesh->AsString()));

	if(auto mtl = vn->One(L"Material"))
		v->SetMaterial(ImportMaterial(mtl->AsString(), l));
	
	//if(p->One(L"IsReceiveLight")->AsBool())
	//{
	//	v->SetLightGroup(LightGroup);
	//}
}

void CVwmImporter::ImportActive(CXon * n, CXon * vn, CActive * a)
{
	//a->SetCullMode(n->One(L"BackCull")->AsBool() ? ECullMode::CCW : ECullMode::None);

	if(auto mesh = vn->One(L"Mesh"))
		a->SetMesh(ImportMesh(mesh->AsString()));
}

CLight * CVwmImporter::ImportLight(const CString & file)
{
	auto i = Lights.find(file);
	if(i != Lights.end())
	{
		return i->second;
	}
		
	auto f = Directory->OpenReadStream(file);
	auto & mwx = CBonDocument(CXonBinaryReader(f));
	Directory->Close(f);

	auto root = &mwx;

	if(root->One(L"Type")->Get<CString>() == L"Directional")
	{
		auto l = new CDirectionalLight(&Level->Engine->EngineLevel);
		l->SetDirection(root->One(L"Direction")->Get<CFloat3>());
		l->SetDiffuseIntensity(root->One(L"DiffuseIntensity")->AsFloat32());
		Lights[file] = l;
		return l;
	}
	return null;
}

CMaterial * CVwmImporter::ImportMaterial(const CString & file, CLight * l)
{
	auto i = Materials.find(file);
	if(i != Materials.end())
	{
		return i->second;
	}

	auto f = Directory->OpenReadStream(file);
	auto & mwx = CBonDocument(CXonBinaryReader(f));
	Directory->Close(f);

	auto root = &mwx;
	
	CMaterial * m = null;

	if(root->One(L"Type")->Get<CString>() == L"Standard")
	{
		m = LoadStandardMaterial(root, l);
	}
	else if(root->One(L"Type")->Get<CString>() == L"Blend")
	{
		m = LoadBlendMaterial(root, l);
	}
	else
	{
		Level->Log->ReportWarning(this, L"LoadMaterial: Unknown material detected.");
	}
	
	if(root->One(L"Name") != null)
	{
		m->Name = root->Get<CString>(L"Name");
	}

	Materials[file] = m;
	return Materials[file];
}

CMaterial * CVwmImporter::LoadStandardMaterial(CXon * root, CLight * l)
{
	auto diffuse	= root->One(L"Diffuse");
	auto ambient	= root->One(L"Ambient");
	auto alpha		= root->One(L"Alpha");

	auto dmn = root->One(L"DiffuseMap");
	auto amn = root->One(L"AlphaMap");
	auto rmn = root->One(L"ReflectionMap");
	auto simn = root->One(L"SelfIlluminationMap");

	auto sh = new CShader(L"StandardMaterial");
	auto m = new CMaterial(&Engine->EngineLevel, root->Get<CString>(L"Name"));

	auto cgeo = sh->AddConstantBuffer(L"Geometry");
	auto cmtl = sh->AddConstantBuffer(L"Material");
	sh->AddConstant(cgeo, L"WVP",	L"float4x4", EPipelineStage::Vertex);
	sh->AddConstant(cgeo, L"W",		L"float4x4", EPipelineStage::Vertex);
	sh->AddVertexInput(L"Position", L"float3", L"POSITION");
	sh->AddVertexInput(L"Normal",	L"float3", L"NORMAL");
	sh->AddVertexInput(L"UV",		L"float2", L"TEXCOORD");
	sh->AddVertexOutput(L"PositionWVP", L"float4", L"SV_POSITION");
	sh->AddVertexOutput(L"NormalW",		L"float3", L"NORMAL");
	sh->VertexProgram = L"output.PositionWVP = mul(float4(input.Position, 1.0f), WVP);\n"
						L"output.NormalW	= normalize(mul(float4(input.Normal, 0.0f), W));\n";
	
	if(rmn == null)
	{
		sh->AddConstant(cmtl, L"DiffuseColor", L"float4", EPipelineStage::Pixel);

		m->Float4s[L"DiffuseColor"] = CFloat4(1);

		if(!dmn)
		{
			sh->PixelProgram = L"output.Color = DiffuseColor;";
		}
		else
		{
			sh->AddTexture(L"DiffuseTexture", L"Texture2D");
			sh->AddSampler(L"DiffuseSampler");
			sh->AddVertexOutput(L"UV", L"float2", L"TEXCOORD");
			sh->VertexProgram += L"output.UV = input.UV;\n";
			sh->PixelProgram = L"output.Color = DiffuseColor * DiffuseTexture.Sample(DiffuseSampler, input.UV);";
		}

		if(diffuse)
		{
			m->Float4s[L"DiffuseColor"] = CFloat4(diffuse->Get<CFloat3>(), 1.f);
		}
		if(alpha)
		{
			m->AlphaBlending = true;
			m->Float4s[L"DiffuseColor"] = CFloat4(diffuse->Get<CFloat3>(), alpha->AsFloat32());
		}

		if(dmn && dmn->Get<CString>(L"Type") == L"BitmapTexture")
		{
			sh->AddConstant(cmtl, L"UVTransformationMatrix", L"float4x4", EPipelineStage::Vertex);

			sh->VertexProgram += L"output.UV = mul(float4(output.UV, 0, 0), UVTransformationMatrix).xy;\n";

			m->Textures[L"DiffuseTexture"] = LoadTexture2d(dmn->One(L"File"));
			
			if(amn && (amn->Get<CString>(L"File") == dmn->Get<CString>(L"File")))
			{
				m->AlphaBlending = true;
			}

			ETextureAddressMode umode, vmode;

			CString uam = dmn->One(L"UAddrMode")->AsString();
			if(uam == L"Wrap")		umode = ETextureAddressMode::Wrap; else 
			if(uam == L"Mirror")	umode = ETextureAddressMode::Mirror; else 
			if(uam == L"Clamp")		umode = ETextureAddressMode::Clamp;

			CString vam = dmn->One(L"VAddrMode")->AsString();
			if(vam == L"Wrap")		vmode = ETextureAddressMode::Wrap; else 
			if(vam == L"Mirror")	vmode = ETextureAddressMode::Mirror; else 
			if(vam == L"Clamp")		vmode = ETextureAddressMode::Clamp;

			m->Samplers[L"DiffuseSampler"].SetAddressMode(umode, vmode);

			m->Matrixes[L"UVTransformationMatrix"] = dmn->Get<CMatrix>(L"UVTransformationMatrix");
		}
//		if(amn)
//		{
//			m->AlphaBlending = true;
//		}
	}
	else if(rmn && rmn->Get<CString>(L"Type") == L"ReflectionTexture")
	{
		//sh->Buffers[cmtl].VertexStage = true;

		auto cglo = sh->AddConstantBuffer(L"Globals");
		sh->AddConstant(cglo, L"Time",			L"float",	EPipelineStage::Vertex);

		sh->AddConstant(cgeo, L"CameraPosition",L"float3",	EPipelineStage::Vertex);

		sh->AddConstant(cmtl, L"Scale",			L"float3",	EPipelineStage::Vertex);
		sh->AddConstant(cmtl, L"WaveSpeed",		L"float",	EPipelineStage::Vertex);
		sh->AddConstant(cmtl, L"NoiseSpeed",	L"float",	EPipelineStage::Vertex);
		sh->AddConstant(cmtl, L"WaterColor",	L"float4",	EPipelineStage::Pixel);
		sh->AddConstant(cmtl, L"FadeBias",		L"float",	EPipelineStage::Pixel);
		sh->AddConstant(cmtl, L"FadeExp",		L"float",	EPipelineStage::Pixel);
		
		sh->AddSampler(L"SkyBoxSampler");
		sh->AddSampler(L"NoiseSampler");
		sh->AddTexture(L"SkyBox", L"TextureCube");
		sh->AddTexture(L"Noise", L"Texture3D");

		sh->AddVertexOutput(L"PositionWScaled",	L"float3", L"TEXCOORD");
		sh->AddVertexOutput(L"vVec",			L"float3", L"TEXCOORD");

		sh->VertexProgram +=L"	float4 posW 				= mul(float4(input.Position, 1.0f), W);\n"
							L"	output.PositionWScaled		= posW.xyz * Scale;\n"
							L"	output.vVec 				= posW.xyz - CameraPosition;\n"
							L"	output.PositionWScaled.x	+= WaveSpeed  * Time;\n"
							L"	output.PositionWScaled.z	+= NoiseSpeed * Time;\n";
		
		//sh->PixelProgram +=	L"output.Color = SkyBox.Sample(SkyBoxSampler, reflect(input.vVec, float3(0, 1, 0)));";
		sh->PixelProgram +=	L"	float3 noisy = Noise.Sample(NoiseSampler, input.PositionWScaled.xzy);\n"
							L"	float3 bump = 2 * noisy - 1;\n"
							L"	bump.xz *= 0.15;\n"
							L"	bump.y	= 0.8 * abs(bump.y) + 0.2;\n"
							L"	bump = normalize(input.NormalW + bump);\n"
							L"	float3 reflVec = reflect(input.vVec, bump);\n"
							L"	float4 refl	= SkyBox.Sample(SkyBoxSampler, reflVec.xyz);\n"
							L"	float lrp = 1 - dot(-normalize(input.vVec), bump);\n"
							L"	output.Color = lerp(WaterColor, refl, saturate(FadeBias + pow(abs(lrp), FadeExp)));\n";

		m->Textures[L"SkyBox"]	= LoadCubeTexture(rmn);
		m->Textures[L"Noise"]	= LoadVolumeTexture(rmn->One(L"FileNoise"));

		m->Float3s[L"Scale"]		= CFloat3(0.01f, 0.01f, 0.01f);
		m->Floats[L"WaveSpeed"]		= 0.1f;
		m->Floats[L"NoiseSpeed"]	= 0.4f;
		m->Float4s[L"WaterColor"]	= CFloat4(0.f, 0.f, 0.f, 1.f);
		m->Floats[L"FadeBias"]		= 0.5f;
		m->Floats[L"FadeExp"]		= 6.08f;
	}

	if(l)
	{
		sh->AddConstant(cmtl, L"LightDiffuseIntensity",	L"float",	EPipelineStage::Pixel);
		sh->AddConstant(cmtl, L"LightDirection",		L"float3",	EPipelineStage::Pixel);

		auto dl = dynamic_cast<CDirectionalLight *>(l);
		m->Floats[L"LightDiffuseIntensity"] = dl->DiffuseIntensity;
		m->Float3s[L"LightDirection"]		= dl->Direction;

		sh->PixelProgram += L"	output.Color.xyz *= LightDiffuseIntensity * dot(input.NormalW, -LightDirection);\n";
	}

	m->Bind(sh);
	sh->Free();
	return m;
}

CMaterial * CVwmImporter::LoadBlendMaterial(CXon * root, CLight * l)
{
	auto sh = new CShader();
	auto m = new CMaterial(&Engine->EngineLevel);

	auto cgeo = sh->AddConstantBuffer(L"Geometry");
	sh->AddConstant(cgeo, L"WVP",	L"float4x4", EPipelineStage::Vertex);
	sh->AddConstant(cgeo, L"W",		L"float4x4", EPipelineStage::Vertex);
	sh->AddVertexInput(L"Position", L"float3", L"POSITION");
	sh->AddVertexInput(L"Normal",	L"float3", L"NORMAL");
	sh->AddVertexInput(L"Diffuse",	L"float3", L"COLOR", L"Z8Y8X8W8_UNORM");
	sh->AddVertexInput(L"UV",		L"float2", L"TEXCOORD");

	auto cmtl = sh->AddConstantBuffer(L"Material");
	sh->AddConstant(cmtl, L"MatrixA",	L"float4x4", EPipelineStage::Vertex);
	sh->AddConstant(cmtl, L"MatrixB",	L"float4x4", EPipelineStage::Vertex);
	sh->AddTexture(L"DiffuseTextureA", L"Texture2D");
	sh->AddTexture(L"DiffuseTextureB", L"Texture2D");
	sh->AddSampler(L"DiffuseSamplerA");
	sh->AddSampler(L"DiffuseSamplerB");

	sh->AddVertexOutput(L"PositionWVP", L"float4", L"SV_POSITION");
	sh->AddVertexOutput(L"NormalW", L"float4", L"NORMAL");
	sh->AddVertexOutput(L"Diffuse", L"float4", L"COLOR");
	sh->AddVertexOutput(L"UVa", L"float2", L"TEXCOORD");
	sh->AddVertexOutput(L"UVb", L"float2", L"TEXCOORD");
	
	sh->VertexProgram =	L"output.PositionWVP = mul(float4(input.Position, 1.0f), WVP);\n"
						L"output.NormalW	= normalize(mul(float4(input.Normal, 0.0f), W));\n"
						L"output.Diffuse	= float4(input.Diffuse, 1.0f);\n"
						L"output.UVa		= mul(float4(input.UV, 0, 0), MatrixA).xy;\n"
						L"output.UVb		= mul(float4(input.UV, 0, 0), MatrixB).xy;\n";
	
	sh->PixelProgram = L"output.Color = lerp(DiffuseTextureA.Sample(DiffuseSamplerA, input.UVa), DiffuseTextureB.Sample(DiffuseSamplerB, input.UVb), input.Diffuse);\n";

	auto list = root->Many(L"Material");

	if(list.size() == 2)
	{
		auto stdmA = list.front();
		auto stdmB = list.back();
			
		if(stdmA->Get<CString>(L"Type") == L"Standard" && stdmB->Get<CString>(L"Type") == L"Standard")
		{
			m->Textures[L"DiffuseTextureA"] = LoadTexture2d(stdmA->One(L"DiffuseMap/File"));
			m->Textures[L"DiffuseTextureB"] = LoadTexture2d(stdmB->One(L"DiffuseMap/File"));
	
			m->Matrixes[L"MatrixA"] = stdmA->Get<CMatrix>(L"DiffuseMap/UVTransformationMatrix");
			m->Matrixes[L"MatrixB"] = stdmB->Get<CMatrix>(L"DiffuseMap/UVTransformationMatrix");
		}
	}

	if(l)
	{
		sh->AddConstant(cmtl, L"LightDiffuseIntensity",	L"float",	EPipelineStage::Pixel);
		sh->AddConstant(cmtl, L"LightDirection",		L"float3",	EPipelineStage::Pixel);

		auto dl = dynamic_cast<CDirectionalLight *>(l);
		m->Floats[L"LightDiffuseIntensity"] = dl->DiffuseIntensity;
		m->Float3s[L"LightDirection"]		= dl->Direction;

		sh->PixelProgram += L"	output.Color.xyz *= LightDiffuseIntensity * dot(input.NormalW, -LightDirection);\n";
	}

	m->Bind(sh);
	sh->Free();
	return m;
}

CTexture * CVwmImporter::LoadTexture2d(CXon * fileNode)
{
	CString file = fileNode->Get<CString>();

	auto i = Textures.find(file);
	if(i != Textures.end())
	{
		return i->second;
	}

	auto f = Directory->OpenReadStream(file);
	auto t = Engine->TextureFactory->CreateTexture();
	t->Load(f);
	Directory->Close(f);

	Textures[file] = t;
	return t;
}

CTexture * CVwmImporter::LoadVolumeTexture(CXon * fileNode)
{
	CString file = fileNode->Get<CString>();

	auto i = Textures.find(file);
	if(i != Textures.end())
	{
		return i->second;
	}

	auto f = Directory->OpenReadStream(file);
	auto t = Engine->TextureFactory->CreateTexture();
	t->Load(f);
	Directory->Close(f);

	Textures[file] = t;
	return t;
}

CTexture * CVwmImporter::LoadCubeTexture(CXon * n)
{
	CString file =	n->One(L"FileBK")->Get<CString>() + 
					n->One(L"FileDN")->Get<CString>() + 
					n->One(L"FileFR")->Get<CString>() + 
					n->One(L"FileLF")->Get<CString>() + 
					n->One(L"FileRT")->Get<CString>() + 
					n->One(L"FileUP")->Get<CString>();

	auto i = Textures.find(file);
	if(i != Textures.end())
	{
		return i->second;
	}

	auto zp = Directory->OpenReadStream(n->Get<CString>(L"FileBK"));
	auto yn = Directory->OpenReadStream(n->Get<CString>(L"FileDN"));
	auto zn = Directory->OpenReadStream(n->Get<CString>(L"FileFR"));
	auto xn = Directory->OpenReadStream(n->Get<CString>(L"FileLF"));
	auto xp = Directory->OpenReadStream(n->Get<CString>(L"FileRT"));
	auto yp = Directory->OpenReadStream(n->Get<CString>(L"FileUP"));

	auto t = Engine->TextureFactory->CreateTexture();
	t->LoadArray(xp, xn, yp, yn, zp, zn);
	Textures[file] = t;

	Directory->Close(xp);
	Directory->Close(xn);
	Directory->Close(yp);
	Directory->Close(yn);
	Directory->Close(zp);
	Directory->Close(zn);

	return t;
}

CMesh * CVwmImporter::ImportMesh(const CString & file)
{
	auto i = Meshes.find(file);
	if(i != Meshes.end())
	{
		return i->second;
	}

	auto mesh = Engine->CreateMesh();
	Meshes[file] = mesh;

	auto f = Directory->OpenReadStream(file);
	auto & mwx = CBonDocument(CXonBinaryReader(f));
	Directory->Close(f);
		
	auto root = &mwx;

	auto vertexes	= root->One(L"Vertexes");
	auto normals	= root->One(L"Normals");
	auto vcolors	= root->One(L"VColors");
	auto uvs		= root->One(L"UVs");
	auto indexes	= root->One(L"Indexes");
	auto bbox		= root->One(L"BBox");
	
	mesh->SetBBox(CAABB(bbox->Get<CFloat3Array>()[0], bbox->Get<CFloat3Array>()[1]));
	mesh->SetPrimitiveInfo(EPrimitiveType::TriangleList);
	
	if(vertexes)
	{
		mesh->SetVertices(UOS_MESH_ELEMENT_POSITION, vertexes->Get<CFloat3Array>());
	}
	
	if(normals)
	{
		mesh->SetVertices(UOS_MESH_ELEMENT_NORMAL, normals->Get<CFloat3Array>());
	}
	
	if(vcolors)
	{
		mesh->SetVertices(UOS_MESH_ELEMENT_VERTEXCOLOR, vcolors->Get<CInt32Array>());
	}
	
	if(uvs)
	{
		mesh->SetVertices(UOS_MESH_ELEMENT_UV, uvs->Get<CFloat2Array>());
	}
	
	if(indexes)
	{
		mesh->SetIndexes(indexes->Get<CInt32Array>());
	}
	return mesh;
}

CXonDocument * CVwmImporter::GetMetadata()
{
	return Graph;
}

