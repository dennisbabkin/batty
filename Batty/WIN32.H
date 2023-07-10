//
//  Batty game (partial)
//
//  Custom definitions and type declarations for the Batty game
//
//
//  Created by dennisbabkin.com
//
//  This project is a part of the blog post.
//  (The source code does not compile!)
//
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11800
//




#if !defined(AFX_WIN32_H__8D192172_F9A9_11D3_879D_8DD8074C8537__INCLUDED_)
#define AFX_WIN32_H__8D192172_F9A9_11D3_879D_8DD8074C8537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

char			MYSTAMP[]={		//MY OWN AUTHENTIFICATION STAMP
				'b'^99,'y'^98,'D'^97,'e'^96,'n'^95,'n'^94,'i'^93,'S'^92,
				'A'^91,'.'^90,'B'^89,'a'^89,'b'^88,'k'^87,'i'^86,'n'^85,
				'2'^84,'0'^83,'0'^82,'0'^81,};

char	TextFatalError[]="Fatal Error At The Batty Initialization",
		TextUnkn[]="<Unknown>",
		TextUnkn2[]="???",
		TextCantRemove[]="Batty - Can't Remove",
		TextDeleteConfirm[]="Delete Confirmation",
		TextFatal[]="Batty - Fatal Error",
		TextInitNewLevel[]="New Level has been initiated...",
		TextLeftPl[]="Left Bat",
		TextRightPl[]="Right Bat",
		TextRock[]="Round advance",
		TextCantLoad[]="Batty - Can't Load",
		BLMfile[]="Batty Level Maker.exe",
		TextBLM[]="Batty Level Maker",
		BLMlink[]="Batty Level Maker.lnk",
		AuxFile[]="datafil*";

char	CongrText[]={'B'^77,'a'^77,'t'^77,'t'^77,'y'^77,' '^77,'B'^77,'O'^77,
					'N'^77,'U'^77,'S'^77,'\n'^77,'\n'^77,'C'^77,'o'^77,'n'^77,
					'g'^77,'r'^77,'a'^77,'t'^77,'u'^77,'l'^77,'a'^77,'t'^77,
					'i'^77,'o'^77,'n'^77,'s'^77,','^77,' '^77,'t'^77,'h'^77,'e'^77,
					' '^77,'H'^77,'I'^77,' '^77,'S'^77,'c'^77,'o'^77,'r'^77,
					'e'^77,' '^77,'r'^77,'e'^77,'c'^77,'o'^77,'r'^77,'d'^77,
					' '^77,'w'^77,'a'^77,'s'^77,' '^77,'a'^77,'c'^77,'h'^77,
					'i'^77,'e'^77,'v'^77,'e'^77,'d'^77,'!'^77,' '^77,'N'^77,
					'o'^77,'w'^77,' '^77,'y'^77,'o'^77,'u'^77,' '^77,'m'^77,
					'a'^77,'y'^77,' '^77,'u'^77,'s'^77,'e'^77,' '^77,'a'^77,
					' '^77,'n'^77,'e'^77,'w'^77,' '^77,'B'^77,'a'^77,'t'^77,
					't'^77,'y'^77,' '^77,'S'^77,'u'^77,'i'^77,'t'^77,'e'^77,
					' '^77,'f'^77,'e'^77,'a'^77,'t'^77,'u'^77,'r'^77,'e'^77,
					' '^77,'c'^77,'a'^77,'l'^77,'l'^77,'e'^77,'d'^77,' '^77,
					'B'^77,'a'^77,'t'^77,'t'^77,'y'^77,' '^77,'L'^77,'e'^77,
					'v'^77,'e'^77,'l'^77,' '^77,'M'^77,'a'^77,'k'^77,'e'^77,
					'r'^77,'.'^77,' '^77,'T'^77,'h'^77,'i'^77,'s'^77,' '^77,
					'i'^77,'s'^77,' '^77,'a'^77,'n'^77,' '^77,'a'^77,'p'^77,
					'p'^77,'l'^77,'i'^77,'c'^77,'a'^77,'t'^77,'i'^77,'o'^77,
					'n'^77,' '^77,'t'^77,'h'^77,'a'^77,'t'^77,' '^77,'a'^77,
					'l'^77,'l'^77,'o'^77,'w'^77,'s'^77,' '^77,'y'^77,'o'^77,
					'u'^77,' '^77,'t'^77,'o'^77,' '^77,'c'^77,'r'^77,'e'^77,
					'a'^77,'t'^77,'e'^77,' '^77,'y'^77,'o'^77,'u'^77,'r'^77,
					' '^77,'o'^77,'w'^77,'n'^77,' '^77,'B'^77,'a'^77,'t'^77,
					't'^77,'y'^77,' '^77,'l'^77,'e'^77,'v'^77,'e'^77,'l'^77,
					's'^77,'.'^77,' '^77,'Y'^77,'o'^77,'u'^77,' '^77,'c'^77,
					'a'^77,'n'^77,' '^77,'r'^77,'u'^77,'n'^77,' '^77,'i'^77,
					't'^77,' '^77,'f'^77,'r'^77,'o'^77,'m'^77,' '^77,'t'^77,
					'h'^77,'e'^77,' '^77,'B'^77,'a'^77,'t'^77,'t'^77,'y'^77,
					' '^77,'f'^77,'o'^77,'l'^77,'d'^77,'e'^77,'r'^77,','^77,
					' '^77,'o'^77,'r'^77,' '^77,'f'^77,'r'^77,'o'^77,'m'^77,
					' '^77,'t'^77,'h'^77,'e'^77,' '^77,'D'^77,'e'^77,'s'^77,
					'k'^77,'t'^77,'o'^77,'p'^77,' '^77,'n'^77,'o'^77,'w'^77,
					'.'^77,'\n'^77,'\n'^77,'E'^77,'n'^77,'j'^77,'o'^77,'y'^77,
					' '^77,'I'^77,'t'^77,'!'^77,
					77};

int	textIDs[]={IDS_STRING7,IDS_STRING121,IDS_STRING122,
				IDS_STRING123,

				IDS_STRING125};

DWORD	PasswordMask=0x9a871c;

#define BKGMAX			128	//max number of redraw images at one time
#define MAXDYN			40	//max number of dynamic objects in a list
#define MAXSTAT			20	//max number of static objects in a list
#define ONEDYNOBJ		0x3c	//# of bytes for one dynamic object data
#define ONESTATOBJ		0x18	//# of bytes for one static object data
#define ADDSHAD			0xa		//# of pixels to add to get a shade coords
#define NUMDYN			38	//# of dynamic objects in a list
#define NUMSTAT			17	//# of static objects in a list
#define CELLVERT		16	//maximum number of CELL rows
#define WALLWIDTH		15	//width of vertical bounding wall (and height for horizontal)
#define MAXCELL			CELLVERT*15		//total # of CELLs available
#define CELLWIDTH		34	//graphic width of CELL
#define CELLHEIGHT		16	//graphic height of CELL
#define CELLPAUSE		8	//pause in CELL flashing
#define BATWIDTH		48	//width of BAT in pixels
#define BATHEIGHT		15	//height of BAT in pixels
#define BALLWIDTH		10	//width and height of BALL
#define LETTERSHEIGHT	12	//height in pixels of graphic letterset
#define AUXBALLMEM		0x40//number of bytes needed extra for every BALL's info
#define SQUEAKREACT		0xff//speed of changing of SQUEAKER
#define MINVEER			20	//minumum angle from straight flight from SQUEAKER
#define NUMSOUNDCHUNKS	64	//number of sound chunks
#define NUMPLAYCHUNKS	16	//number of sound chunks set to play at one iteration
#define CHUNKVOL		0x100//number of bytes in every sound chunk
#define SCOREFORLIFE	25000	//Score that gives extra life
#define BE_NUM			16	//number of BAT explosion symbols
#define	STALPHA			187	//starting angle of BAT explosion
#define	ENDALPHA		-8	//ending angle of BAT explosion
#define	BE_ONE_FLIGHT	64	//flight of each BAT explosion image
#define TESTCYCLES		60	//Number of cycles in beg. of game to test its speed
#define CYCTIME			3	//number of miliseconds for each CYCLE
#define TIMLIMIT		5	//MAX number of miliseconds for one CYCLE
#define BALLSLOW		0.6	//Ball speed if SLOW PRIZE caught
#define BALLINITVEL		0.7	//Ball initial velocity
#define PAUSEATSHOOT	50	//Pause at shooting SHELLs
#define ROCKETFLY		600	//Number of cycles for ROCKETS to fly with BAT
#define INITSTUCK		5	//Number of consecutive hits when BALL is stuck

#define SIZEOF(n)		(sizeof(n)/sizeof(n[0]))

#ifdef _DEBUG
#define BEEPZERO(f)		{if(!(f))MessageBeep(-1);}
#else
#define BEEPZERO(f)		f
#endif

typedef struct tagMUSICDATA{
	LPBYTE	AddrOfWaveBlock;
	DWORD	LengthOfWaveBlock;			//if ZERO => no sound in this element
}MUSICDATA, *PMUSICDATA, *LPMUSICDATA;

typedef struct tagCHUNKINFO{
	LPBYTE	lpSourcePump;
	DWORD	BytesPump;					//if ZERO or less => this place is empty
}CHUNKINFO, *PCHUNKINFO, *LPCHUNKINFO;

typedef struct tagPUMPINFO{
	DWORD		ChunkToPump;			//if -1 => nothing to pump
	CHUNKINFO	ChunksInfo[8];
}PUMPINFO, *LPPUMPINFO;

typedef struct tagPRIZ{
	LPVOID		PrizeFunc;
	DWORD		ScoreForPrize;
}PRIZ, *PPRIZ, *LPPRIZ;

typedef struct tagMEMALLOCINF{
	VOID		**lpMemAddrHoler;
	DWORD		NumBytes;
}MEMALLOCINF, *PMEMALLOCINF, *LPMEMALLOCINF;

typedef struct tagGAME{
	WORD		Size;			//Size of this structure in bytes
	BYTE		nLeftNameLen;	//Without last ZERO
	BYTE		nRightNameLen;	//Without last ZERO
	DWORD		GameType;
	DWORD		LeftLostBalls;
	DWORD		RightLostBalls;
	BYTE		LContr_Left;
	BYTE		LContr_Right;
	BYTE		LContr_Fire;
	BYTE		RContr_Left;
	BYTE		RContr_Right;
	BYTE		RContr_Fire;
	WORD		StartLevel;
	DWORD		LeftCaughtBombs;
	DWORD		RightCaughtBombs;
	DWORD		LeftTimePlayed;
	DWORD		RightTimePlayed;
	DWORD		TimeSince1Play;
	DWORD		TimeLastPlayed;
	BYTE		nGameProjLen;	//Without last ZERO
	BYTE		ScrRatio;
	BYTE		StartLives;
	BYTE		nGameNameLen;	//Without last ZERO
	DWORD		EncryptCode;
	DWORD		HiScore;
	DWORD		LeftCaughtPrizes;
	DWORD		RightCaughtPrizes;
	char		ProjSRC;
	BYTE		Reserved1;
	WORD		Reserved2;
}GAME, *PGAME, *LPGAME;

typedef struct tagBGPINF{
	LONG		FileSRC;		//Signed BYTE value
	DWORD		Mask;
	DWORD		NumberLevels;
	DWORD		nNameLen;
	DWORD		nCommentLen;
	WORD		DelPassword;
	DWORD		Reserved;
	char		Text[1];
}BGPINF, *PBGPINF, *LPBGPINF;

typedef struct tagBGP{
	char		FileName[9];	//ZERO ended file name (ex.: "ugp012"/0)
	WORD		DelPassword;	//ZERO if none
	BYTE		Flags;			//0bit=1 if Initial Levels
}BGP, *LPBGP;

typedef struct tagFINDGAME{
	LPGAME		lpLastGameReturned;
	int			LastData012etc;
	int			LastInf01_9;
	int			LastGame;
}FINDGAME, *LPFINDGAME;

typedef struct tagOLDPROC{
	HWND		hWnd;
	LONG		OldProcFunc;
}OLDPROC, *LPOLDPROC;

typedef struct tagKEYS{
	BYTE		VirtualCode;
	BYTE		ShiftStatus;
}KEYS, *LPKEYS;

typedef struct tagBATTYREG{
	int				NumberGames012;
	ULARGE_INTEGER	TimeInstalled;
}BATTYREG, *LPBATTYREG;

typedef struct tagPLAY{
	WORD		Data012etc;
	BYTE		Inf01_9;
	BYTE		Game0123;
}PLAY, *LPPLAY;

typedef struct tagINPUT{
	LPSTR		lpTitle;
	LPSTR		lpText;
	LPSTR		lpInitEdit;
	DWORD		EditStyle;
	int			LimitText;
}INPUT, *LPINPUT;

typedef struct tagENTRY{
	DWORD		Score;
	DWORD		Flags;	//0bit=1 if two names in record
}ENTRY, *LPENTRY;


#endif // !defined(AFX_WIN32_H__8D192172_F9A9_11D3_879D_8DD8074C8537__INCLUDED_)
