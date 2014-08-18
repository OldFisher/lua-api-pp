/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_CLOSURE_HPP_INCLUDED
#define LUA_CLOSURE_HPP_INCLUDED



namespace lua {
//! @cond
	namespace _ {

		class lazyClosureUtils final{
			template<typename...> friend class lazyClosure;
		private:
			static void makeClosure(lua_State* L, CFunction fn, size_t uvnum) noexcept;
		};

		//! Lazy policy for closures.
		template<typename ... UVTypes>
		class lazyClosure final: public lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyClosure(lazyClosure<UVTypes...>&&) noexcept = default;

		private:
			lazyClosure(Context& S, CFunction fn, UVTypes&& ... upvalues) noexcept:
				Fn(fn),
				uvlazy(S, std::forward<UVTypes>(upvalues)...)
			{
			}

			void push(Context& S);
			void pushSingle(Context& S)
			{
				push(S);
			}

			void onDestroy(Context&)
			{
				uvlazy.onDestroy();
			}

			void moveout() noexcept
			{
				uvlazy.moveout();
			}

			// data
			const CFunction Fn;
			Lazy<lazySeries<UVTypes...>> uvlazy;
		};



		//! Lazy policy for chunks
		class lazyChunk final: public lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyChunk(lazyChunk&& src) noexcept:
				ChunkText(src.ChunkText)
			{
			}

		private:
			lazyChunk(Context&, const char* chunkText):
				ChunkText(chunkText)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			// data
			const char* const ChunkText;
		};



		//! Lazy policy for file chunks
		class lazyFileChunk final: public lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyFileChunk(lazyFileChunk&& src) noexcept:
				FileName(src.FileName)
			{
			}

		private:
			lazyFileChunk(Context&, const char* fileName):
				FileName(fileName)
			{
			}

			void push(Context& S);
			void pushSingle(Context& S)
			{
				push(S);
			}

			// data
			const char* const FileName;
		};

	}
//! @endcond
}


#endif // LUA_CLOSURE_HPP_INCLUDED
