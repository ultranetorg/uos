#include "stdafx.h"
#include "ConstantBuffer.h"

using namespace uos;

CDirectConstantBuffer::CDirectConstantBuffer()
{
}

CDirectConstantBuffer::~CDirectConstantBuffer()
{
	for(auto i : DxBuffers)
		i.second->Release();
	
	if(Shader)
		Shader->Free();
}

void CDirectConstantBuffer::Bind(CShader * sh, CString const & cbuffer)
{
	Shader = sh_assign(Shader, sh);

	Slot = sh->FindConstantBuffer(cbuffer);

	VStage = Shader->UsesStage(Slot, EPipelineStage::Vertex);
	PStage = Shader->UsesStage(Slot, EPipelineStage::Pixel);

	int s = 0;

	for(auto & i : Shader->Buffers[Slot].Constants)
	{
		if(s/16 != (s + i.Size)/16 && ((s + i.Size) % 16) != 0)
			s += 16 - s % 16;
	
		s += i.Size;
	}

	if((s % 16) != 0)
		s += 16 - s % 16;
	
	Data.resize(s);
}

int CDirectConstantBuffer::FindSlot(CString const & name)
{
	int s = 0;

	for(auto & i : Shader->Buffers[Slot].Constants)
	{
		if(s/16 != (s + i.Size)/16 && ((s + i.Size) % 16) != 0)
			s += 16 - s % 16;

		if(i.Name == name)
		{
			return s;
		}

		s += i.Size;
	}

	return -1;
}

void CDirectConstantBuffer::Apply(CDirectDevice * d)
{
	auto & b = DxBuffers[d];

	if(b == null)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = (UINT)Data.size();
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		Verify(d->DxDevice->CreateBuffer(&desc, null, &b));
	}

	if(!IsChanged.Contains(d) || IsChanged(d))
	{
		D3D11_MAPPED_SUBRESOURCE m;
		Verify(d->DxContext->Map(b, 0, D3D11_MAP_WRITE_DISCARD, 0, &m));
		CopyMemory(m.pData, Data.data(), Data.size());
		d->DxContext->Unmap(b, 0);

		IsChanged[d] = false;
	}

	if(VStage)
		d->DxContext->VSSetConstantBuffers(Slot, 1, &b);
	
	if(PStage)
		d->DxContext->PSSetConstantBuffers(Slot, 1, &b);
}

void CDirectConstantBuffer::SetValue(int slot, float v)
{
	assert(slot + sizeof(float) <= Data.size());

	*((float *)(Data.data() + slot)) = v;

	for(auto & i : IsChanged)
		i.second = true;
}

void CDirectConstantBuffer::SetValue(int slot, CFloat3 & v)
{
	assert(slot + sizeof(float) * 3 <= Data.size());
	
	CopyMemory(Data.data() + slot, &v.x, sizeof(float) * 3);

	for(auto & i : IsChanged)
		i.second = true;

}

void CDirectConstantBuffer::SetValue(int slot, CFloat4 & v)
{
	assert(slot + sizeof(float) * 4 <= Data.size());

	CopyMemory(Data.data() + slot, &v.x, sizeof(float) * 4);

	for(auto & i : IsChanged)
		i.second = true;

}

void CDirectConstantBuffer::SetValue(int slot, CMatrix & v)
{
	assert(slot + sizeof(float) * 16 <= Data.size());

	CopyMemory(Data.data() + slot, &v._11, sizeof(float) * 16);

	for(auto & i : IsChanged)
		i.second = true;

}
