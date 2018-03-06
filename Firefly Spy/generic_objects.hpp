#pragma once

#include "native.hpp"
#include "addresses.hpp"

#include "TSingleton.hpp"
#include "ZList.hpp"
#include "ZMap.hpp"
#include "ZRef.hpp"
#include "ZXString.hpp"
#include "TSecType.hpp"

namespace firefly
{
	class IWzShape2D
	{
		virtual ~IWzShape2D() = 0;
	};

	class IWzVector2D : public IWzShape2D
	{
		virtual ~IWzVector2D() = 0;
	};

	class IGObj
	{
	public:
		virtual void Update() = 0;											// + 0x00
	};
	
	class IUIMsgHandler
	{
	public:
		virtual void OnKey(unsigned int wParam, unsigned int lParam) = 0;	// + 0x00
	};
	
	class CWnd : public IGObj, public IUIMsgHandler
	{
		~CWnd() = delete;

	protected:
		virtual void Update() = 0;
		virtual int OnDragDrop(int nState, void* ctx, int rx, int ry) = 0;
		
		virtual void PreCreateWnd(int l, int t, int w, int h, int z, int bScreenCoord, void* pData) = 0;
		virtual void OnCreate(void* pData) = 0;
		virtual void OnDestroy() = 0;

		virtual void OnMoveWnd(int l, int t) = 0;
		virtual void OnEndMoveWnd() = 0;

		virtual void OnChildNotify(unsigned int nId, unsigned int param1, unsigned int param2) = 0;
		virtual void OnButtonClicked(unsigned int nId) = 0;
	};

	struct SECPOINT
	{
		TSecType<long> y;
		TSecType<long> x;
	};
	
	static_assert_size(sizeof(IWzShape2D), 0x04);
	static_assert_size(sizeof(IWzVector2D), 0x04);
	static_assert_size(sizeof(IGObj), 0x04);
	static_assert_size(sizeof(IUIMsgHandler), 0x04);
	static_assert_size(sizeof(CWnd), 0x08);
	static_assert_size(sizeof(SECPOINT), 0x18);
}