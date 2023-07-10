//
//  Batty game (partial)
//
//  Assembly code (with some mixed C) for the main game play
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


extern int		rnd();

extern HDC		hDC, hDCMem;
extern LONG		dlx, dly;
extern LPWORD	lpGrxW;
extern LPDWORD	lpIm;
extern DWORD	ImNum;
extern LPDWORD	lpDyn, lpStat, lpCurDyn, lpCurStat, lpCell;
extern LPBYTE	lpGrxStart;
//extern UINT		NumStat;
extern BOOL		nodraw;
extern UINT		ScWpix, ScHpix, ScWbyte;
extern DWORD	PureColor, FillColor;
extern UINT		OneUp, TwoUp, Hi;
extern DWORD	r;
extern int		i;
extern LPWAVEHDR	lpWaveHdr;
extern LPBYTE	lpMusic;
extern DWORD	dwDataSize;
extern LPVOID	lpSin;
extern UINT		initAlpha;
extern double	initR;
extern LPVOID	lpGrx;
extern MUSICDATA	*lpMData;
extern DWORD	BatColor;
extern LPBYTE	lpSData, lpBSData;
extern DWORD	PlayChunk;
extern PUMPINFO	SPump;
extern WAVEHDR	whdr[NUMSOUNDCHUNKS];
extern LONG		lenToPlay;
extern DWORD	begPlay, endPlay;
extern DWORD	TrackSound;
extern double	arctgMUL;
extern DWORD	BATw_div1,BATw_div2,BATw_div3,BATw_div4;
extern DWORD	BALLw_div1, BALLw_div2;
extern DWORD	BATh_div1,BATh_div2,BATh_div3,BATh_div4;
extern LPVOID	lpBEinf;
extern BOOL		extraSoundOn;
extern LPWAVEHDR	lpExtraWh;
extern DWORD	Level, Round;
extern DWORD	BUCellsOutRatio;
extern DWORD	SpeedUpTime;
extern int		Dif;
extern BOOL		NewGame;
extern BGP		PlayFile;
extern PLAY		PlayStruct;
extern BOOL		BLMpresent;
extern BYTE		BLMcode;
extern BITMAPINFO	Gbmi;
extern BOOL		InfoOK;


extern BOOL		__stdcall updatebackground(UINT x, UINT y, UINT ddx, UINT ddy);
extern BOOL				  shadowdraw(UINT x, UINT y, UINT N);
extern VOID				  cellshadowdraw(UINT x, UINT y);
extern BOOL				  celldraw(UINT x, UINT y, UINT N);
extern VOID __stdcall	  drawlives(VOID);
extern VOID __stdcall	  setball(DWORD x, DWORD y, double R, DWORD alpha, UINT Ball_012, BOOL NeedForStickySet, BOOL Left_or_Right);
extern BOOL				  setRound(int level012, int round012);
extern VOID __stdcall	  SBdrawlevel(void);
extern VOID				  inGameText(LPSTR lpText);
extern LPGAME			  getGameStruct(UINT Data012etc, UINT Inf01_9, int Game0123);
extern int					getProjFile(LPGAME lpGameStruct, LPSTR lpDest);
extern BOOL					readProjFile(LPSTR lpFile, DWORD FileOffs, LPVOID lpWhere, DWORD NumBytes, int *SRCorNULL);
extern BOOL					readRegKey(UINT Data012etc, UINT Inf01_9, DWORD OffsetFromKeyBegin, LPVOID lpWhere, DWORD NumBytes);
extern BOOL					writeRegKey(UINT Data012etc, UINT Inf01_9, DWORD OffsetFromKeyBegin, LPVOID lpFrom, DWORD NumBytes, BOOL TrueForNew);
extern char*				getfilename(LPSTR File);



extern		int			xx,yy;


#include "Win32.h"

static DWORD	digits[6];
static DWORD	cellaux;
static DWORD	NumLives, NewNumLives;
static DWORD	NumLevels;
static DWORD	LivesMinus=1;
static DWORD	hitting=0;
static DWORD	direc;
static DWORD	hitcorner=0;
static DWORD	CrSpeedReached=0;
static DWORD	TotScore=0;

static DWORD	TimerIDs=0;
static BOOL		HIpraize=0;					//TRUE if HI Score beaten

static DWORD	OneUpLife=SCOREFORLIFE;
static BOOL		GameOver=FALSE;				//TRUE=>game is over
static BOOL		GameQuit=FALSE;

static BOOL		DoublePlay;					//TRUE=>DOUBLEPLAY game (two BATs)
static BOOL		DoubleBat;					//TRUE=>DOUBLEPLAY game (both BATs have the same controls)
static BOOL		TwoPlayers;					//TRUE if DoublePlay==TRUE && DoubleBat==FALSE (two players in game)
static BOOL		Encryption;					//TRUE if currently played game is encrypted (used for text messages only)
static DWORD	NumBombsL,NumBombsR;		//Number of bombs caught in this game
static DWORD	NumPrizL,NumPrizR;			//Number of caught Prizes
static DWORD	LostBallL,LostBallR;		//Number of times the Ball were lost
static ULARGE_INTEGER	CountDwnTime;		//Time the game lasted
static DWORD	LCyc,RCyc;					//Cycles each Bat played
static LPDWORD	lpCyc;						//Pointer to eather LCyc, or RCyc
static DWORD	VK_mouse_left;

static BOOL		Player;						//0-left, 1-right PLAYER
//static BOOL		ScorePlayer;
static BOOL		BatKilled=FALSE;			//TRUE=>BAT is killed
static BOOL		BatInactive=FALSE;			//TRUE=>BAT is killed or start of round
static BOOL		ClearObjects=FALSE;			//TRUE=>erase BALLs, BATs, PRIZEs
static DWORD	SecondBatExplode;
static DWORD	SecondExplosion;			//deferement until 2nd explosion
static DWORD	NumExplodes=0;
static DWORD	HitCnt=INITSTUCK;

static BOOL		RoundEnd=FALSE;				//TRUE- to end Round
static DWORD	GoNextRound;

static BOOL		PrestartDraw=FALSE;			//TRUE=>PRE-START actions
static DWORD	preVar=0;
static DWORD	prePaus1, prePaus2;
static DWORD	preX, preY, preDDX, preDDY;
static DWORD	preGlit=0;
static BOOL		NoAuto=FALSE;

static DWORD	LeftDx,RightDx;

static BOOL		Xsound=FALSE;
static BOOL		SmashOn=FALSE;
static DWORD	SmashToOff;
static BOOL		WhoMadeSmash;				//which BAT made SMASH BALLs

static BOOL		RockMode=FALSE;
static DWORD	RockCount, ScoreRockCount, RockBat, ScoreDec, SecAdd;

//lpBatA* contains:{VK-left,VK-right,VK-fire,bits 0,1,2 for a BALL 1,2,3
//					that has autotimer triggered if 1,}
static DWORD	lpBatA1[]={'Q','E','W',0x0,0},
                lpBatA2[]={VK_LEFT,VK_RIGHT,VK_CONTROL,0x0,0};
static LPVOID	lpBatAux[2]={lpBatA1, lpBatA2};

static UINT		joyID;		//-1 if no joystick
static int		joyDeltaL,joyDeltaR;
static int		joyOldX,prejoyOldX;
static LPJOYINFOEX	joyStruct;

static BOOL		AutoTim[3]={FALSE, FALSE, FALSE};
static LPDWORD	AutoInf[3];
static int		AutoBall[3];

static DWORD	ExpBat[0x4*4*2];

static BOOL		AlienKill=FALSE, AKon=FALSE;
static DWORD	AKWhichBat=0;
static DWORD	WhoShot=-1;

static BOOL		prize_on=FALSE;

static DWORD	Balls=0;	//which BALLs are in game 2bit-BALL1, 1bit-BALL2, 0bit-BALL3
static DWORD	NumBalls=1;	//Number of BALLs in game
static BOOL		SwapBall1=FALSE;
static BOOL		BallsFreez=FALSE;

static BOOL		Seen=0;
static BOOL		Paused=FALSE;
static DWORD	EscCursor=1, EscFlash=1, EscFlashFreq=40;

static DWORD	WhichSound, SoundChosen;
static DWORD	BUmove, BUleftW, BUrightW, BUfreeflight=0;
static DWORD	BUalpha;
static double	BU_R, BUfx, BUfy;
static DWORD	NoTouch=0;
static DWORD	BUcff;

static BOOL		SqNoChange;

static double	fXplus, fYplus;
static double	PSmall=(double)1/4503599627370496;
static double	NSmall=(double)-1/4503599627370496;
static LPVOID	auxEbx;
static DWORD	sav;

static double	div360_512=(double)360/512;
static BOOL		BUreCreate=FALSE;
static BOOL		BUexpl_on=FALSE;		//if TRUE => Bird/UFO explosion is active
static DWORD	BUexpCNT, BUexp_image;
static DWORD	UFOim=0;
static DWORD	wbird_ufo=-1;			//0=>BIRD; not 0=>UFO; -1=>nothing

static DWORD	bomb_on=0;
static BOOL		ShitMode=FALSE, ShitModeSwitch=FALSE;

static float	flyA,flyB;
static DWORD	flyX, flyStX, flyStY, flyRight;
static int		fly400_on=FALSE;

static DWORD	BEImInfo[]={14, 11,	9,	7};

static DWORD	WhatFlash;
static DWORD	EscWidth=1;
static DWORD	auxDraw=0, IfVisible, FlashFreq=0;
static DWORD	FlashingTime, FlashFreqOrig;
static LPSTR	OffsetText;
static DWORD	DrawLetterColor, DrawLetterShade;

static DWORD	DigWidths[10]={13,8,13,12,13,13,13,12,13,13};

static DWORD	CellColors[]={  0x0002, 0x03e0, 0x7c00, 0x7fe5,
                                0x001f,	0x0bff,	0x7c1f,	0x7eff,
                                0x01e7,	0x6318,	0x4000,	0x7fff,
                                0x0273,	0x7e08,	0x0014,	0x7c10
                                };

static WORD		PhysObjInfo[]=	//Information for objects collisions
                {//WIDTH, HEIGHT, X+, Y+
                0,		0,		0,		0,	
                8,		8,		1,		1,			//01:BALL
                46,		15,		1,		0,			//02:BAT
                38,		38,		4,		4,			//03:SQUEAKER
                12,		9,		0,		0,			//04:BIRD
                6,		11,		2,		2,			//05:BOMB
                6,		16,		1,		3,			//06:TURDBOMB
                42,		15,		0,		0,			//07:SHIT! words
                28,		17,		0,		0,			//08:UFO
                8,		27,		1,		1,			//09:PARTITION
                34,		15,		0,		0,			//10:400 words
                31,		8,		2,		2			//11:ROCKETS (flying)
                };

static char		EscText[]="  RESUME GAME\0  END ROUND\0  QUIT GAME";
static char		texRound[]="Round X",
                texLevel[]="Level XXX",
                texGameover[]="Game over";



BOOL			adddynobj(UINT object, UINT x, UINT y, DWORD direction, DWORD multifac, DWORD flags);
VOID			adddynobjN(DWORD NumberObject,UINT object, UINT x, UINT y, DWORD direction, DWORD multifac, DWORD flags);
BOOL			addstatobj(UINT x, UINT y, DWORD multifac, DWORD flags);
VOID __stdcall	setflashcell(UINT x, UINT y);
VOID __fastcall	mainjob(VOID);
                lastatmove(VOID);
                empty_job(VOID);
                ball1(VOID);
                bat1(VOID);
                squeaker(VOID);
                squeakercheck(VOID);
                overlap(VOID);
//				flashoverlap(VOID);
                oneupscore(VOID);
                twoupscore(VOID);
                hiscore(VOID);
                lives(VOID);
                hitRU(LONG xoL, LONG xoR, LONG yoU, LONG yoD, LONG xtL, LONG xtR, LONG ytU, LONG ytD);
                hitLU(LONG xoL, LONG xoR, LONG yoU, LONG yoD, LONG xtL, LONG xtR, LONG ytU, LONG ytD);
                hitLD(LONG xoL, LONG xoR, LONG yoU, LONG yoD, LONG xtL, LONG xtR, LONG ytU, LONG ytD);
                hitRD(LONG xoL, LONG xoR, LONG yoU, LONG yoD, LONG xtL, LONG xtR, LONG ytU, LONG ytD);
                goRUorU(VOID);
                goLU(VOID);
                goLDorL(VOID);
                goRDorRorD(VOID);
                hitcells(VOID);
                m_one(VOID);
                Xm_one(VOID);
                mDRu_d(VOID);
                XmDRu_d(VOID);
                mDRl_r(VOID);
                XmDRl_r(VOID);
                mDRall(VOID);
                XmDRall(VOID);
                mDR01(VOID);
                XmDR01(VOID);
                mDR03(VOID);
                XmDR03(VOID);
                mDR05(VOID);
                XmDR05(VOID);
                mDR11(VOID);
                XmDR11(VOID);
                mDR13(VOID);
                XmDR13(VOID);
                mDR07(VOID);
                XmDR07(VOID);
                mDLl_r(VOID);
                XmDLl_r(VOID);
                mDLall(VOID);
                XmDLall(VOID);
                mDL01(VOID);
                XmDL01(VOID);
                mDL02(VOID);
                XmDL02(VOID);
                mDL03(VOID);
                XmDL03(VOID);
                mURu_d(VOID);
                XmURu_d(VOID);
                mURall(VOID);
                XmURall(VOID);
                mUR04(VOID);
                XmUR04(VOID);
                mUR05(VOID);
                XmUR05(VOID);
                mULall(VOID);
                XmULall(VOID);
                mUL08(VOID);
                XmUL08(VOID);
                celltouch(VOID);
                Xcelltouch(VOID);
                ifhitball(VOID);
                ifhitallballs(void);
BOOL			soundplay(DWORD SoundNumber);
                addscore(LONG AddPoints, BOOL WhichBat);
                bathit(VOID);
LPSTR __stdcall	drawletters(LPSTR text, LONG x, LONG y, DWORD LetterColor, DWORD ShadeColor);
LONG __stdcall	findmiddle(LPSTR text, LONG WidthOfFrame);
VOID __stdcall	setflashinwords(LPSTR text, UINT FreqForFlash, UINT TimesToFlash, DWORD WordsColor, DWORD BackgndColor);
                auxPrinting(VOID);
                semiopaque(DWORD Color);
VOID __stdcall	setballtimer(int BallNumber123);
BOOL __stdcall	killalltimers(void);
                bird_ufo(VOID);
VOID __stdcall	startbird_ufo(BOOL wBU);
                hit(int xoL, int xoR, int yoU, int yoD, int xtL, int xtR, int ytU, int ytD);
VOID __stdcall	mixsound(LPBYTE lpSource, LPBYTE lpDest, DWORD NumBytes);
VOID __stdcall	putsound(LPBYTE lpSource, LPBYTE lpDest, DWORD NumBytes, DWORD def_offset);	//def_offeset=offset from begin of CHUNK (always less than CHUNKVOL)
                I(VOID);
                II(VOID);
                BU_explode(VOID);
BOOL __stdcall	setBUexplosion(DWORD x, DWORD y);
                veerball(VOID);
                veerballC(VOID);
                veerballAC(VOID);
                bomb(VOID);
VOID __stdcall	setbomb(DWORD x, DWORD y);
                fly400(VOID);
VOID __stdcall	setfly400(DWORD xMiddle, DWORD yMiddle, DWORD turds_or_400);
                batexplode_obj(VOID);
                batexplode_draw(VOID);
VOID __stdcall	setbatexplosion(DWORD WhichBat);
VOID __stdcall	killbat(DWORD WhichBat);
BOOL __stdcall	soundplayextra(DWORD SoundNumber);
VOID __stdcall	BEclearobjs(LPVOID objs);
                prestart(VOID);
                ifstartBU(VOID);
                prize(VOID);
                priz5000(VOID);
                prizLIFEPLUS(VOID);
                prizSLOW(VOID);
                prizTRIBALL(VOID);
                prizHAND(VOID);
                prizSMASH(VOID);
                prizALIENKILL(VOID);
                prizEXPAND(VOID);
                prizSHOOTER(VOID);
                prizROCKETS(VOID);
BOOL __stdcall	setprize(DWORD xMid, DWORD yMid, DWORD Prize);
                antistraight(VOID);
                makeASCIIscore(VOID);
VOID __stdcall	ballsbackfromSMASH(VOID);
                ifprizevalid(VOID);
BOOL __stdcall	unEXPANDbat(LPVOID BatAddr);
BOOL __stdcall	unSHOOTERbat(LPVOID BatAddr);
                makeEBX27(void);
                shell(VOID);
                shellexplode(VOID);
                rocketbat(VOID);
                exhaust(VOID);
                rockets(VOID);
BOOL __stdcall	setAutotimer(int Ball012, BOOL WhichBat);
BOOL __stdcall	killAutotimer(int Ball012);
                undoprizes(VOID);
                input(VOID);
BOOL __stdcall	killAllAutotimers(VOID);
BOOL __stdcall	killballtimer(int BallNumber123);
BOOL			createShortcut(LPSTR lpFileName, LPSTR lpDescr, int FolderID, LPSTR lpLinkName);



static LPVOID	mjjmps[]=
                {
                empty_job,	ball1,		bat1,		squeaker,
                bird_ufo,	BU_explode,	bomb,		fly400,
                batexplode_obj,	prize,	shell,		shellexplode,
                rocketbat,	rockets,	exhaust,	empty_job,

                };

static LPVOID	sdrjmps[]=
                {
                oneupscore,	twoupscore,	hiscore,	lives,
                batexplode_draw
                };

static LPVOID	balljmps[]=
                {
                goRDorRorD,	goLDorL,	goRUorU,	goLU
                };

static LPVOID	bhitjmps[]=
                {
                m_one,	mDRu_d,	mDRl_r,	mDRall,
                m_one,	mDRu_d,	mDLl_r,	mDLall,
                m_one,	mURu_d,	mDRl_r,	mURall,
                m_one,	mURu_d,	mDLl_r,	mULall
                };

static LPVOID	Xbhitjmps[]=
                {
                Xm_one,	XmDRu_d,	XmDRl_r,	XmDRall,
                Xm_one,	XmDRu_d,	XmDLl_r,	XmDLall,
                Xm_one,	XmURu_d,	XmDRl_r,	XmURall,
                Xm_one,	XmURu_d,	XmDLl_r,	XmULall
                };

static LPVOID	mDRalljmps[]=
                {
                mDRl_r,	mDR01,	mDRu_d,	mDR03,
                mDRl_r,	mDR05,	m_one,	mDR07,
                mDRl_r,	mDR01,	mDRu_d,	mDR11,
                mDRl_r,	mDR13,	m_one,	empty_job
                };

static LPVOID	XmDRalljmps[]=
                {
                XmDRl_r,	XmDR01,	XmDRu_d,	XmDR03,
                XmDRl_r,	XmDR05,	Xm_one,		XmDR07,
                XmDRl_r,	XmDR01,	XmDRu_d,	XmDR11,
                XmDRl_r,	XmDR13,	Xm_one,		empty_job
                };

static LPVOID	mDLalljmps[]=
                {
                mDLl_r,	mDL01,	mDL02,	mDL03,
                mDLl_r,	mDL01,	mDL02,	mDR07,
                mDLl_r,	mDR13,	mDRu_d,	mDR11,
                mDLl_r,	mDR13,	m_one,	empty_job
                };

static LPVOID	XmDLalljmps[]=
                {
                XmDLl_r,	XmDL01,	XmDL02,		XmDL03,
                XmDLl_r,	XmDL01,	XmDL02,		XmDR07,
                XmDLl_r,	XmDR13,	XmDRu_d,	XmDR11,
                XmDLl_r,	XmDR13,	Xm_one,		empty_job
                };

static LPVOID	mURalljmps[]=
                {
                mDR03,	mDR03,	mDR03,	mDR03,
                mUR04,	mUR05,	mUR04,	mDR07,
                mURu_d,	mDR11,	mURu_d,	mDR11,
                mDRl_r,	mDR13,	m_one,	empty_job
                };

static LPVOID	XmURalljmps[]=
                {
                XmDR03,		XmDR03,	XmDR03,		XmDR03,
                XmUR04,		XmUR05,	XmUR04,		XmDR07,
                XmURu_d,	XmDR11,	XmURu_d,	XmDR11,
                XmDRl_r,	XmDR13,	Xm_one,		empty_job
                };

static LPVOID	mULalljmps[]=
                {
                mDL03,	mDL03,	mDL03,	mDL03,
                mUR05,	mUR05,	mDR07,	mDR07,
                mUL08,	mUL08,	mURu_d,	mDR11,
                mDLl_r,	mDR13,	m_one,	empty_job
                };

static LPVOID	XmULalljmps[]=
                {
                XmDL03,		XmDL03,	XmDL03,		XmDL03,
                XmUR05,		XmUR05,	XmDR07,		XmDR07,
                XmUL08,		XmUL08,	XmURu_d,	XmDR11,
                XmDLl_r,	XmDR13,	Xm_one,		empty_job
                };

static	LPVOID	birdjmps[]={I, I, II, II};

static LPVOID	lpBallsAux[4]={0,0,0,0};

static DWORD	hitcorns[16]={0,0,0,0,
                              0,0,0,4,
                              0,0,0,3,
                              0,2,1,0};

static BYTE		LAngles[24];
static BYTE		RAngles[24];

static BYTE		LettersInfo[]={0,0,11,  11,0,10, 21,0,9,	//abc
                               30,0,10, 40,0,10, 50,0,10,	//def
                               60,0,9,	69,0,11, 80,0,6,	//ghi
                               86,0,10, 96,0,11, 0,12,10,	//jkl
                               10,12,13,23,12,11,34,12,9,	//mno
                               43,12,10,53,12,11,64,12,11,	//pqr
                               75,12,10,85,12,10,95,12,11,	//stu
                               0,24,11,	11,24,13,24,24,11,	//vwx
                               35,24,11,46,24,10,56,24,7,	//yz1
                               63,24,10,73,24,10,83,24,10,	//234
                               93,24,10,0,36,10, 10,36,10,	//567
                               20,36,10,30,36,10,40,36,10,	//890
                               107,0,5,	112,0,6, 106,12,12,	//'*%
                               103,24,4,107,24,4,111,24,7,	//.!"
                               50,36,5,	55,36,5, 60,36,7,	//()-
                               67,36,8, 75,36,5, 80,36,10,	//+,?
                               90,36,4, 94,36,5, 99,36,8,	//:;=
                               107,36,10					//$
                                };

static BYTE		LetterSet[]={255,40,41,47,51,38,45,36,42,43,37,45,46,44, //-
                             39,47,35,26,27,28,29,30,31,32,33,34,48, //:
                             49,47,50,47,47,47,						 //@
                             0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,	//A-Z
                             42,47,43,47,255,36,					 //`
                             0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,	//a-z
                             42,47,43,41,47};

static DWORD	digOne[6]={7,7,7, 7,7,7};
static DWORD	digTwo[6]={7,7,7, 7,7,7};
static DWORD	digHi[6]={7,7,7, 7,7,7};

static PRIZ		prizjmps[]=
                {priz5000, 5000,		prizLIFEPLUS, 540,
                 prizTRIBALL, 400,		prizSLOW, 400,
                 prizSMASH,450,			prizALIENKILL,350,
                 prizEXPAND,300,		prizHAND,350,
                 prizSHOOTER,400,		prizROCKETS,400
                    };



BOOL	addstatobj(UINT x, UINT y,
                   DWORD multifac, DWORD flags)
{
    __asm							//see 'adddynobj'
    {
        mov		ebx,[lpCurStat]

        mov		eax,[x]
        mov		[ebx+0x4],eax
        mov		eax,[y]
        mov		[ebx+0x8],eax

        mov		eax,[multifac]
        mov		[ebx+0x14],eax

        and		eax,0xff
        mov		esi,[lpGrxStart]
        movzx	ecx,word ptr [esi+eax*8+0x4]		//ddx

        movzx	edx,word ptr [esi+eax*8+0x6]		//ddy

        mov		eax,[flags]
        test	eax,0x2
        jnz		short	aso1
        add		ecx,ADDSHAD
        add		edx,ADDSHAD
aso1:
        and		eax,0xff
        ror		eax,8
        mov		[ebx],eax
        mov		[ebx+0xc],ecx
        mov		[ebx+0x10],edx

        add		ebx,ONESTATOBJ
        mov		[lpCurStat],ebx

    }
    return TRUE;
}





VOID	adddynobjN(DWORD NumberObject,UINT object, UINT x, UINT y,
                   DWORD direction, DWORD multifac, DWORD flags)
{
    lpCurDyn=(LPDWORD)(NumberObject*ONEDYNOBJ+(LPBYTE)lpDyn);

    adddynobj(object, x, y, direction, multifac, flags);

    return;
}





BOOL	adddynobj(UINT object, UINT x, UINT y, DWORD direction,
                  DWORD multifac, DWORD flags)
{
    __asm						//flags: 7-0 bits go to DWORD [EBX+0] bits 24-31
    {							//multifac: 7-0 bits is GRAPHIC OBJECT #
        mov		ebx,[lpCurDyn]	//multifac: 24-31 bits is SHADOW OBJECT #
                                //direction: 16-23 bits is PHYS OBJECT #
        mov		eax,[x]
        mov		[ebx+0x4],eax
        mov		[ebx+0x14],eax
        mov		eax,[y]
        mov		ecx,[multifac]
        mov		[ebx+0x8],eax
        mov		[ebx+0x18],eax
        
        mov		[ebx+0x38],ecx

        mov		edi,[lpGrxStart]
        and		ecx,0xff
        movzx	eax,word ptr [edi+ecx*8+0x4]		//ddx
        movzx	edx,word ptr [edi+ecx*8+0x6]		//ddy

        mov		ecx,[flags]
        test	ecx,0x2
        jnz		short	ado1
        add		eax,ADDSHAD
        add		edx,ADDSHAD
ado1:
        test	ecx,0x4					//26bit
        jz		short	ado2
        
        mov		eax,[multifac]			//Objects with special drawing
        mov		edx,eax
        and		eax,0xffff
        shr		edx,16
ado2:
        and		ecx,0xff
        ror		ecx,8
        mov		[ebx],ecx				//set 24-31bits

        mov		[ebx+0xc],eax
        mov		[ebx+0x1c],eax
        mov		[ebx+0x10],edx
        mov		[ebx+0x20],edx

        mov		eax,[object]
        or		[ebx],eax

        mov		eax,[direction]
        mov		edx,eax
        shr		edx,16
        and		eax,0xff00ffff
        and		edx,0xff
        mov		[ebx+0x24],eax

        lea		edi,[edx*8+PhysObjInfo]

        movzx	eax,word ptr [edi]				//phys WIDTH
        mov		[ebx+0x30],eax
        movzx	eax,word ptr [edi+0x2]			//phys HEIGHT
        mov		[ebx+0x34],eax

        movsx	eax,word ptr [edi+0x4]			//phys X+
        add		eax,[ebx+0x4]
        mov		[ebx+0x28],eax

        movsx	eax,word ptr [edi+0x6]			//phys Y+
        add		eax,[ebx+0x8]
        mov		[ebx+0x2c],eax

        add		ebx,ONEDYNOBJ
        mov		[lpCurDyn],ebx
    }
    return TRUE;
}






VOID __fastcall mainjob(VOID)
{
    static DWORD	BEobjs[]={0,27,32,33,34,13,14,15,16,17,18,19,0};
    static DWORD	RFobjs[]={0,13,14,15,16,17,18,19,27,29,34,0};
    static DWORD	Prizs[]={6,8,2,7,3,9,1,4,0,5, 6,3,6,7,8,2};

    if(nodraw||Seen) goto endingret;

    __asm
    {
        push	ebp

        cmp		[FlashFreq],1			    //FOR FLASHING MESSAGES
        jnz		short	mme

        mov		eax,[WhatFlash]			    //WhatFlash=...
        test	eax,eax						//0-draw message in midscreen
        jz		short	flpos				//1-draw Escape menu
        js		short	flent				//31bit=1-redraw entire screen
        dec		eax							//2+31bit-redraw all only
        jz		short	flesc
        dec		eax
        jz		short	flall
        dec		eax							//3-pre-start drawing
        jz		short	flpre

flall:
        and		[FlashFreq],0
        jmp		short	mme

flpre:
        push	[preDDY]					//FOR PRE-START MESSAGES
        push	[preDDX]
        push	[preY]
        and		[FlashFreq],0
        push	[preX]
        jmp		short	fl


flesc:
        mov		eax,LETTERSHEIGHT		//FOR ESCAPE MENU
        shl		eax,2
        push	eax						//ddy
        push	[EscWidth]				//ddx
        mov		eax,[ScHpix]
        shr		eax,2
        push	eax						//y
        mov		ebx,[ScWpix]
        shr		ebx,3
        push	ebx						//x
        jmp		short	fl

flent:
        and		eax,0x3f				//REDRAW ENTIRE SCREEN
        mov		[WhatFlash],eax
        xor		ebx,ebx
        push	[ScHpix]
        push	[ScWpix]
        push	ebx
        push	ebx
        jmp		short	fl

flpos:
        push	[ScWpix]
        push	[OffsetText]
        call	findmiddle				//(text, WidthOfFrma)

        mov		ebx,LETTERSHEIGHT
        push	ebx
        push	ecx
        mov		edi,[ScHpix]
        sub		edi,ebx
        shr		edi,1
        push	edi
        push	eax
fl:
        call	addforredraw			//(x,y,ddx,ddy)
        test	eax,eax
        jz		short	mme

        mov		edi,edx
        mov		eax,[edi]
        mov		edx,[edi+0x4]
        mov		esi,[edi+0x8]
        add		esi,eax
        mov		edi,[edi+0xc]
        add		edi,edx

        call	overlap
mme:


        mov		edi,[lpCell]			//PREPARE CELLs FOR FLASHING
        mov		ecx,CELLVERT			//or HIT THEM OFF (if 7&6bits=0)
        mov		ebx,60
        mov		esi,17
mjf1:
        mov		dl,[edi+3]
        test	dl,0x20
        jz		short	mjf20

        mov		ax,[edi+1]
        dec		ah
        mov		[edi+2],ah
        test	ah,0xf
        jnz		short	mjf20

        push	ecx
        test	byte ptr [edi],0xc0		//check for 7bit AND 6bit
        jz		short	mjhoff

        add		al,2
        or		dl,0x40					//set 30bit
        or		ah,CELLPAUSE

        cmp		al,40
        jae		short	mjfnof1

        mov		[edi+1],ax
        mov		[edi+3],dl

        push	CELLHEIGHT
        push	CELLWIDTH
        push	ebx						//ebx=y
        push	esi						//esi=x

        call	addforredraw			//(x,y,ddx,ddy)
        test	eax,eax
        jz		short	mjf1c

        push	esi
        push	edi

        mov		edi,edx
        mov		eax,[edi]
        mov		edx,[edi+0x4]
        mov		esi,eax
        add		esi,[edi+0x8]
        mov		edi,[edi+0xc]
        add		edi,edx

/*		mov		eax,esi
        mov		edx,ebx
        lea		esi,[eax+CELLWIDTH]
        lea		edi,[ebx+CELLHEIGHT]
*/
mjf1b:
        call	overlap				//overlapping with Dynamic objects

        pop		edi
        pop		esi
mjf1c:
        pop		ecx
mjf20:
        add		esi,CELLWIDTH
        cmp		esi,544-CELLWIDTH
        jae		short	mjf21
        add		edi,4
        jmp		short	mjf1



mjfnof1:								//stopped flashing
        test	edx,0x8
        jnz		short	mjfnof2			//if PRE-START glitter
        and		byte ptr [edi],0x7f
mjfnof2:
        and		dl,0xd7					//reset 30bit AND 29bit
        sub		al,2
        mov		[edi+3],dl
        mov		[edi+1],ax
        jmp		short	mjf1c



mjhoff:										//HIT OFF the CELL
        xor		ecx,ecx
        and		ah,0x10					//choose which BAT to give
        jz		short	mjhoff12
        inc		ecx
mjhoff12:
        test	dl,0x10
        mov		eax,70
        jz		short	mjhoff1
        add		eax,70
mjhoff1:
        push	ecx
        push	eax
        call	addscore				//ADD SCORES

        dec		[NumCellsOut]
        jnz		short	mjhoff2

        or		[BatKilled],TRUE
        or		[RoundEnd],TRUE
mjhoff2:
        call	ifstartBU				//if to start Bird/UFO


        cmp		[prize_on],TRUE			//if to throw a PRIZE
        jz		short	mjhpre
        mov		edx,[Dif]
        test	edx,edx
        jz		short	mjhpr0
        cmp		[bomb_on],TRUE
        jz		short	mjhpre
//		dec		edx
mjhpr0:
        mov		ecx,[edi]
        neg		edx
        and		ecx,0x7800000
        add		edx,5
        shr		ecx,23

        call	rnd
        and		eax,0xf
        cmp		edx,5
        jbe		short	mjhpr01
        mov		edx,1
mjhpr01:
        test	ecx,ecx					//no IG
        jz		short	mjhpr03
        cmp		ecx,11					//IG:No Prize
        jz		short	mjhpre
        jb		short	mjhpr10			//IG:Certain Prize
        cmp		ecx,12					//IG:!!!
        jz		short	mjhpr10
        cmp		ecx,13					//IG: 2 times more
        jnz		short	mjhpr02
        add		edx,edx
mjhpr02:
        cmp		ecx,14					//IG: 2 times less
        jnz		short	mjhpr03
        sar		edx,1
mjhpr03:
        cmp		eax,edx
        jae		short	mjhpre

mjhpr10:
        mov		ebp,ecx
        mov		edx,7					//chose which side is prize from
        dec		ebp
        cmp		[DoublePlay],TRUE
        jnz		short	mjhpr1
        mov		eax,esi
        mov		cl,CELLWIDTH
        sub		eax,17
        div		cl
        movzx	edx,al

mjhpr1:
        cmp		ebp,10					//Select Prize
        jae		short	mjhpr11

        mov		eax,ebp					//IG: Certain Prize
        cmp		ebp,9
        jz		short	mjhpr2
        call	ifprizevalid
        jc		short	mjhpre
        jmp		short	mjhpr2

mjhpr11:xor		ecx,ecx
        cmp		[Dif],2
        jb		short	mjhpr12
        mov		ecx,4
        sub		ecx,[Dif]

mjhpr12:
        push	ecx
        call	rnd						//Random Prize
        and		eax,0xf
        mov		eax,[eax*4+Prizs]

        call	ifprizevalid			//needs EDX!
        pop		ecx
        jnc		short	mjhpr2
        dec		ecx
        jnz		short	mjhpr12
        jmp		short	mjhpre

mjhpr2:
        push	eax						//prize
        mov		eax,ebx
        mov		edx,esi
        add		eax,CELLHEIGHT/2
        add		edx,CELLWIDTH/2
        push	eax						//yMid
        push	edx						//xMid

        call	setprize				//(xMid, yMid, Prize)
mjhpre:

        mov		eax,[edi]
        push	CELLHEIGHT+ADDSHAD
        and		eax,0xdfffffcf
        push	CELLWIDTH+ADDSHAD
        or		eax,0x20
        push	ebx						//ebx=y
        mov		[edi],eax
        push	esi						//esi=x

        call	addforredraw			//(x,y,ddx,ddy)

        push	esi
        push	edi

        mov		eax,esi
        mov		edx,ebx
        lea		esi,[eax+CELLWIDTH+ADDSHAD]
        lea		edi,[ebx+CELLHEIGHT+ADDSHAD]
        jmp		short	mjf1b



mjf21:
        mov		esi,17
        add		ebx,CELLHEIGHT
        add		edi,4
        dec		ecx
        jnz		short	mjf1



    }
    __asm
    {

        mov		ecx,[Focus]
        xor		ecx,TRUE
        or		ecx,[Paused]
        jnz		short	mj6


        cmp		[BUreCreate],FALSE		//check if Bird/UFO to be recreated
        jz		short	mj2

        mov		eax,[Round]
        and		eax,1
        push	eax
        call	startbird_ufo
        and		[BUreCreate],0

mj2:
        mov		eax,[lpCyc]
        mov		ebx,[lpDyn]
        mov		ecx,NUMDYN
        cmp		[Paused],TRUE
        jz		short	mj3
        inc		dword ptr [eax]

mj3:
        mov		eax,[ebx]				//JMP EAX ---> OBJECTS' ACTIONS
        test	eax,0x50000000			//30bit or 28bit
        jnz		short	mj5

        or		byte ptr [ebx+3],0x20
        and		eax,0xf

        push	offset mj4
        jmp		NEAR dword ptr [mjjmps+eax*4]

mj4:
        and		byte ptr [ebx+3],0xdf
mj5:
        add		ebx,ONEDYNOBJ
        dec		ecx
        jnz		short	mj3
mj6:


        cmp		[BatKilled],TRUE		//if BAT exploded
        jnz		short	mjco1

        cmp		[RockMode],TRUE
        jnz		short	ero

        mov		esi,[RockCount]			//ROCKETS fly
        dec		esi
        jz		short	quitrok
        mov		[RockCount],esi

        cmp		esi,ROCKETFLY-1
        jnz		short	rf1

        mov		edx,offset RFobjs
        cmp		[RockBat],0
        mov		eax,31
        jz		short	rf0
        dec		eax
rf0:
        mov		[edx],eax
        push	edx
        call	BEclearobjs				//clear other objects

        push	10
        call	soundplay				//play sound
rf1:
        cmp		esi,[ScoreRockCount]
        jge		short	rf2

        mov		eax,[ScoreDec]
        sub		esi,eax			//Pause at adding score
        add		eax,2
        mov		[ScoreDec],eax
        push	[RockBat]
        mov		[ScoreRockCount],esi
        push	70
        call	addscore		//(AddPoins, WhichBat)

        cmp		[DoublePlay],TRUE
        jnz		short	rf2
        dec		[SecAdd]
        jnz		short	rf2
        mov		[SecAdd],3		//how less scores for another BAT
        mov		esi,[RockBat]
        xor		esi,0x1
        push	esi
        push	70
        call	addscore
rf2:
        jmp		short	mjco1


quitrok:
        and		[RockMode],0			//ENDing of ROCKETS fly
        cmp		[InfoOK],TRUE
        jnz		short	mjco1
        or		[RoundEnd],TRUE
        jmp		short	mjco1


ero:
        cmp		[RoundEnd],TRUE			//END OF ROUND
        jnz		short	mjco

        mov		edx,offset buffer
        mov		ecx,13
        xor		eax,eax
ero0:
        mov		[edx+eax*4],ecx
        inc		ecx
        inc		eax
        cmp		ecx,34
        jbe		short	ero0
        and		dword ptr [edx+eax*4],0

        push	edx
        call	BEclearobjs

        xor		eax,eax
        cmp		[FlashFreq],eax
        jbe		short	ero1
        cmp		[WhatFlash],eax
        jnz		short	ero1

        mov		[FlashFreq],1
        mov		[auxDraw],eax
ero1:
        and		[BatKilled],eax
        and		[RoundEnd],eax
        mov		[GoNextRound],15		//Deaf pause time
        jmp		short	mjco1

mjco:
        dec		[SecondExplosion]		//EXPLOSION of BAT
        jnz		short	mjco01

        mov		ebx,[lpDyn]
        mov		eax,[SecondBatExplode]
        add		ebx,ONEDYNOBJ*30
        test	eax,eax
        jz		short	mjco00
        add		ebx,ONEDYNOBJ
mjco00:
        push	eax
        call	setbatexplosion			//start 2nd explosion

        and		byte ptr [ebx+0x3],0x3f
        or		byte ptr [ebx+0x3],0x50
mjco01:
        cmp		[ClearObjects],TRUE
        jnz		short	mjco1

        mov		esi,offset BEobjs
        mov		eax,30
        and		[ClearObjects],0
        cmp		[DoublePlay],TRUE
        jnz		short	mjco02

        mov		edx,[SecondBatExplode]
        xor		edx,1
        add		eax,edx
mjco02:
        mov		[esi],eax
        push	esi

        call	BEclearobjs
mjco1:



        mov		edi,[ImNum]
        mov		ebx,[lpIm]				//UPDATE BACKGROUND
        test	edi,edi
        jle		short	mjue
mju1:
        mov		eax,[ebx+0xc]			//ddy
        mov		ecx,[ebx+0x8]			//ddx
        mov		edx,[ebx+0x4]			//y
        mov		esi,[ebx]				//x

        push	eax
        push	ecx
        push	edx
        push	esi

        call	updatebackground

        add		ebx,0x10
        dec		edi
        jnz		mju1
mjue:
    




        mov		eax,[PureColor]
        mov		[FillColor],eax

        mov		edi,[lpStat]			//DRAW STATIC OBJECTS
        mov		esi,NUMSTAT
mjs1:
        test	byte ptr [edi+3],0x80	//test 31bit
        jz		short	mjs2

        mov		ebx,[edi+0x14]
        and		ebx,0xff
        push	ebx						//N
        mov		ebx,[edi+0x8]
        push	ebx						//y
        mov		eax,[edi+0x4]
        push	eax						//x

        cmp		esi,NUMSTAT-3
        ja		short	mjs1a
        call	objectdraw
        jmp		short	mjs1b
mjs1a:
        call	celldraw
mjs1b:

        test	byte ptr [edi+0x3],0x2	//test 25bit
        jnz		short	mjs2

        mov		eax,[edi+0x14]
        rol		eax,8
        and		eax,0xff
        push	eax
        add		ebx,ADDSHAD
        push	ebx						//y
        mov		eax,[edi+0x4]
        add		eax,ADDSHAD
        push	eax						//x

        call	shadowdraw
mjs2:
        add		edi,ONESTATOBJ
        dec		esi
        jnz		short	mjs1




        mov		eax,[PureColor]
        xor		eax,0x1
        mov		[FillColor],eax

        mov		edi,[lpDyn]				//DRAW first 13 DYNAMIC OBJECTS
        mov		esi,13
        call	DrawDynamicObjects

        push	edi



        mov		edi,[lpCell]			//DRAW CELLS
        mov		esi,MAXCELL
        mov		ebp,17					//x
        mov		ebx,60					//y
mjc1:
        mov		eax,[edi]
        test	eax,0x20				//5bit
        jnz		short	mjc20

        test	eax,0xc0000010
        jz		short	mjc20

        test	eax,0x40000010
        jz		short	mjc30			//if 31bit=1 OR/AND 30bit=1
//mjc2:
        and		eax,0xf
        mov		ecx,[eax*4+CellColors]
        mov		al,[edi+1]				//38 or 39
        mov		[FillColor],ecx
        push	eax
        push	ebx
        push	ebp

        call	celldraw				//(x,y,N)

        lea		eax,[ebx+ADDSHAD]
        push	eax
        lea		eax,[ebp+ADDSHAD]
        push	eax
        call	cellshadowdraw
mjc3:
        and		dword ptr [edi],0x3fffffef	//31bit, 30bit AND 4bit
mjc20:
        add		ebp,CELLWIDTH
        cmp		ebp,544-CELLWIDTH
        jae		short	mjc21

        add		edi,4
        dec		esi
        jnz		short	mjc1
        jmp		short	mjc22


mjc30:									//if either 31bit=1 OR 30bit=1
        lea		eax,[ebx+ADDSHAD]		//if 31bit=1
        push	eax
        lea		eax,[ebp+ADDSHAD]
        push	eax
        call	cellshadowdraw

        jmp		short	mjc3
/*mjc31:
        mov		eax,[edi]				//if 30bit=1
        and		eax,0xf
        mov		ecx,[eax*4+CellColors]
        mov		[FillColor],ecx
        mov		al,[edi+1]
        push	eax
        push	ebx
        push	ebp

        call	celldraw
        jmp		short	mjc3
*/

mjc21:
        mov		ebp,17
        add		ebx,CELLHEIGHT
        add		edi,4
        dec		esi
        jnz		short	mjc1
mjc22:




        mov		eax,[BatColor]
        pop		edi						//DRAW other DYNAMIC OBJECTS
        mov		[FillColor],eax
        mov		esi,NUMDYN-13
        call	DrawDynamicObjects





        cmp		[PrestartDraw],TRUE		//PRE-START ACTIONS
        jnz		short	mdpre
        call	prestart
mdpre:
        mov		esi,[auxDraw]			//PRINTING MESSAGES
        test	esi,esi
        jz		short	mdl

        call	auxPrinting
mdl:


        mov		ebx,[ImNum]				//MAIN DRAWING TO PHYSICAL SCREEN
        mov		esi,0xCC0020
        test	ebx,ebx
        jz		short	lbl2

        mov		edi,[lpIm]
        cmp		[ScrMag],1
        jz		short	lbl1

lblm1:	//2x1
        push	esi							//dwROP
        push	DIB_RGB_COLORS				//iUsage

        mov		edx,offset Gbmi
        mov		ecx,[lpGrx]
        push	edx							//BITMAPINFO
        push	ecx							//*lpBits

        mov		eax,[edi+0xc]
        mov		ebp,[edi+0x8]
        push	eax							//hs
        push	ebp							//ws
        mov		ecx,[edi+0x4]
        mov		edx,[edi]
        push	ecx							//ys
        push	edx							//xs

        shl		ecx,1
        shl		edx,1
        shl		eax,1
        shl		ebp,1

        push	eax							//hd
        push	ebp							//wd
        push	ecx							//yd
        mov		eax,[hDC]
        push	edx							//xd
        push	eax							//hdc

        call	dword ptr [StretchDIBits]	//(hdc,xd,yd,wd,hd,xs,ys,ws,hs,lpBits,BMinfo,iUsage,dwRop)
        add		edi,0x10
        dec		ebx
        jnz		short	lblm1

        jmp		short	lble

lbl1:
        push	esi				//SRCCOPY		//1X1
        mov		edx,[edi+4]		//y
        mov		ecx,[edi]		//x
        push	edx
        push	ecx

        mov		ebp,[hDCMem]
        mov		eax,[edi+0xc]	//dy
        push	ebp
        push	eax

        mov		ebp,[edi+8]		//dx
        mov		eax,[hDC]
        push	ebp
        push	edx
        push	ecx
        push	eax

        call	dword ptr [BitBlt]	//(hDC,x,y,dx,dy,hDCMem,xS,yS,SRCCOPY);

        add		edi,0x10
        dec		ebx
        jnz		short	lbl1
lble:
        mov		[ImNum],ebx			//set ImNum=0
        call	dword ptr [GdiFlush]
lbl2:


        //SWAP 1st BALL with 2nd or 3rd, if needed

        cmp		[SwapBall1],TRUE
        jnz		short	bllos5

        mov		esi,[lpDyn]
        and		[SwapBall1],0				//if BALL1 is lost
        add		esi,ONEDYNOBJ*33
        mov		edx,1
        lea		ebx,[esi-ONEDYNOBJ]

        test	byte ptr [esi+0x3],0x10
        jz		short	bllos3
        inc		edx
        add		esi,ONEDYNOBJ
        test	byte ptr [esi+0x3],0x10
        jnz		short	bllos5
bllos3:

        mov		ecx,(ONEDYNOBJ/4)-1
bllos4:
        mov		eax,[esi+ecx*4]				//change BALLs info locations
        mov		ebp,[ebx+ecx*4]
        mov		[ebx+ecx*4],eax
        mov		[esi+ecx*4],ebp
        dec		ecx
        jns		short	bllos4

        mov		eax,[lpBallsAux+edx*4]		//Xchange AuxBalls info ptrs
        mov		ebp,[lpBallsAux]
        mov		[lpBallsAux],eax
        mov		[lpBallsAux+edx*4],ebp

        //Transfer autostart bits

        mov		ecx,2
        cmp		edx,1
        jz		short	bllos41
        shl		ecx,1
bllos41:
        mov		eax,ecx
        and		eax,[lpBatA1+0xc]
        neg		eax
        sbb		eax,eax
        neg		eax

        mov		esi,ecx
        and		esi,[lpBatA2+0xc]
        neg		esi
        sbb		esi,esi
        or		ecx,1
        not		ecx
        neg		esi

        and		[lpBatA1+0xc],ecx
        and		[lpBatA2+0xc],ecx
        or		[lpBatA1+0xc],eax
        or		[lpBatA2+0xc],esi

        //Autotimer adjustments

        mov		eax,[AutoBall+edx*4]
        mov		ebp,[AutoBall]
        mov		[AutoBall],eax
        mov		[AutoBall+edx*4],ebp

        mov		eax,[AutoInf+edx*4]
        mov		ebp,[AutoInf]
        mov		[AutoInf],eax
        mov		[AutoInf+edx*4],ebp

//		push	-1
//		call	dword ptr [MessageBeep]

bllos5:


        mov		esi,[lpDyn]
        mov		ecx,NUMDYN				//CLEANING UP
mje1:
        and		byte ptr [esi+3],0x5f	//erase 31, 29 bits
        add		esi,ONEDYNOBJ
        dec		ecx
        jnz		short	mje1


        mov		ebx,[lpStat]
        mov		ecx,NUMSTAT
mje2:
        and		byte ptr [ebx+3],0x5f	//erase 31, 29 bits
        add		ebx,ONESTATOBJ
        dec		ecx
        jnz		short	mje2


        and		[pX],0
        and		[pX_],0

        cmp		[joyID],-1
        jz		short	mjj1
        mov		eax,[prejoyOldX]
        mov		[joyOldX],eax
mjj1:
        pop		ebp
    }

    if(GoNextRound)
    {
        LPSTR	lpText;

        if(--GoNextRound) return;

        ballsbackfromSMASH();
        r=0x3a;
        BEEPZERO(killalltimers());
        BEEPZERO(killAllAutotimers());

        //Check time to introduce BLM

        if(HIpraize ||
            (Round==2 && Level+1==NumLevels))
        if(!BLMpresent || (BLMpresent && (DWORD)BLMcode!=r))
        {
            if(readRegKey(0,0,0,&i,sizeof(i)))
                if(writeRegKey(0,0,4+i,&r,1,TRUE))
                {
                    BLMpresent=TRUE;
                    BLMcode=(BYTE)r;

                    if(lpText=(LPSTR)HeapAlloc(hHeap,0,
                        sizeof(CongrText)+1))
                    {
                        AuxFile[7]='0';
                        lstrcpy(lpText,getfilename(AuxFile));
                        getfilename(BLMfile);
                        if(CopyFile(lpText,buffer,FALSE))
                            DeleteFile(lpText);

                        AuxFile[7]++;
                        lstrcpy(lpText,getfilename(AuxFile));
                        getfilename("BLMhelp.hlp");
                        if(CopyFile(lpText,buffer,FALSE))
                            DeleteFile(lpText);

                        AuxFile[7]++;
                        lstrcpy(lpText,getfilename(AuxFile));
                        getfilename("contents.cnt");
                        if(CopyFile(lpText,buffer,FALSE))
                            DeleteFile(lpText);

                        //Create a short-cuts

                        createShortcut(BLMfile,TextBLM,
                            CSIDL_DESKTOPDIRECTORY,BLMlink);

                        createShortcut(BLMfile,TextBLM,
                            CSIDL_PROGRAMS,
                            "Batty Suite\\Batty Level Maker.lnk");

                        createShortcut("BLMhelp.hlp",
                            "BLM help",
                            CSIDL_PROGRAMS,
                            "Batty Suite\\Batty Level Maker Help.lnk");


                        for(i=0; i<sizeof(CongrText); i++)
                            lpText[i]=CongrText[i]^77;

                        inGameText(lpText);
                        HeapFree(hHeap,0,lpText);
                    }
                }
        }

        if(!GameQuit)	//Go to Next Round
        {
            if((++Round)>3)
            {
                BOOL	pr,rr;

                NewNumLives=NumLives;
                Level++;
                Round=0;
                pr=TRUE;

                if(Level>=NumLevels)
                {
                    char	flnam[8+1+3+1];
                    int		ln[2];
                    LPGAME	lpgm;

                    Level=0;
                    SBcomment("Batty levels passed");

                    if(NewGame)
                    {
                        lstrcpy(flnam,PlayFile.FileName);
                        lstrcat(flnam,".bgp");
                        rr=TRUE;
                    }
                    else
                    {
                        if(lpgm=getGameStruct(PlayStruct.Data012etc,
                            PlayStruct.Inf01_9,PlayStruct.Game0123))
                        {
                            getProjFile(lpgm,flnam);
                            rr=TRUE;
                            BEEPZERO(HeapFree(hHeap,0,lpgm));
                        }
                        else
                            rr=FALSE;
                    }

                    if(rr && readProjFile(flnam,3*0x4,ln,sizeof(ln),NULL))
                        if(ln[1]>0)
                            if(lpText=(LPSTR)HeapAlloc(hHeap,
                                HEAP_ZERO_MEMORY,ln[1]+1))
                            {
                                if(readProjFile(flnam,
                                    6*0x4+0x2+ln[0],lpText,ln[1],
                                    NULL))
                                {
                                    inGameText(lpText);
                                }
                                HeapFree(hHeap,0,lpText);
                            }

                    if(ln[1]<=0) pr=FALSE;
                }

                if(pr)
                    SBcomment(TextInitNewLevel);
            }

            if(!setRound(Level,Round))
                goto over;

            SBdrawlevel();
            InvalidateRect(hMainWnd, NULL, FALSE);

        }
        else			//Quit Game
        {
over:
            GameQuit=FALSE;
            DrawingStarted=FALSE;
            nodraw=TRUE;
            SetWindowText(hMainWnd, "Batty - GAME OVER");

            DestroyWindow(hMainWnd);
        }
    }

    return;			//*******************************************


    __asm
    {								//SPECIAL DRAWING
sdr1:
        xor		eax,eax
        push	offset mjd2
        mov		al,[edi+1]
        jmp		NEAR dword ptr [eax*4+sdrjmps]



DrawDynamicObjects:
mjd1:
        mov		al,[edi+3]
        test	eax,0x80			//test 31bit
        jz		short	mjd2
        test	eax,0x04			//test 26bit
        jnz		short	sdr1

        mov		ebx,[edi+0x38]
        and		ebx,0xff
        shr		eax,1				//test 24bit
        push	ebx						//N
        mov		ebx,[edi+0x18]
        mov		eax,[edi+0x14]
        push	ebx						//y
        push	eax						//x

        jnc		short	mjd1a
        call	celldraw
        jmp		short	mjd1b
mjd1a:
        call	objectdraw
mjd1b:
        test	byte ptr [edi+0x3],0x2	//test 25bit
        jnz		short	mjd2

        mov		eax,[edi+0x38]
        rol		eax,8
        and		eax,0xff
        push	eax
        add		ebx,ADDSHAD
        mov		eax,[edi+0x14]
        push	ebx						//y
        add		eax,ADDSHAD
        push	eax						//x

        call	shadowdraw
mjd2:
        add		edi,ONEDYNOBJ
        dec		esi
        jnz		short	mjd1

        ret

    }


endingret:
    return;
}







VOID __stdcall setflashcell(UINT x, UINT y)
{
    __asm
    {
        mov		eax,[x]
        imul	edx,[y],15*4
        lea		edx,[eax*4+edx]
        add		edx,[lpCell]

        mov		eax,[edx]
        test	eax,0x20000020
        jnz		short	sfc1

        mov		ecx,eax
        and		ecx,0x0000ff00
        sub		ch,14
        or		eax,0x20000000
        and		eax,0xff0000ff
        or		eax,ecx
        mov		ecx,CELLPAUSE
        shl		ecx,16
        or		eax,ecx
        mov		[edx],eax
sfc1:
    }
    return;
}







__declspec ( naked ) hiscore(void)
{
    __asm
    {
        mov		eax,offset digHi
        jmp		(offset oneupscore + 5)
    }
}




__declspec ( naked ) twoupscore(void)
{
    __asm
    {
        mov		eax,offset digTwo
        jmp		(offset oneupscore + 5)
    }
}




__declspec ( naked ) oneupscore(void)
{
    __asm
    {
        mov		eax,offset digOne		//DO NOT CHANGE THIS LINE
//ous0:									//Here is where other JMPs go!!!!!
        mov		ebx,[PureColor]

        push	esi
        mov		[FillColor],ebx
        mov		esi,eax

        mov		ebx,6
        mov		edx,[edi+0x14]

        mov		ecx,6
        call	coundigwid			//EAX=width of all 6 digits
        mov		ecx,78
        sub		ecx,eax
        jbe		short	ous12
        shr		ecx,1
        add		edx,ecx
ous12:
ous20:
        mov		eax,[esi]
        push	edx
        push	eax

        push	eax
        mov		eax,[edi+0x18]
        push	eax
        push	edx

        call	celldraw			//(x,y,N)

        pop		eax
        pop		edx
        sub		eax,7
        add		edx,[eax*4+DigWidths]

        add		esi,4
        dec		ebx
        jnz		short	ous20

        xor		[FillColor],0x1
        pop		esi
        ret



coundigwid:
        push	edx
        push	edi
        xor		edx,edx
        xor		eax,eax
cdw1:
        mov		edi,[esi+edx*4]
        sub		edi,7
        inc		edx
        add		eax,[edi*4+DigWidths]
        dec		ecx
        jnz		short	cdw1

        pop		edi
        pop		edx

        ret
    }
}




VOID __stdcall BEclearobjs(LPVOID objs)
{									//clears objects from objs[]

    __asm
    {
        mov		ecx,[objs]
beco0:
        mov		edx,[ecx]
        mov		ebx,[lpDyn]
        test	edx,edx
        jz		short	beco3

        imul	edx,ONEDYNOBJ
        add		ebx,edx

        test	byte ptr [ebx+0x3],0x10
        jnz		short	beco1
        call	lastatmove
beco1:
        and		byte ptr [ebx+0x3],0x3f
        or		byte ptr [ebx+0x3],0x50

        add		ecx,0x4
        jmp		short	beco0
beco3:
    }

    return;
}





VOID __stdcall killbat(DWORD WhichBat)			//-1=single BAT
{												//0=left BAT
                                                //1=right BAT

#define BE_PAUSE	25			//PAUSE after 1st explosion

    if(WhichBat==0)
    {
        SecondBatExplode=1;
        SecondExplosion=BE_PAUSE;
    }
    else
    {
        if(WhichBat==-1)
        {
            SecondExplosion=0;
        }
        else
            {
                SecondBatExplode=0;
                SecondExplosion=BE_PAUSE;
            }
    }

    drawlives();
    ballsbackfromSMASH();

    if((signed)(NumLives-=LivesMinus)<0)
    {
        NumLives=0;
        GameOver=TRUE;
    }
    killalltimers();
    killAllAutotimers();

    if(GameOver)
        lstrcpy(buffer, texGameover);
    else
        lstrcpy(buffer, "Round lost...");

    if(!LivesMinus)
    {
        lstrcat(buffer, " - Infinite rounds mode is on.");
    }
    SBcomment(buffer);

    setbatexplosion(WhichBat);

    BatInactive=TRUE;
    BatKilled=TRUE;
    ClearObjects=TRUE;

    if(FlashFreq>0 && WhatFlash==0)
    {
        FlashFreq=1;
        auxDraw=0;
    }
    if(DoublePlay) Player^=1;
    return;
}





VOID __stdcall	setbatexplosion(DWORD WhichBat)	//-1=single BAT
{												//0=left BAT
    DWORD	xCenter, yCenter;					//1=right BAT
    LPDWORD	pnt, pntI;
    DWORD	obj, n;


    if((WhichBat==-1)||(WhichBat==0))
    {
        xCenter=*(LPDWORD)&lpDyn[(30*ONEDYNOBJ+0x30)/0x4]/2+
                    *(LPDWORD)&lpDyn[(30*ONEDYNOBJ+0x28)/0x4];

        yCenter=*(LPDWORD)&lpDyn[(30*ONEDYNOBJ+0x34)/0x4]/2+
                    *(LPDWORD)&lpDyn[(30*ONEDYNOBJ+0x2c)/0x4];

        pnt=(LPDWORD)lpBEinf;
        n=(*(LPDWORD)&lpDyn[(30*ONEDYNOBJ+0x38)/0x4]&0x1c0000)?12:8;
        obj=35;
    }
    else
    {
        xCenter=*(LPDWORD)&lpDyn[(31*ONEDYNOBJ+0x30)/0x4]/2+
                    *(LPDWORD)&lpDyn[(31*ONEDYNOBJ+0x28)/0x4];

        yCenter=*(LPDWORD)&lpDyn[(31*ONEDYNOBJ+0x34)/0x4]/2+
                    *(LPDWORD)&lpDyn[(31*ONEDYNOBJ+0x2c)/0x4];

        pnt=(LPDWORD)lpBEinf+(0x14+BE_NUM*0x24)/0x4;
        n=(*(LPDWORD)&lpDyn[(31*ONEDYNOBJ+0x38)/0x4]&0x1c0000)?12:8;
        obj=36;
    }

    xCenter-=BEImInfo[0]/2;

    pnt[0]=80;
    pnt[1]=BE_ONE_FLIGHT;
    pnt[2]=n;				//starting N value
    pnt[3]=xCenter;
    pnt[4]=yCenter;
    pntI=&pnt[5];

    for(int a=0; a<BE_NUM; a++)
    {
        pntI[0]=0x08;
        pntI[1]=xCenter;
        pntI[2]=yCenter;
        pntI[3]=2;
        pntI[4]=2;

        pntI+=9;
    }

    adddynobjN(obj, 0x0408, 0, 0,
                (DWORD)((WORD)((DWORD)pnt-(DWORD)lpBEinf)),
                0x000b000b, 0x0c);

    NumExplodes++;
    soundplay(5);

    return;
}





__declspec ( naked ) batexplode_obj(void)
{
#define	ALPHAPLUS	-((ENDALPHA-STALPHA)/(BE_NUM-1))

    __asm
    {
        push	ecx						//set up BAT explosion redraw
        push	ebx

        movzx	eax,word ptr [ebx+0x24]
        mov		edi,[lpBEinf]
        mov		edx,STALPHA
        lea		esi,[eax+edi]				//ESI=common info pointer
        mov		ecx,BE_NUM
        lea		ebx,[esi+0x14]
beo1:
        mov		eax,[lpSin]
        fild	dword ptr [esi+0x8]
        fld		st(0)
        fmul	dword ptr [eax+edx*4]
        fchs
        fistp	dword ptr [ebx+0x18]		//Ycoord=N*sin(EDX)
        fmul	dword ptr [eax+edx*4+90*4]
        fistp	dword ptr [ebx+0x14]		//Xcoord=N*cos(EDX)

        mov		eax,[esi+0xc]
        mov		ebp,[esi+0x10]
        add		eax,[ebx+0x14]
        jns		short	beo2
        neg		eax
beo2:
        add		[ebx+0x18],ebp

        mov		edi,[esi]
        sub		edx,ALPHAPLUS
        jnc		short	beo21
        add		edx,360
beo21:
        lea		ebp,[edi*4+BEImInfo-80*4]
        push	edx

        mov		edi,[ebp]
        mov		edx,ADDSHAD
        add		edx,edi
        mov		[ebx+0x1c],edx
        mov		[ebx+0x20],edx

        add		edi,eax
        sub		edi,[ScWpix]
        jb		short	beo3
        mov		eax,[ScWpix]
        sub		eax,edi
        sub		eax,[ebp]
beo3:
        mov		[ebx+0x14],eax

        call	lastatmove
        pop		edx
        add		ebx,0x24
        dec		ecx
        jnz		short	beo1


        pop		ebx

        mov		eax,[esi+0x4]			//change drawing image
        or		byte ptr [ebx+0x3],0x80
        mov		edx,[esi]
        dec		eax
        jnz		short	beo5
        inc		edx
        mov		eax,BE_ONE_FLIGHT
        cmp		edx,84
        jb		short	beo4

        or		byte ptr [ebx+0x3],0x50		//take explosion away
        dec		[NumExplodes]
        jnz		short	beo4a
        or		[PrestartDraw],TRUE
beo4a:
        and		byte ptr [ebx+0x3],0x7f
beo4:
        mov		[esi],edx
beo5:
        mov		[esi+0x4],eax

        inc		dword ptr [esi+0x8]

        pop		ecx
        ret
    }
}





__declspec ( naked ) batexplode_draw(void)
{
    static	DWORD	image;

    __asm
    {
        push	edi
        push	esi

        movzx	eax,word ptr [edi+0x24]
        mov		ecx,[lpBEinf]
        mov		esi,BE_NUM
        lea		ebx,[eax+ecx+0x14]
        mov		edi,[ebx-0x14]
        mov		[image],edi
bed1:
        mov		eax,[image]
        mov		edi,[ebx+0x14]
        push	eax					//N
        mov		edx,[ebx+0x18]
        push	eax					//N
        push	edx					//Y
        push	edi					//X

        call	celldraw			//(x,y,N)

        mov		eax,[ebx+0x18]
        add		eax,ADDSHAD
        add		edi,ADDSHAD
        push	eax					//Y
        push	edi					//X

        call	shadowdraw			//(x,y,N)

        add		ebx,0x24
        dec		esi
        jnz		short	bed1

        pop		esi
        pop		edi
        ret
    }
}





__declspec ( naked ) lives(void)
{
    __asm
    {
        mov		eax,[PureColor]

        mov		ebx,[NumLives]
        mov		[FillColor],eax
        test	ebx,ebx
        jz		short	lvs2
        mov		eax,21
        cmp		ebx,eax
        jbe		short	lvs0
        xchg	ebx,eax
lvs0:
        mov		edx,[edi+0x14]
lvs1:
        push	edx

        mov		eax,[edi+0x18]
        push	23
        push	eax
        push	edx

        call	celldraw			//(x,y,N)

        pop		edx
        add		edx,24

        dec		ebx
        jnz		short	lvs1
lvs2:
        mov		eax,[PureColor]
        xor		eax,0x1
        mov		[FillColor],eax

        ret
    }
}







__declspec ( naked ) makeEBX27(void)	//sets speckle in DOUBLE PLAY
{										//EBX=Info addr of BAT
    __asm
    {
        push	esi
        cmp		[DoublePlay],TRUE
        jnz		short	meb3

        test	byte ptr [ebx+0x38+2],0x1
        jnz		short	meb1
        lea		esi,[ebx+ONEDYNOBJ]
        jmp		short	meb2
meb1:
        lea		esi,[ebx-ONEDYNOBJ]
meb2:
        mov		ecx,[ebx+0x38]
        xor		ecx,[esi+0x38]
        and		ecx,0x480000
        jnz		short	meb3

        mov		cl,[esi+0x27]
        pop		esi
        mov		[ebx+0x27],cl
        ret

meb3:
        mov		byte ptr [ebx+0x27],4
        pop		esi
        ret
    }
}






__declspec ( naked ) rocketbat(void)
{
    __asm
    {
        mov		esi,[lpDyn]
        add		esi,32*ONEDYNOBJ
        mov		eax,[esi+0x2c]
        mov		[ebx+0x2c],eax
        mov		[ebx+0x18],eax

        call	lastatmove
        ret
    }
}




__declspec ( naked ) exhaust(void)
{
    static DWORD	exhInf[]={
                                2,17,29+ADDSHAD,19+ADDSHAD,
                                0,19,30+ADDSHAD,16+ADDSHAD,
                               -1,17,32+ADDSHAD,20+ADDSHAD
                            };
    __asm
    {
        dec		byte ptr [ebx+0x24]
        jnz		short	exhe

        mov		edx,[ebx+0x38]
        mov		esi,edx
        and		esi,0xff
        test	edx,0x100
        jnz		short	exh1

        add		edx,0x01000001
        cmp		esi,125
        jb		short	exh3
        mov		edx,0x7c00017c
        jmp		short	exh3
exh1:
        sub		edx,0x01000001
        cmp		esi,123
        ja		short	exh3
        mov		edx,0x7c00007c
exh3:
        mov		[ebx+0x38],edx

        mov		eax,[ebx-ONEDYNOBJ+0x2c]
        mov		esi,edx
        mov		ebp,[ebx-ONEDYNOBJ+0x28]
        and		edx,0xff
        mov		byte ptr [ebx+0x24],10		//Pause in exhaust
        sub		edx,123
        shl		edx,4
        add		edx,offset exhInf

        add		ebp,[edx]
        add		eax,[edx+0x4]

        mov		[ebx+0x14],ebp
        mov		[ebx+0x18],eax

        mov		ebp,[edx+0x8]
        mov		eax,[edx+0xc]
        mov		[ebx+0x1c],ebp
        mov		[ebx+0x20],eax


        call	lastatmove

exhe:
        ret
    }
}




__declspec ( naked ) rockets(void)
{
    __asm
    {
        dec		byte ptr [ebx+0x24]
        jnz		short	roce

        movzx	eax,byte ptr [ebx+0x25]
        mov		edx,[ebx+0x38]
        cmp		eax,1
        jle		short	roc1
        dec		eax
        mov		[ebx+0x25],al
        jmp		short	roc2
roc1:
        inc		word ptr [ebx+0x38+1]

roc2:
        shr		edx,8
        mov		byte ptr [ebx+0x24],al
        and		edx,0xffff
        shr		edx,6
        mov		eax,[ebx+0x18]
        mov		ebp,[ebx+0x2c]
        sub		eax,edx
        sub		ebp,edx
        mov		[ebx+0x18],eax
        mov		[ebx+0x2c],ebp

        call	lastatmove

        cmp		dword ptr [ebx+0x2c],-60
        jge		short	roce

        or		byte ptr [ebx+3],0x50			//TAKE ROCKETS AWAY
        or		byte ptr [ebx+ONEDYNOBJ+3],0x50
        or		byte ptr [ebx-ONEDYNOBJ+3],0x50
        or		byte ptr [ebx-ONEDYNOBJ*2+3],0x50
roce:
        ret
    }
}






__declspec ( naked ) bat1(void)			//MAIN BAT
{
    static	int	dir;
    static  WORD	BatSpec[32];
    static	BOOL	AutoStartTrig;
    static	LPVOID	BatAux;

    __asm
    {		//0bit of byte [EBX+0x38+2]=0 => LEFT BAT 
            //1bit of byte [EBX+0x38+2]=1 => BAT is STICKY
            //2bit of byte [EBX+0x38+2]=1 => make BAT expand
            //3bit of byte [EBX+0x38+2]=1 => BAT is expanded
            //4bit of byte [EBX+0x38+2]=1 => return BAT from expanded
            //5bit of byte [EBX+0x38+2]=1 => make SHOOTER BAT
            //6bit of byte [EBX+0x38+2]=1 => BAT is SHOOTER
            //7bit of byte [EBX+0x38+2]=1 => return from SHOOTER BAT

        mov		al,[ebx+0x38+2]
        xor		esi,esi
        and		eax,0x1
        push	ecx
        mov		ebp,[eax*4+lpBatAux]

        push	0
        call	input
        shr		eax,15
        push	1
        and		eax,0x1
        or		esi,eax
        call	input
        shr		eax,14
        and		eax,0x2
        or		esi,eax
        mov		[BatAux],ebp


        mov		eax,[ebp+0xc]			//check for autostart
        and		eax,0x7
        mov		[dir],esi
        mov		[AutoStartTrig],eax
        jnz		short	runball

        push	2
        call	input					//FIRE
        and		eax,0x8000
        jz		short	bff

runball:
                cmp		[BatInactive],TRUE
                jz		short	bf3
                mov		esi,[lpDyn]
                mov		ebp,2
                xor		edi,edi
bf1:
                test	byte ptr [esi+34*ONEDYNOBJ+3],0x10
                jnz		short	bf2
                test	byte ptr [esi+34*ONEDYNOBJ+0x24+3],0x80
                jz		short	bf2

                mov		edx,[ebx+0x38]
                shr		edx,(16-1)
                mov		ecx,[ebp*4+lpBallsAux]
                xor		edx,[ecx+0x3c]
                and		edx,0x2
                jnz		short	bf2

                mov		ecx,ebp
                mov		eax,1
                mov		edx,[AutoStartTrig]
                shl		eax,cl
                test	edx,0x7
                jz		short	bf11
                and		eax,edx
                jz		short	bf2
bf11:
                not		eax
                push	ecx
                mov		edx,[BatAux]
                inc		ecx
                and		[edx+0xc],eax
                push	ecx
                call	setballtimer	//(BallNumber123)
                call	killAutotimer	//(Ball012)

                and		byte ptr [esi+34*ONEDYNOBJ+0x24+3],0x7f
                inc		edi
bf2:
                sub		esi,ONEDYNOBJ
                dec		ebp
                jns		short	bf1




                test	[AutoStartTrig],0x7
                jnz		short	bshue

                test	byte ptr [ebx+0x38+2],0x40	//SHOOTER's FIRE
                jz		short	bshue

                cmp		byte ptr [ebx+0x24+2],0
                jnz		short	bshue

                mov		esi,[lpDyn]
                mov		byte ptr [ebx+0x24+2],PAUSEATSHOOT
                add		esi,14*ONEDYNOBJ
                mov		ecx,6
bshu1:
                test	byte ptr [esi+3],0x10
                jz		short	bshu2

                cmp		[DoublePlay],TRUE	//make one shot at a time
                jnz		short	bshu11
                mov		ebp,[lpDyn]
                mov		edx,[ebx+0x38]
                mov		eax,[ebp+30*ONEDYNOBJ+0x38]
                and		edx,0x10000
                and		eax,[ebp+31*ONEDYNOBJ+0x38]
                and		eax,0x400000
                jz		short	bshu11
                cmp		edx,[WhoShot]
                jz		short	bshue
                mov		[WhoShot],edx
bshu11:
                mov		eax,[ebx+0x38]
                mov		dword ptr [esi],0x8100000a
                shr		eax,16
                mov		dword ptr [esi+0x38],0x75000075
                mov		dword ptr [esi+0x24],eax

                mov		eax,2
                mov		edx,5
                mov		[esi+0x30],eax
                mov		[esi+0x34],edx
                add		eax,4+ADDSHAD
                add		edx,8+ADDSHAD
                mov		[esi+0xc],eax
                mov		[esi+0x10],edx
                mov		[esi+0x1c],eax
                mov		[esi+0x20],edx

                mov		eax,[ebx+0x30]
                mov		edx,[ebx+0x2c]
                shr		eax,1
                sub		edx,3
                add		eax,[ebx+0x28]
                mov		[esi+0x2c],edx
                dec		eax
                sub		edx,2
                mov		[esi+0x28],eax
                mov		[esi+0x18],edx
                sub		eax,2
                mov		[esi+0x8],edx
                mov		[esi+0x14],eax
                mov		[esi+0x4],eax

                test	edi,edi
                jnz		short	bshue

                push	4
                call	soundplay
                jmp		short	bshue
bshu2:
                add		esi,ONEDYNOBJ
                dec		ecx
                jnz		short	bshu1
bshue:


                //ENDING routine

                test	edi,edi
                jz		short	bf3

                and		word ptr [ebx+0x24],0

                push	4
                call	soundplay
bf3:
        }

    __asm
    {
bff:
        cmp		byte ptr [ebx+0x24+2],0
        jz		short	bshdec
        dec		byte ptr [ebx+0x24+2]		//Pause for shooting
bshdec:

        //EXPANSION if neded

        test	byte ptr [ebx+0x38+2],0x4
        jz		short	bextno

        test	byte ptr [ebx+0x38+2],0xe0
        jnz		short	bextno

        mov		esi,[ebx+0x38]
        shr		esi,(16-4)
        and		esi,0x10
        add		esi,offset ExpBat

        dec		dword ptr [esi]
        jnz		short	bextno

        mov		dword ptr [esi],7		//Pause at expansion

        mov		eax,[esi+0x4]
        cmp		eax,105
        jnz		short	bext9
        push	9
        call	soundplay
        mov		eax,105
bext9:
        inc		eax
        cmp		eax,108
        jbe		short	bext10

        or		byte ptr [ebx+0x38+2],0x8
        mov		eax,[Round]
        and		byte ptr [ebx+0x3],0xfe
        and		eax,0x3
        mov		edx,3
        add		eax,102
        and		byte ptr [ebx+0x38+2],0xfb
        call	makeEBX27
        jmp		short	bext11
bext10:
        or		byte ptr [ebx+0x3],1
        lea		edx,[eax-106]
bext11:
        lea		edx,[edx*4+50]			//EDX=phys Width of BAT

        mov		[ebx+0x38],al
        mov		ecx,edx
        mov		[esi+0x4],eax
        sub		ecx,[ebx+0x30]
        mov		[ebx+0x38+3],al
        shr		ecx,1					//ECX=how more to expand each side

        mov		eax,[lpDyn]
        mov		edi,ecx
        mov		ebp,[ebx+0x28]
        add		eax,ONEDYNOBJ*12
        mov		esi,WALLWIDTH
        cmp		[DoublePlay],TRUE
        jnz		short	bext20
        test	byte ptr [ebx+0x38+2],0x1
        jz		short	bext20
        mov		esi,[eax+0x28]
        add		esi,[eax+0x30]
bext20:
        sub		ebp,esi
        cmp		ebp,ecx
        jae		short	bext21
        mov		ecx,ebp
bext21:

        mov		esi,[ScWpix]
        mov		ebp,[ebx+0x28]
        sub		esi,WALLWIDTH
        cmp		[DoublePlay],TRUE
        jnz		short	bext22
        test	byte ptr [ebx+0x38+2],0x1
        jnz		short	bext22
        mov		esi,[eax+0x28]
bext22:
        add		ebp,[ebx+0x30]
        sub		esi,ebp
        sub		edi,esi
        jbe		short	bext23
        add		ecx,edi
bext23:

        or		[dir],0x4
        mov		eax,[ebx+0x28]		//set new BAT's dims
        mov		[ebx+0x30],edx
        sub		eax,ecx
        mov		[ebx+0x28],eax
        add		edx,2+ADDSHAD
        sub		eax,1
        mov		[ebx+0x1c],edx
        mov		[ebx+0x14],eax

bextno:
        //RETURN from expansion if neede

        test	byte ptr [ebx+0x38+2],0x10
        jz		short	rfeno

        mov		esi,[ebx+0x38]
        shr		esi,(16-4)
        and		esi,0x10
        add		esi,offset ExpBat

        dec		dword ptr [esi]
        jnz		short	rfeno

        mov		dword ptr [esi],7		//Pause at expansion

        mov		eax,[esi+0x4]
        dec		eax
        cmp		eax,106
        jae		short	rfe10

        and		byte ptr [ebx+0x38+2],0xf7
        mov		eax,[Round]
        and		byte ptr [ebx+0x3],0xfe
        and		eax,0x3
        add		eax,40
        and		byte ptr [ebx+0x38+2],0xef
        call	makeEBX27
        jmp		short	rfe11
rfe10:
        or		byte ptr [ebx+0x3],1
rfe11:
        mov		[ebx+0x38],al
        mov		edx,[ebx+0x30]
        mov		[esi+0x4],eax
        sub		edx,4
        mov		[ebx+0x38+3],al

        or		[dir],0x4
        mov		eax,[ebx+0x28]		//set new BAT's dims
        mov		[ebx+0x30],edx
        add		eax,2
        mov		[ebx+0x28],eax
        add		edx,2+ADDSHAD
        sub		eax,1
        mov		[ebx+0x1c],edx
        mov		[ebx+0x14],eax
rfeno:


        //in-SHOOTER transformation, if neded

        test	byte ptr [ebx+0x38+2],0x20
        jz		short	msbno

        test	byte ptr [ebx+0x38+2],0x1c
        jnz		short	msbno

        mov		esi,[ebx+0x38]
        shr		esi,(16-4)
        and		esi,0x10
        add		esi,offset ExpBat

        dec		dword ptr [esi+0x8]
        jnz		short	msbno

        mov		dword ptr [esi+0x8],14		//Pause

        mov		eax,[esi+0xc]
        cmp		eax,117
        jnz		short	msb2
        push	11
        call	soundplay
        mov		eax,117
msb2:
        dec		eax
        cmp		eax,113
        jae		short	msb3

        or		byte ptr [ebx+0x38+2],0x40
        mov		eax,[Round]
        and		byte ptr [ebx+0x3],0xfe
        and		eax,0x3
        add		eax,109
        and		byte ptr [ebx+0x38+2],0xdf
        call	makeEBX27
        jmp		short	msb4
msb3:
        or		byte ptr [ebx+0x3],1
msb4:
        mov		[ebx+0x38],al
        or		[dir],0x4
        mov		[esi+0xc],eax
        mov		[ebx+0x38+3],al
msbno:


        //out-of-SHOOTER transformation, if neded

        test	byte ptr [ebx+0x38+2],0x80
        jz		short	ostno

        mov		esi,[ebx+0x38]
        shr		esi,(16-4)
        and		esi,0x10
        add		esi,offset ExpBat

        dec		dword ptr [esi+0x8]
        jnz		short	ostno

        mov		dword ptr [esi+0x8],14		//Pause

        mov		eax,[esi+0xc]
        inc		eax
        cmp		eax,116
        jbe		short	ost3

//		and		byte ptr [ebx+0x38+2],0xbf
        mov		eax,[Round]
        and		byte ptr [ebx+0x3],0xfe
        and		eax,0x3
        add		eax,40
        and		byte ptr [ebx+0x38+2],0x7f
        call	makeEBX27
        jmp		short	ost4
ost3:
        or		byte ptr [ebx+0x3],1
ost4:
        mov		[ebx+0x38],al
        or		[dir],0x4
        mov		[esi+0xc],eax
        mov		[ebx+0x38+3],al
ostno:



        //CHECK for movements

        mov		eax,[dir]
        cmp		[BatInactive],TRUE
        jnz		short	bt0
        xor		eax,eax
bt0:
        mov		ecx,[ebx+0x28]
        mov		esi,[DoublePlay]
        shr		eax,1
        push	ecx
        jnc		short	bt1

        sub		ecx,[LeftDx]					//LEFT
        mov		edx,WALLWIDTH
        movzx	edi,byte ptr [ebx+0x24+1]
        and		esi,esi
        jz		short	bt1a
        test	byte ptr [ebx+0x38+2],0x1
        jz		short	bt1a
        mov		edx,[ebx-ONEDYNOBJ*(31-12)+0x28]
        add		edx,[ebx-ONEDYNOBJ*(31-12)+0x30]
bt1a:
        add		edx,edi
        cmp		ecx,edx
        jge		short	bt1
        mov		ecx,edx


bt1:
        shr		eax,1						//RIGHT
        jnc		short	bt2

        mov		edx,[ScWpix]
        add		ecx,[RightDx]
        movzx	edi,byte ptr [ebx+0x24]
        sub		edx,WALLWIDTH
        and		esi,esi
        jz		short	bt2a
        test	byte ptr [ebx+0x38+2],0x1
        jnz		short	bt2a
        mov		edx,[ebx-ONEDYNOBJ*(30-12)+0x28]
bt2a:
        sub		edx,[ebx+0x30]
        sub		edx,edi
        cmp		ecx,edx
        jbe		short	bt2
        mov		ecx,edx
bt2:


        pop		eax
        test	[dir],0x3
        jz		short	bt5

        sub		eax,ecx					//check for overlapping with
        sub		[ebx+0x14],eax			//any of 3 BALLS
        mov		[ebx+0x28],ecx

        xor		edx,edx
        call	ifhitball
        jnc		short	bt3
        mov		edi,[lpBallsAux]
        call	btht
bt3:
        inc		edx
        call	ifhitball
        jnc		short	bt4
        mov		edi,[lpBallsAux+0x4]
        call	btht
bt4:
        inc		edx
        call	ifhitball
        jnc		short	bt5
        mov		edi,[lpBallsAux+0x4*2]
        call	btht
bt5:



        cmp		[dir],0
        setnz	al
        dec		byte ptr [ebx+0x39]
        sets	ah
        or		al,ah
        jz		short	btl1

        test	ah,ah
        jz		short	btl0

        test	byte ptr [ebx+0x38+2],0xb4	//if EXPANSION => no specles
        jnz		short	btl0

        mov		edi,offset BatSpec			//Set moving speckles
        mov		eax,[BatColor]
        mov		ecx,8
        mov		edx,10
        mov		esi,0x8505
        test	byte ptr [ebx+0x38+2],0x48
        jz		short	btsp1
        sub		edx,3					//EXPANDED BAT
        shl		ecx,1
        mov		esi,0x8d0d
btsp1:
        rol		eax,16
        mov		byte ptr [ebx+0x39],dl
        push	edi
        mov		edx,[Round]
        or		eax,[BatColor]
        rep		stosd

        mov		ecx,esi

        mov		al,[ebx+0x27]
        and		edx,3
        test	al,al
        jns		short	btsp2
        dec		al
        js		short	btsp3
        xor		eax,eax
        jmp		short	btsp3
btsp2:
        inc		al
        cmp		al,cl
        jbe		short	btsp3
        mov		al,ch
btsp3:
        mov		[ebx+0x27],al
        pop		esi
        and		eax,0xf
        shl		edx,3
        mov		edi,[lpGrxStart]

        mov		dword ptr [esi+eax*2+2],0x001f001f
        test	byte ptr [ebx+0x38+2],0x8
        jz		short	btsp4

        add		edi,[edi+edx+102*8]
        neg		eax
        add		edi,9*64*2+16*2
        mov		dword ptr [esi+eax*2+29*2],0x001f001f
        mov		ecx,16
        jmp		short	btsp5
btsp4:
        test	byte ptr [ebx+0x38+2],0x40
        jz		short	btsp41
        add		edi,[edi+edx+109*8]			//SHOOTER BAT
        jmp		short	btsp42
btsp41:
        add		edi,[edi+edx+40*8]
btsp42:
        neg		eax
        add		edi,9*BATWIDTH*2+16*2
        mov		dword ptr [esi+eax*2+13*2],0x001f001f
        mov		ecx,8
btsp5:
        rep		movsd


btl0:
        call	lastatmove
btl1:
        pop		ecx
        ret




btht:
        push	edx
        mov		[r],eax
        fld		qword ptr [edi]			//CHECK if BALL isn't stuck between
        fist	dword ptr [i]

        test	eax,eax					//EAX=direction
        mov		ecx,[i]
        jns		short	bthl1

        add		ecx,[BALLw_div1]		//BAT MOVING RIGHT
        mov		esi,[ScWpix]
        sub		ecx,eax
        sub		esi,WALLWIDTH
        cmp		[DoublePlay],TRUE
        jnz		short	bthr1
        test	byte ptr [ebx+0x38+2],0x1
        jnz		short	bthr1
        mov		esi,[ebx-ONEDYNOBJ*(30-12)+0x28]
bthr1:
        sub		esi,[BALLw_div2]
        cmp		ecx,esi
        ja		short	bthl2
        jmp		short	bthl3

bthl1:
        sub		ecx,eax					//BAT MOVING LEFT
        mov		esi,WALLWIDTH
        cmp		[DoublePlay],TRUE
        jnz		short	bthl11
        test	byte ptr [ebx+0x38+2],0x1
        jz		short	bthl11
        mov		esi,[ebx-ONEDYNOBJ*(31-12)+0x28]
        add		esi,[ebx-ONEDYNOBJ*(31-12)+0x30]
bthl11:
        add		esi,[BALLw_div2]
        cmp		ecx,esi
        ja		short	bthl3
bthl2:
        or		byte ptr [edi+0x3c],1
        jmp		short	bthl4
bthl3:
        fisub	dword ptr [r]
bthl4:
        fstp	qword ptr [edi]
        pop		edx
        ret


    }
}







__declspec ( naked ) ball1(void)		//MAIN BALL
{

    __asm
    {
        cmp		[BallsFreez],TRUE
        jz		short	bl_ee

        push	ecx
        mov		ebp,NUMDYN-32
        mov		edx,[ebx+0x24]			//31bit=1 => STICKY BALL
        sub		ebp,ecx					//30bit=1 => SMASH BALL
        and		ebp,0x3
        mov		eax,[ebp*4+lpBallsAux]
        test	edx,edx
        mov		[auxEbx],eax

        js		short	blst1		//jump if STICKY BALL

//test	edx,0x30000000
//jnz		bllle
//or		byte ptr [ebx+0x24+3],0x10	//CLEAR WORK OF 'Q' KEY TOO!!!!

        mov		edx,[eax+0x18]			//alpha
        or		[SoundChosen],-1
        shl		edx,2
        add		edx,[lpSin]
        fld		qword ptr [eax+0x10]	//R
        fld		st(0)
        fmul	dword ptr [edx]
        fchs
        fst		[fYplus]				//fYplus=-R*sin(alpha)
        fadd	qword ptr [eax+0x8]
        fistp	dword ptr [i]

        fmul	dword ptr [edx+90*4]
        fst		[fXplus]				//fXplus=R*cos(alpha)
        fadd	qword ptr [eax]
        fistp	dword ptr [r]

        xor		edx,edx
        mov		ebp,[i]
        mov		ecx,[r]
        mov		[hitting],edx

        sub		ebp,[ebx+0x2c]			//EDX=direction
        rcl		edx,1					//01=>left & down, left
        sub		ecx,[ebx+0x28]
//		rol		ebp,1					//00=>right & down, right, down
//		rol		ecx,1					//10=>right & up, up
        rcl		edx,1					//11=>left & up

        or		ebp,ecx
        jz		short	bll0
        mov		[direc],edx

        mov		ecx,[i]
        mov		ebp,[r]
        push	offset bll1
        jmp		NEAR dword ptr [edx*4+balljmps]


bll0:
//		mov		eax,[auxEbx]
        fld		[fXplus]				//here, if BALL didn't move graphically
        fadd	qword ptr [eax]
        fstp	qword ptr [eax]

        fld		[fYplus]
        fadd	qword ptr [eax+0x8]
        fstp	qword ptr [eax+0x8]
        jmp		short	bllle

bll1:
        mov		eax,[auxEbx]
        fld		[fXplus]				//update BALL coordinates
        fadd	qword ptr [eax]
        fist	dword ptr [r]			//r=new X value
        fstp	qword ptr [eax]

        fld		[fYplus]
        fadd	qword ptr [eax+0x8]
        fist	dword ptr [i]			//i=new Y value
        fstp	qword ptr [eax+0x8]

        xor		edx,edx
        mov		ecx,[r]
        mov		ebp,[i]
        mov		[ebx+0x28],ecx
        mov		[ebx+0x2c],ebp

        mov		dl,[eax+0x3c+3]
        cmp		ecx,[eax+0x2c]			//set anti-block coords
        jae		short	ab1
        mov		[eax+0x2c],ecx
ab1:
        cmp		ebp,[eax+0x34]
        jae		short	ab2
        mov		[eax+0x34],ebp
ab2:
        cmp		ecx,[eax+0x30]
        jbe		short	ab3
        mov		[eax+0x30],ecx
ab3:
        cmp		ebp,[eax+0x38]
        jbe		short	ab4
        mov		[eax+0x38],ebp
ab4:

        sub		ecx,edx					//getting of graphic coords
        sub		ebp,edx					//from PHYS
        mov		[ebx+0x14],ecx
        mov		[ebx+0x18],ebp


        cmp		ebp,[ScHpix]		//check for BALL in game
        ja		short	bllost
        cmp		ecx,[ScWpix]
        ja		short	bllost
blll:
        call	lastatmove
bllle:
        cmp		[SmashOn],TRUE
        jnz		short	bl_e
        dec		[SmashToOff]
        jnz		short	bl_e

        call	ballsbackfromSMASH
bl_e:
        pop		ecx
bl_ee:
        ret



blst1:								//STICKY BALL

        mov		edi,[lpDyn]			//Sticky in the life beginning
        add		edi,ONEDYNOBJ*30
        test	byte ptr [eax+0x3c],0x2
        jz		short	blst11
        add		edi,ONEDYNOBJ
blst11:

        mov		ecx,[edi+0x14]
//	mov		ecx,126//260
//	mov		dword ptr [eax+0x18],180
        add		ecx,[eax+0x1c]
        mov		[ebx+0x14],ecx

        mov		ebp,[edi+0x18]
//	mov		ebp,358
        add		ebp,[eax+0x20]
        mov		[ebx+0x18],ebp

        add		ecx,[eax+0x24]
        mov		[ebx+0x28],ecx

        add		ebp,[eax+0x28]
        mov		[ebx+0x2c],ebp

        fild	dword ptr [ebx+0x28]
        fstp	qword ptr [eax]
        cmp		dword ptr [ebx+0x2c],400
        jb		short	www
        nop
www:
        fild	dword ptr [ebx+0x2c]
        fstp	qword ptr [eax+0x8]

        jmp		short	blll



bllost:									//BALL LOST
        dec		[NumBalls]
        jz		short	bllos20
        mov		eax,ebx

        or		byte ptr [ebx+0x3],0x50		//take BALL away
        sub		eax,[lpDyn]
        and		byte ptr [ebx+0x3],0x7f
        and		byte ptr [ebx+0x24+3],0xbf

        mov		ecx,2
        sub		eax,ONEDYNOBJ*32
        ja		short	bllos2
        mov		[SwapBall1],TRUE		//if BALL1 is lost

        test	byte ptr [ebx+ONEDYNOBJ+0x3],0x10
        jz		short	bllos3
        jmp		short	bllos21
bllos2:
        sub		eax,ONEDYNOBJ
        jz		short	bllos3
bllos21:
        inc		ecx
bllos3:
        push	ecx
        call	killballtimer			//(BallNumber123)
        jmp		short	blll

bllos20:
        mov		eax,[ebx+0x30]			//EXPLODE BAT(s)
        mov		edx,[ScWpix]
        shr		eax,1
        shr		edx,1
        add		eax,[ebx+0x28]

        or		esi,-1
        mov		ecx,offset LostBallL
        cmp		[DoublePlay],TRUE
        jnz		short	bllost1
        inc		esi
        cmp		eax,edx
        jb		short	bllost1
        inc		esi
        mov		ecx,offset LostBallR
bllost1:
        push	esi
        inc		dword ptr [ecx]
        call	killbat					//(WhichBat) 0=>left, 1=>right

        jmp		short	blll
    }
}






__declspec ( naked ) hitcells(void)
{	__asm
    {
        mov		edi,[esp+0x10]			//yoD
        mov		ebp,[esp+0xc]			//yoU
        sub		edi,60
        jle		short	nohit
        cmp		ebp,60+CELLHEIGHT*CELLVERT
        jnc		short	nohit

        sub		ebp,60
        jns		short	hc0
        xor		ebp,ebp
hc0:
        mov		cl,4					//2^4=CELLHEIGHT
        dec		edi
        shr		edi,cl
        mov		eax,[esp+0x4]			//xoL
        shr		ebp,cl

        sub		eax,17
        jns		short	hc1
        xor		eax,eax
hc1:
        mov		ecx,[esp+0x8]			//xoR
        cmp		eax,CELLWIDTH*15
        jae		short	nohit

        sub		ecx,17
        jle		short	nohit

        mov		edx,CELLWIDTH
        dec		ecx
                                //		push	esi
                                //		push	ebp
                                //		mov		esi,CELLWIDTH
                                //		xor		edx,edx
        div		dl				//		div		esi
                                //		xchg	ebp,eax
        xchg	eax,ecx			//		mov		eax,ecx
                                //		xor		edx,edx
        div		dl				//		div		esi
                                //		xchg	edx,eax
                                //		xchg	ebp,eax
                                //		pop		ebp
                                //		pop		esi
        mov		dl,al
        cmp		edi,CELLVERT
        movzx	eax,cl
        jb		short	hc2
        mov		edi,CELLVERT-1
hc2:
        cmp		edx,15
        jb		short	hc3
        mov		edx,14
hc3:

        cmp		eax,edx
        mov		edx,[direc]
        rcl		edx,1
        cmp		ebp,edi
        rcl		edx,1

        lea		edi,[ebp*8]
        lea		edi,[edi*2+eax]
        sub		edi,ebp				//EDI=EBP*15+EAX
        shl		edi,2
        shl		ebp,4
        add		edi,[lpCell]

        imul	ecx,eax,CELLWIDTH
        add		ebp,60				//EBP=Y coord of leftmost top CELL

        mov		eax,offset bhitjmps
        test	byte ptr [ebx+0x24+3],0x40	//select among SMASH/NORMAL BALL
        jz		short	ofs1
        cmp		[NoTouch],0
        jnz		short	ofs1
        mov		eax,offset Xbhitjmps
ofs1:
        add		ecx,17				//ECX=X coord of leftmost top CELL

        jmp		NEAR dword ptr [edx*4+eax]

nohit:
        ret
    }
}






__declspec ( naked ) m_one(void)
{	__asm
    {
        test	byte ptr [edi],0x20  //for ANY direction, for ONE-CELL hit
        jnz		short	nohit

        call	celltouch

        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        mov		eax,CELLWIDTH
        add		eax,ecx
        mov		[esp+0x18],eax			//xtR
        mov		eax,CELLHEIGHT
        add		eax,ebp
        mov		[esp+0x20],eax			//ytD

        jmp		NEAR	esi

nohit:
        ret
    }
}





__declspec ( naked ) Xm_one(void)
{	__asm
    {
        test	byte ptr [edi],0x20  //for ANY direction, for ONE-CELL hit
        jnz		short	nohit

        call	Xcelltouch
        jnc		short	nohit

        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        mov		eax,CELLWIDTH
        add		eax,ecx
        mov		[esp+0x18],eax			//xtR
        mov		eax,CELLHEIGHT
        add		eax,ebp
        mov		[esp+0x20],eax			//ytD

        jmp		NEAR	esi

nohit:
        ret
    }
}





__declspec ( naked ) mDRu_d(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING right&down, right, down
        mov		edx,[edi+15*0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mDRud10		//jmp if only LOWER CELL present

        test	edx,0x20
        jnz		short	mDRud20		//jmp if only UPPER CELL present

        mov		eax,ebp					//LOWER & UPPER CELLS present
        mov		edx,[esp+0x10]			//EDX=yoD
        add		eax,CELLHEIGHT
        sub		edx,eax
        sub		eax,[ebx+0x2c]			//EAX=EAX-yoU(old)
        cmp		eax,edx
        jg		short	mDRud20			//jmp if hit UPPER CELL
        je		short	mDRud30			//jmp if hit UPPER & LOWER CELLs

mDRud10:
        add		edi,0x4*15				//hit LOWER CELL
        add		ebp,CELLHEIGHT

mDRud20:
        call	celltouch				//hit UPPER CELL

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mDRud30:
        push	edi				//UPPER & LOWER CELLs hit
        add		edi,0x4*15
        call	celltouch
        pop		edi
        jmp		short	mDRud20

nohit:
        ret
    }
}






__declspec ( naked ) XmDRu_d(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING right&down, right, down
        mov		edx,[edi+15*0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mDRud10		//jmp if only LOWER CELL present

        test	edx,0x20
        jnz		short	mDRud20		//jmp if only UPPER CELL present

        and		eax,0x40
        jz		short	mDRud30

        mov		eax,ebp					//LOWER & UPPER CELLS present
        mov		edx,[esp+0x10]			//EDX=yoD
        add		eax,CELLHEIGHT
        sub		edx,eax
        sub		eax,[ebx+0x2c]			//EAX=EAX-yoU(old)
        cmp		eax,edx
        jg		short	mDRud20			//jmp if hit UPPER CELL
        je		short	mDRud30			//jmp if hit UPPER & LOWER CELLs

mDRud10:
        add		edi,0x4*15				//hit LOWER CELL
        add		ebp,CELLHEIGHT

mDRud20:
        call	Xcelltouch				//hit UPPER CELL
        jnc		short	nohit
mDRud21:
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mDRud30:
        add		edi,0x4*15				//UPPER & LOWER CELLs hit
        call	Xcelltouch
        lea		edi,[edi-0x4*15]
        jnc		short	mDRud20

        add		ebp,CELLHEIGHT
        call	Xcelltouch
        jmp		short	mDRud21

nohit:
        ret
    }
}





__declspec ( naked ) mDRl_r(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING right&down, right, down
        mov		edx,[edi+0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mDRlr10		//jmp if only RIGHT CELL present

        test	edx,0x20
        jnz		short	mDRlr20		//jmp if only LEFT CELL present

        mov		eax,ecx					//LEFT & RIGHT CELLS present
        add		eax,CELLWIDTH
        mov		edx,[esp+0x8]			//EDX=xoR
        sub		edx,eax
        sub		eax,[ebx+0x28]			//EAX=EAX-xoL(old)
        cmp		eax,edx
        jg		short	mDRlr20			//jmp if hit LEFT CELL
        je		short	mDRlr30			//jmp if hit LEFT & RIGHT CELLs

mDRlr10:
        add		edi,0x4					//hit RIGHT CELL
        add		ecx,CELLWIDTH

mDRlr20:
        call	celltouch				//hit LEFT CELL

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mDRlr30:
        push	edi				//LEFT & RIGHT CELLs hit
        add		edi,0x4
        call	celltouch
        pop		edi
        jmp		short	mDRlr20

nohit:
        ret
    }
}





__declspec ( naked ) XmDRl_r(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING right&down, right, down
        mov		edx,[edi+0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mDRlr10		//jmp if only RIGHT CELL present

        test	edx,0x20
        jnz		short	mDRlr20		//jmp if only LEFT CELL present

        and		eax,0x40
        jz		short	mDRlr30

        mov		eax,ecx					//LEFT & RIGHT CELLS present
        mov		edx,[esp+0x8]			//EDX=xoR
        add		eax,CELLWIDTH
        sub		edx,eax
        sub		eax,[ebx+0x28]			//EAX=EAX-xoL(old)
        cmp		eax,edx
        jg		short	mDRlr20			//jmp if hit LEFT CELL
        je		short	mDRlr30			//jmp if hit LEFT & RIGHT CELLs

mDRlr10:
        add		edi,0x4					//hit RIGHT CELL
        add		ecx,CELLWIDTH

mDRlr20:
        call	Xcelltouch				//hit LEFT CELL
        jnc		short	nohit
mDRlr21:
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mDRlr30:
        push	edi				//LEFT & RIGHT CELLs hit
        add		edi,0x4
        call	Xcelltouch
        pop		edi
        jnc		short	mDRlr20

        add		ecx,CELLWIDTH
        call	Xcelltouch
        jmp		short	mDRlr21

nohit:
        ret
    }
}





__declspec ( naked ) mDRall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING right&down, right, down
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+mDRalljmps]
    }
}




__declspec ( naked ) XmDRall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING right&down, right, down
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+XmDRalljmps]
    }
}




__declspec ( naked ) mDR01(void)
{	__asm
    {
        add		edi,0x4
        call	celltouch
        add		edi,14*0x4
        call	celltouch

        add		ecx,CELLWIDTH			//_O  CELLS disposition
        mov		edx,ebp					//OO
        mov		eax,ecx	
        add		edx,CELLHEIGHT
        sub		eax,[ebx+0x28]
        sub		edx,[ebx+0x2c]
        cmp		eax,edx
        jnz		short	mDR0110

        add		edi,0x4
        call	celltouch
mDR0110:
        pop		esi
        
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitRD

        sub		ecx,CELLWIDTH
        add		ebp,CELLHEIGHT

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitRD
    }
}




__declspec ( naked ) XmDR01(void)
{
    static DWORD cInf[]={
                            0,0,0,0,
                            CELLWIDTH,	CELLHEIGHT, -1,-1,
                            0,			CELLHEIGHT,	-1,-1,
                            0,			CELLHEIGHT,	CELLWIDTH,0,
                            CELLWIDTH,	0,			-1,-1,
                            CELLWIDTH,	0,			0,CELLHEIGHT,
                            CELLWIDTH,	0,			-CELLWIDTH,CELLHEIGHT
                        };

    __asm
    {
        xor		esi,esi					//_O  CELLS disposition
        add		edi,0x4					//OO
        call	Xcelltouch
        rcl		esi,1
        add		edi,14*0x4
        call	Xcelltouch
        rcl		esi,1

        cmp		esi,0x3
        jz		short	mDR0110

        add		edi,0x4
        call	Xcelltouch
mDR0110:
        rcl		esi,1
        mov		edi,offset cInf
        test	esi,esi
        jz		short	nohit

        shl		esi,4
        add		edi,esi
        pop		esi
        
        add		ecx,[edi]
        add		ebp,[edi+0x4]

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitRD

        mov		eax,[edi+0xc]
        add		ecx,[edi+0x8]
        cmp		eax,-1
        jz		short	mDR0111
        add		ebp,eax

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitRD
mDR0111:
        push	esi
nohit:
        ret
    }
}


__declspec ( naked ) mDR03(void)
{	__asm
    {
        add		ebp,CELLHEIGHT		//__  CELLS disposition
        add		edi,15*0x4			//OO
        jmp		mDRl_r
    }
}



__declspec ( naked ) XmDR03(void)
{	__asm
    {
        add		ebp,CELLHEIGHT		//__  CELLS disposition
        add		edi,15*0x4			//OO
        jmp		XmDRl_r
    }
}



__declspec ( naked ) mDR05(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//_O
        jmp		mDRu_d
    }
}



__declspec ( naked ) XmDR05(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//_O
        jmp		XmDRu_d
    }
}



__declspec ( naked ) mDR11(void)
{	__asm
    {
        add		ebp,CELLHEIGHT		//__  CELLS disposition
        add		edi,15*0x4			//O_
        jmp		m_one
    }
}




__declspec ( naked ) XmDR11(void)
{	__asm
    {
        add		ebp,CELLHEIGHT		//__  CELLS disposition
        add		edi,15*0x4			//O_
        jmp		Xm_one
    }
}




__declspec ( naked ) mDR13(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//__
        jmp		m_one
    }
}




__declspec ( naked ) XmDR13(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//__
        jmp		Xm_one
    }
}




__declspec ( naked ) mDR07(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//__  CELLS disposition
        add		edi,0x4*15+0x4		//_O
        add		ebp,CELLHEIGHT
        jmp		m_one
    }
}




__declspec ( naked ) XmDR07(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//__  CELLS disposition
        add		edi,0x4*15+0x4		//_O
        add		ebp,CELLHEIGHT
        jmp		Xm_one
    }
}








__declspec ( naked ) mDLl_r(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING left&down, or left
        mov		edx,[edi+0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mDLlr10		//jmp if only RIGHT CELL present

        test	edx,0x20
        jnz		short	mDLlr20		//jmp if only LEFT CELL present

        mov		eax,ecx					//LEFT & RIGHT CELLS present
        mov		edx,[ebx+0x28]
        add		eax,CELLWIDTH
        add		edx,[ebx+0x30]			//EDX=xoR(old)
        sub		edx,eax
        sub		eax,[esp+0x4]			//EAX=EAX-xoL
        cmp		eax,edx
        jg		short	mDLlr20			//jmp if hit LEFT CELL
        je		short	mDLlr30			//jmp if hit LEFT & RIGHT CELLs

mDLlr10:
        add		edi,0x4					//hit RIGHT CELL
        add		ecx,CELLWIDTH

mDLlr20:
        call	celltouch				//hit LEFT CELL

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mDLlr30:
        call	celltouch				//LEFT & RIGHT CELLs hit
        jmp		short	mDLlr10


nohit:
        ret
    }
}




__declspec ( naked ) XmDLl_r(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING left&down, or left
        mov		edx,[edi+0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mDLlr10		//jmp if only RIGHT CELL present

        test	edx,0x20
        jnz		short	mDLlr20		//jmp if only LEFT CELL present

        and		eax,0x40
        jz		short	mDLlr30

        mov		eax,ecx					//LEFT & RIGHT CELLS present
        mov		edx,[ebx+0x28]
        add		eax,CELLWIDTH
        add		edx,[ebx+0x30]			//EDX=xoR(old)
        sub		edx,eax
        sub		eax,[esp+0x4]			//EAX=EAX-xoL
        cmp		eax,edx
        jg		short	mDLlr20			//jmp if hit LEFT CELL
        je		short	mDLlr30			//jmp if hit LEFT & RIGHT CELLs

mDLlr10:
        add		edi,0x4					//hit RIGHT CELL
        add		ecx,CELLWIDTH

mDLlr20:
        call	Xcelltouch				//hit LEFT CELL
        jnc		short	nohit
mDLlr21:
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mDLlr30:
        call	Xcelltouch				//LEFT & RIGHT CELLs hit
        jnc		short	mDLlr10
        add		edi,0x4
        call	Xcelltouch
        jmp		short	mDLlr21

nohit:
        ret
    }
}






__declspec ( naked ) mDLall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING left&down, or left
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+mDLalljmps]
    }
}




__declspec ( naked ) XmDLall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING left&down, or left
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+XmDLalljmps]
    }
}





__declspec ( naked ) mDL02(void)
{	__asm
    {
        call	celltouch
        add		edi,15*0x4+0x4
        call	celltouch

        mov		eax,[ebx+0x28]			//O_  CELLS disposition
        mov		edx,ebp					//OO
        add		eax,[ebx+0x30]
        add		edx,CELLHEIGHT
        sub		eax,ecx
        sub		edx,[ebx+0x2c]
        sub		eax,CELLWIDTH
        cmp		eax,edx
        jnz		short	mDL0210

        sub		edi,0x4
        call	celltouch
mDL0210:
        pop		esi
        
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitLD

        add		ecx,CELLWIDTH
        add		ebp,CELLHEIGHT

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitLD
    }
}




__declspec ( naked ) XmDL02(void)
{
    static DWORD cInf[]={
                            0,0,0,0,
                            0,			CELLHEIGHT,	-1,-1,
                            CELLWIDTH,	CELLHEIGHT,	-1,-1,
                            0,			CELLHEIGHT,	CELLWIDTH,0,
                            0,			0,			-1,-1,
                            0,			0,			0,CELLHEIGHT,
                            0,			0,			CELLWIDTH,CELLHEIGHT
                        };

    __asm
    {
        xor		esi,esi				//O_  CELLS disposition
        call	Xcelltouch			//OO
        rcl		esi,1
        add		edi,15*0x4+0x4
        call	Xcelltouch
        rcl		esi,1

        cmp		esi,3
        jz		short	mDL0210

        sub		edi,0x4
        call	Xcelltouch
mDL0210:
        rcl		esi,1
        mov		edi,offset cInf
        test	esi,esi
        jz		short	nohit

        shl		esi,4
        add		edi,esi
        pop		esi
        
        add		ecx,[edi]
        add		ebp,[edi+0x4]

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitLD

        mov		eax,[edi+0xc]
        add		ecx,[edi+0x8]
        cmp		eax,-1
        jz		short	mDL0111
        add		ebp,eax

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitLD
mDL0111:
        push	esi
nohit:
        ret
    }
}




__declspec ( naked ) mDL03(void)
{	__asm
    {
        add		ebp,CELLHEIGHT		//__  CELLS disposition
        add		edi,0x4*15			//OO
        jmp		mDLl_r
    }
}




__declspec ( naked ) XmDL03(void)
{	__asm
    {
        add		ebp,CELLHEIGHT		//__  CELLS disposition
        add		edi,0x4*15			//OO
        jmp		XmDLl_r
    }
}




__declspec ( naked ) mDL01(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//OO
        jmp		mDRu_d				//mDLu_D==mDRu_d
    }
}





__declspec ( naked ) XmDL01(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//OO
        jmp		XmDRu_d				//mDLu_D==mDRu_d
    }
}








__declspec ( naked ) mURu_d(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING right&up, or up
        mov		edx,[edi+15*0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mURud10		//jmp if only LOWER CELL present

        test	edx,0x20
        jnz		short	mURud20		//jmp if only UPPER CELL present

        mov		edx,[ebx+0x2c]
        mov		eax,ebp					//LOWER & UPPER CELLS present
        add		edx,[ebx+0x34]			//EDX=yoD(old)
        add		eax,CELLHEIGHT
        sub		edx,eax
        sub		eax,[esp+0xc]			//EAX=EAX-yoU
        cmp		eax,edx
        jg		short	mURud20			//jmp if hit UPPER CELL
        je		short	mURud30			//jmp if hit UPPER & LOWER CELLs

mURud10:
        add		edi,0x4*15				//hit LOWER CELL
        add		ebp,CELLHEIGHT

mURud20:
        call	celltouch				//hit UPPER CELL

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mURud30:
        call	celltouch			//UPPER & LOWER CELLs hit
        jmp		short	mURud10


nohit:
        ret
    }
}





__declspec ( naked ) XmURu_d(void)
{	__asm
    {
        mov		eax,[edi]				//MOVING right&up, or up
        mov		edx,[edi+15*0x4]
        and		eax,edx
        test	eax,0x20
        jnz		short	nohit

        test	dword ptr [edi],0x20
        jnz		short	mURud10		//jmp if only LOWER CELL present

        test	edx,0x20
        jnz		short	mURud20		//jmp if only UPPER CELL present

        and		eax,0x40
        jz		short	mURud30

        mov		eax,ebp					//LOWER & UPPER CELLS present
        mov		edx,[ebx+0x2c]
        add		eax,CELLHEIGHT
        add		edx,[ebx+0x34]			//EDX=yoD(old)
        sub		edx,eax
        sub		eax,[esp+0xc]			//EAX=EAX-yoU
        cmp		eax,edx
        jg		short	mURud20			//jmp if hit UPPER CELL
        je		short	mURud30			//jmp if hit UPPER & LOWER CELLs

mURud10:
        add		edi,0x4*15				//hit LOWER CELL
        add		ebp,CELLHEIGHT

mURud20:
        call	Xcelltouch				//hit UPPER CELL
        jnc		short	nohit
mURud21:
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x14],ecx			//xtL
        mov		[esp+0x1c],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x18],eax			//xtR
        mov		[esp+0x20],edx			//ytD

        jmp		NEAR	esi


mURud30:
        call	Xcelltouch			//UPPER & LOWER CELLs hit
        jnc		short	mURud10
        add		edi,0x4*15
        call	Xcelltouch
        jmp		short	mURud21

nohit:
        ret
    }
}







__declspec ( naked ) mURall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING up&right, or up
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+mURalljmps]
    }
}




__declspec ( naked ) XmURall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING up&right, or up
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+XmURalljmps]
    }
}




__declspec ( naked ) mUR04(void)
{	__asm
    {
        call	celltouch
        add		edi,15*0x4+0x4
        call	celltouch

        mov		edx,[ebx+0x2c]
        mov		eax,ecx
        add		edx,[ebx+0x34]
        add		eax,CELLWIDTH			//OO  CELLS disposition
        sub		edx,ebp
        sub		eax,[ebx+0x28]			//_O
        sub		edx,CELLHEIGHT
        cmp		eax,edx
        jnz		short	mUR0410

        sub		edi,15*0x4
        call	celltouch
mUR0410:
        pop		esi
        
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitRU

        add		ecx,CELLWIDTH
        add		ebp,CELLHEIGHT

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitRU
    }
}



__declspec ( naked ) XmUR04(void)
{
    static DWORD cInf[]={
                            0,0,0,0,
                            CELLWIDTH,	0,			-1,-1,
                            CELLWIDTH,	CELLHEIGHT,	-1,-1,
                            CELLWIDTH,	0,			0,CELLHEIGHT,
                            0,			0,			-1,-1,
                            0,			0,			CELLWIDTH,0,
                            0,			0,			CELLWIDTH,CELLHEIGHT
                        };

    __asm
    {
        xor		esi,esi				//OO  CELLS disposition
        call	Xcelltouch			//_O
        rcl		esi,1
        add		edi,15*0x4+0x4
        call	Xcelltouch
        rcl		esi,1

        cmp		esi,3
        jz		short	mUR0410

        sub		edi,15*0x4
        call	Xcelltouch
mUR0410:
        rcl		esi,1
        mov		edi,offset cInf
        test	esi,esi
        jz		short	nohit

        shl		esi,4
        add		edi,esi
        pop		esi
        
        add		ecx,[edi]
        add		ebp,[edi+0x4]

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitRU

        mov		eax,[edi+0xc]
        add		ecx,[edi+0x8]
        cmp		eax,-1
        jz		short	mUR0111
        add		ebp,eax

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitRU
mUR0111:
        push	esi
nohit:
        ret
    }
}





__declspec ( naked ) mUR05(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//_O
        jmp		mURu_d
    }
}





__declspec ( naked ) XmUR05(void)
{	__asm
    {
        add		ecx,CELLWIDTH		//_O  CELLS disposition
        add		edi,0x4				//_O
        jmp		XmURu_d
    }
}







__declspec ( naked ) mULall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING up&left
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+mULalljmps]
    }
}




__declspec ( naked ) XmULall(void)
{	__asm
    {
        mov		edx,[edi+15*0x4+0x4]
        xor		eax,eax				//MOVING up&left
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+15*0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi+0x4]
        shl		dl,3
        rcl		eax,1
        mov		edx,[edi]
        shl		dl,3
        rcl		eax,1

        mov		edx,[eax*4+hitcorns]
        mov		[hitcorner],edx

        jmp		NEAR dword ptr [eax*4+XmULalljmps]
    }
}





__declspec ( naked ) mUL08(void)
{	__asm
    {
        add		edi,0x4
        call	celltouch
        add		edi,14*0x4
        call	celltouch

        mov		edx,[ebx+0x2c]
        mov		eax,[ebx+0x28]			//OO  CELLS disposition
        add		edx,[ebx+0x34]
        add		eax,[ebx+0x30]			//O_
        sub		edx,ebp
        sub		eax,ecx
        sub		edx,CELLHEIGHT
        sub		eax,CELLWIDTH
        cmp		eax,edx
        jnz		short	mUL0810

        sub		edi,15*0x4
        call	celltouch
mUL0810:
        pop		esi
        add		ecx,CELLWIDTH
        
        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitLU

        sub		ecx,CELLWIDTH
        add		ebp,CELLHEIGHT

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitLU
    }
}






__declspec ( naked ) XmUL08(void)
{
    static DWORD cInf[]={
                            0,0,0,0,
                            0,			0,			-1,-1,
                            0,			CELLHEIGHT,	-1,-1,
                            0,			0,			0,CELLHEIGHT,
                            CELLWIDTH,	0,			-1,-1,
                            0,			0,			CELLWIDTH,0,
                            CELLWIDTH,	0,			-CELLWIDTH,CELLHEIGHT
                        };

    __asm
    {
        xor		esi,esi			//OO  CELLS disposition
        add		edi,0x4			//O_
        call	Xcelltouch
        rcl		esi,1
        add		edi,14*0x4
        call	Xcelltouch
        rcl		esi,1

        cmp		esi,3
        jz		short	mUL0810

        sub		edi,15*0x4
        call	Xcelltouch
mUL0810:
        rcl		esi,1
        mov		edi,offset cInf
        test	esi,esi
        jz		short	nohit

        shl		esi,4
        add		edi,esi
        pop		esi
        
        add		ecx,[edi]
        add		ebp,[edi+0x4]

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        call	hitLU

        mov		eax,[edi+0xc]
        add		ecx,[edi+0x8]
        cmp		eax,-1
        jz		short	mUL0111
        add		ebp,eax

        mov		eax,CELLWIDTH
        mov		edx,CELLHEIGHT
        mov		[esp+0x10],ecx			//xtL
        mov		[esp+0x18],ebp			//ytU
        add		eax,ecx
        add		edx,ebp
        mov		[esp+0x14],eax			//xtR
        mov		[esp+0x1c],edx			//ytD

        push	esi
        jmp		hitLU
mUL0111:
        push	esi
nohit:
        ret
    }
}










__declspec ( naked ) celltouch(VOID)
{	__asm								//EDI=addr of CELL's info dword
    {
        mov		eax,[edi]
        test	eax,0x20
        jnz		short	ct1			//jmp	if CELL is out

        cmp		[NoTouch],1
        jz		short	ct1

        test	eax,0xc0
        jz		short	ct2			//jmp	if one-touch CELL
        test	eax,0x20000000
        jnz		short	ct3

        mov		edx,eax
        and		[WhichSound],0
        and		edx,0x0000ff00
        or		eax,0x20000000
        sub		dh,14
        and		eax,0xfff000ff
        or		eax,edx
        mov		edx,CELLPAUSE
        shl		edx,16
        or		eax,edx
        mov		[edi],eax
ct1:
        ret
ct2:
        and		eax,0xdfe0ffff			//HIT CELL OFF
        mov		[WhichSound],1
        or		eax,0x20010000
        mov		edx,[auxEbx]
        mov		[edi],eax
        mov		eax,[edx+0x3c]
        and		eax,0x2
        shl		eax,19
        or		[edi],eax
        ret
ct3:
        and		[WhichSound],0
        test	eax,0x40
        jnz		short	ct1			//jmp	if no-out CELL

        and		ah,1
        add		ah,38
        and		eax,0xdfe0ff7f
        or		eax,0x20010000
        mov		edx,[auxEbx]
        mov		[edi],eax
        mov		eax,[edx+0x3c]
        and		eax,0x2
        shl		eax,19
        or		[edi],eax
        ret

    }
}




__declspec ( naked ) Xcelltouch(VOID)
{	__asm								//EDI=addr of CELL's info dword
    {									//RETURNS: C-CELL is NO-OFF
        mov		eax,[edi]
        test	eax,0x20
        jnz		short	ct1			//jmp	if CELL is out

        test	eax,0x40
        jnz		short	ct3

        and		eax,0xdfe0ffff			//HIT CELL OFF
        or		[Xsound],TRUE
        or		eax,0x20010000
        mov		edx,[auxEbx]
        mov		[edi],eax
        mov		eax,[edx+0x3c]
        and		eax,0x2
        shl		eax,19
        mov		edx,[SoundChosen]
        or		[edi],eax

        test	edx,edx
        mov		eax,0x1ffff
        jns		short	sch1
        mov		[SoundChosen],eax
        ret
sch1:
        and		edx,0xffff0000
        or		edx,1
        mov		[SoundChosen],edx
ct1:
        ret

ct3:
        and		[WhichSound],0
        test	eax,0x20000000
        jnz		short	ct4

        mov		edx,eax				//NO-OFF CELL
        and		edx,0x0000ff00
        or		eax,0x20000000
        sub		dh,14
        and		eax,0xfff000ff
        or		eax,edx
        mov		edx,CELLPAUSE
        shl		edx,16
        or		eax,edx
        mov		[edi],eax
ct4:
        stc
        ret
    }
}






__declspec ( naked ) goRUorU(void)
{	__asm
    {
        mov		eax,[ScHpix]
        xor		edi,edi
        push	eax				//ytD
        push	edi				//ytU
        mov		eax,[ScWpix]
        mov		esi,[ebx+0x34]
        push	eax				//xtR
        add		esi,ecx
        sub		eax,15
        mov		[WhichSound],2
        push	eax				//xtL

        push	esi				//ESI=yoD
        push	ecx				//EAX=yoU

        mov		esi,[ebx+0x30]
        add		esi,ebp
        push	esi				//ESI=xoR
        push	ebp				//EAX=xoL

        call	hitRU		//RIGHT wall



        mov		[esp+0x10],edi		//xtL
        mov		[esp+0x18],edi		//ytU
        mov		eax,[ScWpix]
        lea		edx,[edi+15]
        mov		[esp+0x14],eax		//xtR
        mov		[esp+0x1c],edx		//ytD

        call	hitRU		//UPPER wall


        cmp		[DoublePlay],TRUE
        jnz		short	nodp

        mov		esi,[lpDyn]
        mov		eax,[esi+ONEDYNOBJ*12+0x28]
        mov		edx,[esi+ONEDYNOBJ*12+0x2c]
        mov		[esp+0x10],eax
        mov		[esp+0x18],edx
        add		eax,[esi+ONEDYNOBJ*12+0x30]
        add		edx,[esi+ONEDYNOBJ*12+0x34]
        mov		[esp+0x14],eax
        mov		[esp+0x1c],edx

        call	hitRU						//PARTITION
nodp:

        call	squeakercheck				//8 SQUEAKERS
        
        mov		esi,offset hitRU		//CELLS
        call	hitcells



                                    //DIRECTION SELECTION (right-up)

        mov		ecx,[hitting]		//0bit=1 => hit RIGHT wall
        test	ecx,ecx				//3bit=1 => hit UPPER wall
        jz		short	hrwee

        mov		ebp,[auxEbx]
        dec		[HitCnt]			//Prevent BALL sticking
        jnz		short	hcnt1
        or		byte ptr [ebp+0x3c],0x1
hcnt1:
        cmp		ecx,0x9
        jz		short	hrwe3
        cmp		ecx,0x1
        jz		short	hrwe2

        mov		ecx,360				//hit target higher
        mov		esi,[hitcorner]
        mov		edx,[ebx+0x30]
        shr		edx,1
        cmp		esi,1
        jnz		short	nos1
        mov		eax,[esp+0x14]
        sub		eax,[esp+0x0]		//xtR-xoL
        cmp		eax,edx
        jb		short	swerve
nos1:
        cmp		esi,2
        jnz		short	noswerve
        mov		eax,[esp+0x4]
        sub		eax,[esp+0x10]		//xoR-xtL
        cmp		eax,edx
        jae		short	noswerve
swerve:
        sub		edx,eax				//SWERVE BALL a little
        shr		edx,1
        call	rnd
        jc		short	swe_p
        mov		eax,[ebp+0x18]
        cmp		eax,edx
        jbe		short	noswerve
        add		ecx,edx
        jmp		short	noswerve
swe_p:
        sub		ecx,edx
        jmp		short	noswerve

hrwe2:
        mov		ecx,180				//hit target to the right
        mov		esi,[hitcorner]
        mov		edx,[ebx+0x34]
        shr		edx,1
        cmp		esi,4
        jnz		short	nos2
        mov		eax,[esp+0xc]
        sub		eax,[esp+0x18]		//yoD-ytU
        cmp		eax,edx
        jb		short	swerve2
nos2:
        cmp		esi,2
        jnz		short	noswerve
        mov		eax,[esp+0x1c]
        sub		eax,[esp+0x8]		//ytD-yoU
        cmp		eax,edx
        jae		short	noswerve
swerve2:
        sub		edx,eax				//SWERVE BALL a little
        shr		edx,1
        call	rnd
        jc		short	swe_pp
        mov		eax,90
        sub		eax,[ebp+0x18]
        cmp		eax,edx
        jbe		short	noswerve
        sub		ecx,edx
noswerve:
        sub		ecx,[ebp+0x18]
        jmp		short	hrwe4
swe_pp:
        add		ecx,edx
        jmp		short	noswerve

hrwe3:
        mov		ecx,[ebp+0x18]
        add		ecx,180			//hit both upper and right walls

hrwe4:
        mov		[hitcorner],0
        cmp		ecx,360
        jb		short	hrwe5
        test	ecx,ecx
        js		short	hrwe40
        sub		ecx,360
        jmp		short	hrwe4
hrwe40:
        add		ecx,360
        jmp		short	hrwe4

hrwe5:
        mov		[ebp+0x18],ecx
hrwe55:
        mov		eax,[SoundChosen]		//Play hit sounds
        test	eax,eax
        js		short	hrwe1
        mov		ecx,eax
        and		eax,0xf
        shr		ecx,16
        cmp		eax,0xf
        jnz		short	ps1
        push	ecx
        call	soundplay
        jmp		short	hrwe1
ps1:
        push	eax
        push	ecx
        call	soundplay
        call	soundplay
hrwe1:
        and		[Xsound],0
        add		esp,0x4*8
        ret

hrwee:
        mov		[HitCnt],INITSTUCK
        cmp		[Xsound],TRUE
        jz		short	hrwe55

        add		esp,0x4*8
        ret
    }
}






__declspec ( naked ) hitRU(LONG xoL,		//ESP+0x4==new object left X
                           LONG xoR,		//ESP+0x8==new object right X
                           LONG yoU,		//ESP+0xC==new object up Y
                           LONG yoD,		//ESP+0x10==new object down Y
                           LONG xtL,		//ESP+0x14==hit target left X
                           LONG xtR,		//ESP+0x18==hit target right X
                           LONG ytU,		//ESP+0x1C==hit target up Y
                           LONG ytD)		//ESP+0x20==hit target down Y

                           //EBX=must point at OBJECT info
{	__asm					//RETURN: NC-if not hit
    {
        mov		eax,[esp+0x14]			//xtL
        mov		edx,[esp+0x18]			//xtR
        sub		eax,[esp+0x8]			//xoR
        sbb		edx,[esp+0x4]			//xoL
        xor		eax,edx
        jns		short	nohit

        mov		eax,[esp+0x1c]
        mov		edx,[esp+0x20]
        sub		eax,[esp+0x10]
        sub		edx,[esp+0xc]
        dec		edx
        xor		eax,edx
        jns		short	nohit

        mov		eax,[ebx+0x2c]
        sub		eax,[esp+0x20]			//EAX=(old)yoU-ytD
        jbe		short	hru5			//jmp if hit -> way (or |^ if ZERO)
        mov		[r],eax

        mov		edx,[ebx+0x28]
        mov		eax,[esp+0x14]
        add		edx,[ebx+0x30]
        sub		eax,edx					//EAX=xtL-(old)xoR
        jbe		short	hru20			//jmp if hit |^ way

        fild	dword ptr [r]
        mov		[i],eax	

        fidiv	dword ptr [i]			//st0=((old)yoU-ytD)/(xtL-(old)xoR)
        
        fld		[fYplus]
        fdiv	[fXplus]				//st1=fYplus/fXplus

        fcompp							//compare: st0 vs st1 and POP both
        fnstsw	ax
        test	ah,0x41
        jz		short	hru20			//jmp if hit |^ way


        fild	dword ptr [i]		//RIGHT-UP HIT (target to the right)
        fld		st(0)

        fmul	[fYplus]
        fdiv	[fXplus]

        fist	dword ptr [i]
        fstp	[fYplus]

        fist	dword ptr [r]
        fstp	[fXplus]

        mov		eax,[r]
        mov		edx,[i]
        add		[esp+0x4],eax		//update xoL, xoR, yoU, yoD
        add		[esp+0xc],edx
        add		[esp+0x8],eax
        add		[esp+0x10],edx

        or		[hitting],0x1

        mov		edx,[SoundChosen]
        mov		eax,[WhichSound]
        .           Bãö+ö+  åö+3D    ..          Bãö+ö+  åö+    ÂM F C s p  êl i t . n c   b   ÂFCSPLITNCB  Bãö+Ã,  3ÉÃ,4D  ÂR e a d M  se . t x t     ˇˇˇˇÂEADME  TXT  Hãö+ö+  ‚ú°*=D  ÂM F C s p  *l i t . d s   w   ÂFCSPLITDSW  Iãö+Ã,  „ú°*>D  ÂM F C s p  [l i t . h     ˇˇˇˇÂFCSPLITH    Iãö+Ã,  ‚ú°*?Db  ÂM F C s p  ·l i t . c p   p   ÂFCSPLITCPP  Iãö+Ã,  ß¢*@Dï  ÂS t d A f  ‡x . h   ˇˇˇˇ  ˇˇˇˇÂTDAFX  H    Iãö+Ã,  ‚ú°*AD  ÂS t d A f  gx . c p p     ˇˇˇˇÂTDAFX  CPP  Iãö+Ã,  ‚ú°*BD“   ÂM a i n F  ür m . h   ˇˇ  ˇˇˇˇÂAINFRM H    Jãö+Ã,  ù¥h+CDv  ÂM a i n F  ¶r m . c p p     ˇˇÂAINFRM CPP  Lãö+Ã,  ∫h+DD˜  ÂM F C s p  ˘l i t D o c   . h ÂFCSPL~1H    Sãö+Ã,  ‚ú°*EDŸ  Âp p   ˇˇˇˇ  ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂM F C s p   l i t D o c   . c ÂFCSPL~1CPP  Vãö+Ã,  ‚ú°*FDˆ  Âh   ˇˇˇˇˇˇ Zˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂM F C s p  Zl i t V i e   w . ÂFCSPL~2H    Wãö+Ã,  ´±h+GD  Âc p p   ˇˇ ‡ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂM F C s p  ‡l i t V i e   w . ÂFCSPL~2CPP  Zãö+Ã,  ≠±h+HDz
  Âr e s o u  r c e . h     ˇˇˇˇÂESOURCEH    [ãö+Ã,  Âû®*IDÍ  ÂM F C s p  ol i t . r c     ˇˇÂFCSPLITRC   ^ãö+Ã,  Ôûh+JDÙ?  ÂM F C s p  #l i t . d s   p   ÂFCSPLITDSP  `ãö+Ã,  πh+KD  ÂM F C s p  öl i t . p l   g   ÂFCSPLITPLG  cãö+ö+  ∫h+LDe  ÂF o r m V  i e w 1 . h     ˇˇÂORMVI~1H    dãö+Ã,  °h+MD‹  ÂF o r m V  i e w 1 . c   p p ÂORMVI~1CPP  gãö+Ã,  â≥h+ND8  ÂL i s t V  Çi e w 1 . h     ˇˇÂISTVI~1H    iãö+Ã,  Â±h+OD´  ÂL i s t V  âi e w 1 . c   p p ÂISTVI~1CPP  oãö+Ã,  Ò±h+PD’  Âs d i . i  c o   ˇˇˇˇˇˇ  ˇˇˇˇÂDI     ICO  qãö+ö+  Wû°*QD6  ÂP r o p 1  ˙. h   ˇˇˇˇˇˇ  ˇˇˇˇÂROP1   H    sãö+Ã,  vu¢*RD
  ÂM F C s p  Ël i t . o p   t   ÂFCSPLITOPT  ;3ÉÃ,Ã,  4ÉÃ,»* “  ÂP r o p 1  . c p p   ˇˇ  ˇˇˇˇÂROP1   CPP  wãö+Ã,  v¢*WD  ÂD i a l o  lg 2 . h   ˇˇ  ˇˇˇˇÂIALOG2 H    {ãö+Ã,  Sx¢*XD≤  ÂD i a l o  Úg 2 . c p p     ˇˇÂIALOG2 CPP  {ãö+Ã,  Xx¢*YD	  ÂP a g e 1  í. h   ˇˇˇˇˇˇ  ˇˇˇˇÂAGE1   H    {ãö+Ã,  =y¢*ZD‰  ÂP a g e 1  ô. c p p   ˇˇ  ˇˇˇˇÂAGE1   CPP  {ãö+Ã,  Cy¢*[Dj  ÂL i s t 1  . c p p   ˇˇ  ˇˇˇˇÂIST1   CPP  {ãö+Ã,  }~¢*\DQ  ÂC a p t i  ™o n . h   ˇˇ  ˇˇˇˇÂAPTION H    |ãö+Ã,  'µ¢*]Dö  ÂC a p t i  1o n . c p p     ˇˇÂAPTION CPP  |ãö+Ã,  ¿π¢*^Di  ÂM F C s p  Êl i t . c l   w   ÂFCSPLITCLW  |ãö+Ã,  ôi+_DA  ÂN e w V i  ¢e w . h   ˇˇ  ˇˇˇˇÂEWVIEW H    ~ãö+Ã,  U∏®*`D™  ÂN e w V i  )e w . c p p     ˇˇÂEWVIEW CPP  Äãö+Ã,  ´i©*aDâ  ÂN e w L i  Ûs t C t r l   . h ÂEWLIS~1H    Çãö+Ã,  ®*bDÖ  Âp p   ˇˇˇˇ ˙ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂN e w L i  ˙s t C t r l   . c ÂEWLIS~1CPP  Ñãö+Ã,  ;®*cDﬁ  Âh   ˇˇˇˇˇˇ 1ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂS m a l l  1T o o l b a   r . ÂMALLT~1H    Öãö+Ã,  ˙¥®*dDè  Âc p p   ˇˇ ˜ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂS m a l l  ˜T o o l b a   r . ÂMALLT~1CPP  Öãö+Ã,  Eµ®*eDæ  ÂN e w S p  l i t t e r   . h ÂEWSPL~1H    áãö+Ã,  ⁄ºµ*fD}  Âp p   ˇˇˇˇ ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂN e w S p  l i t t e r   . c ÂEWSPL~1CPP  àãö+Ã,  ÏΩµ*gD˚  ÂN e w P a  ˝n e . h   ˇˇ  ˇˇˇˇÂEWPANE H    âãö+Ã,  ‘±h+hDF  ÂN e w P a  n e . c p p     ˇˇÂEWPANE CPP  ããö+Ã,  Á±h+iD˘  ÂN e w P a  ≥n e V i e w   . h ÂEWPAN~1H    çãö+Ã,  »±h+jD{  Âp p   ˇˇˇˇ ∫ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂN e w P a  ∫n e V i e w   . c ÂEWPAN~1CPP  çãö+Ã,   ±h+kDK  ÂM F C s p  dl i t . a p   s   ÂFCSPLITAPS  éãö+ö+  ¶±h+lDXã  ÂS h e e t  ^. h   ˇˇˇˇˇˇ  ˇˇˇˇÂHEET   H    êãö+Ã,  E≤h+oD‹  ÂS h e e t  ‰. c p p   ˇˇ  ˇˇˇˇÂHEET   CPP  ìãö+Ã,  G≤h+pD  ÂS p l i t  1. h   ˇˇˇˇˇˇ  ˇˇˇˇÂPLIT   H    îãö+Ã,  8¥h+qDc  ÂS p l i t  ˜. c p p   ˇˇ  ˇˇˇˇÂPLIT   CPP  ïãö+Ã,  8¥h+rD¨  Âp   ˇˇˇˇˇˇ ±ˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂM y S p l  ±i t t e r .   c p ÂYSPLI~1CPP  ïãö+Ã,  ;∏h+sDÑ˛  ÂM y S p l  *i t t e r .   h   ÂYSPLI~1H    öãö+Ã,  o∑h+wD¶  Âr e s   ˇˇ äˇˇˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂES         õãö+ö+  åö+xD    ÂD e b u g  ⁄  ˇˇˇˇˇˇˇˇˇˇ  ˇˇˇˇÂEBUG       £ãö+ö+  åö+ÄD    ÂVC9263 TMP  ;3ÉÃ,Ã,  4ÉÃ,»* “  ÂVC9264 TMP  sãö+Ã,  ≤j+SD “                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  Microsoft C/C++ program database 2.00
JG      Å Z  È|                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         Äˇˇ    ‡ˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇ            ¿ÛˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇˇÖ           ˜           e            X            M            )         q          µ        ' G 2         ˇ        I V °       	 " % ç       
         
 
  + Ö   Ñ   ƒ   …   ‹   Â   Ñ     ±   Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ,∫0ƒÿÍ;     /names /ncb/targetinfo /ncb/moduleinfo /ncb/storeinfo /ncb/iinstdefs /ncb/module/D:\C++\MFCsplit\Caption.cpp /ncb/module/D:\C++\MFCsplit\Dialog2.cpp /ncb/module/D:\C++\MFCsplit\FormView1.cpp /ncb/module/D:\C++\MFCsplit\ListView1.cpp /ncb/module/D:\C++\MFCsplit\MainFrm.cpp /ncb/module/D:\C++\MFCsplit\MFCsplit.cpp /ncb/module/D:\C++\MFCsplit\MFCsplitDoc.cpp /ncb/module/D:\C++\MFCsplit\MFCsplitView.cpp /ncb/module/D:\C++\MFCsplit\NewListCtrl.cpp /ncb/module/D:\C++\MFCsplit\NewPane.cpp /ncb/module/D:\C++\MFCsplit\NewPaneView.cpp /ncb/module/D:\C++\MFCsplit\NewSplitter.cpp /ncb/module/D:\C++\MFCsplit\NewView.cpp /ncb/module/D:\C++\MFCsplit\Page1.cpp /ncb/module/D:\C++\MFCsplit\Prop1.cpp /ncb/module/D:\C++\MFCsplit\SmallToolbar.cpp /ncb/module/D:\C++\MFCsplit\StdAfx.cpp /ncb/module/D:\C++\MFCsplit\Caption.h /ncb/module/D:\C++\MFCsplit\Dialog2.h /ncb/module/D:\C++\MFCsplit\FormView1.h /ncb/module/D:\C++\MFCsplit\ListView1.h /ncb/module/D:\C++\MFCsplit\MainFrm.h /ncb/module/D:\C++\MFCsplit\MFCsplit.h /ncb/module/D:\C++\MFCsplit\MFCsplitDoc.h /ncb/module/D:\C++\MFCsplit\MFCsplitView.h /ncb/module/D:\C++\MFCsplit\NewListCtrl.h /ncb/module/D:\C++\MFCsplit\NewPane.h /ncb/module/D:\C++\MFCsplit\NewPaneView.h /ncb/module/D:\C++\MFCsplit\NewSplitter.h /ncb/module/D:\C++\MFCsplit\NewView.h /ncb/module/D:\C++\MFCsplit\Page1.h /ncb/module/D:\C++\MFCsplit\Prop1.h /ncb/module/D:\C++\MFCsplit\Resource.h /ncb/module/D:\C++\MFCsplit\SmallToolbar.h /ncb/module/D:\C++\MFCsplit\StdAfx.h /ncb/module/D:\C++\MFCsplit\Sheet.cpp /ncb/module/D:\C++\MFCsplit\Sheet.h /ncb/module/D:\C++\MFCsplit\Split.cpp /ncb/module/D:\C++\MFCsplit\Split.h /ncb/module/D:\C++\MFCsplit\MySplitter.cpp /ncb/module/D:\C++\MFCsplit\MySplitter.h /ncb/target/MFCsplit - Win32 Release /ncb/target/MFCsplit - Win32 Debug /ncb/versioninfo 1   ^      Ùï ˇi·]|&    ‡     ∂  2   €  3   '      £     -     ø             .   ©  +   W  )   ï      Á           {     3  (   ç     ≥     m   
         Ù  -   ç  1   ?     ì     S  É  6     / / Û        5 5 p        C C ˜         / / d           X           M   n        z       5 9 z       / 3 z       C E -  ∫      N       & , ã       D D ƒ       1 1        L V ¸        $ §      @ A       
 
 x  
   É  É  É  Ñ   @    g  Ñ   z  §  ±  Ñ   €  Â      ∫                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         =        ) ) ˜         ) ) ò           X            ¶  ·        ‚           M            2      ) - {      3 6 d        Í         & ì      8 ; G      + + »      
 v Ä µ       C k       
 o t ∞  e               _ =    Ñ     g    §  Ñ   ƒ   ·    ƒ   !  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ç          ˜           ¿           X            M   ∫        Ç  	     ! j      0 3 ‹         ·       > u ≥     
 5 8 û         Œ       & * Ö       Ü ã ó       w Ñ ƒ  π            
 
  ,
 ç    Ñ   ƒ   Ù  
  !  /  @  ‹   Q    §  Ñ     ·  ƒ   ·    Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ?         ˜           e           ò           ¿           X                                  M   c        ◊         '      ` c q       * .       
 ≥ ˚      e h ‚      
   à       4 P $       p ¨ I       Æ ± ª       R Z        0 2         ]  d    % 4 
 ?    Ñ   ƒ   ß  /  7  ?  Y  Q    §  Ñ   ±   ù  ƒ   ?  Q    ƒ   !  Ñ      h                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ¿       e { É      e e   	      ˜         X   0       p  Â       Ä  &                 M   n       ¿       ä é    @   } Å    @  h h ‰      r r ‰    *  É à ˛       l l É  4 @   e f ≥         Ω      	 # ' ë  &   
 1 _      
                              ÿ       ë ï      
 
 ﬂ  	 @   l l ¨      , , & &      0 & &       É  Ñ   Ñ     g    g    Ñ   ƒ   Ñ   e  r  Ñ   •  ≤  Ñ   7  K  Ñ   Ü  í       ≥                                                                                                                                                                                                                                                                                                                                                                                                                                                                  z	          ˜   &       X   M        p           M   c        m	        ≥	      G J 	        ! Ñ	     L O ó	       ' 0 H	      7 A ,	  &   	 # %      
   & z	    Ñ     §  ƒ     `	  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ∫
          ˜   u       X            p           Ä  n        M            ?               T W ›
       ! % +     Y \ ?
       ^ b M     	   M  I      M         ˘         à
       F I ˙
       6 ; ¿
       K N ›      h m Ó	       @ D i
      + 1 !
       ' )          n 
 )
 
 ú ∫
    Ñ     §  Z
  Ñ   ó  ¨  Ñ   ø  ¨  Ñ   g  }  Ñ     ß
  ≠
    ·    ß
  ≠
  ƒ   ·  ƒ   
  ƒ   !  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  %          ˜          X            Ì           M   n          &      ˝  &      /         f       " W J               
 
   & %  Ñ   Ñ   ±   ù  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  •  c       ˜           X   Q        ô           M           æ         «        j         S       # ( ™               
 
 “ , •  Ñ   Ñ   ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ∫
          ˜          X            ˙          M            A
         N
     . 1 &
         h
      3 6 4         {
       $ (       
 < A 

               
 
   	 ∫
    Ñ     §  Ñ     ·  ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              Õ
          ˜          X            ∞
  Q        M            ¿
        ! t        Û
         ⁄
        D       1 6       	 8 > ´     
 & - è              
 
 & &   Õ
  Ñ   Ñ   Ñ     b    +  7    ¿  À  “  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ∫
        - - ˜         - - X            Ì        
 
            M                   - 2 M      Q T c        & 	  ù   V Y !       / / 8       0 0 ⁄  d   
 ï û «       _ Ü v       7 K á       ° ≤ Ó       à ì ı  y    ( *   ˇ   
 
 & &
   ∫
    Ñ     §  Ñ   Ñ   ƒ   ö  ®  ±   ù    ·  ƒ   ö  ®  ∂    ¿  À  “  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            Â     & & ˜        & & X   &        ¶           M   n        s      & * 7         \        # z      ( ( F       / 9 '     	        
 
 
 & &   Ñ     g  Ñ   €  Â    Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ˝        # # ˜        # # X   
        ©           M            ˆ       # ' ≥         ≥  U   	        
 , /                
 
 = ˝  Ñ   Ê  @  ’  Ñ   ¬  @  ’    Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  ó         ˜          X           K           M   &        §        \  U      ≤         “       " - y             
 
 ˇ   ó  Ñ   Ñ   ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  M   c                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        )  c      + Ö          q   @    µ    @    ˇ      ' ' °    @      #   (  % % ç   @ 	 ! ! Ö   4@  	  
 &             Ñ   ƒ   …   ‹   Â     ±   4  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   É  &       z  n      0 É  4&@     
 -   @    N       m             ' ' ¸   @    ﬂ  	 @       &           Ñ   X    g                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  =          2  
  
    = e   n  
     ü    
     =  4 @ 
     {   
  4 4 d    
    Í    
  * * ì   
  5 5 Ê     
    »    
  : : µ   @
     ( (     
  + + Õ   (
  1 1 ™   (
 
 0 0 ∞   
  2 2 ﬂ  	 @ 
        Ú          
   i .  
  	   Ñ     g    §  ƒ   ·    ƒ   !  )  ¬  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ç          Ç         5 e   Q     
 
        	 	 ç  4n@      j     * * ‹  @    ·  @ 
 ! ! ≥     + + Œ      $ $ Ö     2 2 ó  @  " " h    (  0 0 *  @     ƒ   @    @ . 2
     	   4   
   Ñ   ƒ   Ù  
  !  /  @  ‹   H    §    ·  ƒ   ·    )  <  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ?          ◊  &      B e           ¿  n       ô        	 	 ˙       
 
 ∞
         ?  4@      '   H  ) ) q          -@ 	 % % ˚   H  * * à  c  
 = = $  ·   ; ; I   @  & & ª   @      V  u (  7 7 ;    (  6 6 q  Q (  8 8 ã   (  9 9 °  Ω (  : : !    (  5 5     (  4 4 õ    (  . . ¡    (  / / „  ô (  3 3    @  ' ' Ù (	   
     ˆ  ”  à   €          
 M   Ñ   ƒ   ß  /  7  ?  Y  Q    §  ±   ù  ƒ   ?  Q    ƒ   !  )  )  )  æ  A
  Õ
  ¿
  ∂  ⁄  ˘  Ñ                                                                                                                                                                                                                                                                                                                                                                                                           ≥  ·      (          ÷  c       Ω  @    ë  @    ÿ   @  # #   4e@     3 &     Ñ   ƒ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         z	         m	  c      1 z	  4&@     
 ≥	  H  $ $ 	        Ñ	   H  % % ó	  @    H	  @    ,	  @  " " ? &           Ñ     §  ƒ     `	  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ?  c      5 ∫
          H  ) ) ›
        +  &H  * * ?
  @    à
      ! ! ˙
  @ 
   ¿
     
 " " ›      2 2 Ó	         i
   @    !
   @  ' ' ∫
  4 @    
 0
          
         •  	   Ñ     §  Z
    ·  
    ·    ·  
  ƒ   ·  ƒ   
  ƒ   !  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   %            n      ( %  4&@     
 ˝  @    f     $ $ J  @    ? & ¸   Ñ   ±   ù  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   æ         ' •         «  @    S      $ $ ™  Q@    •  4 @    
 e .     Ñ   ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   A
  n      - ∫
         N
    # # &
   @    h
  &  $ $ {
             * * 

   @ 	   ∫
  4Q@  	  
 C *     * *     Ñ     §    ·  ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ¿
  &      - Õ
        ü  c     
 
 t   @    D   @         @  * * ´   @ 
 ) ) è   @  % % Õ
  4@     9 &         Ñ     b    +  7    ¿  À  “  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    6 ∫
   
     Ì  n  
     K  &  
     M  ≈
  ( ( c    
    	  È
  ) ) ⁄   
      «    
     / / v   
    á  } 
  ! ! Ó    
  0 0 œ    $
  5 5 Æ  @ 
    Ω   $
  4 4 ı  ° 
  & & ∫
  4 @ 
     &  
  	    
                Ñ     §  ƒ   ö  ®  ±   ù    ·  ƒ   ö  ®  ∂    ¿  À  “  ‰    §  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   s  c      ,         7   @    \     ! !          F     ( ( '  @      4 @    
 ﬂ  	@     9 &           Ñ     g    Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ˆ         + ˝         ≥   @    ≥  @      @ 	 " "   @ 
 # # ˝  4@  
  
 . &     Ñ   ¬  .  F  Ñ   Ê  .  F    Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       §        ) ó  c      \  @    “      % % y   @      ó  4@    
 &       Ñ   ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   “          π          ≈           ü           Æ  c                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ˝  ›     ! ! ˜         ! ! ‚  Å        ’          M            Ï       ! %                	   =      
 # # V       * / Û      
     l   
 
 7  ˝  Ñ   Ê  @  ’  Ñ   ¬  @  ’  Ñ   ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ˝         Ï         * ˝  4@     
    @      @    V     
 ' ' Û  @  " " * *       Ñ   Ê  .  F  Ñ   ¬  .  F  ƒ   ·  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ≈        # # ˜         # # Ö  Å        ’  ›        M            “       # ' Ÿ         è          µ               
 
   ≈  Ñ   ƒ   ¶  Q  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      ≈         “  M      . ü  n     
 
 ≈  4 @      Ÿ        è      " "      (    µ   @  ' ' 4        Ñ   ƒ   ?  Q  Õ
  Ñ                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ¡*         > ˜        { { û-          M              -   @  : : G-   @  < < a-   @  = = 
-   @   7 7 Â,   @   5 5 ¯,   @   6 6 ’,   @   4 4 ƒ,   @   3 3 •+   @  	 $ $ ⁄+   @  
 $ $ √+   @   $ $ Ò+   @   % % ,   @  
 % % +   @     Ú*   @     <+   @      *   @     (+   @     ﬁ*   @     +   @     V+   @     a,   @   - - },   @   . . ï,   @   / / ê+   @   ! ! p+   @       :,   @   ) ) N,   @   * * ,   @   ( ( ≤,   @   2 2 3-   @  ; ; 8     -   { è {;     - " Ï	<    - # ?	I	ΩB     - $ W s [;     - & ›Í18     - . ë ∏ £?     - 3 — t:     - 6 oÄH8     - < ª œ e8     - B m;     - D  GË:     - F ˜ò:     - I 0<ò;     - J ≥÷∏;     - N $Å—;     - R ÉÿN@     - V ˝2(<    - X K	d	;     - [ %	9	Ò8    - _ Œ’ˆ?     - b Yÿ?    - d M\k9    - g êóª8    - k ∫¡-9    - l ‚Íé@     - m pê¿?    - o [óü9    - r ÇãÉ9     - v ô±SD     - w ˇˇˇˇ◊C     - x Ö Ö ûC     - y É É ZC     - z Å Å 8C     - { Ä Ä ~C     - | Ç Ç ¸B     - } ~ ~ D     - ~ á á C     -    ‹B     - Ä } } ºC     - Å Ñ Ñ 2D     - Ç à à ıC     - É Ü Ü ∆A     - Ñ EHÈ@     - á †ßùB     - à €ﬂ:     - å >qB     - ê „T:     - í ú≠„A     - ñ JçäA     - ô >lA     - ú  ™A     - ü @C8A     - ¢ Ω˛HB     - ¶ ∂·eB     - ® A     - ¨ ©±TA     - ≠ 4hA     - ± jp˝A     - µ ·ÁÄB     - ∏ èõ.B     - º !◊9     - Ω í˚=;     - ¡ 	#	9     - ≈ ◊‡÷8     - … √ÃL9     - À Ï+o@     - Õ wªÃ:     - œ ∆ı≥:     - — ïƒÆ@     - ” ^ûÃ@     - ’ “.@     - ÿ π–@     - € †∑Ï;     - ‹ u y Ô  	   - ‹ C O     - › 	 	 D     - · vì•"     - È In©      - Ó ÏDô$     - Ô ttê     -  ss˚     - Ò uuß*    - Ú @ @ ± 4 !   Í 4         
               !   
  	 $ $ "    Í 4I F -H qD uC E -B 1J  A L  G  K õ@ Ñ       ƒ   ƒ   ƒ   ƒ   ƒ   ∫+  ∫+  ∫+  ∫+  ∫+  ±   ±   ±   ±   ±   ±   ±   ±   w,  w,  ™,  â+  â+  2,  2,  2,  §   Ñ   •    ‰    Ñ   ƒ   ƒ   ƒ   @  X  e  r  Q  !  ‹   ƒ   @  r  !  ‹   ƒ   !  ‹   ƒ   @  ö  §  !  ‹   ƒ   /  7  ?  Y  Q    j%    s     /  7  ƒ   ƒ   !  2  C  ƒ   !  ë%  C    ·  ’  ‚    §  h    &    7  ∑#  ¬#    —!  D"    À  h  /  7    /  #   .   ˛$  h  ±   :  ±   /  7  ƒ   ˜    &  Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ   Ñ     ƒ   ö  ®      ·  $  $    á!  î!  û!    b    ô   §   §     §   :    §   :    §   "    §   :  ƒ   ‚  Ó  7  ƒ   ?  ƒ   ö  ®  ∂    ƒ   ˜  \  j    ¿  À  “    ‹   ®    á!  î!  û!      /  7  ˜    7  Y!  e!    /  G$  S$    !    —!  ƒ   W#  ƒ   t$    —!        ¡  7    ◊"  /  Ñ      W  µD  ç  ù  ∫   ∏"  ˜  ¡  ◊"  À  “  ›"  ∫   ﬁD     ˘   ù  û  û    Ø*      ¡  7    ◊"  /  Ñ      8  n  \    ± =<   @ _   l r •  ì _     {;   @_  Z Z <   H_  g g ΩB   @_    ß<   @ _  q q <   @ _  o o Y<   @ _  n n [;   @_ 	 Y Y 18   @_    £?    _  ç ç t:   @_  H H H8   @_    e8   @_ # ! " ;   @_ % O O Ë:   @_ ' N N ò:   @_ * K K ò;   @_ + [ [ ∏;   @_ / ^ _ —;   @_ 3 ` a N@    _ 7 ì ì (<   H_ 9 h h Ù9  	  (_ 9 B B ;   @_ < R R ;    _ ? U U ú8   H_ @ ' ' Ò8   H_ D , , ˆ?    _ G ê ê ÿ?   _ I è è k9   H_ L 4 4 Ä8   H_ M & & ª8   H_ Q * * -9   H_ R 0 0 é@    _ S ï ï ¿?   _ U é é ü9   H_ X 7 7 É9   @_ \ 6 6 É9   @_ ` 5 5 º9   @_ a 9 9 ∆A    _ b ¶ ¶ È@    _ e ú ú ùB    _ f Æ Æ :    _ j C C B    _ n © © T:    _ p D D „A    _ t ß ß äA    _ w § § lA    _ z £ £ ™A    _ } • • 8A    _ Ä ° ° HB    _ Ñ ´ ´ eB    _ Ü ¨ ¨ A    _ ä ù ù TA    _ ã ¢ ¢ A    _ è † † ˝A    _ ì ® ® ÄB    _ ñ ≠ ≠ .B    _ ö ™ ™ ◊9   @_ õ = = =;   @_ ü S S 9   @_ £ - - ÷8   @_ ß + + L9   @_ © 1 1 o@    _ ´ î î Ã:   @_ ≠ M M ≥:   @_ Ø L L Æ@    _ ± ò ò Ã@    _ ≥ ô ô .@    _ ∂ í í @    _ π ë ë G>    (_ ∫ Å Å e>    (_ ª Å Å ◊>    (_ º Ü Ü Û>    (_ Ω Ü Ü ·=    (_ æ } } a=    (_ ø { { '=    (_ ¿ z z °=    (_ ¡ | | ¸=    (_ ¬ } } Å=    (_ √ { { D=    (_ ƒ z z ¡=    (_ ≈ | | â?    (_ ∆ ä ä />    (_ « Ä Ä =    (_ » w w Ò<    (_ … w w >    (_   Ä Ä É>    (_ À Ç Ç Õ<    (_ Ã v v º>    (_ Õ É É ?    (_ Œ á á 0?    (_ œ à à L?    (_ – â â j?    (_ — â â Ï;   @_ “ e e •  4 @ _ “  
  ` 8  
  
    # #  +>  = ! >?   % & $ ' #;   D+ D- S e  B  A     ƒ 8 <  	        _     Y  V U  M  Q L P  N  R 8K O  W  T õG H  X  Z ¢I J  [ \ "] ^  S    #      F  E    @  "  C  D  )  5  7  2  6  0  /  1  (  .  8  ,  :  3  4  9  *   $   /'    Ñ   ±   ±   ±   ƒ   /'  ƒ   @  X  e  r  Q  €%  ƒ   @  r  !  ‹   ƒ   !  ‹   ƒ   @  ö  §  6&  ƒ   /  7  ?  Y  Q    j%    s     /  7  ƒ   ƒ   !  2  B'  ƒ   !  ë%  B'    ·  ’  ‚    §  h  ˚&  '  h  ì&  ú&  ±     7  ∑#  ¬#    —!  À    À  h  /  7  ±     /  #   .   ˛$  h  ±   :  ±   /  7  ƒ   ˜  ì&  ú&  ƒ   ˜    &  ƒ     ƒ   ö  ®      ·  .:  b    á!  î!  û!    b    ô   ö*  +    +  :    +  :    +  :    +  :  ƒ   +  Ó  :  ƒ   ?  ƒ   ö  ®  ∂    ƒ   ˜  \  j    ¿  À  “    ‹   ®    á!  î!  û!      /  7  '    7  Y!  e!    /  G$  S$    !    —!  ƒ   W#  ƒ   t$    —!        ¡  7    ◊"  /  ƒ   ƒ   ƒ   ƒ   ±   ±   ±   ±   ±   ±   ±   ±   ±   ±   ±   ±   ±   û>  (  û>  "  C*  C*  C*  Ñ                                                                                                                                                                                                                                                                                                                  E  "   E   	   È   
   ~  *   ø     …                 b  0     !   ˛  4   Œ  ,     '        g     o  #         ø  %   >  /   È  &   6      f     ï  $   :                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  5   Z  È«  ,È    úÚÈ    ¨ÚÈ∫U  ºÚÈ   ÏÚÈ\  ¸ÚÈ   ÛÈ   ÛÈ  0ÛÈ|  @ÛÈd  PÛÈ\  `ÛÈ»  pÛÈ@  ÄÛÈ¸   êÛÈ  †ÛÈ‘   ∞ÛÈ‘   ¿ÛÈ  –ÛÈ(  ‡ÛÈú  ÛÈ    ÙÈ‰   ÙÈ‘    ÙÈ   0ÙÈ–   @ÙÈƒ   PÙÈ|  `ÙÈl  pÙÈx  ÄÙÈå   êÙÈÃ   †ÙÈ`  ∞ÙÈÄ   ¿ÙÈÄ   –ÙÈ–   ‡ÙÈ‘   ÙÈ†   ıÈ¿   ıÈ¨    ıÈ    0ıÈÄ   @ıÈP   PıÈ   `ıÈ∞   pıÈ¿   ÄıÈ¨   êıÈ‹  †ıÈ‘
  ∞ıÈN   ¿ıÈN   –ıÈ   ‡ıÈ|   ? P Q R S T U V W X Y Z [ \ ] ^ _ ` a b c d e D E F G H I J K A L M N O j                ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > B C k                                                                                                                                                                                                                                                                                                                                                                                                                                       —    i    t\[   h 
   
                     ! " # $                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   %             	 
   
                     ! " # $                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   %             	 
   
                     ! " # $                                                                       x\: Å H  N                                   Ä   @                             @                                                      XÈÔ:   z  ‚       P   B       @F
A                                                                                                           “‹Í; i ¢  û       P   B                   $ ÄXH                                                                                             à  ÷ŸÍ;   Û         X  à      @               †2I@å"                                                                                               $ËÍ;   S  ÿ         "                                                                        !@"Å                                       N<: ! Ò  ≥	  
   
    "                                                                                                                   òÿÓ:   Õ	  ˘        P   B                                             Ä !Üë                                                            Ä     VŸÍ;   Õ  f        H                                                                                            à@
                            í˜: w }  j       P   B                                                             ö                                                       à   ¬ŸÍ; ST⁄  4       P   B                                                              ¢ê                                                   B    åŸÍ; TYê
  ´        H                                                                                                          I            Ãp	; PIŸ  Ó        H                                             @                                    @                                 DA! B2
˘: _(ˇ  z     
  Ä   Ç         Ä                                   )   1   2  
                               ÄÄ  @                                                                                           x\: Å H  N                                   Ä   @                             @                                                      XÈÔ:   z  ‚       P   B       @F
A                                                                                                           “‹Í; i ¢  û       P   B                   $ ÄXH                                                                                             à  ÷ŸÍ;   Û         X  à      @               †2I@å"                                                                                               $ËÍ;   S  ÿ         "                                                                        !@"Å                                       N<: ! Ò  ≥	  
   
    "                                                                                          @                         òÿÓ:   Õ	  ˘        P   B                                             Ä !Üë                                                            Ä     VŸÍ;   Õ  f        H                                                                                            à@
                            í˜: w }  j       P   B                                                             ö                                                       à   ¬ŸÍ; ST⁄  4       P   B                                                              ¢ê                                                   B    åŸÍ; TYê
  ´        H                                                                                                          I            Ãp	; PIŸ  Ó        H                                             @                                    @                                 DA! B2
˘: _(ˇ  z     
  Ä   Ç         Ä                                                                                      I"                      ÎÔ: ETè         Ä   Ç                                                                                                   † 0	                  ¯ÂÔ: de*  “       Ä   Ç                                                                                                        Aê             >¯:   Ó  M                                                                                                                                          òÿÓ:   	  #  	   	   @                                                                                                                         T:   >  m  	             (   Ñ@                                                                                                     Ä          NÈÔ:   É  Ê    
                  B ( Ñ                                                                                               
Ñ       ºÍ;     Ö                                ÄXH                                                                     Ä                       æŸÍ;   Å         @                          2H@Ñ"                                                      Ñâà                                 ⁄ﬁÍ;   ª  ÷                                                                                                                                òÿÓ:   „  ≥	  	                                                                                                     @                         òÿÓ:     ›                                                          Ä !Üë                                                             Ä      RŸÍ;      f                                                                                                     Ä@                            N˜:   >  S                                                                         ò                                                          †ŸÍ;   X    	   	                                                                     ¢ê                                                       àŸÍ;   v  ü  	                                                                                             Ç                                 Ãn	;   î  œ    
                                                                               @I í                             †           “B¯:   Í    	                                             Ä                                           @J$                                       ÎÔ:          
                                                                                                           Ä 0	                  ‡‰Ô:   [                                                                                                                                              ﬁ¯:   v  “                                                                                                                     Aê              x=¯:   ï  “                                                                                                                                Ä*      òÿÓ:   ª  V         à                                                                                                       ÄN@                v⁄Í;     V                                                                                                                   L                 r⁄Í;   k  Ÿ  
       à                                                                                                       Ä   Ö              ﬁÍ;   Ë                                                                                                                        Ñ             ﬁÍ;   º  SD  v  - Ú †                                             Ä                      @€Ì˝/                 êEkK¢∂µ÷      ∂’⁄∂nkµZ      ≤‰Í;   æ%  ΩB  a  _ “                        @                                                                                 »÷j≠E≠µZõ™U´+≠V[’⁄V€*  "‰Í;         ò     0öÜ ‚       Æ@     h ' N
      
 ¿     p˜Ü v       +     	       –vÜ ¡       ‹B     I ' 	       ¶     wÜ §     PÜ Œ       V+      ' T:     R ' 18     ( ' ?     PÜ ∞       ç         ©      q ' ˚     
  <      ( h
      
 ≤       á       Ã@     i ' ·       M     ∞Ü        ,	       „       p+      ' °      
          {
     
 
 »     
  H8     + ' ¸B     F ' º>     Z ( t:     * ' û-      ' =      # “       È@     N ' µ         ß<      ( $       H	     
  e8     , ' ê+      ' C     H '        ◊>     I ( Í     
  V     	 # «     
  ò:     / ' A     Z ' •+      ' ∞
     –Ü 8C     D ' !       m	      wÜ ⁄       Û>     J (        Õ<     Y ( A     \ ' ≥:     g ' ¿
     pÜ Ó       √+      ' Ñ	       ;       Ö      % ?     [ ( !   ô$  ê  ˚  D  •"  ©   ¡*  ¿  )  z  2  Ç  ◊  ≥  m	  ?  8    æ  A
  ¿
    s  ˆ  Ï  §  “  ﬂ  Ô  §  ]  ¨    J  $     8~Dz|7º1⁄ÂÁ_„ø“_◊˚7ûÆ#O< Òóú“ˇEl¯æn‘øÕˇˇ·∑º˛ˇ!‹ˇ0¯ø¯3Zú-“fÎÂ       Ä8Ä»@Ç†@%† » Ä® DÄÅ é   ä@@  @    (  <  Ä@Ä Ä   >     W ( ¯,     	 ' q       -     
  eB     Y ' ◊9     ` ' ¿
       	     	  ﬁ*      ' û     
  N@     3 ' 
-      ' />     T ( à       <     % '      –öÅ ˙     †öÅ Ú*      ' h       !     
  ÄB     ^ ' Ù9      ( ›
       N       K     êöÅ Ω       

      
 +      ' G>     G ( ô$     r ' o@     e ' (<     4 ' 8       \       ùB     O ' :     P ' +      ' Æ     
  ˙
       ÿ       ’     ÄöÅ M      0êÅ &
      
 ª       =<      ( M       e>     H ( Ω       ‚      êÅ õ       X      ∞èÅ (+      ' y     	  é@     < '        8     êÅ Ï     †èÅ ΩB     & ' e      ÄèÅ •"     p ' c       œ       A
     pèÅ Y<      ( ◊     `èÅ Û     
 # <+      ' É>     X ( q         ò     PèÅ ‚       Æ@     h ' N
      
 ¿     0èÅ v       +     	        èÅ ¡       ‹B     I ' 	       ¶     èÅ §      èÅ Œ       V+      ' T:     R ' 18     ( ' ?     éÅ ∞       ç         ©      q ' ˚     
  <      ( h
      
 ≤       á       Ã@     i ' ·       M     –éÅ        ,	       „       p+      ' °      
          {
     
 
 »     
  H8     + ' ¸B     F ' º>     Z ( t:     * ' û-      ' =      # “       È@     N ' µ         ß<      ( $       H	     
  e8     , ' ê+      ' C     H '        ◊>     I ( Í     
  V     	 # «     
  ò:     / ' A     Z ' •+      ' ∞
     ¿éÅ 8C     D ' !       m	     ∞éÅ ⁄       Û>     J (        Õ<     Y ( A     \ ' ≥:     g ' ¿
     †éÅ Ó       √+      ' Ñ	       ;       Ö      % ?     [ ( ZC     C ' ˇ      	   8A     W ' Ò<     V ( ⁄
       #       è      % Ã:     f ' p     ÄéÅ ˚     t ' ó	     	  ⁄+     
 ' 2     péÅ V       ª8     : ' Ä     `éÅ       éÅ Û
       =     U ( 0?     \ ( Ë:     . ' TA     [ ' Ò+      ' j       G       ≥	       ~C     E ' ë       µ      % )     çÅ '     
 
 q       ÷8     c ' 2        lA     U ' ,      ' '=     M ( L?     ] ( Ì     –çÅ Ç     ¿çÅ 7      
      
  ;     - ' Ô     m ' d     	  ûC     B ' ¨       Æ      " “     ∞çÅ ≥     †çÅ ˝       F     	 
 ã       Ò8     6 ' m       Ÿ      % ó     
  ,       ' π      " äA     T ' ;     5 ' ¿     êçÅ D=     Q ( j?     ^ ( {       D     o ' ≈      " Ó	             
  \      
 °       ºC     J '      ÄçÅ d       9     b ' “       " ≥     	  D     	  :,      ' ì       a=     L ( ™A     V ' ﬂ     `çÅ        & ›       =;     a ' s     PçÅ ‰     
  â?     S ( z     0çÅ /       ƒ       ◊C     A ' z      
 ü     çÅ N,      ' -9     ; ' ™              ã       ∆A     M ' [;     ' ' £?     ) ' µ       ÷       a,      ' !
       ˘       ‹       J     	  Å=     P ( ıC     L ' ˛       t       §       L9     d ' „A     S ' ¿?     = ' ©       Õ     
  ?
     
  },      '       
      åÅ f       °=     N ( {;     $ ' è       ≥     ‡åÅ D     G ' k9     9 ' ƒ     
  ï,      ' Ê     	  ÿ?     8 '      –åÅ ˝A     ] ' 4     	 
 ò;     0 ' ¡=     R ( ´            ¿åÅ 2D     K ' i
       É9     ? ' B     Q ' '     
  ê     s ' ≤,     ! ' ˆ?     7 ' ô     ∞åÅ S      	 ∏;     1 ' ·=     K ( ƒ,      ' ™     	 	 ü9     > ' à
       SD     @ ' .B     _ ' ˆ     †åÅ ¸              @     k ' I       j      	 ’,     
 ' —;     2 ' æ     êåÅ *     
  ¸=     ˛Ô˛Ô   E   MFCsplit - Win32 Release MFCsplit - Win32 Debug D:\C++\MFCsplit\Caption.cpp "stdafx.h" "MFCsplit.h" "Caption.h" CCaption::CCaption  CStatic CCaption::~CCaption CCaption::dummy int CCaption::Make BOOL CWnd *pMainPaneWnd UINT nID BOOL bCloseButton MESSAGE CCaption::OnPaint void THIS_FILE char %[] CCaption CCaption::ON_WM_PAINT D:\C++\MFCsplit\Dialog2.cpp "Dialog2.h" INTERFACE CDialog2 CDialog CDialog2::INTERFACE_PART IID_IDialog2 Dispatch const IID CDialog2::ON_EN_CHANGE IDC_EDIT1 OnChangeEdit1 DISPATCH CDialog2::OnFinalRelease CDialog2::OnChangeEdit1 CDialog2::CDialog2 CWnd *pParent CDialog2::DoDataExchange CDataExchange *pDX D:\C++\MFCsplit\FormView1.cpp "FormView1.h" "Page1.h" CFormView1::~CFormView1 CFormView1::OnEraseBkgnd CDC *pDC CFormView1::DoDataExchange CFormView1::PreCreateWindow CREATESTRUCT &cs CFormView1 CFormView CFormView1::ON_WM_ERASEBKGND CFormView1::CFormView1 CFormView1::AssertValid CFormView1::Dump CDumpContext &dc CFormView1::OnInitialUpdate D:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\COMMON\MSDEV98\BIN\win32.ncb D:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\COMMON\MSDEV98\BIN\crt.ncb D:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\COMMON\MSDEV98\BIN\mfcatl.ncb D:\C++\MFCsplit\ListView1.cpp "ListView1.h" CListView1::OnDraw CListView1::Create LPCTSTR lpszClassName LPCTSTR lpszWindowName DWORD dwStyle const RECT &rect CWnd *pParentWnd CCreateContext *pContext CListView1::AssertValid CListView1 CListView CListView1::OnInitialUpdate CListView1::Dump CListView1::~CListView1 CListView1::CListView1 D:\C++\MFCsplit\MainFrm.cpp "MainFrm.h" "NewView.h" CMainFrame::AssertValid CFrameWnd CMainFrame::OnDummy indicators UINT %[] CMainFrame::CMainFrame CMainFrame::OnCreate LPCREATESTRUCT lpCreateStruct CMainFrame::PreCreateWindow CMainFrame CMainFrame::ON_WM_CREATE CMainFrame::Dump CMainFrame::~CMainFrame CMainFrame::OnCreateClient LPCREATESTRUCT lpcs D:\C++\MFCsplit\MFCsplit.cpp "MFCsplitDoc.h" "MFCsplitView.h" CMFCsplitApp::InitInstance theApp CMFCsplitApp CAboutDlg CAboutDlg::CAboutDlg enum CAboutDlg::DoDataExchange CAboutDlg::IDD_ABOUTBOX CWinApp CMFCsplitApp::ON_COMMAND ID_FILE_PRINT_SETUP CWinApp::OnFilePrintSetup ID_FILE_OPEN CWinApp::OnFileOpen ID_FILE_NEW CWinApp::OnFileNew ID_APP_ABOUT OnAppAbout CMFCsplitApp::CMFCsplitApp CMFCsplitApp::OnAppAbout D:\C++\MFCsplit\MFCsplitDoc.cpp CMFCsplitDoc::CMFCsplitDoc CMFCsplitDoc::~CMFCsplitDoc CMFCsplitDoc::Serialize CArchive &ar CMFCsplitDoc CDocument CMFCsplitDoc::Dump CMFCsplitDoc::OnNewDocument CMFCsplitDoc::AssertValid D:\C++\MFCsplit\MFCsplitView.cpp CMFCsplitView::OnPreparePrinting CPrintInfo *pInfo CMFCsplitView::~CMFCsplitView CMFCsplitView::GetDocument CMFCsplitDoc * CMFCsplitView::PreCreateWindow CMFCsplitView::OnBeginPrinting CDC * CPrintInfo * CView CMFCsplitView::OnEndPrinting CMFCsplitView::CMFCsplitView CMFCsplitView::OnDraw CMFCsplitView::AssertValid CMFCsplitView::Dump CMFCsplitView CMFCsplitView::ON_COMMAND ID_FILE_PRINT_PREVIEW CView::OnFilePrintPreview ID_FILE_PRINT_DIRECT CView::OnFilePrint ID_FILE_PRINT D:\C++\MFCsplit\NewListCtrl.cpp "NewListCtrl.h" CNewListCtrl::CNewListCtrl CNewListCtrl CListCtrl CNewListCtrl::ON_WM_CREATE CNewListCtrl::~CNewListCtrl CNewListCtrl::OnCreate D:\C++\MFCsplit\NewPane.cpp "NewPane.h" CWnd CNewPane::~CNewPane CNewPane CNewPane::CNewPane D:\C++\MFCsplit\NewPaneView.cpp "NewPaneView.h" CNewPaneView::~CNewPaneView CNewPaneView::CNewPaneView CNewPaneView CNewPaneView::AssertValid CNewPaneView::Dump CNewPaneView::OnDraw D:\C++\MFCsplit\NewSplitter.cpp "NewSplitter.h" CNewSplitter CSplitterWnd CNewSplitter::ON_WM_SIZE CNewSplitter::ON_WM_LBUTTONUP CNewSplitter::OnLButtonUp UINT nFlags CPoint point CNewSplitter::OnInvertTracker const CRect &rect CNewSplitter::CNewSplitter CNewSplitter::~CNewSplitter CNewSplitter::OnSize UINT nType int cx int cy D:\C++\MFCsplit\NewView.cpp CNewView::~CNewView CNewView::Dump CNewView CNewView::ON_WM_CREATE CNewView::ON_WM_SIZE CNewView::AssertValid CNewView::CNewView CNewView::OnDraw CNewView::OnNotify WPARAM wParam LPARAM lParam LRESULT *pResult CNewView::OnCreate CNewView::OnCommand CNewView::OnSize D:\C++\MFCsplit\Page1.cpp CPropertyPage CPage1::~CPage1 CPage1::CPage1 CPage1::OnChangeEdit1 CPage1::DoDataExchange CPage1 CPage1::ON_EN_CHANGE D:\C++\MFCsplit\Prop1.cpp "Prop1.h" CProp1::CProp1 LPCTSTR pszCaption UINT iSelectPage UINT nIDCaption CProp1 CPropertySheet CProp1::dummy CProp1::~CProp1 D:\C++\MFCsplit\SmallToolbar.cpp "SmallToolbar.h" CSmallToolbar::CSmallToolbar CSmallToolbar::~CSmallToolbar CToolBarCtrl CSmallToolbar CSmallToolbar::ON_WM_ERASEBKGND CSmallToolbar::OnEraseBkgnd D:\C++\MFCsplit\StdAfx.cpp D:\C++\MFCsplit\Caption.h CCaption::pStWnd CStatic * D:\C++\MFCsplit\Dialog2.h CWnd *pParent = NULL CDialog2::IDD_DIALOG2 D:\C++\MFCsplit\FormView1.h <afxext.h> CFormView1::m_wndDlgBar CDialogBar CFormView1::m_wndCaption CFormView1::IDD_FORM1_FORM D:\C++\MFCsplit\ListView1.h <afxcview.h> CListView1::pList CListCtrl * CCreateContext *pContext = NULL CListView1::m_wndCaption D:\C++\MFCsplit\MainFrm.h CMainFrame::m_wndStatusBar CStatusBar CMainFrame::m_wndToolBar CToolBar CMainFrame::pListView CListView1 * CMainFrame::m_wndSplitter2 CMainFrame::m_wndSplitter CMainFrame::m_wndCaptionLT CMainFrame::m_wndCaptionLB CMainFrame::m_wndCaptionR CMainFrame::m_wndPane CMainFrame::m_wndPaneView D:\C++\MFCsplit\MFCsplit.h "resource.h" D:\C++\MFCsplit\MFCsplitDoc.h D:\C++\MFCsplit\MFCsplitView.h D:\C++\MFCsplit\NewListCtrl.h D:\C++\MFCsplit\NewPane.h D:\C++\MFCsplit\NewPaneView.h D:\C++\MFCsplit\NewSplitter.h D:\C++\MFCsplit\NewView.h CNewView::list CNewView::toolbar CNewView::italicFont CFont D:\C++\MFCsplit\Page1.h CPage1::IDD_DIALOG1 D:\C++\MFCsplit\Prop1.h CWnd *pParentWnd = NULL UINT iSelectPage = 0 D:\C++\MFCsplit\Resource.h D:\C++\MFCsplit\SmallToolbar.h D:\C++\MFCsplit\StdAfx.h <afxwin.h> <afxdisp.h> <afxdtctl.h> <afxcmn.h> CMFCsplitView::OnEraseBkgnd CMFCsplitView::ON_WM_ERASEBKGND CNewPaneView::OnEraseBkgnd CNewPaneView::ON_WM_ERASEBKGND CNewPane::OnEraseBkgnd CNewPane::ON_WM_ERASEBKGND CListView1::OnEraseBkgnd CListView1::ON_WM_ERASEBKGND D:\C++\MFCsplit\Sheet.cpp "mfcsplit.h" "Sheet.h" CSheet CSheet::~CSheet CSheet::CSheet D:\C++\MFCsplit\Sheet.h "" "heet" "heet.h" CSheet::ON_WM_ERASEBKGND CSheet::OnEraseBkgnd D:\C++\MFCsplit\Split.cpp "Split.h" CSplit::OnCreateClient LPCREATESTRUCT CSplit::~CSplit CMDIChildWnd CSplit CSplit::CSplit D:\C++\MFCsplit\Split.h CSplit::m_wndSplitter CMainFrame::CreateMyView int row int col CRuntimeClass *pViewClass SIZE sizeInit CSplitterWndint row CSplitterWnd *row CSplitterWnd *pSplint row CSplitterWnd *pSplit D:\C++\MFCsplit\MySplitter.cpp CSplitterWnd::SetActivePane CWnd *pWnd CSplitterWnd::IsChildPane int *pRow int *pCol CSplitterWnd::Dump CSplitterWnd::Create int nMaxRows int nMaxCols SIZE sizeMin CSplitterWnd::CreateStatic int nRows int nCols CSplitterWnd::CreateCommon CSplitterWnd::ActivateNext BOOL bPrev HitTestValue CSplitterWnd::StopTracking BOOL bAccept CSplitterWnd::OnDisplayChange CSplitterWnd::OnSetCursor UINT nHitTest UINT message CSplitterWnd::OnNcCreate _afx_hcurLast AFX_STATIC_DATA HCURSOR CSplitterWnd::DrawAllSplitBars int cxInside int cyInside CSplitterWnd::OnInvertTracker CSplitterWnd::ON_WM_SETCURSOR CSplitterWnd::ON_WM_MOUSEMOVE CSplitterWnd::ON_WM_PAINT CSplitterWnd::ON_WM_LBUTTONDOWN CSplitterWnd::ON_WM_LBUTTONDBLCLK CSplitterWnd::ON_WM_LBUTTONUP CSplitterWnd::ON_WM_KEYDOWN CSplitterWnd::ON_WM_SIZE CSplitterWnd::ON_WM_HSCROLL CSplitterWnd::ON_WM_VSCROLL CSplitterWnd::ON_WM_NCCREATE CSplitterWnd::ON_WM_SYSCOMMAND CSplitterWnd::ON_WM_CANCELMODE CSplitterWnd::ON_MESSAGE_VOID WM_SETTINGCHANGE OnDisplayChange WM_WININICHANGE WM_DISPLAYCHANGE CSplitterWnd::ON_WM_MOUSEWHEEL CSplitterWnd::DoScroll CView *pViewFrom UINT nScrollCode BOOL bDoScroll CSplitterWnd::GetPane CWnd * CSplitterWnd::DeleteView CSplitterWnd::OnSysCommand CSplitterWnd::TrackColumnSize int x CSplitterWnd::OnMouseWheel UINT fFlags short zDelta _afx_idcPrimaryLast AFX_STATIC_DATA UINT CSplitterWnd::HitTest CPoint pt _AfxCanSplitRowCol AFX_STATIC int CSplitterWnd::CRowColInfo *pInfoBefore int nBeforeSize int nSizeSplitter CSplitterWnd::GetInsideRect CRect &rect CSplitterWnd::CSplitterWnd CSplitterWnd::OnCommand CSplitterWnd::GetRowInfo int &cyCur int &cyMin CSplitterWnd::CreateScrollBarCtrl CSplitterWnd::DeleteRow int rowDelete CSplitterWnd::OnKeyDown UINT nChar UINT _AfxLayoutRowCol AFX_STATIC void CSplitterWnd::CRowColInfo *pInfoArray int nMax int nSize CSplitterWnd::CanActivateNext CSplitterWnd::GetActivePane CSplitterWnd::SetColumnInfo int cxIdeal int cxMin CSplitterWnd::OnVScroll UINT nSBCode UINT nPos CScrollBar *pScrollBar CSplitterWnd::StartTracking int ht CSplitterWnd::SetSplitCursor CSplitterWnd::OnLButtonUp CPoint CSplitterWnd::OnSize CSplitterWnd::GetHitRect CRect &rectHit CSplitterWnd::RecalcLayout