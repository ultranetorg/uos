#include "StdAfx.h"
#include "DirectionalLight.h"
#include "Pipeline.h"

using namespace uos;

CDirectionalLight::CDirectionalLight(CEngineLevel * e) : CLight(e)
{
}

CDirectionalLight::~CDirectionalLight()
{
}

void CDirectionalLight::SetDirection(CFloat3 & d)
{
	Direction = d;
}

void CDirectionalLight::SetDiffuseIntensity(float i)
{
	DiffuseIntensity = i;
}

void CDirectionalLight::Apply(CDirectPipeline * p)
{
///	p->SetConstant("LightDirection", Direction);
///	p->SetConstant("LightDiffuseIntensity", DiffuseIntensity);
}

void CDirectionalLight::BuildShaderCode(CShader & c)
{
///	c.AddCommon(UOS_SHADER_COMMON_NORMALW);
///
///	c.AddConstant(L"float		LightDiffuseIntensity;\n");
///	c.AddConstant(L"float3		LightDirection;\n");
///
///	c.AddPixelProgram(L"	r.Color.xyz *= LightDiffuseIntensity * dot(p.NormalW, -LightDirection);\n");
}
