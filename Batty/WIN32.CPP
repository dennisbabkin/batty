//
//  Batty game (partial)
//
//  Mixed C/Assembly code for the game window's internal plumbing & basic game functions
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



#include "stdafx.h"
#include "resource.h"
#include <mmsystem.h>
#include <commdlg.h>
#include <commctrl.h>
#include <windowsx.h>
#include <objbase.h>
#include <shlobj.h>

#include "Win32.h"

#include "WndProcFile.h"
#include "MainLoopFile.h"
#include "Dialog.h"


// Global Variables:
HANDLE				hMut;
HWND				hDesk;
DWORD				FPU_CW;
HINSTANCE			hInst;
HWND				hMainWnd=NULL;
HANDLE				hMainEvent;
HICON				hStIcon;
ATOM				Atom;
HWND				hStWnd;
DWORD				BatErr=0;
LONG				OverH;

BITMAPINFO			Gbmi;
int					WinX,WinY,WinH,WinW;

DWORD				BATw_div1,BATw_div2,BATw_div3,BATw_div4;        //BAT width divided on 1,2,3, or 4
DWORD				BATh_div1,BATh_div2,BATh_div3,BATh_div4;        //BAT height divided on 1,2,3, or 4
DWORD				BALLw_div1, BALLw_div2;

LPVOID				lpBEinf;					                    //Info for BAT explosion

DWORD				TrackSound=-1;
LONG				lenToPlay=-1;
DWORD				begPlay=0, endPlay=0;
PUMPINFO			SPump={-1, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0};	//info for 8 Sounds to pump
LPBYTE				lpSData, lpBSData;
WAVEHDR				whdr[NUMSOUNDCHUNKS];       //headers for Sound CHUNKS

BOOL				YesSound=FALSE;             //if FALSE=>no sound
BOOL				IfWaveOpen=FALSE;           //if TRUE=>wave device is currently open
BOOL				LastYesSound;
HWAVEOUT			hwo;
BOOL				NoPlay=FALSE;
BOOL				extraSoundOn=FALSE;         //TRUE-if sound for ESC is activated
LPWAVEHDR			lpExtraWh;

MUSICDATA			*lpMData=NULL;              //Addr of Info for each sound
double				arctgMUL;
double				arctg180=180;

LPVOID				lpSin;                      //mem for SIN values
int					sbParts[3];
BOOL				Esc=FALSE;
BOOL				CommentSet=FALSE;

char				buffer[0x104];
char				ClassName[]="Batty";
char				SwapFile[]="btgrx.swp";
char				GrxFile[]="Btty.grx";

BOOL				InfoOK;                     //FALSE if text is corrupt
LONG				dlx=0x220;                  //x size of game screen
LONG				dly=0x180;                  //y size of game screen
DWORD				Level=0;
DWORD				Round=0;

int					wW;                         //Game Window Frame width
int					wH;                         //Game Window Frame height

UINT				ScWpix;                     //width of screen in pixels (equal to dlx)
UINT				ScHpix;                     //height of screen in pixels (equal to dly)
UINT				ScWbyte;                    //width of screen in bytes (i.e., ScWpix*2)
UINT				BkgWpix;                    //width of backgroud screen frame in pixels
UINT				BkgWpix_;                   //=BkgWpix-1 (for AND)
UINT				BkgHpix;                    //height of backgroud screen frame in pixels
UINT				BkgHpix_;                   //=BkgHpix-1 (for AND)
UINT				BkgWbyte;                   //width of backgroud screen frame in bytes
BYTE				powW=7;                     //power of width of background frame in pixels (i.e., 2^(powW+1)=BkgWpix)

UINT				BkgSize;                    //size of background frame in bytes

DWORD				ShadeColor;                 //Color of shade
DWORD				PureColor;                  //Value for current Pure Color
DWORD				FillColor;                  //Color to fill in CellDraw()
DWORD				BatColor;                   //Main color of BAT according to Level

DWORD				BatCols[4]={0x7ff0,0x33f0,0x33ff,0x77bd};   //Possible BAT colors
DWORD				PureCols[4]={0x7fe0,0x23e8,0x43ff,0x7bde};  //Possible Pure (backgound) colors
DWORD				ShadeCols[4]={0x7f20,0x300,0x339,0x6b5b};   //Possible Shade colors


UINT				OneUp;					//Score for 1UP
UINT				TwoUp;					//Score for 2UP
UINT				Hi;						//Score for HI

double				initR;					//initial BALL speed
UINT				initAlpha=60;			//initial alpha value for BALL
double				MaxR;					//max value for BALL speed
DWORD				SpeedUpTime;			//Number of second when to speed up BALL each time

HANDLE				hHeap;

LPDWORD				lpDyn;					//addr for beginning of dynamic objects
LPDWORD				lpCurDyn;				//addr for last dynamic object
LPDWORD				lpStat;					//addr for beginning of static objects
LPDWORD				lpCurStat;				//addr for last static object
LPDWORD				lpCell;					//addr of start of CELLs mem
DWORD				NumCellsOut=0;			//Number of beatable CELLs to hit-off
DWORD				BUCellsOutRatio;		//Number (less than NumCellsOut) when Bird/UFO to appear
DWORD				BUcoef;					//Per cent number of when Bird/UFO appear (100-immediately, 0-never)

LPBYTE				lpNextData;				//addr of Sound Part of Btty.grx file
LPDWORD				lpSInf;					//Sound information
DWORD				NumSounds;				//Number of possible sounds

HANDLE				hMapFOpen;
HANDLE				hMapFile;
HDC					hDC, hDCMem;
HBITMAP				hScrBitmap;
LPVOID				lpGrx=0;				//Start of screen buffer in mem
LPWORD				lpGrxW;					//Same as lpGrx

LPVOID				lpBkg;					//Start of background frame in mem
LPVOID				lpBkgLast;				//first byte after background frame in mem

HANDLE				hGrxFile;					//Btty.grx file handle
DWORD				GrxFSize;					//size of Btty.grx file
HANDLE				hGrxMap;				//handle for mapped Btty.grx
LPBYTE				lpFGrx;					//address of beg. of Btty.grx
LPBYTE				lpGrxStart;				//addr of graphics table

HICON				hBigIcon, hSmallIcon;
HBITMAP				hBitmapDel, hBitmapDel_;

BOOL				nodraw=FALSE;			//TRUE=>no drawing is shown or made
BOOL				DrawingStarted=FALSE;
LPDWORD				lpIm;
DWORD				ImNum;
BOOL				Focus=FALSE;			//TRUE=>window has focus

DWORD				id[]={IDS_STRING1, IDS_STRING3, IDS_STRING4,
                           IDS_STRING5, IDS_STRING6, IDS_STRING120};

DWORD				StatObj[NUMSTAT][3]={
                            111,	27,		0x4,		//1UP
                            392,	27,		0x5,		//2UP
                            261,	27,		0x6,		//HI

                            0,		54,		0x11000011,
                            0,		54+110,	0x11000011,
                            0,		54+220,	0x11000011,
                            529,	54,		0x11000011,
                            529,	54+110,	0x11000011,
                            529,	54+220,	0x11000011,

                            0,		15,		0x12000012,
                            529,	15,		0x12000012,

                            0,		0,		0x13000013,

                            68,		0,		0x14000014,
                            68+102,	0,		0x14000014,

                            68+204, 0,		0x15000015,
                            68+306, 0,		0x15000015,

                            68+408, 0,		0x16000016
                                                };

DWORD				rnda, rndb;

int					i;			//AUXILIARY COMMON-USE VARIABLE
DWORD				r;			//AUXILIARY COMMON-USE VARIABLE

MEMALLOCINF			MemAl[]={
                    (VOID**)&lpIm,			BKGMAX*0x10,
                    (VOID**)&lpDyn,			MAXDYN*ONEDYNOBJ,
                    (VOID**)&lpStat,		MAXSTAT*ONESTATOBJ,
                    (VOID**)&lpCell,		MAXCELL*4,
                    (VOID**)&lpBEinf,		(0x14+BE_NUM*0x24)*2,
                    (VOID**)&lpSData,		NUMSOUNDCHUNKS*CHUNKVOL,
                    (VOID**)&lpBSData,		NUMSOUNDCHUNKS*CHUNKVOL,
                    (VOID**)&lpSin,			450*0x4,
                    (VOID**)&joyStruct,		sizeof(JOYINFOEX),
                    (VOID**)&PlayName,		32,
                    (VOID**)&LPlayName,		32,
                    (VOID**)&RPlayName,		32,
};






// Foward declarations of functions used in this module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
int					getstring(UINT uID, LPVOID pString, int BufferMax);
VOID				registerclass(VOID);
                    rnd(void);
BOOL				createwindow(int cmd);
LONG				initializeDIBsection(HDC hdc);
char*				getfilename(LPSTR File);
BOOL				addforredraw(DWORD x, DWORD y, DWORD dx, DWORD dy);
BOOL __stdcall		updatebackground(UINT x, UINT y, UINT ddx, UINT ddy);
BOOL				objectdraw(UINT x, UINT y, UINT N);
BOOL				shadowdraw(UINT x, UINT y, UINT N);
BOOL				celldraw(UINT x, UINT y, UINT N);
VOID				cellshadowdraw(UINT x, UINT y);
VOID __stdcall		adjliveswidth(VOID);
VOID __stdcall		drawlives(VOID);
VOID __stdcall		setball(DWORD x, DWORD y, double R, DWORD alpha, UINT Ball_012, BOOL NeedForStickySet, BOOL Left_or_Right);
double __fastcall	sqart(double val);
void __stdcall		preparesound(void);
void __stdcall		openwave(void);
BOOL __stdcall		closewave(void);
VOID __stdcall		SBdrawlevel(void);
VOID __stdcall		SBcomment(LPSTR text);
VOID __fastcall		soundjob(VOID);
VOID				setGameWndDims(int ClientWidth, int ClientHeight);
BOOL				setRound(int level012, int round012);
VOID				setInitVars(VOID);
BOOL				protectInfo(VOID);






BOOL __declspec ( naked ) addforredraw(DWORD x, DWORD y,
                                       DWORD ddx, DWORD ddy)
{									//RETURNS: (modified values, if TRUE)
    __asm								//[EDX]=x coord
    {									//[EDX+0x4]=y coord
        push	ebp						//[EDX+0x8]=ddx width
        mov		ebp,esp					//[EDX+0xC]=ddy height
        push	ebx
        push	edi
        mov		ecx,[ImNum]
        push	esi

        cmp		ecx,BKGMAX
        jae		short	afrf

        mov		eax,[x]				//EAX=x
        mov		edx,[ddx]
        mov		ebx,[y]				//EBX=y
        test	edx,edx
        jle		short	afrf
        add		edx,eax				//EDX=x+ddx

/*		and		eax,0xfffffffe		//needed aligning
        test	edx,0x1
        jz		short	adra1
        or		edx,0x1
        inc		edx
adra1:*/
        mov		ebp,[ddy]
        mov		edi,[lpIm]
        test	ebp,ebp
        jle		short	afrfp
        add		ebp,ebx				//EBP=y+ddy


        cmp		eax,[ScWpix]		//check for correct coords
        jge		short	afrfp
        cmp		ebx,[ScHpix]
        jge		short	afrfp

        test	eax,eax
        jns		short	afrp1
        xor		eax,eax
afrp1:
        test	ebx,ebx
        mov		esi,edx				//if x+ddx OR y+ddy is less then ZERO
        jns		short	afrp2
        xor		ebx,ebx
afrp2:
        or		esi,ebp
        js		short	afrfp

        cmp		edx,[ScWpix]
        jbe		short	afrp3
        mov		edx,[ScWpix]
afrp3:
        cmp		ebp,[ScHpix]
        jbe		short	afrp4
        mov		ebp,[ScHpix]
afrp4:


        test	ecx,ecx
        jz		short	afr4
afr2:
        mov		esi,[edi]				//x
        cmp		esi,eax
        jg		short	afr3
        add		esi,[edi+8]				//x+dx
        cmp		esi,eax
        jl		short	afrmiss			//jmp if missed
afr2a:
        mov		esi,[edi+4]				//y
        cmp		esi,ebx
        jg		short	afr2b
        add		esi,[edi+0xc]
        cmp		esi,ebx
        jl		short	afrmiss			//jmp if missed
afrhit:


//jmp	short	afrmiss

        mov		ecx,[edi]
        cmp		eax,ecx					//UPDATE OLD
        jge		short	afrh1
        mov		[edi],eax
afrh1:
        add		ecx,[edi+8]
        cmp		ecx,edx
        jge		short	afrh2
        mov		ecx,edx
afrh2:
        sub		ecx,[edi]
        mov		[edi+8],ecx

        mov		ecx,[edi+4]
        cmp		ebx,ecx
        jge		short	afrh3
        mov		[edi+4],ebx
afrh3:
        add		ecx,[edi+0xc]
        cmp		ecx,ebp
        jge		short	afrh4
        mov		ecx,ebp
afrh4:
        sub		ecx,[edi+4]
        mov		[edi+0xc],ecx

        jmp		afr5



afr2b:
        cmp		esi,ebp
        jle		short	afrhit
        jmp		short	afrmiss
afr3:
        cmp		esi,edx
        jle		short	afr2a
afrmiss:
        add		edi,0x10
        dec		ecx
        jnz		short	afr2

afr4:
        mov		[edi],eax			//SETTING NEW OBJECT
        mov		[edi+0x4],ebx
        sub		edx,eax
        sub		ebp,ebx
        mov		[edi+0x8],edx
        mov		[edi+0xc],ebp

        inc		[ImNum]
afr5:
        mov		edx,edi

        mov		eax,1
        pop		esi
        pop		edi
        pop		ebx
        pop		ebp

        ret		0x10

afrfp:
afrf:
        xor		eax,eax
        pop		esi
        pop		edi
        pop		ebx
        pop		ebp

        ret		0x10
    }
}





int	getstring(UINT uID, LPVOID pString, int BufferMax)
{
    return	LoadString(hInst, uID, (LPSTR)pString, BufferMax);
}






VOID	registerclass(VOID)
{
    WNDCLASSEX	wc;

    hStIcon=LoadIcon(hInst, (LPSTR)IDI_SMALLICON);

    wc.cbClsExtra=NULL;
    wc.cbSize=sizeof(wc);
    wc.cbWndExtra=NULL;
    wc.hbrBackground=NULL;
    wc.hCursor=NULL;//LoadCursor(NULL, (LPSTR)IDC_ARROW);
    wc.hIcon=hBigIcon;
    wc.hIconSm=hSmallIcon;
    wc.hInstance=hInst;
    wc.lpfnWndProc=WndProc;
    wc.lpszClassName=ClassName;
    wc.lpszMenuName=NULL;
    wc.style=CS_DBLCLKS;

    Atom=RegisterClassEx(&wc);

    return;
}






BOOL	createwindow(int cmd)
{
    int w,h,x,y,dx,dy;

    dx=dlx+GetSystemMetrics(SM_CXFIXEDFRAME)*2;
    dy=dly+GetSystemMetrics(SM_CYFIXEDFRAME)*2+
                                GetSystemMetrics(SM_CYCAPTION);

    WinW=w=DlgRec.right-DlgRec.left;
    WinH=h=DlgRec.bottom-DlgRec.top;

    WinX=x=DlgRec.left+w/2-dx/2;
    WinY=y=DlgRec.top+h/2-dy/2;

    hMainWnd=CreateWindowEx(0,
                            ClassName,
                            ClassName,
                            WS_CAPTION | WS_VISIBLE,
                            x,y,dx,dy,
                            hDesk,
                            NULL,
                            hInst,
                            NULL);

    if(!hMainWnd) return FALSE;

    ShowWindow(hMainWnd, SW_RESTORE);
    UpdateWindow(hMainWnd);
    
    return TRUE;
}





VOID	createstatusbar(HWND hWnd)
{
    RECT	rect, SBrect, Crect;
    int		w;

    GetWindowRect(hWnd, &rect);

    hStWnd=
    CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                        "",
                        hWnd,
                        0);

    ShowWindow(hStWnd, SW_SHOWNORMAL);

    GetWindowRect(hStWnd, &SBrect);
    GetClientRect(hWnd, &Crect);

    w=Crect.right-Crect.left;

    MoveWindow(hWnd, rect.left, rect.top, rect.right-rect.left,
                rect.bottom-rect.top+SBrect.bottom-SBrect.top, TRUE);

    MoveWindow(hStWnd, 0, Crect.bottom-Crect.top,
                w, SBrect.bottom-SBrect.top, TRUE);


    sbParts[0]=w/4;
    sbParts[1]=sbParts[0]+w/4;
    sbParts[2]=-1;

    SendMessage(hStWnd, SB_SETPARTS, 3, (LPARAM)&sbParts);

    SendMessage(hStWnd, SB_SETTEXT, 0 | SBT_OWNERDRAW, 0x123);	//draw icon

    SBdrawlevel();
    return;
}






VOID __stdcall SBdrawlevel(void)
{

    wsprintf(buffer, "Level: %d  Round: %d", Level+1, Round+1);

    SendMessage(hStWnd, SB_SETTEXT, 1, (LPARAM)buffer);

    return;
}




VOID __stdcall SBcomment(LPSTR text)
{
    SendMessage(hStWnd, SB_SETTEXT, 2, (LPARAM)text);

    if(CommentSet)
    {
        KillTimer(hMainWnd, 0x4);
    }
    
    CommentSet=TRUE;
    SetTimer(hMainWnd, 0x4, 1000*5, NULL);

    return;
}








char* getfilename(LPSTR File)
{
    int l;

    l=GetModuleFileName(hInst, buffer, sizeof(buffer));

    if(!l) return NULL;

    while(buffer[--l]!='\\')
        if(l<=0) break;

    buffer[++l]=0;
    lstrcat(&buffer[l], File);

    return buffer;
}





LONG	initializeDIBsection(HDC hdc)		//returns ZERO, if okay
{
    int	l;

    BatErr=0;

    l=GetDeviceCaps(hdc, RASTERCAPS);
    if(!( (l&RC_BITBLT) &&
          (l&RC_STRETCHBLT) &&
          (l&RC_STRETCHDIB) ) )
            return 0x1;						//1=> video device can't support our graphix



    hMapFOpen=CreateFile(getfilename(SwapFile),
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN |
                        FILE_ATTRIBUTE_TEMPORARY |
                        FILE_FLAG_RANDOM_ACCESS |
                        FILE_FLAG_DELETE_ON_CLOSE,
                        NULL);
    
    if(hMapFOpen==INVALID_HANDLE_VALUE)
    {
        BatErr=GetLastError();
        return 0x2;		//2=> error at opening swap file
    }


    hMapFile=
    CreateFileMapping(hMapFOpen,
                        NULL,
                        PAGE_READWRITE,
                        0,					//max size (high)
                        dlx*dly*2,			//max size (low)
                        NULL);

    if( (!hMapFile) || (GetLastError()) )
    {
        BatErr=GetLastError();
        CloseHandle(hMapFOpen);
        return 0x3;					//3=> error at mapping a file
    }

    Gbmi.bmiHeader.biBitCount=16;
    Gbmi.bmiHeader.biClrImportant=0;
    Gbmi.bmiHeader.biClrUsed=0;
    Gbmi.bmiHeader.biCompression=BI_RGB;
    Gbmi.bmiHeader.biHeight=-dly;
    Gbmi.bmiHeader.biPlanes=1;
    Gbmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    Gbmi.bmiHeader.biSizeImage=0;
    Gbmi.bmiHeader.biWidth=dlx;
    Gbmi.bmiHeader.biXPelsPerMeter=NULL;
    Gbmi.bmiHeader.biYPelsPerMeter=NULL;

    *(LPDWORD)Gbmi.bmiColors=NULL;

    hScrBitmap=
       CreateDIBSection(hdc,
                        &Gbmi,
                        DIB_RGB_COLORS,
                        &lpGrx,
                        hMapFile,
                        0);

    if(!hScrBitmap || !lpGrx)
    {
        BatErr=GetLastError();
        CloseHandle(hMapFile);
        CloseHandle(hMapFOpen);
        return 0x4;					//4=> error at allocating DIB bitmap
    }

    lpGrxW=(LPWORD)lpGrx;


    hDCMem=CreateCompatibleDC(hdc);

    l=(int)SelectObject(hDCMem, hScrBitmap);

    if(!l || l==GDI_ERROR)
    {
        DeleteDC(hDCMem);
        DeleteObject(hScrBitmap);
        CloseHandle(hMapFile);
        CloseHandle(hMapFOpen);
        return 0x5;					//5=> screen bitmap is not accepted
    }

    SetMapMode(hDCMem, GetMapMode(hdc));

    GdiFlush();

    return 0x0;
}






int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    static char		Dummy[]=
        "\r\n\r\nThank you for studying The Batty from inside.\r\n";

    MSG				msg;
    int				i;
    SYSTEMTIME		SysTime;
    FILETIME		FileTime;

    hInst=hInstance;
    CoInitialize(NULL);

    i=Dummy[0];
    setGameWndDims(544,384);

    hDesk=GetDesktopWindow();

    VK_mouse_left=GetSystemMetrics(SM_SWAPBUTTON)?VK_RBUTTON:VK_LBUTTON;

    //Check for running Batty applications

    hMut=CreateMutex(NULL,TRUE,"Batty_Mutex_Obj");
    if(GetLastError()==ERROR_ALREADY_EXISTS)
    {
        ReleaseMutex(hMut);
        return FALSE;
    }
    
    InitCommonControls();
    hBigIcon=LoadIcon(hInst, (LPSTR)IDI_MAINICON);
    hSmallIcon=LoadIcon(hInst, (LPSTR)IDI_SMALLICON);
    hBitmapDel=LoadBitmap(hInst, (LPSTR)IDB_DELCROSS);
    hBitmapDel_=LoadBitmap(hInst, (LPSTR)IDB_DELCROSS_);
    hBatty=LoadBitmap(hInst,(LPSTR)IDB_BATTY);

    registerclass();

    __asm
    {
        fnstcw	dword ptr [FPU_CW]
        fldpi
        fdivr	[arctg180]
        fstp	[arctgMUL]
    }

    GetLocalTime(&SysTime);
    SystemTimeToFileTime(&SysTime, &FileTime);
    rndb=++FileTime.dwLowDateTime;
    rnda=++FileTime.dwHighDateTime;

    for(i=0; i<0x1000; i++)
        rnd();

    int		e;
    HDC		hdc;
    HWND	hdtWnd;

    hdc=GetDC(hdtWnd=GetDesktopWindow());
    e=initializeDIBsection(hdc);
    ReleaseDC(hdtWnd,hdc);

    if(e)									//error at initialization
    {
firsterror:

        getstring(id[e-1], buffer, sizeof(buffer));

        wsprintf(&buffer[lstrlen(buffer)],
                 "\n\nOperation System error returned: #%d\n"
                 "(The Batty game cannot be loaded any further.)",
                 BatErr);

        MessageBox(hMainWnd, buffer,
                    TextFatalError,
                    MB_OK | MB_ICONERROR);

ferror1:
        CloseHandle(hMut);
        CoUninitialize();
        return FALSE;
    }


    //Allocate Memory

    hHeap=HeapCreate(HEAP_NO_SERIALIZE, 1, 0);
    if(!hHeap)
    { 
        BatErr=GetLastError();
        e=3; goto firsterror;
    }

    for(i=0; i<SIZEOF(MemAl); i++)
    {
        *MemAl[i].lpMemAddrHoler=HeapAlloc(hHeap,
                                        HEAP_NO_SERIALIZE |
                                        HEAP_ZERO_MEMORY,
                                        MemAl[i].NumBytes);
    }

    //Check allocation

    for(i=0; i<SIZEOF(MemAl); i++)
    {
        if(!*MemAl[i].lpMemAddrHoler)
        {
herr1:
            BatErr=0x57;
herr1_:
            HeapDestroy(hHeap);
            e=3; goto firsterror;
        }
    }

    //Set Batty path in the registry

    if(!setBattyPath())
    {
        e=6;
        HeapDestroy(hHeap);
        BatErr=0x57;
        goto firsterror;}


    //Get memory for extra BALLs info (for 3 BALLs)

    r=(DWORD)HeapAlloc(hHeap,
                            HEAP_NO_SERIALIZE,
                            AUXBALLMEM*3);
    if(!r) goto herr1;

    __asm
    {
        mov		edx,[r]
        lea		ebx,[lpBallsAux]
        mov		[ebx],edx
        add		edx,AUXBALLMEM
        mov		[ebx+0x4],edx
        add		edx,AUXBALLMEM
        mov		[ebx+0x8],edx
    }


    //Set SIN values

    __asm
    {
        mov		eax,[FPU_CW]
        and		ax,0xf0c0
        or		ax,0x003f
        mov		[r],eax
        fldcw	[r]					//set FPU settings

        mov		[r],180
        mov		ebx,[lpSin]
        mov		ecx,450				//filling out SIN(x) values
        sub		edx,edx
sinfn1:
        mov		[i],edx
        fldpi
        fmul	dword ptr [i]
        fdiv	dword ptr [r]
        fsin
        fstp	dword ptr [ebx]

        inc		edx
        add		ebx,4
        loop	sinfn1
    }


    //Set angles of BALL reflection from BAT

#define LOANGLE	25
#define HIANGLE	65

    double	N=(HIANGLE-LOANGLE)/sqart(24);

    for(int	a=0; a<24; a++)			//right angles
    {
        RAngles[a]=(BYTE)(N*sqart(23-a)+(double)LOANGLE);
    }



    for(a=0; a<24; a++)				//left angles
    {
        LAngles[a]=(BYTE)(180-(N*sqart(23-a)+(double)LOANGLE));
    }

    //Event used for maintaining game rate by timer

    hMainEvent=CreateEvent(NULL, FALSE, TRUE, NULL);
    if(!hMainEvent)
    {
        BatErr=GetLastError();
        goto herr1_;
    }


    hGrxFile=CreateFile(getfilename(GrxFile),		//OPEN Btty.grx file
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_RANDOM_ACCESS,
                        NULL);

    if(hGrxFile==INVALID_HANDLE_VALUE) goto grxerr;

    GrxFSize=GetFileSize(hGrxFile, NULL);
    if(GrxFSize==-1) goto grxerr2;

    hGrxMap=CreateFileMapping(hGrxFile,
                              NULL,
                              PAGE_WRITECOPY | SEC_COMMIT,
                              0,
                              GrxFSize,
                              NULL);

    if( (!hGrxMap) || (GetLastError()) ) goto grxerr2;


    lpFGrx=(LPBYTE)MapViewOfFile(hGrxMap,
                                 FILE_MAP_COPY,
                                 0,
                                 0,
                                 GrxFSize);

    if(!lpFGrx) goto grxerr3;

    if(*(LPDWORD)&lpFGrx[4]!=0x137a00f2)	//check for mask
    {
grxerr3:
        CloseHandle(hGrxMap);
grxerr2:
        CloseHandle(hGrxFile);

grxerr:
        wsprintf(buffer,
                 "There's no needed file \"%s\" in the folder of The Batty!",
                 GrxFile);

        CloseHandle(hMainEvent);
        HeapDestroy(hHeap);

        MessageBox(hMainWnd, buffer,
                   "File Was Not Found!",
                   MB_OK | MB_ICONERROR);
        goto ferror1;
    }

    lpGrxStart=lpFGrx+0x8;			//lpGrxStart=addr of graphics table

    //check number of GRX images
    if(*(LPDWORD)lpFGrx!=126)
    {
        UnmapViewOfFile((LPCVOID)lpFGrx);
        goto grxerr3;
    }


    //Log fonts

    LOGFONT	lf;

    lf.lfCharSet=DEFAULT_CHARSET;
    lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
    lf.lfEscapement=0;
    lstrcpy(lf.lfFaceName, "MS Sans Serif");
    lf.lfHeight=8;
    lf.lfItalic=FALSE;
    lf.lfOrientation=0;
    lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
    lf.lfPitchAndFamily=DEFAULT_PITCH;
    lf.lfQuality=DEFAULT_QUALITY;
    lf.lfStrikeOut=FALSE;
    lf.lfUnderline=FALSE;
    lf.lfWeight=FW_NORMAL;
    lf.lfWidth=4;

    if(!(hFont=CreateFontIndirect(&lf)))
        hFont=(HFONT)GetStockObject(ANSI_VAR_FONT);

    lf.lfWeight=FW_BOLD;
    if(!(hBoldFont=CreateFontIndirect(&lf)))
        hBoldFont=(HFONT)GetStockObject(ANSI_VAR_FONT);

    lf.lfWeight=FW_BOLD;
    lf.lfUnderline=TRUE;
    lf.lfHeight=16;
    lf.lfWidth=8;
    lstrcpy(lf.lfFaceName, "Tahoma");
    if(!(hBigBoldFont=CreateFontIndirect(&lf)))
        hBigBoldFont=(HFONT)GetStockObject(SYSTEM_FONT);

    BkgWpix=*(LPWORD)&lpGrxStart[4];
    BkgHpix=*(LPWORD)&lpGrxStart[6];
    BkgWpix_=BkgWpix-1;
    BkgHpix_=BkgHpix-1;
    BkgWbyte=BkgWpix*2;
    BkgSize=BkgWbyte*BkgHpix;


    BOOL	FirstTime;
    FirstTime=FALSE;

    //DIALOG OPTIONS
lbldlg:

    if(!DialogBoxParam(hInst,(LPSTR)IDD_DIALOG,hDesk,
            (DLGPROC)InitProc,FirstTime))
    {
aftererr:				//RETURN FROM BATTY GAME
        clearall();
        BEEPZERO(CloseHandle(hMut));
        CoUninitialize();
        return FALSE;
    }

    FirstTime|=TRUE;
    ShowMode=1;
    HIpraize=FALSE;

    hMainWnd=NULL;
    BEEPZERO(protectInfo());		//Check if all names are alright

    //Set game initial values
    LPGAME	lpgm;
    DWORD	antiDif;
    char	prfil[8+1+3+1];
    LONG	psrc;

    LivesMinus=1;

    if(!NewGame)		//Once played game
    {
        lpgm=getGameStruct(PlayStruct.Data012etc,
                PlayStruct.Inf01_9,PlayStruct.Game0123);
        if(!lpgm)
        {
lblun:
            MessageBox(hDesk,
                "Unable to load the game.",
                TextFatalError,
                MB_ICONERROR | MB_OK);
            goto lbldlg;
        }

        DoublePlay=lpgm->GameType&0x80000000?TRUE:FALSE;
        DoubleBat=lpgm->GameType&0x2000000?TRUE:FALSE;
        Encryption=lpgm->GameType&0x1000000?TRUE:FALSE;

        psrc=lpgm->ProjSRC;
        Level=lpgm->StartLevel;
        NewNumLives=NumLives=lpgm->StartLives;
        Dif=(lpgm->GameType>>16)&0xf;
        Hi=lpgm->HiScore;

        NumBombsL=lpgm->LeftCaughtBombs;
        NumBombsR=lpgm->RightCaughtBombs;
        NumPrizL=lpgm->LeftCaughtPrizes;
        NumPrizR=lpgm->RightCaughtPrizes;

        LostBallL=lpgm->LeftLostBalls;
        LostBallR=lpgm->RightLostBalls;

        LControl=lpgm->GameType&0xf;
        RControl=(lpgm->GameType>>8)&0xf;

        if(LControl==0)
        {
            LJoyPos=lpgm->LContr_Left;
        }
        else
        {
            if(LControl==1)
            {
                LMousePos=lpgm->LContr_Left;
                SensL=5-(float)(LMousePos)*4/10;
                if(SensL<1) SensL=1;
            }
            else
            {
                lpBatA1[0]=lpgm->LContr_Left;
                lpBatA1[1]=lpgm->LContr_Right;
                lpBatA1[2]=lpgm->LContr_Fire;
            }

            if(RControl==0)
            {
                RJoyPos=lpgm->RContr_Left;
            }
            else
            {
                if(RControl==1)
                {
                    RMousePos=lpgm->RContr_Left;
                    SensR=5-(float)(RMousePos)*4/10;
                    if(SensR<1) SensR=1;
                }
                else
                {
                    lpBatA2[0]=lpgm->RContr_Left;
                    lpBatA2[1]=lpgm->RContr_Right;
                    lpBatA2[2]=lpgm->RContr_Fire;
                }
            }
        }

        lpBatA1[4]=LControl;
        lpBatA2[4]=RControl;

        getLeftName(lpgm,buffer);			//Copy players' names
        lstrcpyn(LPlayName,buffer,31);

        if(DoublePlay && !DoubleBat)
        {
            getRightName(lpgm,buffer);
            lstrcpyn(RPlayName,buffer,31);
        }

        prfil[0]=0;
        if(lpgm->nGameProjLen<=12)
            getProjFile(lpgm,prfil);

        BEEPZERO(HeapFree(hHeap,0,lpgm));
    }
    else
    {					//New game
        Level=0;
        NumLives=5;
        Hi=100000;
        DoubleBat=FALSE;
        Encryption=GameEnc;
        NewNumLives=5;
        NumBombsL=NumBombsR=NumPrizL=NumPrizR=0;
        LostBallL=LostBallR=0;

        if(DoublePlay)
        {
            if(LControl==RControl)
            {
                if(LControl==2)
                    if(LControls[0].VirtualCode!=RControls[0].VirtualCode ||
                        LControls[1].VirtualCode!=RControls[1].VirtualCode ||
                        LControls[2].VirtualCode!=RControls[2].VirtualCode)
                        goto no;

                DoubleBat=TRUE;
            }
        }
no:
        lstrcpy(prfil,PlayFile.FileName);
        lstrcat(prfil,".bgp");
    }

    int		dat;

    if(!readProjFile(prfil,0x4*2,&NumLevels,sizeof(NumLevels),&dat))
        goto lblun;

    //Check for right Project
    if(NewGame)
    {
        ProjSRC=dat;
    }
    else
    {
        if(psrc!=dat)
        {
            MessageBox(hDesk,
                "The record refers to unknown game project!",
                TextCantLoad,
                MB_OK | MB_ICONWARNING);
            goto lbldlg;
        }
    }

    if((NumLevels-1)>=999) goto lblun;

    TwoPlayers=(DoublePlay && !DoubleBat)?TRUE:FALSE;
    PlayName[0]=0;

    antiDif=3-Dif;
    SpeedUpTime=3+antiDif;
    MaxR=2+Dif*0.5;
    initR=0.68+(double)1/(200+(rnd()&0xff))+Dif*0.02;
    BUcff=0x80/(9-Dif);

    //set Sound to silence

    LPBYTE	ptr;
    ptr=lpSData;
    for(i=0; i<NUMSOUNDCHUNKS*CHUNKVOL; i++)
    {
        *ptr++=0x80;
    }

    ptr=lpBSData;
    for(i=0; i<NUMSOUNDCHUNKS*CHUNKVOL; i++)
    {
        *ptr++=0x80;
    }

    //Get waveOutput devices

    openwave();

    if(YesSound)
    {
        MMTIME		mmt;		//see if wave device can be tracked

        mmt.wType=TIME_BYTES;

        if(waveOutGetPosition(hwo, &mmt, sizeof(mmt))!=MMSYSERR_NOERROR)
            goto sgp1;

        if(mmt.wType!=TIME_BYTES) goto sgp1;
    }
    else
    {
sgp1:
        TrackSound=0;
    }

    //Set up sounds information

    r=((*(LPDWORD)lpFGrx)-1)*0x8;

    lpNextData=lpGrxStart+*(LPDWORD)&lpGrxStart[r]+
                *(LPWORD)&lpGrxStart[r+0x4]*
                *(LPWORD)&lpGrxStart[r+0x6]*2;

    NumSounds=*(LPDWORD)&lpNextData[0x4];
    lpSInf=(LPDWORD)&lpNextData[0x4*2];

    if((unsigned)(lpNextData+*(LPDWORD)lpNextData-lpFGrx)>GrxFSize)
        goto aftererr;

    preparesound();
    LastYesSound=YesSound;

    //Game dimensions

    ScWpix=dlx;
    ScHpix=dly;
    ScWbyte=ScWpix*2;						//16 bit per one pixel


    CountDwnTime.QuadPart=whatTime().QuadPart;
    LCyc=RCyc=0;
    lpCyc=&LCyc;

    DrawingStarted=FALSE;
    if(!createwindow(nCmdShow)) goto aftererr;
    if(!(hDC=GetDC(hMainWnd)))
    {
        DestroyWindow(hMainWnd);
    }

    takeCursAway();

    //Find out about joysticks
    JOYCAPS		jc;
    JOYINFOEX	jpx;
    RECT		jrec;
    int			jw,jo;

    ZeroMemory(&jpx,sizeof(jpx));
    jpx.dwSize=sizeof(jpx);

    if(joyGetPosEx(JOYSTICKID1,&jpx)==JOYERR_NOERROR)
    {
        joyID=JOYSTICKID1;
    }
    else
    {
        if(joyGetPosEx(JOYSTICKID2,&jpx)==JOYERR_NOERROR)
            joyID=JOYSTICKID2;
        else
            joyID=-1;
    }

    if(DoublePlay)
    {
        if(LControl && RControl) joyID=-1;
    }
    else
        if(LControl) joyID=-1;

    if(joyID!=-1)
    {
        if(joyGetDevCaps(joyID,&jc,sizeof(jc))==JOYERR_NOERROR)
        {
            GetWindowRect(hMainWnd,&jrec);
            jw=jrec.right-jrec.left;
            jo=jc.wXmax-jc.wXmin;

            if(jo<=jw)
            {
                joyDeltaL=(int)((1+(float)LJoyPos/10)*(float)jw/jo);
                joyDeltaR=(int)((1+(float)RJoyPos/10)*(float)jw/jo);
            }
            else
            {
                joyDeltaL=(int)(1+(float)LJoyPos/10);
                joyDeltaR=(int)(1+(float)RJoyPos/10);
            }
        }
    }
    

    setInitVars();


    for(i=0; i<NUMDYN; i++)
        adddynobjN(i,0,0,0,0,0,0x58);

    //Prepare Round

    if(!setRound(Level,Round=0))
        DestroyWindow(hMainWnd);

    __asm
    {
        //convert SCOREs

        mov		eax,[OneUp]
        mov		esi,offset digOne
        call	makeASCIIscore

        mov		eax,[TwoUp]
        mov		esi,offset digTwo
        call	makeASCIIscore

        mov		eax,[Hi]
        mov		esi,offset digHi
        call	makeASCIIscore

    }

    //get game coefficients ready

    BATw_div1=PhysObjInfo[2*4];
    BATh_div1=PhysObjInfo[2*4+1];

    BALLw_div1=PhysObjInfo[4];
    BALLw_div2=BALLw_div1/2;

    BATw_div2=BATw_div1/2;
    BATw_div3=BATw_div1/3;
    BATw_div4=BATw_div1/4;

    BATh_div2=BATh_div1/2;
    BATh_div3=BATh_div1/3;
    BATh_div4=BATh_div1/4;


    // Get game dynamic objects ready


    adddynobjN(1, 0x0000, 90, 42, 0, 0x000e004e, 0x4c);		//1up
    adddynobjN(2,0x0100, 374, 42, 0, 0x000e004e, 0x4c);		//2up
    adddynobjN(3,0x0200, 232, 42, 0, 0x000e004e, 0x4c);		//hi
    adddynobjN(0,0x0300, 24, 372, 0, 0x000a0018, 0x4c);		//lives
    adjliveswidth();

    if(DoublePlay)			//set partition
    {
        adddynobjN(12, 0x0, 
                    (DWORD)ScWpix/2-
                    (*(LPWORD)&lpGrxStart[85*0x8+0x4])/2,
                    (DWORD)ScHpix-((30<<0)-BATh_div2)-
                    (*(LPWORD)&lpGrxStart[85*0x8+0x4+0x2])/2,
                    0x090000, 0x55000055, 0x41);
    }

    // Get game static objects ready

    for(i=0; i<NUMSTAT; i++)
    {
        addstatobj(StatObj[i][0], StatObj[i][1],
                   StatObj[i][2],
                   (StatObj[i][2]&0xff000000)?0x40:0x4b);
    }


    DrawingStarted=TRUE;
    InvalidateRect(hMainWnd, NULL, FALSE);
    SBcomment(TextInitNewLevel);
    SBdrawlevel();


    OverH=DoublePlay?3:4;



startloop:				//M A I N     L O O P//

    if(OverH)
    {
lbl_again:
        if(!::ResetEvent(hMainEvent))
                goto erratloop;

        for(;;)
        {
            DWORD r = WaitForSingleObject(hMainEvent, OverH);
            if(r == WAIT_TIMEOUT)
                break;
            if(r == WAIT_OBJECT_0)
                goto lbl_again;
            if(r == WAIT_FAILED)
                goto erratloop;
        }
    }

    while(PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE))
    {
        if(msg.message==WM_QUIT) goto outloop;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        mainjob();
    }

    mainjob();
    soundjob();
    goto startloop;


outloop:
    ifToAddToLog();
    preclearall();
    IfWaveOpen=FALSE;
    YesSound=FALSE;

    TrackSound=-1;
    lenToPlay=-1;
    begPlay=0;
    endPlay=0;
    NoPlay=FALSE;
    extraSoundOn=FALSE;

    ZeroMemory(&SPump,sizeof(SPump));
    SPump.ChunkToPump=-1;
    goto lbldlg;


erratloop:
    DrawingStarted=FALSE;
    nodraw=TRUE;

    InvalidateRgn(hMainWnd, NULL, FALSE);
    UpdateWindow(hMainWnd);

    MessageBox(hMainWnd,
                "System timer does not function properly!\n"
                "The Batty game cannot be run.",
                "Fatal Error",
                MB_OK | MB_ICONERROR);

    preclearall();
    clearall();
    CloseHandle(hMut);
    CoUninitialize();
    return FALSE;
    
}








__declspec ( naked ) rnd(void)
{
//for information about using local variables here see __LOCAL_SIZE macros
//description

    __asm
    {
        push	ecx

        mov		eax,[rndb]
        mov		ecx,[rnda]

        xor		ecx,eax
        shl		ecx,1
        adc		eax,0
        xor		eax,ecx
        ror		eax,15
        mov		[rnda],ecx
        mov		[rndb],eax

        pop		ecx
        ret
    };
//	return;
}






BOOL __stdcall updatebackground(UINT x, UINT y, UINT ddx, UINT ddy)
{
    __asm
    {
        mov		esi,[x]
        mov		ebx,[y]
        cmp		esi,[ScWpix]
        jae		short	ublb9
        cmp		ebx,[ScHpix]
        jae		short	ublb9

        and		ebx,[BkgHpix_]			//M=hight of bkgrnd pattern - 1
        mov		cl,[powW]
        mov		eax,esi
        shl		ebx,cl

        mov		ecx,[BkgWpix_]			//N
        and		esi,ecx
        inc		ecx
        sub		ecx,esi
        shl		esi,1
        add		ebx,[lpBkg]				//Start of background image in mem
        mov		[i],esi
        mov		edi,[y]
        add		esi,ebx

        imul	edi,[ScWbyte]			//width of screen in bytes
        shl		eax,1
        push	ebx
        add		edi,[lpGrx]				//Start of screen buffer in mem
        mov		ebx,[ScWpix]
        add		edi,eax

        sub		ebx,[ddx]
        jc		short	ublb8
        shl		ebx,1
        mov		[r],ebx					//stand for [PLUS]

        mov		eax,[y]
        mov		ebx,[ddy]
        add		eax,ebx
        cmp		eax,[ScHpix]
        jbe		short	ublb2
        mov		ebx,[ScHpix]
        sub		ebx,[y]
ublb2:
        test	ebx,ebx
        jle		short	ublb8

        pop		eax
        mov		edx,[ddx]
        push	ebp
        cmp		ecx,edx
        jbe		short	ublb3
        mov		ecx,edx
ublb3:
        mov		ebp,[BkgWpix]			//N
ublb4:
        push	ecx
        push	edx
ublb5:
        sub		edx,ecx
        shr		ecx,1
        rep		movsd
        jnc		short	ublb51
        movsw
ublb51:
        test	edx,edx					//CHECK TO REMOVE!!!
        je		short	ublb6
        mov		esi,eax
        mov		ecx,edx
        cmp		edx,ebp
        jbe		short	ublb5
        mov		ecx,ebp
        jmp		short	ublb5
ublb6:
        add		eax,[BkgWbyte]
        add		edi,[r]
        cmp		eax,[lpBkgLast]			//addr of first byte after background image in memory
        jb		short	ublb7
        sub		eax,[BkgSize]			//number of bytes of bkg image
ublb7:
        pop		edx
        mov		esi,eax
        pop		ecx
        add		esi,[i]
        dec		ebx
        jnz		short	ublb4
        pop		ebp
    }
    return TRUE;

    __asm
    {
ublb8:
        pop		ebx
ublb9:
    }
    return FALSE;
}







BOOL __declspec ( naked ) objectdraw(UINT x, UINT y, UINT N)
{
    __asm
    {
        push	ebp
        mov		ebp,esp
        push	ebx
        push	esi
        push	edi

        mov		ebx,[x]
        mov		eax,[y]
        cmp		ebx,[ScWpix]
        jae		short	od7
        cmp		eax,[ScHpix]
        jge		short	od7

        mov		ebx,[N]
        mov		esi,[lpGrxStart]
        lea		ebx,[esi+ebx*8]
        add		esi,[ebx]
        movzx	edx,word ptr [ebx+6]		//EDX=ddy
        movzx	ebx,word ptr [ebx+4]		//EBX=ddx

        mov		edi,eax						//EDI=y
        test	eax,eax
        jns		short	od2
        neg		eax
        sub		edx,eax
        jbe		short	od7
        imul	eax,ebx
        xor		edi,edi
        lea		esi,[esi+eax*2]
        xor		eax,eax
od2:
        add		eax,edx						//EAX=y+ddy
        cmp		eax,[ScHpix]
        jbe		short	od3
        mov		edx,[ScHpix]
        sub		edx,edi
od3:
        xor		eax,eax
        imul	edi,[ScWbyte]
        or		eax,[x]
        add		edi,[lpGrx]
        lea		edi,[edi+eax*2]

        mov		ecx,[ScWbyte]
        sub		ecx,ebx						//EBX=ddx
        sub		ecx,ebx
        mov		[r],ecx

        mov		ecx,ebx
        xor		ebp,ebp
        xor		eax,eax
        or		ebp,2
od4:
        lodsw					//MUST BE ALIGNED!!!!!!!!!!
        test	eax,eax
        jz		short	od52
od5:	stosw
        dec		ebx
        jnz		short	od4
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	od4
        jmp		short	od6

od51:
        lodsw					//MUST BE ALIGNED!!!!!!!!!!
        test	eax,eax
        jnz		short	od5
od52:	add		edi,ebp
        dec		ebx
        jnz		short	od51
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	od51
od6:
        mov		eax,1
od61:
        pop		edi
        pop		esi
        pop		ebx
        pop		ebp

        ret		0xc

od7:
        xor		eax,eax
        jmp		short	od61
    }
}







VOID __declspec ( naked ) cellshadowdraw(UINT x, UINT y)
{
    __asm
    {
        push	ebp
        mov		ebp,esp
        push	ebx
        push	edi
        push	esi

        mov		edi,[y]
        mov		eax,[x]
        imul	edi,[ScWbyte]
        mov		ecx,[ShadeColor]
        lea		edi,[eax*2+edi]
        mov		esi,32				//=33-1
        add		edi,[lpGrx]

        mov		eax,[PureColor]
        mov		edx,15
        mov		ebp,esi
        mov		ebx,[ScWbyte]
        nop

cshd5:
cshd6:
        cmp		[edi+ebp*2],ax
        jnz		short	cshd7
        mov		[edi+ebp*2],cx
cshd7:
        cmp		[edi+ebp*2-2],ax		//MUST BE ALIGNED!!!!!!!!!!!!
        jnz		short	cshd71
        mov		[edi+ebp*2-2],cx
cshd71:
        cmp		[edi+ebp*2-4],ax
        jnz		short	cshd72
        mov		[edi+ebp*2-4],cx
cshd72:
        cmp		[edi+ebp*2-6],ax
        jnz		short	cshd73
        mov		[edi+ebp*2-6],cx

cshd73:
        cmp		[edi+ebp*2-8],ax
        jnz		short	cshd74
        mov		[edi+ebp*2-8],cx
cshd74:
        cmp		[edi+ebp*2-10],ax
        jnz		short	cshd75
        mov		[edi+ebp*2-10],cx
cshd75:
        cmp		[edi+ebp*2-12],ax
        jnz		short	cshd76
        mov		[edi+ebp*2-12],cx
cshd76:
        cmp		[edi+ebp*2-14],ax
        jnz		short	cshd77
        mov		[edi+ebp*2-14],cx
cshd77:

        cmp		[edi+ebp*2-16],ax
        jnz		short	cshd78
        mov		[edi+ebp*2-16],cx
cshd78:
        cmp		[edi+ebp*2-18],ax
        jnz		short	cshd79
        mov		[edi+ebp*2-18],cx
cshd79:
        cmp		[edi+ebp*2-20],ax
        jnz		short	cshd7a
        mov		[edi+ebp*2-20],cx
cshd7a:
        sub		ebp,11
        jns		short	cshd6

        mov		ebp,esi					//32
        add		edi,ebx					//[ScWbyte]
        dec		edx
        jnz		short	cshd5

        pop		esi
        pop		edi
        pop		ebx
        pop		ebp

        ret		0x8
    }
}






BOOL __declspec ( naked ) shadowdraw(UINT x, UINT y, UINT N) //+
{
    __asm
    {
        push	ebp
        mov		ebp,esp
        push	ebx
        push	edi
        push	esi

        mov		ebx,[x]
        mov		eax,[y]
        cmp		ebx,[ScWpix]
        jae		short	shd8
        cmp		eax,[ScHpix]
        jge		short	shd8

        mov		ebx,[N]
        mov		esi,[lpGrxStart]
        lea		ebx,[esi+ebx*8]
        add		esi,[ebx]
        movzx	edx,word ptr [ebx+6]
        movzx	ebx,word ptr [ebx+4]

        mov		edi,eax
        test	eax,eax
        jns		short	shd2
        neg		eax
        sub		edx,eax
        jbe		short	shd8
        imul	eax,ebx
        xor		edi,edi
        lea		esi,[esi+eax*2]
        xor		eax,eax
shd2:
        add		eax,edx
        cmp		eax,[ScHpix]
        jbe		short	shd3
        mov		edx,[ScHpix]
        sub		edx,edi
shd3:
        imul	edi,[ScWbyte]
        mov		eax,[x]
        lea		edi,[edi+eax*2]
        add		edi,[lpGrx]

        lea		eax,[ebx*2]				//EBX=ddx
        mov		[r],eax

        mov		eax,[x]
        add		eax,ebx
        cmp		eax,[ScWpix]
        jbe		short	shd4
        mov		ebx,[ScWpix]
        sub		ebx,[x]
shd4:
        mov		ecx,[ShadeColor]
        xor		ebp,ebp
        mov		eax,[PureColor]
        dec		ebx
        xchg	ebp,ebx
shd5:
        push	ebp
shd6:								//NEXT LINE MUST BE ALIGNED!!!!!
        cmp		[esi+ebp*2],bx		//cmp		word ptr [esi+ebx*2],0
        jz		short	shd7		//jz		short	shd7
        cmp		[edi+ebp*2],ax		//mov		eax,ebp
        jnz		short	shd7		//cmpxchg	[edi+ebx*2],cx
        mov		[edi+ebp*2],cx
shd7:
        dec		ebp
        jns		short	shd6
        pop		ebp
        add		edi,[ScWbyte]
        add		esi,[r]
        dec		edx
        jnz		short	shd5

        mov		eax,1
shd71:
        pop		esi
        pop		edi
        pop		ebx
        pop		ebp

        ret		0xc

shd8:
        xor		eax,eax
        jmp		short	shd71
    }
}







BOOL __declspec ( naked ) celldraw(UINT x, UINT y, UINT N)
{
    __asm
    {
        push	ebp
        mov		ebp,esp
        push	ebx
        push	edi
        push	esi

        mov		ebx,[x]
        mov		eax,[y]
        cmp		ebx,[ScWpix]
        jae		short	cd7
        cmp		eax,[ScHpix]
        jge		short	cd7

        mov		ebx,[N]
        mov		esi,[lpGrxStart]
        lea		ebx,[esi+ebx*8]
        add		esi,[ebx]
        movzx	edx,word ptr [ebx+6]		//EDX=ddy
        movzx	ebx,word ptr [ebx+4]		//EBX=ddx

        mov		edi,eax						//EDI=y
        test	eax,eax
        jns		short	cd2
        neg		eax
        sub		edx,eax
        jbe		short	cd7
        imul	eax,ebx
        xor		edi,edi
        lea		esi,[esi+eax*2]
        xor		eax,eax
cd2:
        add		eax,edx						//EAX=y+ddy
        cmp		eax,[ScHpix]
        jbe		short	cd3
        mov		edx,[ScHpix]
        sub		edx,edi
cd3:
        xor		eax,eax
        imul	edi,[ScWbyte]
        xor		eax,[x]
        mov		ecx,[ScWbyte]
        lea		edi,[edi+eax*2]
        sub		ecx,ebx						//EBX=ddx
        add		edi,[lpGrx]

        sub		ecx,ebx
        xor		eax,eax
        mov		[r],ecx

        mov		ebp,2
        mov		ecx,ebx
cd4:
        lodsw					//MUST BE ALIGNED!!!!!!!!!!
        dec		eax
        jz		short	cd51
        inc		eax
        jz		short	cd53
        stosw
        dec		ebx
        jnz		short	cd4
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	cd4
        jmp		short	cd6

        nop
        nop

cd42:	inc		eax				//MUST BE ALIGNED!!!!!!!!
        jz		short	cd53
        stosw
        dec		ebx
        jnz		short	cd4
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	cd4
        jmp		short	cd6

        nop
        nop
        nop
cd5:
        lodsw					//MUST BE ALIGNED!!!!!!!!
        dec		eax
        jnz		short	cd42
cd51:	mov		eax,[FillColor]
        stosw
        dec		ebx
        jnz		short	cd5
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	cd5
        jmp		short	cd6


cd51a:	dec		eax				//MUST BE ALIGNED!!!!!!
        jz		short	cd51
        inc		eax
        stosw
        dec		ebx
        jnz		short	cd4
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	cd4
        jmp		short	cd6

        nop
        nop

cd52:
        lodsw					//MUST BE ALIGNED!!!!!!!!
        test	eax,eax
        jnz		short	cd51a
cd53:	add		edi,ebp
        dec		ebx
        jnz		short	cd52
        add		edi,[r]
        mov		ebx,ecx
        dec		edx
        jnz		short	cd52

cd6:
        mov		eax,1
cd61:
        pop		esi
        pop		edi
        pop		ebx
        pop		ebp

        ret		0xc

cd7:
        xor		eax,eax
        jmp		short	cd61
    }
}






VOID __stdcall adjliveswidth(VOID)
{
    __asm
    {
        mov		eax,[NumLives]
        imul	eax,eax,24			//24=width of one life sign
        mov		edi,[lpDyn]
        mov		[edi+0xc],eax
        mov		[edi+0x1c],eax

        mov		edx,edi
    }

    return;
}




VOID __stdcall drawlives(VOID)
{
    __asm
    {
        call	adjliveswidth

        mov		ebx,edx
        call	lastatmove
    }

    return;
}





VOID __stdcall setball(DWORD x, DWORD y, double R, DWORD alpha,
                       UINT Ball_012, BOOL NeedForStickySet,
                       BOOL Left_or_Right)	//TRUE=right BAT
{
    double*		lpR;

    if(NeedForStickySet)
    {
        x=y=0;
    }

    adddynobjN(Ball_012+32,
                0x1, x, y, 0x80010000,
                0x30000000|(0x2c+(Round&0x3)), 0x0);

    __asm
    {
        mov		ebx,[lpDyn]			//Set initial coordinates for
        mov		eax,[Ball_012]		//STICKY BALL
        and		eax,0x3
        lea		ecx,[eax+32]
        imul	ecx,ecx,ONEDYNOBJ
        lea		edx,[lpBallsAux]
        add		ebx,ecx

        mov		edx,[edx+eax*4]

        cmp		[NeedForStickySet], TRUE
        jnz		short	sb1

        mov		eax,[ebx+0x28]
        mov		[edx+0x24],eax
        mov		eax,[ebx+0x2c]
        mov		[edx+0x28],eax

        mov		dword ptr [edx+0x1c],BATWIDTH/2-BALLWIDTH/2
        mov		dword ptr [edx+0x20],-(BALLWIDTH)
sb1:
        mov		eax,[alpha]
        mov		[edx+0x18],eax
        xor		eax,eax
        mov		[edx+0x30],eax			//RightMOST X
        mov		[edx+0x38],eax			//BottomMOST Y
        dec		eax
        mov		[edx+0x2c],eax			//LeftMOST X
        mov		[edx+0x34],eax			//TopMOST Y
        mov		ebx,[Left_or_Right]
        lea		eax,[edx+0x10]
        and		ebx,TRUE
        mov		[lpR],eax

        shl		ebx,1
        or		ebx,0x01000000
        mov		[edx+0x3c],ebx			//ExtraFlags
    }

    *lpR=R;

    return;
}





double __fastcall sqart(double val)
{
    __asm
    {
        fld		[val]
        ftst
        fnstsw	ax
        sahf
        jbe		short	sq1
        fsqrt
sq1:
        fstp	[val]
    }

    return val;
}








void __stdcall preparesound(void)
{
    DWORD		a;

    lpMData=(MUSICDATA*)HeapAlloc(hHeap,
                                    HEAP_ZERO_MEMORY,
                                    sizeof(MUSICDATA)*NumSounds);
    if(!lpMData)
    {
        YesSound=FALSE;
        return;
    }

    for(a=0; a<NumSounds; a++)		//set music data
    {
        lpMData[a].AddrOfWaveBlock=(LPBYTE)lpSInf+lpSInf[a*2];
        lpMData[a].LengthOfWaveBlock=lpSInf[a*2+1];
    }

    return;	
}






BOOL __stdcall closewave(void)		//closes WAVEOUT AUDIO DEVICE
{
    BOOL			y=TRUE;
    int				i;

    if(YesSound&&IfWaveOpen)
    {
        waveOutReset(hwo);

        for(i=0; i<NUMSOUNDCHUNKS; i++)		//UnPrepare Headers
        {
            whdr[i].lpData=(LPSTR)(LPBYTE)(lpSData+i*CHUNKVOL);
            whdr[i].dwBufferLength=CHUNKVOL;
            whdr[i].dwFlags=0;

            if(waveOutUnprepareHeader(hwo,
                                      &whdr[i],
                                      sizeof(whdr[i]))!=MMSYSERR_NOERROR)
            {
                y=FALSE;
            }
        }

        if(waveOutClose(hwo)!=MMSYSERR_NOERROR) y=FALSE;
        IfWaveOpen=FALSE;

        begPlay=0;
        endPlay=0;
        lenToPlay=0;
        SPump.ChunkToPump=-1;

        for(i=0; i<(NUMSOUNDCHUNKS*CHUNKVOL); i++)	//set sound CHUNKS to silence
            lpSData[i]=0x80;
    }

    return y;
}





void __stdcall openwave(void)	//opens WAVEOUT AUDIO DEVICE
{

    PCMWAVEFORMAT	wfx;
    int				i, y=FALSE;

    if(!IfWaveOpen)
    {
        if(!(i=waveOutGetNumDevs())) goto noopen;	//get # of music devices

        wfx.wf.wFormatTag=WAVE_FORMAT_PCM;
        wfx.wf.nChannels=1;
        wfx.wf.nSamplesPerSec=11025;
        wfx.wf.nAvgBytesPerSec=11025;
        wfx.wf.nBlockAlign=1;
        wfx.wBitsPerSample=8;

        if(i==1)
            i--;
        else
            i=WAVE_MAPPER;

        if(waveOutOpen(NULL,					//check if it is supported
                        (UINT)i,
                        (tWAVEFORMATEX *)&wfx,
                        0,
                        0,
                        WAVE_FORMAT_QUERY)!=MMSYSERR_NOERROR) goto noopen;

        if(waveOutOpen(&hwo,					//open device
                        (UINT)i,
                        (tWAVEFORMATEX*)&wfx,
                        NULL,
                        0,
                        CALLBACK_NULL)!=MMSYSERR_NOERROR) goto noopen;



        for(i=0; i<NUMSOUNDCHUNKS; i++)		//Prepare Headers
        {
            whdr[i].lpData=(LPSTR)((LPBYTE)(lpSData+i*CHUNKVOL));
            whdr[i].dwBufferLength=CHUNKVOL;
            whdr[i].dwFlags=0;

            if(waveOutPrepareHeader(hwo,
                                    &whdr[i],
                                    sizeof(whdr[i]))!=MMSYSERR_NOERROR)
            {
                waveOutClose(hwo);
                goto noopen;
            }
        }

        y=TRUE;			//Device is successfully opened
        IfWaveOpen=TRUE;
noopen:

        YesSound=y;
    }

    return;
}






VOID __fastcall soundjob(VOID)	//plays all sounds
{
    DWORD		d, e, r;
    LONG		c;
    DWORD		Plen, p1, max;
    BOOL		noPump;

    static		SoundWarn=20;


    if(NoPlay) return;			//If sounds are stopped by lost focus

    if(lenToPlay>0)
    {
        d=begPlay;

        if(begPlay<endPlay)
        {
            do
            {
                if(!(whdr[begPlay&(NUMSOUNDCHUNKS-1)].dwFlags&WHDR_DONE))
                    goto sj1;
            }
            while((++begPlay)<endPlay);

            if(SoundWarn<=0) goto sj1;
            if(!(SoundWarn--))
                SBcomment("Sound rate incompatible!");
        }


sj1:
        c=(begPlay-d)*CHUNKVOL;
        if(c>0)						//erase/update old CHUNKS
        {
            d=d&(NUMSOUNDCHUNKS-1);			//start CHUNK #
            e=begPlay&(NUMSOUNDCHUNKS-1);	//end CHUNK #

            if(e>d)
            {
                LPBYTE		ptr;
                DWORD	a1, r1;
                ptr=lpSData+d*CHUNKVOL;			//cleaning
                a1=e*CHUNKVOL-d*CHUNKVOL;
                for(r1=0; r1<a1; r1++)
                    *ptr++=0x80;
                ptr=lpBSData+d*CHUNKVOL;
                for(r1=0; r1<a1; r1++)
                    *ptr++=0x80;

                if(SPump.ChunkToPump==d)
                {								//PUMPING
                    noPump=TRUE;
                    max=0;			//c=# of bytes to pump

                    int	i1;
                    for(i1=0; i1<8; i1++)
                    {
                        if(SPump.ChunksInfo[i1].BytesPump>0)
                        {
                            if((unsigned)c>=SPump.ChunksInfo[i1].BytesPump)
                            {
                                Plen=SPump.ChunksInfo[i1].BytesPump;
                            }
                            else
                            {
                                Plen=c;
                                noPump=FALSE;
                            }

                            mixsound(SPump.ChunksInfo[i1].lpSourcePump,
                                     &lpSData[d*CHUNKVOL],
                                     Plen);

                            if(max<Plen) max=Plen;

                            SPump.ChunksInfo[i1].lpSourcePump+=Plen;
                            SPump.ChunksInfo[i1].BytesPump-=Plen;
                        }
                    }

                    lenToPlay+=max;

                    if(noPump) 
                        SPump.ChunkToPump=-1;
                    else
                        SPump.ChunkToPump=e;
                }
            }
            else
            {
                LPBYTE	ptr11, ptr12;
                DWORD	r2, a2;
                ptr11=lpSData+d*CHUNKVOL;
                ptr12=lpBSData+d*CHUNKVOL;
                a2=NUMSOUNDCHUNKS*CHUNKVOL-d*CHUNKVOL;
                for(r2=0; r2<a2; r2++)
                {
                    *ptr11=0x80;					//cleaning 1
                    *ptr12=0x80;
                    ptr11+=1; ptr12+=1;
                }
                ptr11=lpSData;
                ptr12=lpBSData;
                a2=e*CHUNKVOL;
                for(r2=0; r2<a2; r2++)
                {
                    *ptr11=0x80;					//cleaning 2
                    *ptr12=0x80;
                    ptr11+=1; ptr12+=1;
                }

                if(SPump.ChunkToPump==d)
                {
                    p1=(NUMSOUNDCHUNKS-d)*CHUNKVOL;		//pumping
                    max=0;
                    noPump=TRUE;			//c=# of bytes to pump

                    int	i2;
                    for(i2=0; i2<8; i2++)
                    {
                        if(SPump.ChunksInfo[i2].BytesPump>0)
                        {
                            if((unsigned)c>=SPump.ChunksInfo[i2].BytesPump)
                            {
                                Plen=SPump.ChunksInfo[i2].BytesPump;
                            }
                            else
                            {
                                Plen=c;
                                noPump=FALSE;
                            }

                            if(Plen<=p1)
                            {
                                mixsound(SPump.ChunksInfo[i2].lpSourcePump,
                                         &lpSData[d*CHUNKVOL],
                                         Plen);
                            }
                            else
                            {
                                mixsound(SPump.ChunksInfo[i2].lpSourcePump,
                                         &lpSData[d*CHUNKVOL],
                                         p1);

                                mixsound(SPump.ChunksInfo[i2].lpSourcePump+p1,
                                         lpSData,
                                         Plen-p1);
                            }

                            if(max<Plen) max=Plen;

                            SPump.ChunksInfo[i2].lpSourcePump+=Plen;
                            SPump.ChunksInfo[i2].BytesPump-=Plen;
                        }
                    }

                    lenToPlay+=max;

                    if(noPump) SPump.ChunkToPump=-1;
                    else
                        SPump.ChunkToPump=e;
                }

            }
        }



        if((endPlay-begPlay)>=NUMPLAYCHUNKS)	//make maximum NUMPLAYCHUNKS played at once
            goto noplay;


        c=(NUMPLAYCHUNKS-(endPlay-begPlay))*CHUNKVOL;	//c=how more bytes to waveOut

        if(lenToPlay<=c)
        {
            d=endPlay+((!(lenToPlay&(CHUNKVOL-1))) ?
                (unsigned)lenToPlay/CHUNKVOL : 
                    (((unsigned)lenToPlay/CHUNKVOL)+1));

            lenToPlay=0;
        }
        else
        {
            d=begPlay+NUMPLAYCHUNKS;		//d=new endPlay value
            lenToPlay-=c;
        }



        e=(d-endPlay)*CHUNKVOL;		        //mix with BAT hits
        r=(endPlay&(NUMSOUNDCHUNKS-1))*CHUNKVOL;

        if((r+e)<=(NUMSOUNDCHUNKS*CHUNKVOL))
        {
            mixsound(&lpBSData[r],
                     &lpSData[r],
                     e);
        }
        else
        {
            i=(NUMSOUNDCHUNKS*CHUNKVOL)-r;

            mixsound(&lpBSData[r],
                     &lpSData[r],
                     i);

            mixsound(lpBSData,
                     lpSData,
                     e-i);
        }


        do							//PLAYING sound chunks
        {
            if(waveOutWrite(hwo, 
                            &whdr[(endPlay++)&(NUMSOUNDCHUNKS-1)],
                            sizeof(whdr[0]))!=MMSYSERR_NOERROR)

                            goto playerr;
plbk:;
        }
        while(endPlay<d);

    }



    else				//sound is playing up (clean-up needed?)
    {
        if(begPlay<endPlay)
        {
            d=begPlay;

            do
            {
                if(!(whdr[begPlay&(NUMSOUNDCHUNKS-1)].dwFlags&WHDR_DONE))
                    break;
            }
            while((++begPlay)<endPlay);

            c=(begPlay-d)*CHUNKVOL;
            if(c>0)						//erase old CHUNKS
            {
                d=d&(NUMSOUNDCHUNKS-1);			//start CHUNK #
                e=begPlay&(NUMSOUNDCHUNKS-1);	//end CHUNK #

                if(e>d)
                {
                    LPBYTE		ptr3;
                    DWORD	a3, r3;
                    ptr3=lpSData+d*CHUNKVOL;		//cleaning
                    a3=e*CHUNKVOL-d*CHUNKVOL;
                    for(r3=0; r3<a3; r3++)
                        *ptr3++=0x80;
                    ptr3=lpBSData+d*CHUNKVOL;
                    for(r3=0; r3<a3; r3++)
                        *ptr3++=0x80;

                }
                else
                {
                    LPBYTE	ptr41, ptr42;
                    DWORD	r4, a4;
                    ptr41=lpSData+d*CHUNKVOL;
                    ptr42=lpBSData+d*CHUNKVOL;
                    a4=NUMSOUNDCHUNKS*CHUNKVOL-d*CHUNKVOL;
                    for(r4=0; r4<a4; r4++)
                    {
                        *ptr41=0x80;					//cleaning 1
                        *ptr42=0x80;
                        ptr41+=1; ptr42+=1;
                    }
                    ptr41=lpSData;
                    ptr42=lpBSData;
                    a4=e*CHUNKVOL;
                    for(r4=0; r4<a4; r4++)
                    {
                        *ptr41=0x80;					//cleaning 2
                        *ptr42=0x80;
                        ptr41+=1; ptr42+=1;
                    }
                }
            }
        }
    }
    
noplay:
    return;


playerr:

    SBcomment("Sound malfunction!");
    goto plbk;
}





VOID	setGameWndDims(int ClientWidth, int ClientHeight)
{
    wW=ClientWidth+GetSystemMetrics(SM_CXFIXEDFRAME)*2;
    dlx=ClientWidth;

    dly=ClientHeight;
    wH=dly+GetSystemMetrics(SM_CYFIXEDFRAME)*2+
            GetSystemMetrics(SM_CYCAPTION);

    return;
}




BOOL	setRound(int level012, int round012)
{
    if(!getRound(level012,round012))
    {
        DrawingStarted=FALSE;
        nodraw=TRUE;
        InvalidateRect(hMainWnd, NULL, FALSE);

        MessageBox(hMainWnd,
            "Unable to load Round from the game project.",
            TextFatal,
            MB_ICONERROR | MB_OK);
        return FALSE;
    }

    return TRUE;
}





VOID	setInitVars(VOID)
{
    OneUp=0;				//Score for 1UP
    TwoUp=0;				//Score for 2UP
    TotScore=0;
    OneUpLife=SCOREFORLIFE;

    Player=0;
    ImNum=0;
    lpCurDyn=lpDyn;
    lpCurStat=lpStat;
    return;
}





BOOL	protectInfo(VOID)
{
    int		IDs[]={IDS_STRING1,IDS_STRING3,IDS_STRING4,IDS_STRING5,
                    IDS_STRING6,IDS_STRING7,IDS_STRING120,IDS_STRING121,
                    IDS_STRING122,IDS_STRING123,/*IDS_STRING125*/};

    static	DWORD	c,txtSRC=0xe5b498e6;

    c=0;
    for(int y=0; y<sizeof(IDs); y++)
    {
        LoadString(hInst,IDs[y],buffer,sizeof(buffer)-1);
        c+=countCodeFromText(buffer);
    }

    InfoOK=(c==txtSRC);

    return InfoOK;
}