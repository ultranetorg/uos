#pragma once

namespace uos
{
	class CExpressionList
	{
		public:
			CMap<float *, std::function<float()>>		Floats;
			CMap<CFloat6 *, std::function<CFloat6()>>	Float6s;
			CMap<CSize *, std::function<CSize(bool)>>	Sizes;

			void Execute(bool apply)
			{
				for(auto & i : Floats)
					*i.first = i.second();

				for(auto & i : Float6s)
					*i.first = i.second();

				for(auto & i : Sizes)
					*i.first = i.second(apply);
			}

			bool Contains(float * f)
			{
				return Floats.Contains(f);
			}
			bool Contains(CFloat6 * f)
			{
				return Float6s.Contains(f);
			}
			bool Contains(CSize * f)
			{
				return Sizes.Contains(f);
			}

			void Clear()
			{
				Floats.clear();
				Float6s.clear();
				Sizes.clear();
			}

			bool IsEmpty()
			{
				return Floats.empty() && Float6s.empty() && Sizes.empty();
			}
	};

	struct UOS_WORLD_LINKING CLimits
	{
		CSize											Smax;
		CSize											Pmax;
//		CSize											Fmax;

		static const CLimits							Empty;
		static const CLimits							Max;

		CLimits(){}
		CLimits(float w, float h){ Smax.W = Pmax.W = w; Smax.H = Pmax.H = h; }
		CLimits(const CSize & s, const CSize & p) : Smax(s), Pmax(p){}

		CLimits(std::initializer_list<CSize> a)
		{
			auto i = a.begin();

			if(a.size() > 0)
				Smax = i[0];

			if(a.size() > 1)
				Pmax = i[1];

			//if(a.size() > 2)
			//	Fmax = i[2];
		}

		operator bool () const
		{
			return Smax && Pmax;
		}

	};

}