#ifndef __NMMANAGER_H_DE71CC54_1039_4148_9CFE_29651ACA8374__
#define __NMMANAGER_H_DE71CC54_1039_4148_9CFE_29651ACA8374__

#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif

#ifdef _UNICODE
#undef _UNICODE
#endif

#ifdef UNICODE
#undef UNICODE
#endif

#include "NMGeneral.h"
#include "NMDataObject.h"
#include "NMFunctionObject.h"

//
//	nmcogame.dll 링크 방식
//
#if defined(_NM_DYNAMIC_LOAD)
	//
	//	nmcogame.dll 런타임 로드
	//
	#define NMCONEW_DLL_NAME "nmcogame.dll"
#else
	//
	//	nmcogame.dll 정적 링크
	//
	#pragma comment( lib, "nmco_api/nmcogame" )
#endif

class CNMManager
{
	DECLARE_NMSINGLETON( CNMManager )

protected:
	BYTE*				m_pRetData;
	CRITICAL_SECTION	m_cs;

	CNMManager( void );
public:
	~CNMManager( void );

public:
	void				Init( void );
	void				Finalize( void );

	BOOL				CallNMFunc( CNMFunc* pFunc );
	void				MemFree( LPVOID pBuffer );
	BOOL				SetPatchOption( BOOL bPatch );
	BOOL				SetUseFriendModuleOption( BOOL bUseFriendModule );
	BOOL				SetVersionFileUrl( LPCTSTR bPatch );
	BOOL				SetUseNGMOption( BOOL bUseNGM );
	BOOL				SetLocale( NMLOCALEID uLocaleID, UINT32 uRegionCode );
};

namespace wapp
{
	class CDynamicFunctionBase
	{
	protected:
		HMODULE	m_hDll;
		void*	m_pProc;

		CDynamicFunctionBase( void ) : m_hDll( NULL ), m_pProc( NULL )	{}
		virtual ~CDynamicFunctionBase( void ) { this->Unload(); }

	public:
		BOOL	IsValid( void ) const	{ return ( this->m_pProc != NULL ); }
		void	Unload( void )			{ if ( this->m_hDll ) { ::FreeLibrary( this->m_hDll ); this->m_hDll = NULL; } this->m_pProc = NULL; }
		void	Load( LPCSTR szDllName, LPCSTR aszFnName )
		{
			this->Unload();
			this->m_hDll	= ::LoadLibraryA( szDllName );
			if ( this->m_hDll ) { this->m_pProc	= ( void* )::GetProcAddress( this->m_hDll, aszFnName ); if ( !this->m_pProc ) this->Unload(); }
		}

	};

	template< class T >
	class CDynamicFunctionT : public CDynamicFunctionBase
	{
	public:
		CDynamicFunctionT( void )									{}
		CDynamicFunctionT( LPCTSTR szDllName, LPCSTR aszFnName )	{ this->Load( szDllName, aszFnName ); }
		T operator *() const										{ return ( T )this->m_pProc; }
	};
};

#endif	//	#ifndef __NMMANAGER_H_DE71CC54_1039_4148_9CFE_29651ACA8374__