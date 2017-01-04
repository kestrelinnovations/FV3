// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.



#if !defined(AFX_FVTHREAT_H__661E62C4_F4AD_11D1_8AFE_00104B2886FA__INCLUDED_)
#define AFX_FVTHREAT_H__661E62C4_F4AD_11D1_8AFE_00104B2886FA__INCLUDED_

// #define JTT_BUILD 1
#define USE_TCPIP 1
#define BLUE_FORCE_LABEL_LEN 25
#define SOI_LEN 5
#define QUICK_WAIT 50

typedef double DOUBLE;

// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

typedef struct
{
	//	*	Zulu time in the following order: 
	//	day of month, 
	//	24 hour Zulu time, 
	//	month, year. 
	//	(e.g. 1340Z on 10 Nov 92 is 101340111992). 
	
	CHAR			szRcvZuluTime[16];	// * note
	CHAR			szRptZuluTime[16];	// * note
	LONG			nStartLifetime;	//	Minutes until we expire it
	SHORT			nCurrLifeTime;
	USHORT		nMaxTrail;
	USHORT		nKmGroundspeed;
	USHORT		nPitch;
	USHORT		nYaw;
	USHORT		nRoll;
	FLOAT			fHeading;
	USHORT		nSizeStrength;
	FLOAT			fEllipse;
	DOUBLE		fLatitude;
	DOUBLE		fLongitude;
	LONG			nFtElevation;
	BOOL			bMSL;
	FLOAT			fEllipseAngle;
	FLOAT			fEllipseSmaj;
	FLOAT			fEllipseSmin;
	CHAR			szSOI[SOI_LEN+1];
	CHAR			szCountryCode[3];
	CHAR			szSoiLabel[BLUE_FORCE_LABEL_LEN+1];
	LONG			nTrackID;
	LONG			nMsgID;
	LONG			nDataBufPos;
	LONG			nDataLen;
	LONG			nIntelRcd;
} INTEL_TRACK;

typedef struct
{
	INT			nCmd;
	SHORT			nVersion;
	LONG			nThreatID;
	LONG			nCCode;
	LONG			nKey;
		
	CHAR			szELNOT[10];
	CHAR			szDIEQP[10];
	CHAR			szMilStdId[16];
	CHAR			szIconId[51];
	CHAR			szOverlayId[51];
	CHAR			szLabel[51];
	CHAR			szSource[11];
	USHORT		nFunctionId;

	INTEL_TRACK	track;

}	CF_FVTHREAT;

typedef struct
{
	// edit flags
	BOOL				bVALID;
	BOOL				bMODIFIED;
	BOOL				bDELETED;
	BOOL				bSELECTED;
	BOOL				bBLANK;
	BOOL				bEDITED;

	BOOL				bSHOW_THREAT;
	BOOL				bSHOW_ELLIPSES;
	BOOL				bSHOW_MASKS[2];
	BOOL				bSHOW_RINGS[2];

	LONG				nGridRow;
	LONG				nKey;
	BYTE				nLen;
	LONG				nCorrelationCode;
	char				szOfficialName[51];
	char				szNickName[51];
	char				szLabelText[51];
	char				szFunctionText[51];
	char				szInformation[256];
	char				szDIEQP_Id[10];
	char				szELNOT_Id[10];
	USHORT			nFunctionId;
	char				szMILSTD_Id[16];
	char				szIcon_Id[51];
	BOOL				bOnPalette;
	BOOL				bDisplayFilter;
	char				szOrderOfBattle[11];
	char				szSubtypeTable[21];
	LONG				nSubtypeFKey;
	FLOAT				fDetectionAntennaHt;
	FLOAT				fEngagementAntennaHt;
	double			fDetectionRange;
	double			fEngagementRange;
	char				szSource[26];
	char				szAlarm[51];
	BOOL				bDefaultAssoc;
	LONG				nExpirationMin;
} THREAT_PARAM;

#define ENGAGEMENT   0
#define DETECTION	   1
#define ENGAGEMENT_AND_DETECTION 2
#define OB_SYSTEM    1
#define OB_EQUIPMENT 0

typedef struct
{
	long		ELEVATION;
	short		COLOR;
	BOOL		MASK_VIEWABLE;
} MASKVAL;

typedef struct
{
	LONG		ID;
	BYTE		RADAR_TYPE;
	double	RADAR_LATITUDE_DEG;
	double	RADAR_LONGITUDE_DEG;
	BOOL		SHOW_MASK;
	BOOL		SHOW_RANGE_RINGS;
	double	RANGE_NMI;
	BOOL		RANGE_LIMITED;
	double	CUSTOM_RANGE_NMI;
	long		RADAR_ELEVATION;
	double	ANTENNAE_HEIGHT_FT;
	BOOL		AGL_NOT_MSL;
	MASKVAL	MASK[3];
	short		DRAW_STYLE;
	short		BRUSH_STYLE;
	long		DTED_ELEV;
	long		HIGHPT_ELEV;
} THR32_MASK;

typedef struct thr32rcd_struct
{
	//	cheap hack for a viewlist
	LPVOID	pValidOBView;
	LPVOID	pValidMapView;
	LPVOID	pValidSearchView;

	//	handy pointers for deleting etc
	LPVOID	pMapView;	
	//LPVOID	pGridView;
	LONG		nGridRow;
	LONG		nSearchRow;

	// edit flags
	BOOL		bVALID;
	BOOL		bDELETED;
	BOOL		bSELECTED;
	BOOL		bBLANK;
	
	CHAR		ELNOT[10+1];
	CHAR		DIEQP[10+1];
	CHAR		OVERLAY_ID[51];
	CHAR		ICON_NAME[51];
	CHAR		ORDER_OF_BATTLE[11];
	CHAR		COUNTRY[2+1];
	LONG		TRACK_ID;
	LONG		INTEL_RCD;
	SHORT		FUNCTION_ID;
	LONG		PARAM_KEY;

	long		KEY;
	long		ID;
	long		MSG_ID;
	long		SYSTEM_ID;

	long		CORRELATION_CODE;
	double	LATITUDE_DEG;
	double	LONGITUDE_DEG;

	CHAR		DATE_TIME[50+1];
	CHAR		OFFICIAL_NAME[50+1];
	CHAR		APPROVED_NICKNAME[50+1];
	float		ELLIPSE_ANGLE_DEG;
	float		ELLIPSE_SMAJ_NMI;
	float		ELLIPSE_SMIN_NMI;
	CHAR		INFORMATION[255+1];
	BOOL		SHOW_THREAT;
	BOOL		SHOW_ELLIPSES;
	BOOL		ENABLE_EDIT;
	CHAR		SOURCE[32+1];
	SHORT		OB_TYPE;
	CHAR		MILSTD_ID[15+1];
	CHAR		LABEL_TEXT[50+1];
	CHAR		LABEL_TEXT_LEFT[255+1];
	CHAR		LABEL_TEXT_RIGHT[255+1];

	THR32_MASK	MASK[2];
	struct thr32rcd_struct*	pBackup;

} THR32_RCD;

typedef struct
{
	THR32_RCD*	pRcd;
	CHAR			szPath[ _MAX_PATH ];
}	SEARCH32_RCD;

typedef struct
{
	CHAR		szSYSTEM_NAME[50+1];;
	LONG		nSYSTEM_ID;
	BOOL		bUSE_ENGAGEMENT;
	BOOL		bUSE_DETECTION;
	LPARAM	pEQUIPMENT;
	LONG		nEQUIPMENT_FKEY;
} THR40_SYSTEM;

typedef enum
{
	eEquipment,
	eSystem
} THREAT_MODE;

typedef CF_FVTHREAT	CF_FVINTEL;
typedef THREAT_PARAM	PRM32_RCD;

#define MIL_STD_2525_LEN		15
#define THREAT_NAME_LEN			50
#define OVERLAY_NAME_LEN		25
#define THREAT_DLL_NAME_LEN	15
#define INTELCODE_NAME_LEN		7
#define INTELTYPE_NAME_LEN		3
#define LEADING_SPACES        " "

#define ORDER_OF_BATTLE_COL "---OB---"
#define OFFICIAL_NAME_COL   "OFFICIAL_NAME"
#define DIEQP_COL           "--DIEQP--"
#define ELNOT_COL           "--ELNOT--"
#define AFFILIATION_COL     "AFFIL"
#define LOCATION_COL        "---------LOCATION---------"

#define WM_CREATE_RECORD   WM_USER + 1000
#define WM_SELCHANGING     WM_USER + 1001
#define WM_SELCHANGED      WM_USER + 1002
#define WM_GRID_MODIFIED   WM_USER + 1003
#define WM_MODIFIED		   WM_USER + 1004

#define THR_DISPLAY			0x00000001
#define THR_UNSELECT			0x00000002
#define THR_SELECT			0x00000004
#define THR_INVALIDATEALL	0x00000008
#define THR_INVALIDATE		0x00000010
#define THR_REMOVE			0x00000020
#define THR_REMOVEALL		0x00000040 
#define THR_AMPLIFY			0x00000080 
#define THR_ADD				0x00000100 
#define THR_UPDATE			0x00000200
#define THR_REWIND			0x00000400
#define THR_CENTER			0x00000800
#define THR_RECYCLETRACK	0x00001000
#define THR_DATASYNCH		0x00002000

#define THR_ENG_MASK			0x00004000 
#define THR_ENG_RINGS		0x00008000
#define THR_DET_MASK			0x00010000
#define THR_DET_RINGS		0x00020000
#define THR_THREAT			0x00040000
#define THR_SINGLESTEP		0x00080000
#define THR_EDITABLE			0x00100000
#define THR_LABEL				0x00200000

#define BET_ADVISORY			0x0001 
#define BET_SURVEILLANCE	0x0002 
#define BET_PARAMETRIC		0x0004 
#define BET_QUERY 			0x0008           
#define BET_COMPOSITE		0x0010 
#define BET_IMAGERY			0x0020 
#define BET_STANDARD			BET_ADVISORY|BET_SURVEILLANCE|BET_PARAMETRIC
#define BET_EXTENDED			BET_QUERY|BET_COMPOSITE|BET_IMAGERY
#define BET_ALL				BET_STANDARD|BET_EXTENDED
#define BET_BLUE_FORCE		0x0040
#define BET_TEST				0x0080
#define BET_EXERCISE			0x0100
#define BET_UNKNOWN			0x0200

#define MAX_RCVBUF     256000

typedef enum { eNull, eNo, eNoAll, eYes, eYesAll } UResponse;
typedef LONG	ThreatCCode; 
typedef USHORT	ThreatCommand;
typedef LONG	ThreatUID;
typedef char	MilStdId[ MIL_STD_2525_LEN + 1 ];
typedef char	ThreatName[ THREAT_NAME_LEN + 1 ];
typedef char	IntelCode[ INTELCODE_NAME_LEN + 1 ];		
typedef char	IntelType[ INTELTYPE_NAME_LEN + 1 ];
typedef char	ElnotID[ INTELCODE_NAME_LEN + 1 ];
 
typedef struct
{
	char				fileExt[ 3 + 1 ];
} THREAT_DLL;

typedef UINT	( CALLBACK * INPUTFUNC )( LPSTR, CF_FVTHREAT* );
typedef UINT	( CALLBACK * INPUTFUNC2 )( LPSTR, BOOL, BOOL, UINT, THR32_RCD* );

typedef BOOL	( * INITFUNC )( CWnd*, LPSTR, LPSTR );
typedef UINT	( * CONNECTFUNC )( BOOL, LPSTR, INPUTFUNC, SHORT );
typedef USHORT	( * CONNECTSELSFUNC ) ( CComboBox* );
typedef BOOL	( * SETTINGSFUNC ) ( SHORT, BOOL );

typedef INT		( * OUTPUTFUNC )( CF_FVTHREAT* );
typedef BOOL	( * OPTIONSFUNC )( BOOL, THREAT_DLL* );
typedef BOOL	( * EDITORFUNC )( BOOL );
typedef BOOL	( * LOADFUNC )( INPUTFUNC2, LPSTR, BOOL );
typedef BOOL	( * CLOSEFUNC )( LPSTR );
typedef BOOL	( * SAVEFUNC )( LPSTR );
typedef BOOL	( * SAVEASFUNC )( LPSTR, LPSTR );
typedef BOOL	( * PATHFUNC )( LPSTR* );

typedef BOOL	( * ADDRCDFUNC )( LPSTR, THR32_RCD* );
typedef BOOL	( * DELETERCDFUNC )( LPSTR, LONG, THR32_RCD*, BOOL );
typedef BOOL	( * MOVETOTOPFUNC ) ( LPSTR, LONG, THR32_RCD* );
typedef BOOL	( * GETRCDFUNC )( LPSTR, LONG, THR32_RCD** );
typedef BOOL	( * GETROWFUNC )( LPSTR, LONG, THR32_RCD** );
typedef LONG	( * GETROWCOUNTFUNC )( LPSTR );
typedef BOOL	( * SELRCDFUNC )( LPSTR, LONG, BOOL, BOOL );
typedef BOOL	( * ISACTIVEFUNC ) ();
typedef BOOL	( * UPDATEVIEWSFUNC ) ( BOOL, BOOL, LPSTR, BOOL );	

typedef LONG	( * GETPARAMDBFUNC )();
typedef LONG	( * LOOKUPPARAMFUNC )( LONG );

typedef BOOL	( * GETIMAGEFUNC )();
typedef BOOL	( * GETKEYFUNC )();

#define INITFUNC_NAME			"InitFunc320"
#define CONNECTFUNC_NAME		"ConnectFunc320"
#define CONNECTSELSFUNC_NAME	"ConnectSelsFunc320"
#define SETTINGSFUNC_NAME		"SettingsFunc320"
#define OUTPUTFUNC_NAME			"OutputFunc320"
#define OPTIONSFUNC_NAME		"OptionsFunc320"
#define EDITORFUNC_NAME			"EditorFunc320"
#define DRAWFUNC_NAME			"DrawFunc320"
#define SYMBOLFUNC_NAME			"SymbolFunc320"

#define LOADFUNC_NAME			"LoadFunc330"
#define CLOSEFUNC_NAME			"CloseFunc330"
#define SAVEFUNC_NAME			"SaveFunc330"
#define SAVEASFUNC_NAME			"SaveAsFunc330"
#define PATHFUNC_NAME			"PathFunc330"
#define GETRCDFUNC_NAME			"GetRcdFunc330"
#define GETROWFUNC_NAME			"GetRowFunc330"
#define GETROWCOUNTFUNC_NAME	"GetRowCountFunc330"
#define ADDRCDFUNC_NAME			"AddRcdFunc330"
#define DELETERCDFUNC_NAME		"DeleteRcdFunc330"
#define MOVETOTOPFUNC_NAME		"MoveToTopFunc330"
#define SELRCDFUNC_NAME			"SelRcdFunc330"
#define UPDATEVIEWSFUNC_NAME	"UpdateViewsFunc330"
#define ISACTIVEFUNC_NAME		"IsActiveFunc330"
#define GETPARAMDBFUNC_NAME	"GetParamDBFunc330"
#define LOOKUPPARAMFUNC_NAME	"LookupParamFunc330"

#define ICONFUNC_NAME			"IconFunc320"
#define DRAGFUNC_NAME			"DragFunc320"
#define SHOWFUNC_NAME			"ShowFunc320"
#define ISVISIBLEFUNC_NAME		"IsVisibleFunc320"
#define CDRAGFUNC_NAME			"CancelDragFunc320"
#define EDRAGFUNC_NAME			"EndDragFunc320"

extern "C"
{
	__declspec(dllexport) UINT		InitFunc320( CWnd*, LPSTR, LPSTR );
	__declspec(dllexport) UINT		ConnectFunc320( BOOL, LPSTR, INPUTFUNC, SHORT );
	__declspec(dllexport) USHORT	ConnectSelsFunc320( CComboBox* );
	__declspec(dllexport) BOOL		SettingsFunc320( SHORT, BOOL );
	__declspec(dllexport) BOOL		OptionsFunc320( BOOL, THREAT_DLL* );
	__declspec(dllexport) BOOL		OutputFunc320( CF_FVTHREAT* );

	__declspec(dllexport) LPARAM	LoadFunc330( INPUTFUNC2, LPSTR, BOOL );
	__declspec(dllexport) BOOL		SaveFunc330( LPSTR );
	__declspec(dllexport) BOOL		SaveAsFunc330( LPSTR, LPSTR );
	__declspec(dllexport) BOOL		CloseFunc330( LPSTR );
	__declspec(dllexport) BOOL		PathFunc330( LPSTR* );
	__declspec(dllexport) BOOL		GetRcdFunc330( LPSTR, LONG rcd, THR32_RCD** );
	__declspec(dllexport) BOOL		GetRowFunc330( LPSTR, LONG row, THR32_RCD** );
	__declspec(dllexport) BOOL		GetRowCountFunc330( LPSTR );
	__declspec(dllexport) BOOL		AddRcdFunc330( LPSTR, THR32_RCD* );
	__declspec(dllexport) BOOL		DeleteRcdFunc330( LPSTR, LONG rcd, THR32_RCD*, BOOL );
	__declspec(dllexport) BOOL		MoveToTopFunc330( LPSTR, LONG rcd, THR32_RCD** );
	__declspec(dllexport) BOOL		SelRcdFunc330( LPSTR, LONG, BOOL, BOOL );
	__declspec(dllexport) BOOL		IsActiveFunc330();
	__declspec(dllexport) BOOL		UpdateViewsFunc330( BOOL, BOOL, LPSTR, BOOL );
	__declspec(dllexport) LONG		GetParamDBFunc330();
	__declspec(dllexport) LONG		LookupParamFunc330( LONG nCode );

//	UINT CALLBACK InputFunc320( char* overlay_name, CF_FVTHREAT* msg );
//	UINT CALLBACK InputFunc330( char* overlay_name, BOOL bEng, BOOL bDet, UINT nMsg, THR32_RCD* );
}

#endif


