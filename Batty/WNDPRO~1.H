//
//  Batty game (partial)
//
//  Mixed C/Assembly code for the game window's message processing procedure
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




extern VOID				createstatusbar(HWND hWnd);
extern char*			getswapfilename();
extern BOOL				addforredraw(DWORD x, DWORD y, DWORD dx, DWORD dy);
extern BOOL				objectdraw(UINT x, UINT y, UINT N);
extern BOOL __stdcall	updatebackground(UINT x, UINT y, UINT ddx, UINT ddy);
extern BOOL				shadowdraw(UINT x, UINT y, UINT N);
extern					overlap(VOID);
extern VOID __stdcall	setflashcell(UINT x, UINT y);
extern int				getstring(UINT uID, LPVOID pString, int BufferMax);
extern BOOL				soundplay(DWORD SoundNumber);
extern VOID __stdcall	setflashinwords(LPSTR text, UINT FreqForFlash, UINT TimesToFlash, DWORD WordsColor, DWORD BackgndColor);
extern BOOL __stdcall	killalltimers(void);
extern LONG __stdcall	findmiddle(LPSTR text, LONG WidthOfFrame);
extern BOOL __stdcall	closewave(void);
extern void __stdcall	openwave(void);
extern VOID __stdcall	SBcomment(LPSTR text);
extern BOOL __stdcall	soundplayextra(DWORD SoundNumber);
extern VOID __stdcall	killbat(DWORD WhichBat);
extern					ifstartBU(VOID);
extern					antistraight(VOID);
extern					rnd(VOID);
extern					empty_job(VOID);
extern 					prize(VOID),priz5000(VOID),prizLIFEPLUS(VOID),
						prizSLOW(VOID),	prizTRIBALL(VOID),prizHAND(VOID),
						prizSMASH(VOID), prizALIENKILL(VOID),
						prizEXPAND(VOID), prizSHOOTER(VOID);
extern					addscore(LONG AddPoints, BOOL WhichBat);
extern					ifprizevalid(VOID);
extern BOOL __stdcall	killAutotimer(int Ball012);
extern BOOL __stdcall	killAllAutotimers(VOID);
extern					undoprizes(VOID);
extern BOOL __stdcall	setprize(DWORD xMid, DWORD yMid, DWORD Prize);
extern ULARGE_INTEGER	whatTime(VOID);




extern HANDLE			hMapFOpen;
extern HANDLE			hMapFile;
extern HBITMAP			hScrBitmap;
extern HDC				hDC, hDCMem;
extern HWND				hMainWnd;
extern char				ClassName[];
extern HINSTANCE		hInst;
extern HANDLE			hHeap;
extern HANDLE			hGrxMap;
extern HANDLE			hGrxFile;
extern LPBYTE			lpFGrx;
extern BOOL				DrawingStarted;
extern char				buffer[0x104];
extern BOOL				nodraw;
extern DWORD			ImNum;
extern LPDWORD			lpDyn;
extern DWORD			FPU_CW;
extern BOOL				YesSound;
extern HWAVEOUT			hwo;
extern HMMIO			hMm;
extern DWORD			auxDraw, IfVisible, FlashFreq;
extern BOOL				Focus, Seen, Paused;
extern LPVOID			lpBallsAux[4];
extern double			MaxR;
extern DWORD			WhatFlash, EscWidth, EscCursor;
extern char				EscText[];
extern UINT				ScWpix, ScHpix;
extern BOOL				Esc, LastYesSound;
extern HICON			hStIcon;
extern BOOL				CommentSet;
extern HWND				hStWnd;
extern DWORD			CrSpeedReached;
extern HANDLE			hMainEvent;
extern DWORD			NumCellsOut;
extern LPDWORD			lpCell;
extern BOOL				ShitModeSwitch;
extern BOOL				NoPlay;
extern LPWAVEHDR		lpExtraWh;
extern BOOL				extraSoundOn;
extern BOOL				BatInactive, BatKilled;
extern BOOL				DoublePlay, GameQuit;
extern BOOL				NoAuto;
extern DWORD			LivesMinus, NumLives;
extern LONG				OverH;
extern BOOL				BallsFreez;
extern BOOL				AutoTim[3];
extern LPDWORD			AutoInf[3];
extern LPVOID			lpBatAux[2];
extern HBITMAP			hBatty;
extern DWORD			GoNextRound;
extern int				WinX,WinY,WinH,WinW;
extern MUSICDATA		*lpMData;
extern ULARGE_INTEGER	CountDwnTime;
extern BOOL				Encryption;
extern int				AutoBall[3];
extern HWND				hTextDlg;
extern int				ScrMag;



#include "Win32.h"


static BOOL		TextOn;
static RECT		cliprec;
static int		pX=0,pX_=0;
static float	SensL,SensR;

static LPSTR	PlayName;	//32 bytes only including last ZERO
static LPSTR	LPlayName;	//32 bytes only including last ZERO
static LPSTR	RPlayName;	//32 bytes only including last ZERO

static HFONT	hFont, hBoldFont, hBigBoldFont;
static LONG		LastOverH;
static BOOL		OverHon=FALSE;
static int		CurMidX,CurMidY,LastCurX,LastCurX_;
static BOOL		MouseTrack=FALSE;



VOID			clearall(VOID);
VOID __stdcall	unpausebatty(VOID);
VOID __stdcall	pausebatty(VOID);
VOID __stdcall	screencommand(LPSTR text);
BOOL __stdcall	extrasoundoff(VOID);
VOID __stdcall	clientdraw_empty(HDC hdc, HWND hWnd);
				priz5000_(VOID);
VOID __stdcall	scrcomERR(VOID);
BOOL			takeCursAway(VOID);
BOOL			returnOldCurs(VOID);
VOID			setPlayerName(LPSTR lpText);
BOOL			clickMenu(LPARAM lParam);
VOID			preclearall(VOID);



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	int				a;
	RECT			rect;
	SIZE			size;
	static POINT	Cpnt;
	static BOOL		Ffch=FALSE, EscLastPaused=0;
	static BYTE		ComText[32]={
								0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
								0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
								0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
								0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
								};
	static LPVOID	LastCom=ComText;
	static BYTE		Coms[]={
							'T'^99,'I'^99,'U'^99,'Q'^99,0xff,
							'E'^99,'E'^99,'R'^99,'F'^99,'O'^99,'D'^99,0xff,
							'E'^99,'D'^99,'O'^99,'M'^99,'D'^99,'R'^99,'U'^99,'T'^99,0xff,
							'S'^99,'U'^99,'N'^99,'I'^99,'M'^99,'O'^99,'N'^99,0xff,
							'E'^99,'M'^99,'A'^99,'G'^99,'O'^99,'C'^99,'O'^99,'L'^99,0xff,
							'O'^99,'T'^99,'U'^99,'A'^99,'O'^99,'N'^99,0xff,
							'0'^99,'C'^99,0xff,
							'1'^99,'C'^99,0xff,
							'2'^99,'C'^99,0xff,
							'3'^99,'C'^99,0xff,
							'4'^99,'C'^99,0xff,
							'5'^99,'C'^99,0xff,
							'6'^99,'C'^99,0xff,
							'7'^99,'C'^99,0xff,
							'8'^99,'C'^99,0xff,
							'E'^99,'R'^99,'O'^99,'M'^99,'W'^99,'O'^99,'H'^99,0xff,
							'Z'^99,'E'^99,'E'^99,'R'^99,'F'^99,'L'^99,'L'^99,'A'^99,'B'^99,0xff,
							'0'^99,'P'^99,0xff,
							'1'^99,'P'^99,0xff,
							'2'^99,'P'^99,0xff,
							'3'^99,'P'^99,0xff,
							'4'^99,'P'^99,0xff,
							'5'^99,'P'^99,0xff,
							'6'^99,'P'^99,0xff,
							'7'^99,'P'^99,0xff,
							'8'^99,'P'^99,0xff,
							'9'^99,'P'^99,0xff,
							'C'^99,'D'^99,0xff,
							0xff
							};
	static BYTE		Offens[]={
							'T'^99,'I'^99,'H'^99,'S'^99,0xff,
							'K'^99,'C'^99,'U'^99,'F'^99,0xff,
							'T'^99,'N'^99,'U'^99,'C'^99,0xff,
							'S'^99,'S'^99,'I'^99,'P'^99,0xff,
							'S'^99,'S'^99,'A'^99,0xff,
							'N'^99,'M'^99,'A'^99,'D'^99,0xff,
							'L'^99,'L'^99,'E'^99,'H'^99,0xff,
							'Y'^99,'S'^99,'U'^99,'O'^99,'L'^99,0xff,
							'H'^99,'C'^99,'T'^99,'I'^99,'B'^99,0xff,
							0xff
							};
	static BYTE		WatchText[]={'W'^55,'a'^55,'t'^55,'c'^55,'h'^55,' '^55,'y'^55,
								 'o'^55,'u'^55,'r'^55,' '^55,'l'^55,'a'^55,'n'^55,
								 'g'^55,'u'^55,'a'^55,'g'^55,'e'^55,','^55,' '^55,
								 'b'^55,'u'^55,'d'^55,'d'^55,'y'^55,'!'^55,55
								};
	static char		ObsBuffer[7*4+4];
	LPDRAWITEMSTRUCT lpdis;
	static	char	Text1[]="Player:";


	switch (message) 
	{
		case WM_MOUSEMOVE:
			POINT	pnt;
			int		p,px,Cx;
			BOOL	boo;

			if(!MouseTrack) break;

			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);
			ClientToScreen(hWnd,&pnt);

			px=pnt.x-LastCurX;

			Cx=pnt.x;
			boo=FALSE;

			if(pnt.x>=(cliprec.right-1) || pnt.x<=cliprec.left)
			{
				SetCursorPos(Cx=CurMidX,CurMidY);
				boo=TRUE;
			}

			if((p=(int)(px/SensL))!=0)
			{
				LastCurX=Cx;
				pX=p;
			}
			else
				if(boo)
					LastCurX-=pnt.x-CurMidX;

			if(DoublePlay)
			{
				if((p=(int)((pnt.x-LastCurX_)/SensR))!=0)
				{
					LastCurX_=Cx;
					pX_=p;
				}
				else
					if(boo)
						LastCurX_-=pnt.x-CurMidX;
			}

			break;





		case WM_PAINT:
			{
				PAINTSTRUCT		ps;

				HDC hdc=BeginPaint(hWnd, &ps);

				if(DrawingStarted)
				{
					__asm
					{
						mov		eax,[ps.rcPaint.left]
						mov		edx,[ps.rcPaint.top]
						mov		edi,[ps.rcPaint.bottom]
						mov		esi,[ps.rcPaint.right]

						cmp		[ScrMag],1
						jz		short	lblmp1
						shr		eax,1
						shr		edx,1
						shr		edi,1
						shr		esi,1
lblmp1:
						sub		edi,edx
						sub		esi,eax

						push	edi
						push	esi
						push	edx
						push	eax
						call	addforredraw
						test	eax,eax
						jz		short	wmp1

						mov		edi,edx
						mov		eax,[edi]
						mov		edx,[edi+0x4]
						mov		esi,[edi+0x8]
						add		esi,eax
						mov		edi,[edi+0xc]
						add		edi,edx


						push	ebp
						call	overlap
						pop		ebp
wmp1:
					}
				}
				else
				{
					clientdraw_empty(hdc, hWnd);
				}


				EndPaint(hWnd, &ps);
				break;
			}



		case WM_ACTIVATE:
			{
				if(TextOn)
					goto defaul;

				Seen=HIWORD(wParam);
				break;
			}


		case WM_KILLFOCUS:
			{
				if(TextOn)
				{
					SendMessage(hTextDlg,WM_APP,0,0);
					goto defaul;
				}

				Focus=FALSE;
				Ffch=TRUE;

				//return cursor
				returnOldCurs();

				if(extraSoundOn)
				{
					if(!(lpExtraWh->dwFlags&WHDR_DONE))
						waveOutReset(hwo);

					waveOutUnprepareHeader(hwo,
										  lpExtraWh,
										  sizeof(WAVEHDR));

					extraSoundOn=FALSE;
				}

				closewave();
				pausebatty();
				Esc=FALSE;

				goto defaul;
			}

		case WM_SETFOCUS:
			{
				if(TextOn)
					goto defaul;

				Focus=TRUE;

				//Take cursor away
				takeCursAway();

				if(Ffch)
				{
					unpausebatty();
					openwave();
					NoPlay=FALSE;

					if((LastYesSound!=YesSound)&&(YesSound==FALSE))
						auxDraw=4;		//to check for sound presence
					else
						SBcomment("Wave output restored, possible sound loss");
					break;
				}
				goto defaul;
			}



		case WM_CREATE:
			{
				createstatusbar(hWnd);
				break;
			}



		case WM_TIMER:
			{
				static double	PlusToR=0.05;
				static char		CriText[]="Ball critical velocity reached";
				static DWORD	BallsRec[3*5]={
												-1,0,-1,0, 0,
												-1,0,-1,0, 0,
												-1,0,-1,0, 0
												};
				static BOOL		CriReach;

				if((wParam>=0x1)&&(wParam<=3))
				if((!Seen)&&Focus&&(!Paused))	//BALLs acceleration
				{
					__asm
					{
						cmp		[BallsFreez],TRUE
						jz		short	noacc

						mov		esi,[wParam]
						mov		eax,[lpDyn]
						mov		ecx,ONEDYNOBJ
						add		eax,ONEDYNOBJ*31
						xor		edi,edi
						imul	ecx,esi
						test	byte ptr [eax+ecx+0x24+3],0x80
						jnz		short	noacc		//if BALL STICKY

						mov		ebx,[esi*4+lpBallsAux-0x4]

						fld		qword ptr [ebx+0x10]
						fld		[MaxR]
						fcomp
						fnstsw	ax
						sahf
						jb		short	nRi1

						inc		edi
						fadd	[PlusToR]
nRi1:
						mov		[CriReach],edi
						fstp	qword ptr [ebx+0x10]
						jae		short	nRi2

						cmp		[CrSpeedReached],1
						mov		[CrSpeedReached],1
						jz		short	nRi2

						push	offset CriText
						call	SBcomment
nRi2:
						//ANTI-BLOCK mechanism

						lea		edi,[esi*4+BallsRec-0x4]
						mov		eax,[ebx+0x38]
						xor		esi,esi
						cmp		eax,60+CELLHEIGHT*CELLVERT
						jae		short	noma

						mov		edx,[ebx+0x34]
						sub		eax,[edi+0xc]
						sub		edx,[edi+0x8]
						or		eax,edx
						jnz		short	noma

						mov		eax,[ebx+0x2c]
						mov		edx,[ebx+0x30]
						sub		eax,[edi]
						sub		edx,[edi+0x4]
						or		eax,edx
						jnz		short	noma

						mov		esi,[edi+0x10]
						inc		esi				//same RECT
						cmp		esi,2
						jb		short	noma

						xor		esi,esi			//change BALL's direction
						call	rnd
						mov		ecx,2
						cmp		[CriReach],1
						jnz		short	cbd1
						add		ecx,2
cbd1:
						shr		eax,1
						mov		edx,[ebx+0x18]
						jc		short	cbd2

						sub		edx,ecx
						jnc		short	cbd3
						add		edx,360
						jmp		short	cbd3
cbd2:
						add		edx,ecx
						cmp		edx,360
						jb		short	cbd3
						sub		edx,360
cbd3:
						call	antistraight
						mov		[ebx+0x18],edx

noma:
						xor		eax,eax
						mov		[edi+0x10],esi
						lea		ecx,[eax-1]
						mov		esi,[ebx+0x30]
						mov		edx,[ebx+0x2c]
						mov		[ebx+0x30],eax
						mov		[ebx+0x2c],ecx
						mov		[edi+0x4],esi
						mov		[edi],edx

						mov		esi,[ebx+0x38]
						mov		edx,[ebx+0x34]
						mov		[ebx+0x38],eax
						mov		[ebx+0x34],ecx
						mov		[edi+0xc],esi
						mov		[edi+0x8],edx
noacc:
					}
					break;
				}

				if(wParam==0x4)					//Status text timer
				{
					KillTimer(hMainWnd, wParam);
					CommentSet=FALSE;
					SendMessage(hStWnd, SB_SETTEXT, 2, (LPARAM)"");
					break;
				}

				if((wParam>=0x5)&&(wParam<=7))	//BALLs autostart
				{
					BEEPZERO(killAutotimer(wParam-0x5));
					if(!NoAuto)
						*AutoInf[(wParam-0x5)]|=
							1<<AutoBall[(wParam-0x5)];
					break;
				}

				break;
			}


		case WM_DRAWITEM:			//draw icon in status bar
			{
				HDC			hdcm;
				HBITMAP		hbmp;
				HICON		hIc;
				int			wt,w,h;

				lpdis=(LPDRAWITEMSTRUCT)lParam;

				if(lpdis->itemData==0x123)
				{
					h=lpdis->rcItem.bottom-lpdis->rcItem.top;
					w=lpdis->rcItem.right-lpdis->rcItem.left;

					hdcm=CreateCompatibleDC(lpdis->hDC);
					hbmp=CreateCompatibleBitmap(lpdis->hDC,w,h);
					SelectObject(hdcm,hbmp);

					rect.top=0;
					rect.left=0;
					rect.bottom=lpdis->rcItem.bottom-
						lpdis->rcItem.top;
					rect.right=lpdis->rcItem.right-
						lpdis->rcItem.left;

					FillRect(hdcm,&rect,(HBRUSH)
						GetStockObject(LTGRAY_BRUSH));

					DrawIconEx(hdcm,
								0,0,
								hStIcon,
								h,
								h,
								NULL,
								NULL,
								DI_NORMAL);

					if(Encryption)
					{
						hIc=LoadIcon(hInst,(LPSTR)IDI_KEY);
						DrawIconEx(hdcm,h-10,h-8,hIc,14,8,0,0,DI_NORMAL);
					}

					SelectObject(hdcm, hBoldFont);

					rect.left=9*h/8;
					SetBkMode(hdcm, TRANSPARENT);

					DrawText(hdcm,Text1,-1,&rect,
							 DT_VCENTER |
							 DT_SINGLELINE |
							 DT_LEFT |
							 DT_NOPREFIX);

					lstrcpy(buffer, PlayName);	//here must be copied the name of player to [buffer]

					wt=GetTextExtentPoint32(hdcm,
											 Text1,
											 lstrlen(Text1),
											 &size);

					if(wt)
					{
						rect.left+=size.cx+2;

						SelectObject(hdcm, hFont);

						DrawText(hdcm,		//print name of player
								 buffer,
								 -1,
								 &rect,
								 DT_VCENTER | 								 DT_SINGLELINE |
								 DT_LEFT | DT_NOCLIP |
								 DT_NOPREFIX
								 );
					}

					BitBlt(lpdis->hDC,
						lpdis->rcItem.left,
						lpdis->rcItem.top,w,h,hdcm,
						0,0,SRCCOPY);

					BEEPZERO(DeleteDC(hdcm));
					BEEPZERO(DeleteObject(hbmp));

				}

				return TRUE;
			}



		case WM_KEYDOWN:
			{
			int	key;
			key=wParam;

			switch(key)
			{
				case 0xc0:					//PAUSE
				{
					if(BatInactive) break;

					if(!Esc)
					{
						if(Paused)
						{
							takeCursAway();
							unpausebatty();
						}
						else
						{
							returnOldCurs();
							pausebatty();
						}

						NoPlay^=TRUE;
					}
					break;
				}

				case VK_ESCAPE:				//ESC
					{
						if(BatInactive) break;

						if(!Esc)
						{
							soundplayextra(15);
							returnOldCurs();
							auxDraw=3;
							WhatFlash=1;
							EscLastPaused=Paused;
							Paused=TRUE;
							__asm
							{
								push	[ScWpix]
								push	offset EscText
								call	findmiddle	//(text, widthofframe)
								mov		[EscWidth],ecx
							}
						}
						else
						{
							if(!extrasoundoff())break;

							if(!EscLastPaused) soundplay(15);
							else
								soundplayextra(15);
escenter:
							auxDraw=0;
							takeCursAway();
							Paused=EscLastPaused;
							if(!EscLastPaused)
								WhatFlash=0x2;		//for total redraw
							else
								pausebatty();
						}

						if(!EscLastPaused) NoPlay^=TRUE;

						WhatFlash|=0x80000000;		//for total redraw
						Esc^=TRUE;

						FlashFreq=1;

						break;
					}

				case VK_DOWN:				//DOWN key at ESCAPE menu
					{
						if(Esc)
						{
							if((++EscCursor)>=3)EscCursor=2;
							else
								soundplayextra(4);
						}
						break;
					}

				case VK_UP:				//UP key at ESCAPE menu
					{
						if(Esc)
						{
							if((--EscCursor)&0x80000000)EscCursor=0;
							else
								soundplayextra(4);
						}
						break;
					}

				case 0xd:				//ENTER key at ESCAPE menu
					{
						if(Esc)
						{
							if(EscCursor==0)
							{
								if(!extrasoundoff())break;
								goto escenter;
							}

							if(EscCursor==1)
							{
								BOOL	s;
								if(!extrasoundoff())break;
								s=*(LPDWORD)(((LPDWORD)lpBallsAux[0])+0x3c/4)&(TRUE<<1);
								killbat(DoublePlay?(s?1:0):-1);
								EscLastPaused=FALSE;
								goto escenter;
							}

							if(EscCursor==2)
							{
								if(!extrasoundoff())break;
								GameQuit=TRUE;
								GoNextRound=1;
								SBcomment("Game quit by user command");
								EscLastPaused=FALSE;
								goto escenter;
							}
						}
						break;
					}


				case 0xbb:					//checking COMMANDs
				{
					if(BatInactive) break;

					SBcomment("Command received");

					__asm
					{
						lea		esi,[Coms]
						mov		[a],0
chc00:
						mov		ebx,[LastCom]
chc0:
						dec		ebx
						cmp		ebx,offset ComText
						jae		short	chc1
						mov		ebx,offset ComText+31
chc1:
						lodsb
						cmp		al,0xff
						jz		short	chc2
						xor		al,99
						cmp		[ebx],al
						jz		short	chc0
chc11:
						lodsb
						cmp		al,0xff
						jnz		short	chc11
						inc		[a]
						cmp		byte ptr [esi],0xff
						jnz		short	chc00


//check for obscenities

						lea		edx,[Offens]
						mov		ebx,[LastCom]
och0:
						mov		esi,edx
och00:
						dec		ebx
						cmp		ebx,offset ComText
						jae		short	och1
						mov		ebx,offset ComText+31
och1:
						cmp		ebx,[LastCom]
						jz		short	och2

						mov		al,[esi]
						xor		al,99
						cmp		[ebx],al
						jz		short	och10
						cmp		esi,edx
						jz		short	och00
						mov		ebx,ecx
						jmp		short	och0
och10:
						cmp		esi,edx
						jnz		short	och11
						mov		ecx,ebx
och11:					inc		esi
						cmp		byte ptr [esi],0xff
						jz		short	ochf	//jmp if found
						jmp		short	och00
och2:
						mov		ax,[edx]		//next word
						inc		edx	
						inc		ax
						jz		short	oche
						test	al,al
						jnz		short	och2
						jmp		short	och0

ochf:
						lea		ebx,[ObsBuffer]
						lea		edx,[WatchText]
ochf1:
						mov		al,[edx]
						xor		al,55
						mov		[ebx],al
						jz		short	ochf2
						inc		edx
						inc		ebx
						jmp		short	ochf1
ochf2:
					}

					screencommand(ObsBuffer);
oche:
					goto lastatch;				//Command not found

chc2:				//COMMAND FOUND a=its number

					switch(a)
					{
					case 0:	goto endnow;
					case 1:
						screencommand("All cells but one cleared");
						__asm
						{
							mov		ebx,[lpCell]
							mov		ecx,MAXCELL-1
							xor		edi,edi
							mov		esi,[NumCellsOut]
com11:
							mov		edx,[ecx*4+ebx]
							test	edx,0x20
							jnz		short	com13

							test	edx,0x40		//if no-out CELL
							jnz		short	com121

							test	edi,edi
							jnz		short	com12
							inc		edi
							jmp		short	com13
com12:
							dec		esi
com121:
							and		edx,0xdfffffcf
							or		edx,0x20
							mov		[ecx*4+ebx],edx
com13:
							dec		ecx
							jns		short	com11

							mov		[NumCellsOut],esi
							call	ifstartBU
						}

						InvalidateRect(hMainWnd, NULL, TRUE);
						break;
					case 2:
						screencommand("Turdmode toggled");
						ShitModeSwitch=1;
						break;
					case 3:
						screencommand("Infinite rounds mode toggled");
						LivesMinus^=1;
						break;
					case 4:
						if(OverH)
						{
							screencommand("Hold on!!!");
							OverHon=TRUE;
						}
						else
						{
							screencommand("Normal game rate restored");
							OverH=LastOverH;
						}
						break;
					case 5:
						NoAuto^=TRUE;
						screencommand("Auto ball-off mode toggled");
						break;
					case 6:
					case 7:
					case 8:			//Instant PRIZEs
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
						{
						static LPVOID c6jmps[]={
												priz5000_,
												prizLIFEPLUS,
												prizTRIBALL,
												prizSLOW,
												prizSMASH,
												prizALIENKILL,
												prizEXPAND,
												prizHAND,
												prizSHOOTER
												};
						__asm
						{
							push	ebp

							mov		edi,[a]
							mov		ebp,[lpDyn]
							xor		esi,esi
							sub		edi,6
							mov		edx,7
							add		ebp,ONEDYNOBJ*30
							cmp		[DoublePlay],TRUE
							jnz		short	com61
							mov		eax,[lpBallsAux]
							mov		esi,[eax+0x3c]
							dec		edx
							shr		esi,1
							and		esi,0x1
							jz		short	com61
							add		ebp,ONEDYNOBJ
							add		edx,2
com61:
							mov		eax,edi
							push	ebp
							call	ifprizevalid
							pop		ebp
							jc		short	com62
							call	undoprizes	//UNDO old prizes' effects
							call	NEAR dword ptr [edi*4+c6jmps]
							pop		ebp
						}
						screencommand("Instant gain");
						break;
						__asm
						{
com62:
							pop		ebp
						}
						scrcomERR();
						break;
						}

					case 15:			//HOW MORE

						static char hm[sizeof("-9999999999 cells more")];
						wsprintf(hm,
								"%d cell%s more",
								NumCellsOut,
								(NumCellsOut==1)?"":"s");
						screencommand(hm);
						break;

					case 16:			//BALL FREEZ

						if(!BallsFreez&&(
							lpDyn[(32*ONEDYNOBJ+0x24)/4]&0x80000000 ||
							lpDyn[(33*ONEDYNOBJ+0x24)/4]&0x80000000 ||
							lpDyn[(34*ONEDYNOBJ+0x24)/4]&0x80000000))
						{
							scrcomERR();
							break;
						}
						screencommand("Ball Freeze-mode toggled");
						BallsFreez^=TRUE;
						break;

					case 17:
					case 18:
					case 19:
					case 20:
					case 21:	//Instant prize
					case 22:
					case 23:
					case 24:
					case 25:
					case 26:
						__asm
						{
							mov		eax,[a]
							mov		edx,7
							sub		eax,17
							push	ebp
							call	ifprizevalid
							pop		ebp
							jc		short	comN17

							push	eax
							push	-5
							push	17+7*CELLWIDTH+CELLWIDTH/2
							call	setprize	//(xmid,ymid, prize)
							test	eax,eax
							jz		short	comN17
						}
						screencommand("Instant prize");
						goto comN18;
comN17:
						scrcomERR();
comN18:
						break;

					case 27:					//Hello Anton
						screencommand("Hello Anton!");
						break;

					}
lastatch:
					__asm
					{
						mov		ebx,offset ComText
						xor		eax,eax
						mov		ecx,8
						dec		eax
latch1:
						mov		[ecx*4-4+ebx],eax
						loop	latch1
					}
					break;
				}
				
				case 'S':					//QUIT immediately
				{
					if(GetAsyncKeyState(VK_SHIFT)&0x8000)
						if(GetAsyncKeyState(VK_CONTROL)&0x8000)
endnow:
							DestroyWindow(hWnd);
					goto def;
				}
				default:
					{
def:
						__asm						//SET new command into list
						{
							mov		ebx,[LastCom]
							mov		eax,[key]
							cmp		eax,91
							jae		short	slc3
							cmp		eax,48
							jb		short	slc3

							mov		[ebx],al
							inc		ebx
							cmp		ebx,offset ComText+32
							jb		short	slc2
							mov		ebx,offset ComText
		slc2:
							mov		[LastCom],ebx
		slc3:
						}
					}
				}
				break;
			}



		case WM_DESTROY:
			{
				CountDwnTime.QuadPart=whatTime().QuadPart-
					CountDwnTime.QuadPart;

				DrawingStarted=FALSE;
				nodraw=TRUE;

				BEEPZERO(DestroyIcon((HICON)
					GetClassLong(hMainWnd, GCL_HICON)));

				if(CommentSet){	KillTimer(hMainWnd, 0x4);	}

				BEEPZERO(killAllAutotimers());
				BEEPZERO(killalltimers());

				BEEPZERO(ReleaseDC(hMainWnd, hDC));
				PostQuitMessage(0);
				break;
			}



		case WM_LBUTTONDOWN:
			clickMenu(lParam);
			break;



		case WM_LBUTTONDBLCLK:
			if(clickMenu(lParam))
				PostMessage(hMainWnd,WM_KEYDOWN,0xd,0);
			break;



		case WM_NCLBUTTONDOWN:
			if(GetFocus()!=hWnd)
				SetFocus(hWnd);
			break;

		default:
defaul:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



VOID __stdcall screencommand(LPSTR text)
{
	setflashinwords(text,
					50,
					6,
					0x5f,
					0x0);

	return;
}



VOID	preclearall(VOID)
{
	BEEPZERO(closewave());
	BEEPZERO(HeapFree(hHeap,0,lpMData));

	returnOldCurs();
	return;
}



VOID	clearall(VOID)
{
	BEEPZERO(DeleteObject(hBigBoldFont));
	BEEPZERO(DeleteObject(hBoldFont));
	BEEPZERO(DeleteObject(hFont));

	BEEPZERO(UnmapViewOfFile((LPCVOID)lpFGrx));
	BEEPZERO(CloseHandle(hGrxMap));
	BEEPZERO(CloseHandle(hGrxFile));

	BEEPZERO(HeapDestroy(hHeap));

	BEEPZERO(DeleteDC(hDCMem));
	BEEPZERO(DeleteObject(hScrBitmap));
	BEEPZERO(CloseHandle(hMapFile));
	BEEPZERO(CloseHandle(hMapFOpen));

	BEEPZERO(CloseHandle(hMainEvent));

	BEEPZERO(DeleteObject(hBatty));
	BEEPZERO(DestroyIcon(hStIcon));

	__asm
	{
		fnclex
		fldcw	dword ptr [FPU_CW]
	}

	return;
}






VOID __stdcall unpausebatty(VOID)
{
	SetWindowText(hMainWnd, ClassName);
	auxDraw=0;
	IfVisible=0;

	FlashFreq=1;
	WhatFlash=0x80000002;		//for total redraw

	Paused=FALSE;
	return;
}





VOID __stdcall pausebatty(VOID)
{
	SetWindowText(hMainWnd, "Batty - PAUSED");
	setflashinwords("Game Paused", 100, 0, 0x7ffe, 0x2);
	auxDraw=2;
	WhatFlash=0x80000000;		//for total redraw

	Paused=TRUE;
	return;
}





BOOL __stdcall extrasoundoff(VOID)
{
	if(extraSoundOn)
	{
		if(!(lpExtraWh->dwFlags&WHDR_DONE))
			return FALSE;

		if(waveOutUnprepareHeader(hwo,
								  lpExtraWh,
								  sizeof(WAVEHDR))
			==MMSYSERR_NOERROR)
			extraSoundOn=FALSE;
	}

	return TRUE;
}





VOID __stdcall clientdraw_empty(HDC hdc, HWND hWnd)
{
	RECT	rect;
	HDC		hdc1,hdc2;
	HBITMAP	hBm;
	BITMAP	bm;
	HBRUSH	hBr;
	int		w,h;

	GetClientRect(hWnd, &rect);
	w=rect.right-rect.left;
	h=rect.bottom-rect.top;

	hBr=CreateSolidBrush(RGB(193,193,189));

	hdc1=CreateCompatibleDC(hdc);
	hdc2=CreateCompatibleDC(hdc);
	hBm=CreateCompatibleBitmap(hdc,w,h);
	SelectObject(hdc1,hBm);

	FillRect(hdc1, &rect, (HBRUSH)hBr);

	GetObject(hBatty,sizeof(BITMAP),&bm);
	SelectObject(hdc2, hBatty);
	BitBlt(hdc1,20,20,bm.bmWidth,bm.bmHeight,
		hdc2,0,0,SRCAND);

	BitBlt(hdc,0,0,w,h,hdc1,0,0,SRCCOPY);

	BEEPZERO(DeleteDC(hdc2));
	BEEPZERO(DeleteDC(hdc1));
	BEEPZERO(DeleteObject(hBm));
	BEEPZERO(DeleteObject(hBr));
	return;
}




__declspec ( naked ) priz5000_(void)
{
	__asm
	{
		push	esi
		push	5000
		call	addscore	//(Addpoints, whichBat);
		jmp		priz5000
	}
}






VOID __stdcall scrcomERR(VOID)
{
	setflashinwords("Execution impossible!",
					30,
					8,
					0x7c20,
					0x0);
	return;
}





BOOL	takeCursAway(VOID)
{
	BOOL	res=FALSE;
	POINT	pnt;
	int		x,y;

	SetCursor(NULL);
	while(ShowCursor(FALSE)>=0);

	CurMidX=x=WinX+WinW/2;
	CurMidY=y=WinY+WinH/2;

	if(hMainWnd)
	{
		GetClientRect(hMainWnd,&cliprec);
		pnt.x=cliprec.left;
		pnt.y=cliprec.top;
		ClientToScreen(hMainWnd,&pnt);
		cliprec.left=pnt.x;
		cliprec.top=pnt.y;

		pnt.x=cliprec.right;
		pnt.y=cliprec.bottom;
		ClientToScreen(hMainWnd,&pnt);
		cliprec.right=pnt.x;
		cliprec.bottom=pnt.y;

		SetCursorPos(x,y);
		if(ClipCursor(&cliprec))
		{
			SetCapture(hMainWnd);
			LastCurX_=LastCurX=x;
			MouseTrack=TRUE;
			res=TRUE;
		}
	}
	return res;
}




BOOL	returnOldCurs(VOID)
{
	BOOL	res=FALSE;

	if(ClipCursor(NULL))
	{
//		SetCursorPos((cliprec.right+cliprec.left)/2,
//			(cliprec.bottom+cliprec.top)/2);
		SetCursor(LoadCursor(NULL, (LPSTR)IDC_ARROW));
		while(ShowCursor(TRUE)<0);
		ReleaseCapture();
		MouseTrack=FALSE;
		res=TRUE;
	}

	return res;
}




VOID	setPlayerName(LPSTR lpText)
{
	if(!lpText)
	{
		PlayName[0]=0;
		return;
	}

	for(int i=0; i<31; i++)
		PlayName[i]=lpText[i];

	PlayName[i]=0;

	InvalidateRect(hStWnd,NULL,FALSE);
	return;
}




BOOL	clickMenu(LPARAM lParam)
{
	int		x,y;
	DWORD	u;
	BOOL	res=FALSE;

	if(Esc)
	{
		x=LOWORD(lParam);
		y=HIWORD(lParam);

		if(x>=(signed)ScWpix/8 && x<(signed)ScWpix/8+
			(signed)EscWidth)
		{
			u=(y-ScHpix/4)/(LETTERSHEIGHT*4/3);
			if(u<3)
			{
				res=TRUE;
				if(u!=EscCursor)
				{
					EscCursor=u;
					soundplayextra(4);
				}
			}
		}
	}

	return res;
}




