#include "stdafx.h"
#include "DirectMesh.h"

using namespace uos;

void CDirectMeshBuffer::UpdateIndexes(CArray<int> & ix)
{
	assert(ix.size() * GetIndexSize() <= IbDesc.ByteWidth);

	D3D11_MAPPED_SUBRESOURCE m;
	Verify(Device->DxContext->Map(IB, 0, D3D11_MAP_WRITE_DISCARD, 0, &m));
	CopyMemory(m.pData, ix.data(), ix.size() * GetIndexSize());
	Device->DxContext->Unmap(IB, 0);
}

void CDirectMeshBuffer::UpdateVertexes(CArray<CVertexes *> & v)
{
	char * b = null;
	char * p = null;

	CArray<int> offsets;
	int stride = 0;

	for(auto i : v)
	{
		offsets.push_back(stride);
		stride += i->GetVertexSize();
	}

#ifdef _DEBUG
	for(auto i : v)
	{
		assert(i->GetCount() * (UINT)stride <= VbDesc.ByteWidth);
	}
#endif

	auto o = offsets.begin();

	D3D11_MAPPED_SUBRESOURCE m;
	Verify(Device->DxContext->Map(VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &m));

	b = (char *)m.pData;

	for(auto i : v)
	{
		p = b + *(o++);

		if(i->Type == CFloat::TypeName)
		{
			if(i->Dim == 2)
			{
				for(auto & v : *((CArray<CFloat2> *)i->Data))
				{
					((DirectX::XMFLOAT2 *)p)->x = v.x;
					((DirectX::XMFLOAT2 *)p)->y = v.y;
					p += stride;
				}
			}
			if(i->Dim == 3)
			{
				for(auto & v : *((CArray<CFloat3> *)i->Data))
				{
					*((DirectX::XMFLOAT3 *)p) = *((DirectX::XMFLOAT3 *)v);
					p += stride;
				}
			}
		}

		if(i->Type == CInt32::TypeName)
		{
			if(i->Dim == 1)
			{
				for(auto v : *((CArray<int> *)i->Data))
				{
					*((int *)p) = v;
					p += stride;
				}
			}
		}
	}

	Device->DxContext->Unmap(VB, 0);
}

CDirectMesh::CDirectMesh(CEngineLevel * l, CMesh * m) : CEngineEntity(l)
{
	Mesh = m;
}

CDirectMesh::~CDirectMesh()
{
	for(auto & i : Buffers)
	{
		delete i.second;
	}
}

void CDirectMesh::Invalidate(bool format, bool vertexes, bool indexes)
{
	if(format)
	{	
		IsFormatModified = true;

		for(auto & i : Buffers)
			i.second->IsFormatChanged = true;
	}

	if(vertexes)
		for(auto & i : Buffers)
			i.second->IsVertexesChanged = true;

	if(indexes)
		for(auto & i : Buffers)
			i.second->IsIndexesChanged = true;
}

void CDirectMesh::Draw(CDirectDevice * d)
{
	auto data = Buffers[d];
	if(data == null)
	{
		data = Buffers[d] = new CDirectMeshBuffer(d);
	}

	if(data->IsVertexesChanged || data->IsFormatChanged || data->IsIndexesChanged)
	{
		if(Mesh->IsReady())
		{
			if(IsFormatModified)
			{
				VertexSize = 0;

				for(auto & e : Mesh->Vertexes)
				{
					VertexSize += e->GetVertexSize();
				}

				IsFormatModified = false;
			}

			auto bsize = 0u;

			for(auto & e : Mesh->Vertexes)
			{
				bsize += e->GetSize();
			}

			if(data->VB == null || bsize > data->VbDesc.ByteWidth)
			{
				if(data->VB)
				{
					data->VB->Release();
				}

				data->VbDesc.Usage            = D3D11_USAGE_DYNAMIC;
				data->VbDesc.ByteWidth        = bsize;
				data->VbDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
				data->VbDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
				data->VbDesc.MiscFlags        = 0;

				Verify(data->Device->DxDevice->CreateBuffer(&data->VbDesc, null, &data->VB));
			}

			if(data->IB == null || Mesh->Indexes.size() * data->GetIndexSize() > data->IbDesc.ByteWidth)
			{
				if(data->IB != null)
				{
					data->IB->Release();
				}

				data->IbDesc.Usage            = D3D11_USAGE_DYNAMIC;
				data->IbDesc.ByteWidth        = UINT(Mesh->Indexes.size() * data->GetIndexSize());
				data->IbDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
				data->IbDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
				data->IbDesc.MiscFlags        = 0;

				Verify(data->Device->DxDevice->CreateBuffer(&data->IbDesc, null, &data->IB));
			}
			
			if(data->IsVertexesChanged || data->IsFormatChanged)
			{
				data->UpdateVertexes(Mesh->Vertexes);
			}
			if(data->IsIndexesChanged)
			{
				data->UpdateIndexes(Mesh->Indexes);
			}

			data->IsVertexesChanged = false;
			data->IsFormatChanged = false;
			data->IsIndexesChanged = false;

			data->IsBuilt = true;
		}
		else
		{
			data->IsBuilt = false;
		}
	}

	if(data->IsBuilt)
	{
		uint32_t offset = 0;
		uint32_t stride = VertexSize;
		
		d->DxContext->IASetPrimitiveTopology(ToDx(Mesh->PrimitiveType));
		d->DxContext->IASetVertexBuffers(0, 1, &data->VB, &stride, &offset);
		d->DxContext->IASetIndexBuffer(data->IB, DXGI_FORMAT_R32_UINT, 0);

		d->DxContext->DrawIndexed(UINT(Mesh->Indexes.size()), 0, 0);
	}
}

void CDirectMesh::DrawRepeat(CDirectDevice * d)
{
	d->DxContext->DrawIndexed((UINT)Mesh->Indexes.size(), 0, 0);
}
