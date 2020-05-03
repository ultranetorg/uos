#pragma once

namespace uos
{
	class UOS_WORLD_LINKING CMaterialPool
	{
		public:
			CList<CMaterial *>							Materials;
			CEngine *									Engine;

			CMaterial *									GetMaterial(CFloat3 & c);
			CMaterial *									GetMaterial(CFloat4 & c);
			CMaterial *									GetMaterial(const CString & name);

			CMaterialPool(CEngine * g);
			~CMaterialPool();

	};
}