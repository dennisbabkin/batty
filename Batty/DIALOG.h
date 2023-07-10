//
//  Batty game (partial)
//
//  Mostly C code for the initial welcome dialog & selection controls for the game
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


extern	HICON		hSmallIcon, hBigIcon;
extern	HBITMAP		hBitmapDel, hBitmapDel_;
extern	int			wW,wH;
extern	char		buffer[0x104];
extern	DWORD		BUcoef,NumCellsOut,BatColor,PureColor,ShadeColor,
                    FillColor;
extern	UINT		BkgSize;
extern	LPVOID		lpBkg, lpBkgLast;
extern	LPBYTE		lpGrxStart;
extern	DWORD		BatCols[4],ShadeCols[4],PureCols[4];
extern	HWND		hDesk;

extern	char*		getfilename(LPSTR File);


int		DlgIDs[]={IDC_BUTPLAY,				//0
                    IDC_BUTNEWGAME,			//1
                    IDC_BUTREM1,			//2
                    IDC_BUTREM2,			//3
                    IDC_COMBO1,				//4		//Play combo
                    IDC_COMBO3,				//5		//New Levels combo
                    IDC_RADIO1PL,			//6
                    IDC_RADIO2PL,			//7
                    IDC_RADIOSINGLE,		//8
                    IDC_RADIODOUBLE,		//9
                    IDC_STRBAT,				//10
                    IDC_RADIOJOY_R,			//11
                    IDC_RADIOMOUSE_R,		//12
                    IDC_RADIOKEY_R,			//13
                    IDC_SLIDERJOY_R,		//14
                    IDC_SLIDERMOUSE_R,		//15
                    IDC_BUTSET_R,			//16
                    IDC_SLIDERJOY_L,		//17
                    IDC_SLIDERMOUSE_L,		//18
                    IDC_RADIOJOY_L,			//19
                    IDC_RADIOMOUSE_L,		//20
                    IDC_RADIOKEY_L,			//21
                    IDC_COMBO4,				//22	//Scr Size in NewGame
                    IDC_BUTSET_L,			//23
                    IDC_COMBO5,				//24	//Difficulty
                    IDC_COMBO2,				//25	//Scr Size (Play)
};

int		PlayTextIDs[]={IDC_STGAME,IDC_STLEVELS,
            IDC_STPLAYER_L,IDC_STCONTROLS_L,
            IDC_STLOSTBALL_L,IDC_STPRIZES_L,
            IDC_STBOMBS_L,IDC_STRATIO_L,
            IDC_STPLAYER_R,IDC_STCONTROLS_R,
            IDC_STLOSTBALL_R,IDC_STPRIZES_R,
            IDC_STBOMBS_R,IDC_STRATIO_R,
            IDC_STLEVPASSED,IDC_STTIMEPLAYED,
            IDC_STLASTPLAYED,IDC_STDIF,IDC_STENCC,IDC_STGAM};

HWND	hDlgWnds[SIZEOF(DlgIDs)];
int		Opt=0x0;
int		LJoyPos=1;		//Sensitivites
int		RJoyPos=1;
int		LMousePos=7;
int		RMousePos=7;
int		LControl=1;		//Controls types (0-Joystick,1-Mouse,2-Keyboard)
int		RControl=2;
int		Dif=1;			//0-Baby,1-Normal,2-Enough,3-Hard
int		ScrSizRat;		//Screen size ration [1-9]
BOOL	GameEnc=FALSE;
KEYS	LControls[3]={VK_LEFT,0x8,VK_RIGHT,0x8,VK_CONTROL,0};
KEYS	RControls[3]={'Q',0,'E',0,'W',0};

int		ScrMag=1;		//Screen magnification
int		PlaySel=0;
char	ProjSRC;

char	SubKey[]="Software\\Batty\\Data###";
char	ValueName[]="Inf#";
char	*Text1[]={
        "Game:","Levels:",
        "Player:","Controls:","Lost Ball:","Prizes caught:",
        "Bombs hit:","Play ratio:","Levels passed:",
        "Time played:","Last played:","Difficulty:",
        "Encrypted","Screen:"};

BOOL	IfGameEnc;		//TRUE if currently selected game is encrypted

OLDPROC	OldProcs[3];
HWND	hKeyDlg;
BOOL	DblPl,Dbl;

DWORD	GameDelPass;	//Password for currently selected game
int		CurGameNum=100;	//Consecutive game number that is seleted

BOOL	NewGame;		//If TRUE => new game and all info is in 'PlayFile' struct, otherwise use 'PlayStruct' for info
PLAY	PlayStruct;		//Currently selected game to play.
BGP		PlayFile;		//Bears info about New Game (that is not in PlayStruct, yet)
RECT	DlgRec;			//Window screen rectangle of the Controls window

LONG	OldShowProc;
HBITMAP	hBatty,hJust;
HWND	hIniDlg;
int		ShowMode=0;
BYTE	MainCol=161, ShadCol=222;
int		TextShowTime=1000*7;
int		HallShowTime=1000/4;
int		HallRep;
#define TIMESREP	30			//Number of repetitions for HallFame flicker
int		HallColor=RGB(0,0,187);
LPBYTE	lpHallOfFame;

HWND	hTextDlg;
BOOL	BLMpresent;
BYTE	BLMcode;
BOOL	Shct_restore=TRUE;



LRESULT CALLBACK	InitProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID				initDialog(HWND hDlg);
VOID				setOption(int Option);
VOID				showControls(VOID);
VOID				setCheck(int check012, int initWnd);
VOID				setScrSizeCombo(int Value, int ComboIDNum);
VOID				enableRControls(VOID);
VOID				decodeSegment(LPBYTE lpStart, DWORD InitOffset, int NumBytes);
BOOL				fillLevelsCombo(int ComboID);
BOOL				releaseLevelsCombo(int ComboID);
VOID				enableButton(int ID, BOOL TrueToEnable);
BOOL				copyLevelsComboItemInfo(int Item, LPBGP lpInf);
VOID				codeSegment(LPBYTE lpStart, DWORD InitOffset, int NumBytes);
BYTE				getSRC(LPBYTE lpStart, int NumBytesWithoutLastSRCByte);
BYTE				getSRCEx(LPBYTE lpStart, int NumBytesWithoutLastSRCByte, BYTE StartSRC);
BOOL				writeRegKey(UINT Data012etc, UINT Inf01_9, DWORD OffsetFromKeyBegin, LPVOID lpFrom, DWORD NumBytes, BOOL TrueForNew);
BOOL				readRegKey(UINT Data012etc, UINT Inf01_9, DWORD OffsetFromKeyBegin, LPVOID lpWhere, DWORD NumBytes);
BOOL				setBattyPath(VOID);
LPGAME				getGameStruct(UINT Data012etc, UINT Inf01_9, int Game0123);
LPFINDGAME			findGameFirst(VOID);
BOOL				findGameNext(LPFINDGAME lpFindGameFirst);
VOID				closeGameFind(LPFINDGAME lpFindGameFirst);
BOOL				fillPlayCombo(int ComboID);
VOID				setPlayScreen(BOOL Enabled, LPGAME lpGame, HWND hDlg);
LRESULT CALLBACK	KeyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	KeyDlgSubProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam);
BOOL				writeNewGame(int GameNumber012, LPSTR GameName, LPSTR ProjFile, LPSTR LeftName, LPSTR RightName, DWORD EncrPass);
ULARGE_INTEGER		whatTime(VOID);
int					getGameName(LPGAME lpGameStruct, LPSTR lpDest);
BOOL				setPlayScrByComboSel(BOOL Enabled);
int					getProjFile(LPGAME lpGameStruct, LPSTR lpDest);
int					getLeftName(LPGAME lpGameStruct, LPSTR lpDest);
int					getRightName(LPGAME lpGameStruct, LPSTR lpDest);
int					combineWith(int TextIndex, LPSTR lpAppend, HWND hDlg);
BOOL				readProjFile(LPSTR lpFile, DWORD FileOffs, LPVOID lpWhere, DWORD NumBytes, int *SRCorNULL);
BOOL				getProjText(LPSTR lpProjFile, LPSTR lpWhere, BOOL TrueForProjName);
BOOL				deleteGame(int GameNumber012);
LRESULT CALLBACK	InputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DWORD				countCodeFromText(LPSTR lpText);
int					hexFromChar(char Letter);
BOOL				deleteSelectedProject(HWND hDlg);
LRESULT CALLBACK	ShowProc(HWND hWnd,	UINT mes, WPARAM wParam, LPARAM lParam);
LPVOID				getHallOfFame(VOID);
int					addHallOfFameRecord(int Score, LPGAME lpGame, int Flags);
BOOL				addOrCheck(int NumProjs, LPSTR lpFile, BOOL *mask, BOOL TrueToCheck);

LPSTR				getProjPath(VOID);
BOOL				getRound(int level012, int round012);
LRESULT CALLBACK	InfoProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL				ifToAddToLog(VOID);
BOOL				writeOldGame(int GameNumber012);
VOID				addRecord(VOID);
LRESULT CALLBACK	TextProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID				inGameText(LPSTR lpText);




LRESULT CALLBACK InitProc(HWND hDlg,
                          UINT message,
                          WPARAM wParam,
                          LPARAM lParam)
{
    static	INPUT	InputPass={"Password",
                            "Enter the password to remove this game record:",
                            "",ES_PASSWORD,100};
    static INPUT	InPass={"Password",
                            "Enter the password for this play:",
                            "",ES_PASSWORD,100};
    static	HWND	hShowWnd;

    switch (message)
    {
        case WM_INITDIALOG:
            {
                hJust=LoadBitmap(hInst,(LPSTR)IDB_JUST);

                hShowWnd=GetDlgItem(hDlg,IDC_SHOW);
                OldShowProc=SetWindowLong(hShowWnd,
                                GWL_WNDPROC,(LONG)ShowProc);
                initDialog(hDlg);

                MainCol=161;
                ShadCol=222;
                SetTimer(hDlg,1,lParam?HallShowTime:TextShowTime,NULL);
                HallRep=TIMESREP;
                return FALSE;
            }



        case WM_COMMAND:
            {
                BOOL	re;
                int i,j,a;
                HGLOBAL	hGlob;
                LPVOID	lpMem;
                LPGAME	lpgm;
                LPSTR	lpPass;
                int		sel;
                
                i=LOWORD(wParam);
                j=HIWORD(wParam);

                if(i==IDC_BUTPLAY)
                {
                    sel=SendMessage(hDlgWnds[4],CB_GETCURSEL,0,0);
                    if(sel!=CB_ERR)
                    {
                        hGlob=(HGLOBAL)SendMessage(hDlgWnds[4],
                                CB_GETITEMDATA,sel,0);
                        if((int)hGlob!=CB_ERR)
                            if(lpMem=GlobalLock(hGlob))
                            {
                                NewGame=FALSE;
                                MoveMemory(&PlayStruct,lpMem,
                                    sizeof(PLAY));
                                GlobalUnlock(hGlob);

                                if(lpgm=(LPGAME)
                                    getGameStruct(PlayStruct.Data012etc,
                                        PlayStruct.Inf01_9,
                                        PlayStruct.Game0123))
                                {
                                    re=TRUE;
                                    if(lpgm->GameType&0x1000000)
                                    {
                                        re=FALSE;
                                        if(lpPass=(LPSTR)DialogBoxParam(hInst,
                                                    (LPSTR)IDD_INPUT,hDlg,
                                                    (DLGPROC)InputProc,
                                                    (LPARAM)&InPass))
                                        {
                                            a=countCodeFromText(lpPass);
                                            if(a)
                                                if(a^lpgm->EncryptCode)
                                                {
                                                    MessageBox(hDlg,
                                                        "Sorry, but the password does not match!",
                                                        TextCantLoad,
                                                        MB_OK |MB_ICONWARNING);
                                                }
                                                else
                                                    re=TRUE;
                                        }
                                    }

                                    HeapFree(hHeap,0,lpgm);
                                    if(re)
                                        goto play;
                                }
                                break;
                            }
                    }
                    break;
                }

                if (i==IDC_BUTNEWGAME || i==IDCANCEL) 
                {
                    if(i==IDC_BUTNEWGAME)
                    {
                        NewGame=TRUE;
                        hGlob=(HGLOBAL)SendMessage(
                                    hDlgWnds[5],CB_GETITEMDATA,
                                    SendMessage(hDlgWnds[5],
                                    CB_GETCURSEL,0,0),0);

                        if(lpMem=GlobalLock(hGlob))
                        {
                            MoveMemory(&PlayFile,lpMem,sizeof(BGP));
                            GlobalUnlock(hGlob);
                        }
                        else
                        {
                            PlayFile.DelPassword=0;
                            lstrcpy(PlayFile.FileName,"als");
                            PlayFile.Flags=0x1;
                        }
                    }
play:
                    BEEPZERO(KillTimer(hDlg,1));

                    GetWindowRect(hDlg,&DlgRec);

                    //Save old value

                    LJoyPos=SendMessage(hDlgWnds[17],TBM_GETPOS,0,0);
                    RJoyPos=SendMessage(hDlgWnds[14],TBM_GETPOS,0,0);
                    LMousePos=SendMessage(hDlgWnds[18],TBM_GETPOS,0,0);
                    RMousePos=SendMessage(hDlgWnds[15],TBM_GETPOS,0,0);

                    lpBatA1[0]=LControls[0].VirtualCode;
                    lpBatA1[1]=LControls[1].VirtualCode;
                    lpBatA1[2]=LControls[2].VirtualCode;
                    lpBatA1[4]=LControl;

                    SensL=5-(float)(LMousePos)*4/10;
                    if(SensL<1) SensL=1;
                    SensR=5-(float)(RMousePos)*4/10;
                    if(SensR<1) SensR=1;

                    lpBatA2[0]=RControls[0].VirtualCode;
                    lpBatA2[1]=RControls[1].VirtualCode;
                    lpBatA2[2]=RControls[2].VirtualCode;
                    lpBatA2[4]=RControl;

                    NumLives=5;
                    Dif=SendMessage(hDlgWnds[24],CB_GETCURSEL,0,0);
                    if(Dif>3) Dif=3;

                    GameEnc=(SendMessage(GetDlgItem(hDlg,IDC_ENCRCHECK2),
                        BM_GETCHECK,0,0)&BST_CHECKED)?TRUE:FALSE;

                    a=(i==IDC_BUTPLAY)?25:22;
                    SendMessage(hDlgWnds[a],CB_GETLBTEXT,
                        (WPARAM)SendMessage(hDlgWnds[a],CB_GETCURSEL,
                        0,0),(LPARAM)buffer);
                    a=buffer[0]-'1';
                    if((unsigned)a>=9) a=8;
                    ScrSizRat=a+1;
                    ScrMag=1;

                    DoublePlay=FALSE;
                    if(Opt&0x1 || (Opt&0x3)==0x2)
                        DoublePlay=TRUE;

                    PlaySel=SendMessage(hDlgWnds[4],CB_GETCURSEL,0,0);
                    if(PlaySel==CB_ERR) PlaySel=0;

                    //Release Levels and Play combo memory

                    BEEPZERO(releaseLevelsCombo(4));
                    BEEPZERO(releaseLevelsCombo(5));

                    BEEPZERO(HeapFree(hHeap,0,lpHallOfFame));

                    BEEPZERO(DeleteObject(hJust));

                    SetWindowLong(hShowWnd,GWL_WNDPROC,OldShowProc);

                    EndDialog(hDlg, i==IDCANCEL?FALSE:TRUE);
                    return TRUE;
                }

                if(i==IDC_BUTSET_L || i==IDC_BUTSET_R)
                {
                    DialogBoxParam(hInst,(LPSTR)IDD_KEYSET,hDlg,
                        (DLGPROC)KeyProc,i);
                    break;
                }


                if(i==IDC_BUTREM1)		//Remove Game
                {
                    LPSTR	lpPass;
                    DWORD	p;

                    if(MessageBox(hDlg,
                        "Do you want to remove this game record?",
                        TextDeleteConfirm,
                        MB_YESNO | MB_ICONQUESTION)==IDNO) break;

                    if(!IfGameEnc)
                    {
                        SendMessage(hDlg,WM_APP+2,0,0);
                        break;
                    }

                    if(lpPass=(LPSTR)DialogBoxParam(hInst,
                                (LPSTR)IDD_INPUT,hDlg,
                                (DLGPROC)InputProc,(LPARAM)&InputPass))
                    {
                        if(!(p=countCodeFromText(lpPass))) break;

                        SendMessage(hDlg,WM_APP+1,p,0);

                        if(!RemoveProp(hDlg,ClassName))
                            MessageBox(hDlg,
                                "Password is incorrect. "
                                "The record was not removed!",
                                TextCantRemove,
                                MB_OK | MB_ICONWARNING);
                    }
                    break;
                }


                if(i==IDC_BUTREM2)		//Remove Project
                {
                    if(MessageBox(hDlg,
                        "Do you want to remove this game project?\n\n"
                        "WARNING: All the game records associated with it"
                        " will be also removed from the Play Log!",
                        TextDeleteConfirm,
                        MB_YESNO | MB_ICONQUESTION |
                        MB_DEFBUTTON2)==IDNO) break;

                    if(!deleteSelectedProject(hDlg))
                        MessageBox(hDlg,
                            "Error has occurred! The game project "
                            "was not removed.",
                            TextCantRemove,
                            MB_ICONERROR | MB_OK);

                    BEEPZERO(releaseLevelsCombo(4));
                    BEEPZERO(setPlayScrByComboSel(fillPlayCombo(4)));
                    BEEPZERO(releaseLevelsCombo(5));
                    BEEPZERO(fillLevelsCombo(5));

                    break;
                }


                if(j==CBN_SELCHANGE)
                {
                    if(i==IDC_COMBO3)	//Levels combo
                    {
                        LPBGP	lpbgp;
                        BOOL	b;

                        lpbgp=(LPBGP)buffer;
                        if(copyLevelsComboItemInfo(SendMessage((HWND)
                            lParam,CB_GETCURSEL,0,0),lpbgp))
                        {
                            if(lpbgp->Flags&0x1)
                                b=FALSE;
                            else
                                b=TRUE;

                            enableButton(3,b);
                        }
                        break;
                    }

                    if(i==IDC_COMBO1)	//Play combo
                    {
                        BEEPZERO(setPlayScrByComboSel(TRUE));
                        break;
                    }
                }

                if(j==BN_CLICKED)
                    switch(i)
                {
                    case IDC_RADIO1PL:
                    case IDC_RADIO2PL:

                        Opt&=~0x1;
                        Opt|=(i==IDC_RADIO2PL);
                        setOption(Opt);
                        break;

                    case IDC_RADIOSINGLE:
                    case IDC_RADIODOUBLE:

                        Opt&=~0x2;
                        Opt|=((i==IDC_RADIODOUBLE)<<1);
                        setOption(Opt);
                        break;

                    case IDC_RADIOJOY_L:
                        a=0;
                        goto lbl1;
                    case IDC_RADIOMOUSE_L:
                        a=1;
                        goto lbl1;
                    case IDC_RADIOKEY_L:
                        a=2;
lbl1:
                        LControl=a;
                        showControls();
                        break;


                    case IDC_RADIOJOY_R:
                        a=0;
                        goto lbl2;
                    case IDC_RADIOMOUSE_R:
                        a=1;
                        goto lbl2;
                    case IDC_RADIOKEY_R:
                        a=2;
lbl2:
                        RControl=a;
                        showControls();
                        break;
                }
                break;
            }



        case WM_SIZE:
            if(Shct_restore)
                if(IsZoomed(hDlg))
                    ShowWindow(hDlg,SW_RESTORE);
            return 0;



        case WM_TIMER:
            int	Elapse;

            InvalidateRect(hShowWnd,NULL,FALSE);
            Shct_restore=FALSE;

            if(!(ShowMode&0x1))
            {

                if((++MainCol)==162)
                    Elapse=20;
                else
                    if(MainCol>=250)
                    {
                        if((++ShadCol)>250)
                        {
                            Elapse=HallShowTime;
                            HallRep=TIMESREP;
                            MainCol=161;
                            ShadCol=222;
                            if((++ShowMode)>=SIZEOF(textIDs)*2)
                                ShowMode=0;
                        }
                        else
                        {
                            MainCol=250;
                            break;
                        }
                    }
                    else
                        break;

            }
            else
            {
                if(!(--HallRep))
                {
                    if((++ShowMode)>=SIZEOF(textIDs)*2)
                        ShowMode=0;
                    Elapse=TextShowTime;
                }
                else
                {
                    if(HallRep>3)
                        HallColor^=RGB(0,164,41^187);
                    break;
                }

            }

            BEEPZERO(KillTimer(hDlg,1));
            SetTimer(hDlg,1,Elapse,NULL);
            break;


        case WM_APP+3:		//Show password incorrect message
            MessageBox(hDlg,
                "The Code Number is incorrect. The game project "
                "cannot be removed.",
                TextCantRemove,
                MB_OK | MB_ICONWARNING);
            break;


        case WM_APP+1:		//Check the game deletion password
            int	prop;
            if(prop=((DWORD)wParam==GameDelPass))
                SendMessage(hDlg,WM_APP+2,0,0);

            SetProp(hDlg,ClassName,(HANDLE)prop);
            break;


        case WM_APP+2:		//Removes selected game record
            if(!deleteGame(CurGameNum))
                MessageBox(hDlg,
                    "The error has occurred. The record is not removed!",
                    TextCantRemove,
                    MB_OK | MB_ICONERROR);

            BEEPZERO(releaseLevelsCombo(4));
            BEEPZERO(setPlayScrByComboSel(fillPlayCombo(4)));
            break;
    }
    return FALSE;
}





VOID	initDialog(HWND hDlg)
{
    static	int		TBIDs[]={17,(int)&LJoyPos,
                            14,(int)&RJoyPos,
                            18,(int)&LMousePos,
                            15,(int)&RMousePos};

    static char		*DifVals[]={"Baby","Normal","Enough","Hard"};

    HWND	hWnd;
    RECT	rec;
    int		i;

    //Set Window Handles

    for(i=0; i<SIZEOF(DlgIDs); i++)
        hDlgWnds[i]=GetDlgItem(hDlg,DlgIDs[i]);

    //Set separator

    hWnd=GetDlgItem(hDlg, IDC_SEPARATOR);
    GetWindowRect(hWnd,&rec);
    SetWindowPos(hWnd,0,0,0,rec.right-rec.left,2,
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER |
        SWP_NOZORDER | SWP_SHOWWINDOW);

    //Set dialog Icons

    SendMessage(hDlg,WM_SETICON,ICON_SMALL,(LPARAM)hSmallIcon);
    SendMessage(hDlg,WM_SETICON,ICON_BIG,(LPARAM)hBigIcon);

    //Set Trackbars

    for(i=0; i<SIZEOF(TBIDs); i+=2)
    {
        SendMessage(hDlgWnds[TBIDs[i]],TBM_SETRANGE,FALSE,
            MAKELPARAM(0,10));

        SendMessage(hDlgWnds[TBIDs[i]],TBM_SETPOS,TRUE,
            (LPARAM)*(LPINT)TBIDs[i+1]);
    }

    //Set New Game Option
    setOption(Opt);

    //Set New game controls
    showControls();

    //Set Scr Size value

    int	m,n;

    hWnd=GetDesktopWindow();
    GetWindowRect(hWnd, &rec);
    m=(rec.right-rec.left)/wW;
    n=(rec.bottom-rec.top)/wH;
    if(m>n)m=n;

//	m=8;
    setScrSizeCombo(m,22);

    //Set up levels combo

    if(!fillLevelsCombo(5))
    {
        releaseLevelsCombo(5);
        lstrcpy(&buffer[0x80],"Some files are corrupt!");
error:
        wsprintf(buffer,"%s Reinstall the Batty.",
                    &buffer[0x80]);

        MessageBox(hDesk,buffer,
            TextFatalError,
            MB_OK | MB_ICONERROR);

        EndDialog(hDlg,0);
        return;
    }

    //Set up Difficulty

    SendMessage(hDlgWnds[24],CB_RESETCONTENT,0,0);
    for(i=0; i<SIZEOF(DifVals); i++)
    {
        SendMessage(hDlgWnds[24],CB_ADDSTRING,0,(LPARAM)DifVals[i]);
    }

    SendMessage(hDlgWnds[24],CB_SETCURSEL,1,0);

    //Fill Play combo

    BOOL	boo;

    boo=fillPlayCombo(4);
    setPlayScrByComboSel(boo);

    //Initialize the Hall of Fame

    if(!(lpHallOfFame=(LPBYTE)getHallOfFame()))
    {
        LoadString(hInst,IDS_STRING120,&buffer[0x80],0x80);
        goto error;
    }
    return;
}





VOID	setOption(int Option)
{
    //Option:	0bit -- One/Two Player
    //			1bit -- Single/Double Bat

    BOOL		Ena=TRUE, Ena1=TRUE;
    int			a,b;


    SendMessage(hDlgWnds[7],BM_SETCHECK,
        Option&0x1?BST_CHECKED:BST_UNCHECKED,0);
    SendMessage(hDlgWnds[6],BM_SETCHECK,
        !(Option&0x1)?BST_CHECKED:BST_UNCHECKED,0);

    if(Option&0x1)
        Ena1=FALSE;

    EnableWindow(hDlgWnds[8],Ena1);
    EnableWindow(hDlgWnds[9],Ena1);

    SendMessage(hDlgWnds[9],BM_SETCHECK,
        Option&0x2?BST_CHECKED:BST_UNCHECKED,0);
    SendMessage(hDlgWnds[8],BM_SETCHECK,
        !(Option&0x2)?BST_CHECKED:BST_UNCHECKED,0);

    if(!(Option&0x1))
    {
        if(!(Option&0x2))
            Ena=FALSE;
    }

    for(int i=10; i<=16; i++)
    {
        if(Ena==FALSE || i<14)
            EnableWindow(hDlgWnds[i],Ena);
    }

    if(Option==0x2)
    {
        RControl=LControl;
        enableRControls();
        setCheck(RControl, 11);

        if(LControl!=2)
        {
            if(LControl==0)
            {
                a=14;
                b=SendMessage(hDlgWnds[17],TBM_GETPOS,0,0);
            }
            else
            {
                a=15;
                b=SendMessage(hDlgWnds[18],TBM_GETPOS,0,0);
            }

            SendMessage(hDlgWnds[a],TBM_SETPOS,TRUE,(LPARAM)b);
        }
        else
        {
            MoveMemory(RControls,LControls,sizeof(LControls));
        }
    }
    else
        if(Option&0x1)
            if(LControl==RControl)
            {
                if(LControl!=2)
                    RControl++;

                enableRControls();
                setCheck(RControl, 11);
            }
            else
                EnableWindow(hDlgWnds[14+RControl],TRUE);

    return;
}




VOID	showControls(VOID)
{
    setCheck(RControl, 11);
    setCheck(LControl, 19);

    EnableWindow(hDlgWnds[17],LControl==0?TRUE:FALSE);
    EnableWindow(hDlgWnds[18],LControl==1?TRUE:FALSE);
    EnableWindow(hDlgWnds[23],LControl==2?TRUE:FALSE);

    if(!(Opt&0x3)) return;
        else
        {
            EnableWindow(hDlgWnds[14],RControl==0?TRUE:FALSE);
            EnableWindow(hDlgWnds[15],RControl==1?TRUE:FALSE);
            EnableWindow(hDlgWnds[16],RControl==2?TRUE:FALSE);
        }

    if(LControl!=2 || RControl!=2)
    {
        if(LControl==RControl)
        {
            Opt=0x2;
            setOption(Opt);
        }
        else
            if(Opt==0x2)
            {
                Opt|=0x1;
                setOption(Opt);
            }
    }
    return;
}




VOID	setCheck(int check012, int initWnd)
{
    SendMessage(hDlgWnds[initWnd],BM_SETCHECK,
        !check012?BST_CHECKED:BST_UNCHECKED,0);

    SendMessage(hDlgWnds[++initWnd],BM_SETCHECK,
        !(--check012)?BST_CHECKED:BST_UNCHECKED,0);

    SendMessage(hDlgWnds[++initWnd],BM_SETCHECK,
        !(--check012)?BST_CHECKED:BST_UNCHECKED,0);

    return;
}




VOID	setScrSizeCombo(int Value, int ComboIDNum)
{
    int	m=Value;

    if(m>9) m=9;
    if(m<=0) m=1;

    SendMessage(hDlgWnds[ComboIDNum],CB_RESETCONTENT,0,0);

    for(int c=0; c<m; c++)
    {
        wsprintf(buffer, "%dx1", c+1);

        SendMessage(hDlgWnds[ComboIDNum],CB_ADDSTRING,0,(LPARAM)buffer);
    }

    SendMessage(hDlgWnds[ComboIDNum],CB_SETCURSEL,0,0);

    return;
}



VOID	enableRControls(VOID)
{
    EnableWindow(hDlgWnds[14],RControl==0?TRUE:FALSE);
    EnableWindow(hDlgWnds[15],RControl==1?TRUE:FALSE);
    EnableWindow(hDlgWnds[16],RControl==2?TRUE:FALSE);

    return;
}




BOOL	fillLevelsCombo(int ComboID)
{
    HWND				hWnd;
    BGPINF				bg;
    LPBGP				lpbgp;
    WIN32_FIND_DATA		wd;
    HANDLE				hFind, hFile;
    DWORD				r;
    HGLOBAL				hGlob;
    LPBYTE				lpData;
    char				FName[9];
    int					i,l;
    BOOL				InitFound=FALSE;

    hWnd=hDlgWnds[ComboID];

    SendMessage(hWnd,CB_RESETCONTENT,0,0);

    if((hFind=FindFirstFile(getfilename("*.bgp"),
        &wd))==INVALID_HANDLE_VALUE) goto err;

    do{
        
        hFile=CreateFile(getfilename(wd.cFileName),
            GENERIC_READ,FILE_SHARE_READ,
            NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0);

        if(hFile!=INVALID_HANDLE_VALUE)
        {
            l=lstrlen(wd.cFileName);
            while(wd.cFileName[--l]!='\\')
                if(l<=0) 
                {
                    l--; break;
                }

            l++;
            for(i=0; i<8; i++)
                if(wd.cFileName[l]!='.')
                    FName[i]=wd.cFileName[l++];
                else
                    break;

            FName[i]=0;

            if(ReadFile(hFile,&bg,6*0x4+0x2,&r,NULL) &&
                bg.Mask==0x0015e37a)
            {
                decodeSegment((LPBYTE)(&bg)+2*0x4,0,4*0x4+0x2);

                if(bg.NumberLevels<999 && (bg.nNameLen-1)<1000)
                {
                    hGlob=GlobalAlloc(GHND,sizeof(BGP));
                    if(hGlob)
                    {
                        if(lpbgp=(LPBGP)GlobalLock(hGlob))
                        {
                            lpbgp->DelPassword=bg.DelPassword;

                            lpData=(LPBYTE)HeapAlloc(hHeap,
                                HEAP_ZERO_MEMORY,
                                (bg.nNameLen<0x20)?0x20:bg.nNameLen+1);

                            if(lpData)
                            {
                                if(ReadFile(hFile,lpData,bg.nNameLen,
                                    &r,NULL) && r==bg.nNameLen)
                                {
                                    lstrcpy(lpbgp->FileName,FName);

                                    decodeSegment(lpData,0x4*4+0x2,
                                        bg.nNameLen);

                                    if(bg.nNameLen==4 &&
                                        *(LPDWORD)&lpData[0]==0x0201ffff)
                                    {
                                        lpbgp->Flags=0x1;
                                        lstrcpy((LPSTR)lpData,
                                            "<Initial Levels>");
                                        InitFound=TRUE;
                                    }
                                    else
                                        lpbgp->Flags=0x0;

                                    if(SendMessage(hWnd,CB_SETITEMDATA,
                                            SendMessage(hWnd,CB_ADDSTRING,
                                            0,(LPARAM)lpData),
                                            (LPARAM)hGlob)!=CB_ERR)
                                    {
                                        HeapFree(hHeap,0,lpData);
                                        GlobalUnlock(hGlob);
                                        goto lblnext;
                                    }
                                }

                                HeapFree(hHeap,0,lpData);
                            }

                            GlobalUnlock(hGlob);
                        }
                        
                        GlobalFree(hGlob);
                    }
                }
            }
lblnext:
            CloseHandle(hFile);
        }
    }while(FindNextFile(hFind, &wd));

err:
    FindClose(hFind);

    //See if only Initial present

    l=SendMessage(hWnd,CB_GETCOUNT,0,0);

    if(l<=0 || !InitFound) return FALSE;

    if(l==1)
    {
        GlobalFree((HGLOBAL)SendMessage(hWnd,CB_GETITEMDATA,0,0));

        SendMessage(hWnd,CB_RESETCONTENT,0,0);
        EnableWindow(hWnd,FALSE);
        enableButton(3,FALSE);
    }
    else
    {
        enableButton(3,TRUE);
        SendMessage(hWnd,CB_SETCURSEL,1,0);
    }

    return TRUE;
}





BOOL	releaseLevelsCombo(int ComboID)
{
    HWND	hWnd;
    HGLOBAL	hGlob;
    int		n;
    BOOL	res=TRUE;

    hWnd=hDlgWnds[ComboID];

    n=SendMessage(hWnd,CB_GETCOUNT,0,0);
    if(n==CB_ERR)
        return FALSE;

    for(int i=0; i<n; i++)
    {
        hGlob=(HGLOBAL)SendMessage(hWnd,CB_GETITEMDATA,i,0);
        if((int)hGlob!=CB_ERR)
        {
            if(GlobalFree(hGlob))
                res=FALSE;
        }
        else
            res=FALSE;
    }

    SendMessage(hWnd, CB_RESETCONTENT,0,0);
    return res;
}





VOID	decodeSegment(LPBYTE lpStart, DWORD InitOffset, int NumBytes)
{
    if(NumBytes<=0) return;

    __asm
    {
        mov		ebx,[lpStart]
        mov		edx,[InitOffset]
        mov		esi,[NumBytes]
lbl:
        mov		al,[ebx]
        mov		ah,dl
        ror		ah,6
        sub		al,ah
        xor		al,0xad
        mov		cl,dl
        xor		cl,0xab
        xor		cl,dh
        add		cl,dl
        add		al,cl
        and		cl,0x7
        sub		al,dh
        ror		al,cl
        mov		[ebx],al

        inc		ebx
        inc		edx
        dec		esi
        jnz		short	lbl
    }
    return;
}




VOID	enableButton(int ID, BOOL TrueToEnable)
{
    EnableWindow(hDlgWnds[ID], TrueToEnable);

    SendMessage(hDlgWnds[ID],BM_SETIMAGE,IMAGE_BITMAP,
        (LPARAM)(TrueToEnable?hBitmapDel:hBitmapDel_));

    return;
}




BOOL	copyLevelsComboItemInfo(int Item, LPBGP lpInf)
{
    HGLOBAL	hGlob;
    LPBGP	lpBgp;

    if(Item>=SendMessage(hDlgWnds[5],CB_GETCOUNT,0,0)) return FALSE;

    hGlob=(HGLOBAL)SendMessage(hDlgWnds[5],CB_GETITEMDATA,Item,0);

    lpBgp=(LPBGP)GlobalLock(hGlob);
    if(!lpBgp) return FALSE;

    MoveMemory(lpInf, lpBgp, sizeof(BGP));

    GlobalUnlock(hGlob);
    return TRUE;
}





VOID	codeSegment(LPBYTE lpStart, DWORD InitOffset, int NumBytes)
{
    if(NumBytes<=0) return;

    __asm
    {
        mov		ebx,[lpStart]
        mov		edx,[InitOffset]
        mov		esi,[NumBytes]
lbl:
        mov		al,[ebx]
        mov		cl,dl
        xor		cl,0xab
        xor		cl,dh
        add		cl,dl
        mov		ah,cl
        and		cl,0x7
        rol		al,cl
        add		al,dh
        sub		al,ah
        xor		al,0xad
        mov		ah,dl
        ror		ah,6
        add		al,ah
        mov		[ebx],al

        inc		ebx
        inc		edx
        dec		esi
        jnz		short	lbl
    }
    return;
}




BYTE	getSRC(LPBYTE lpStart, int NumBytesWithoutLastSRCByte)
{
    return getSRCEx(lpStart,NumBytesWithoutLastSRCByte,0);
}




BYTE	getSRCEx(LPBYTE lpStart, int NumBytesWithoutLastSRCByte,
                 BYTE StartSRC)
{
    BYTE	src=StartSRC;
    int		l=NumBytesWithoutLastSRCByte;

    if(l>0)
    {
        __asm
        {
            mov		ebx,[lpStart]
            mov		esi,[l]
            mov		ch,[src]
lbl1:
            mov		al,[ebx]
            mov		ah,8
lbl2:
            test	al,0x04
            setnz	cl
            xor		ch,cl
            test	al,0x40
            setnz	cl
            xor		ch,cl
            test	al,0x80
            setnz	cl
            xor		ch,cl

            rol		al,1
            rol		ch,1
            dec		ah
            jnz		short	lbl2

            inc		ebx
            dec		esi
            jnz		short	lbl1

            mov		[src],ch
        }
    }
    return src;
}





BOOL	readRegKey(UINT Data012etc, UINT Inf01_9,
                   DWORD OffsetFromKeyBegin,
                   LPVOID lpWhere, DWORD NumBytes)
{
    //'Data012etc' value is from [0 to 999]

    HKEY		hKey;
    DWORD		n,type,size;
    LPBYTE		lpVal;
    BOOL		res=FALSE;

    if(Data012etc>999) return FALSE;
    if(Inf01_9>9) return FALSE;

    wsprintf(&SubKey[19],"%d",Data012etc);
    ValueName[3]=Inf01_9+'0';

    if(RegOpenKeyEx(HKEY_CURRENT_USER,SubKey,0,KEY_READ,&hKey)!=
        ERROR_SUCCESS) goto bad;

    if(RegQueryInfoKey(hKey,0,0,NULL,0,0,0,0,0,
        &size,0,NULL)!=ERROR_SUCCESS) goto bad1;

    lpVal=(LPBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size);
    if(!lpVal) goto bad1;

    if(RegQueryValueEx(hKey,ValueName,NULL,&type,lpVal,
        &size)!=ERROR_SUCCESS) goto bad2;

    size--;
    if(OffsetFromKeyBegin>=size) goto bad2;

    n=(OffsetFromKeyBegin+NumBytes)>size?size-OffsetFromKeyBegin:
        NumBytes;

    decodeSegment(lpVal,0,size);

    if(NumBytes)
        if(getSRC(lpVal,size)!=lpVal[size]) goto bad2;

    if(lpWhere && NumBytes)
        MoveMemory(lpWhere,lpVal+OffsetFromKeyBegin,n);

    res=TRUE;

bad2:
    HeapFree(hHeap,0,lpVal);
bad1:
    RegCloseKey(hKey);
bad:
    return res;
}





BOOL	writeRegKey(UINT Data012etc, UINT Inf01_9,
                   DWORD OffsetFromKeyBegin,
                   LPVOID lpFrom, DWORD NumBytes,
                   BOOL TrueForNew)
{
    //CAN ENLARGE THE KEY AND DECREASE IT!

    //'NumBytes'==0 =>Truncate the Key value (if 'OffsetFromKeyBegin' also ==0 => delete Key value
    //'Data012etc' value is from [0 to 999]

    HKEY		hKey;
    DWORD		n,v,type,size;
    LPBYTE		lpVal;
    BOOL		res=FALSE;

    if(Data012etc>999) return FALSE;
    if(Inf01_9>9) return FALSE;

    wsprintf(&SubKey[19],"%d",Data012etc);
    ValueName[3]=Inf01_9+'0';

    if(RegOpenKeyEx(HKEY_CURRENT_USER,SubKey,0,KEY_READ|KEY_WRITE,
        &hKey)!=ERROR_SUCCESS)
    {
        if(RegCreateKeyEx(HKEY_CURRENT_USER,SubKey,0,"",
            REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,0,
            &hKey,&n)!=ERROR_SUCCESS) goto bad;

        if(n!=REG_CREATED_NEW_KEY) goto bad1;
    }

    if(RegQueryInfoKey(hKey,0,0,NULL,0,0,0,&v,0,
        &size,0,NULL)!=ERROR_SUCCESS) goto bad1;

    n=(OffsetFromKeyBegin+NumBytes)>size?(OffsetFromKeyBegin+NumBytes):
        size;

    lpVal=(LPBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,n+1);
    if(!lpVal) goto bad1;

    if(v && size && RegQueryValueEx(hKey,ValueName,0,0,0,
        0)==ERROR_SUCCESS)
    {
        if(RegQueryValueEx(hKey,ValueName,NULL,&type,lpVal,
            &size)!=ERROR_SUCCESS) goto bad2;

        size--;
        if(RegDeleteValue(hKey,ValueName)!=ERROR_SUCCESS) goto bad2;

        decodeSegment(lpVal,0,size);

        if(NumBytes!=0 || OffsetFromKeyBegin!=0)
            if(getSRC(lpVal,size)!=lpVal[size]) goto bad2;
    }

    if(TrueForNew)
        n=OffsetFromKeyBegin+NumBytes;
    else
        n=(OffsetFromKeyBegin+NumBytes)>size?
            (OffsetFromKeyBegin+NumBytes):size;

    if(lpFrom && NumBytes)
        MoveMemory(lpVal+OffsetFromKeyBegin,lpFrom,NumBytes);
    else
        if(!NumBytes)
            n=OffsetFromKeyBegin;

    if(n>0)
    {
        lpVal[n]=getSRC(lpVal,n);
        codeSegment(lpVal,0,n);
        if(RegSetValueEx(hKey,ValueName,0,REG_BINARY,lpVal,
            n+1)!=ERROR_SUCCESS) goto bad2;
    }
    else	//Delete key itself if it is empty
    {
        DWORD	sub;
        if(RegQueryInfoKey(hKey,0,0,NULL,&sub,0,0,&v,0,
            0,0,NULL)==ERROR_SUCCESS && v==0 && sub==0)
        {
            RegDeleteKey(HKEY_CURRENT_USER,SubKey);
        }
    }

    res=TRUE;

bad2:
    HeapFree(hHeap,0,lpVal);
bad1:
    RegCloseKey(hKey);
bad:
    return res;
}




BOOL	setBattyPath(VOID)
{
    int		n,l;
    BOOL	res=FALSE;
    BYTE	BLM;

    BLMpresent=FALSE;

    if(readRegKey(0,0,0,&n,sizeof(n)))
        if(readRegKey(0,0,4+n,&BLM,1))
            BLMpresent=TRUE;

    l=lstrlen(getfilename(""))+1;

    if(writeRegKey(0,0,0,&l,sizeof(l),TRUE))
        if(writeRegKey(0,0,4,buffer,l,FALSE))
        {
            if(BLMpresent)
                writeRegKey(0,0,4+l,&BLM,1,FALSE);

            res=TRUE;
        }

    BLMcode=BLM;
    return res;
}




LPGAME	getGameStruct(UINT Data012etc, UINT Inf01_9, int Game0123)
{
    //IF 'LPGAME' RETURNED AS NOT ZERO =>
    //AFTER USE OF 'LPGAME' RETURNED VALUE, IT IS NEEDED TO
    //FREE IT WITH HeapFree(hHeap,0,this 'LPGAME'); !!!!!

    LPGAME		lpgm=(LPGAME)NULL;
    LPBYTE		lpData;
    DWORD		Offs=0x2*3+1;
    short		n;

    lpData=(LPBYTE)HeapAlloc(hHeap,0,Offs);
    if(lpData)
    {
        if(readRegKey(Data012etc,Inf01_9,0,lpData,Offs))
            if(Game0123<(int)lpData[0])
            {
                if(Game0123>0)
                    Offs=*(LPWORD)&lpData[(Game0123-1)*2+1];

                if(readRegKey(Data012etc,Inf01_9,Offs,&n,
                    sizeof(n)) && n>=sizeof(GAME) &&
                    n<(sizeof(GAME)+0x100*4))
                {
                    lpgm=(LPGAME)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,n);
                    if(lpgm)
                    {
                        if(!readRegKey(Data012etc,Inf01_9,Offs,
                            lpgm,n))
                        {
                            HeapFree(hHeap,0,lpgm);
                            lpgm=NULL;
                        }
                    }
                }
            }

        HeapFree(hHeap,0,lpData);
    }
    
    return lpgm;
}





LPFINDGAME	findGameFirst(VOID)
{
    //Call FindGameClose in the end!!!
    //Returns: ZERO if nothing found

    static FINDGAME	fg;

    fg.lpLastGameReturned=getGameStruct(fg.LastData012etc=1,
        fg.LastInf01_9=0,fg.LastGame=0);

    return &fg;
}




BOOL	findGameNext(LPFINDGAME lpFindGameFirst)
{
    //Call FindGameClose in the end!!!
    //Returns: FALSE if no more found

    LPFINDGAME	lpfg=lpFindGameFirst;
    BOOL		res=FALSE;
    LPGAME		lpgm;

    if(lpfg->lpLastGameReturned)
        BEEPZERO(HeapFree(hHeap,0,lpfg->lpLastGameReturned));

    if((++lpfg->LastGame)>3)
    {
        lpfg->LastGame=0;
        if((++lpfg->LastInf01_9)>9)
        {
            lpfg->LastInf01_9=0;
            lpfg->LastData012etc++;
        }
    }

    lpgm=getGameStruct(lpfg->LastData012etc, lpfg->LastInf01_9,
                        lpfg->LastGame);

    if(lpgm)
    {
        lpfg->lpLastGameReturned=lpgm;
        res=TRUE;
    }

    return res;
}





VOID	closeGameFind(LPFINDGAME lpFindGameFirst)
{
    if(lpFindGameFirst->lpLastGameReturned)
        BEEPZERO(HeapFree(hHeap,0,
            lpFindGameFirst->lpLastGameReturned));

    return;
}




BOOL	fillPlayCombo(int ComboID)
{
    LPFINDGAME	lpfg;
    HWND		hWnd;
    LPPLAY		lppl;
    HGLOBAL		hGlob;
    int			n=0;

    hWnd=hDlgWnds[ComboID];
    SendMessage(hWnd,CB_RESETCONTENT,0,0);

    lpfg=findGameFirst();
    if(!lpfg->lpLastGameReturned) return FALSE;

    do{
        hGlob=GlobalAlloc(GHND, sizeof(PLAY));
        if(hGlob)
        {
            lppl=(LPPLAY)GlobalLock(hGlob);
            if(lppl)
            {
                lppl->Data012etc=lpfg->LastData012etc;
                lppl->Inf01_9=lpfg->LastInf01_9;
                lppl->Game0123=lpfg->LastGame;

                GlobalUnlock(hGlob);

                getGameName(lpfg->lpLastGameReturned, buffer);

                if(SendMessage(hWnd,CB_SETITEMDATA,
                        SendMessage(hWnd,CB_ADDSTRING,
                        0,(LPARAM)buffer),
                        (LPARAM)hGlob)==CB_ERR) goto lblno;
                n++;
            }
            else
lblno:
                GlobalFree(hGlob);
        }
    }while(findGameNext(lpfg));

    if(SendMessage(hWnd,CB_SETCURSEL,PlaySel,0)==CB_ERR)
        SendMessage(hWnd,CB_SETCURSEL,PlaySel=0,0);

//	closeGameFind(lpfg); is not needed here

    return n?TRUE:FALSE;
}




BOOL	setPlayScrByComboSel(BOOL Enabled)
{
    HCURSOR	hCur;
    HGLOBAL	hGlob;
    LPPLAY	lppl;
    LPGAME	lpgm;
    BOOL	res=FALSE;
    HWND	hWnd;
    int		cur;

    hCur=SetCursor(LoadCursor(NULL,(LPSTR)IDC_WAIT));

    hWnd=GetParent(hDlgWnds[4]);
    cur=SendMessage(hDlgWnds[4],CB_GETCURSEL,0,0);

    if(cur==CB_ERR || !Enabled)
    {
        setPlayScreen(FALSE,NULL,hWnd);
        res=TRUE;
        goto end;
    }

    hGlob=(HGLOBAL)SendMessage(hDlgWnds[4],CB_GETITEMDATA,
                    SendMessage(hDlgWnds[4],CB_GETCURSEL,0,0),0);

    if(hGlob)
        if(lppl=(LPPLAY)GlobalLock(hGlob))
        {
            if(lpgm=getGameStruct(lppl->Data012etc,
                lppl->Inf01_9,lppl->Game0123))
            {
                setPlayScreen(TRUE,lpgm,hWnd);

                CurGameNum=((lppl->Data012etc-1)*10+lppl->Inf01_9)*4+
                    lppl->Game0123;

                BEEPZERO(HeapFree(hHeap,0,lpgm));
                res=TRUE;
            }
            GlobalUnlock(hGlob);
        }

end:
    SetCursor(hCur);
    return res;
}




VOID	setPlayScreen(BOOL Enabled, LPGAME lpGame, HWND hDlg)
{
    static	int		ConIDs[]={0,25,4};

    int		i,n,m;
    BOOL	t;
    HWND	hWnd;

    enableButton(2,Enabled);
    for(i=0; i<SIZEOF(ConIDs); i++)
        EnableWindow(hDlgWnds[ConIDs[i]],Enabled);

    SendMessage(hDlgWnds[0],BM_SETSTYLE,Enabled?BS_DEFPUSHBUTTON:0x0,
        TRUE);
    SendMessage(hDlgWnds[1],BM_SETSTYLE,Enabled?0x0:BS_DEFPUSHBUTTON,
        TRUE);

    SetFocus(hDlgWnds[Enabled?0:1]);

    if(Enabled)
    {
        //VISIBLE STATE

        //Game type

        DblPl=(lpGame->GameType&0x80000000)?TRUE:FALSE;
        Dbl=FALSE;

        if(lpGame->GameType&0x80000000)
        {
            if(lpGame->GameType&0x2000000)
            {
                lstrcpy(buffer,"Double play");
                Dbl=TRUE;
            }
            else
                lstrcpy(buffer,"Two players");
        }
        else
            lstrcpy(buffer,"Single play");

        combineWith(0,buffer,hDlg);

        //Levels type

        t=TRUE;
        getProjFile(lpGame,buffer);
        if(!getProjText(buffer,buffer,TRUE))
        {
            lstrcpy(buffer,TextUnkn);
        }
        else
            if(*(LPDWORD)buffer==0x0201ffff &&
                !buffer[4])
            {
                t=FALSE;
                buffer[0]=0;
            }

        EnableWindow(GetDlgItem(hDlg,IDC_STLEVELS),t);
        combineWith(1,buffer,hDlg);

        //Player's controls
        int		ind;
        LPSTR	lpt;

        for(i=0; i<2; i++)
        {
            ind=2+i*6;

            //Player
            buffer[0]='\"';
            if(i && !Dbl)
                getRightName(lpGame,&buffer[1]);
            else
                getLeftName(lpGame,&buffer[1]);
            lstrcat(buffer,"\"");
            combineWith(ind++,buffer,hDlg);

            //Controls
            
            n=(lpGame->GameType)>>(i?8:0);
            n&=0xf;
            if(!n)
                lpt="Joystick";
            else
                if(n==1)
                    lpt="Mouse";
                else
                    lpt="Keyboard";
            combineWith(ind++,lpt,hDlg);

            //Lost Ball
            n=i?lpGame->RightLostBalls:lpGame->LeftLostBalls;
            wsprintf(buffer,"%d [time%s]",n,n==1?"":"s");
            combineWith(ind++,buffer,hDlg);

            //Prizes caught
            n=i?lpGame->RightCaughtPrizes:lpGame->LeftCaughtPrizes;
            wsprintf(buffer,"%d",n);
            combineWith(ind++,buffer,hDlg);

            //Bombs caught
            n=i?lpGame->RightCaughtBombs:lpGame->LeftCaughtBombs;
            wsprintf(buffer,"%d",n);
            combineWith(ind++,buffer,hDlg);

            //Play ratio
            m=lpGame->LeftTimePlayed+lpGame->RightTimePlayed;
            if(m)
                n=100*(i?lpGame->RightTimePlayed:
                    lpGame->LeftTimePlayed)/m;
            else
                n=0;
            if(n>100) n=100;
            wsprintf(buffer,"%d%%",n);
            combineWith(ind++,buffer,hDlg);
        }

        //Levels passed
        wsprintf(buffer,"%d",lpGame->StartLevel);
        combineWith(14,buffer,hDlg);

        //Time played
        int	tt;
        n=lpGame->TimeSince1Play;
        if(n<0)n=-n;
        m=n/(60*60);
        i=(tt=n%(60*60))/60;
        wsprintf(buffer,"%02d:%02d%:%02d",m,i,tt%60);
        combineWith(15,buffer,hDlg);

        //Last played
        n=lpGame->TimeLastPlayed;
        if(n<0)n=-n;
        m=n/(60*60);
        i=(tt=n%(60*60))/60;
        wsprintf(buffer,"%02d:%02d%:%02d",m,i,tt%60);
        combineWith(16,buffer,hDlg);

        //Difficulty
        n=(lpGame->GameType>>16)&0xf;
        if(!n)
            lpt="Baby";
        else
            if(n==1)
                lpt="Normal";
            else
                if(n==2)
                    lpt="Enough";
                else
                    lpt="Hard";
        combineWith(17,lpt,hDlg);

        //Encr and Screen words
        buffer[0]=0;
        combineWith(18,buffer,hDlg);
        combineWith(19,buffer,hDlg);

        //Scr size combo
        setScrSizeCombo(lpGame->ScrRatio,25);

        //Encryption tick
        IfGameEnc=lpGame->GameType&0x1000000?TRUE:FALSE;
        SendMessage(GetDlgItem(hDlg,IDC_ENCRCHECK1),BM_SETCHECK,
            IfGameEnc?BST_CHECKED:BST_UNCHECKED,0);
        GameDelPass=lpGame->EncryptCode;
    }
    else
    {
        //GRAYED STATE

        t=TRUE;
        n=0;
        for(i=0; i<SIZEOF(Text1); i++)
        {
            hWnd=GetDlgItem(hDlg,PlayTextIDs[n]);
            SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)Text1[i]);
            EnableWindow(hWnd,FALSE);

            if((++n)==8)
                if(t)
                {
                    i=1;
                    t=FALSE;
                }
        }

        SendMessage(hDlgWnds[25],CB_RESETCONTENT,0,0);
    }

    return;
}





LRESULT CALLBACK KeyProc(HWND hDlg,
                          UINT message,
                          WPARAM wParam,
                          LPARAM lParam)
{
    static HWND		hKeys[5];
    static LPKEYS	lpChar;
    static KEYS		CopyChar[3];

    int		i;

    switch (message)
    {
        case WM_INITDIALOG:
            {
                static	keyIDs[]={IDC_HOTKEY1,IDC_HOTKEY3,
                            IDC_HOTKEY4,IDC_HOTKEY5,IDC_HOTKEY6};
                RECT	rec;

                hKeyDlg=hDlg;
                lpChar=(lParam==IDC_BUTSET_L)?LControls:RControls;

                MoveMemory(CopyChar,lpChar,sizeof(CopyChar));

                GetWindowRect(GetParent(hDlg),&rec);

                SetWindowPos(hDlg,0,
                    rec.left+(lParam==IDC_BUTSET_L?50:220),
                    rec.top+220,0,0,
                    SWP_NOSIZE|SWP_NOZORDER);

                for(i=0; i<SIZEOF(keyIDs); i++)
                    hKeys[i]=GetDlgItem(hDlg,keyIDs[i]);

                for(i=0; i<3; i++)
                {
                    SendMessage(hKeys[i],HKM_SETRULES,
                        HKCOMB_CA|HKCOMB_SA|HKCOMB_SC|HKCOMB_SCA,
                        MAKELPARAM(HOTKEYF_CONTROL,0));

                    SendMessage(hKeys[i],HKM_SETHOTKEY,
                        MAKEWORD(lpChar[i].VirtualCode,
                        lpChar[i].ShiftStatus),0);

                    OldProcs[i].hWnd=hKeys[i];
                    OldProcs[i].OldProcFunc=SetWindowLong(hKeys[i],
                        GWL_WNDPROC,(LONG)KeyDlgSubProc);
                }

                SendMessage(hKeys[3],HKM_SETHOTKEY,
                    MAKEWORD(0xc0,0),0);
                SendMessage(hKeys[4],HKM_SETHOTKEY,
                    MAKEWORD(VK_ESCAPE,0),0);

                SetFocus(hKeys[0]);
                return FALSE;
            }



        case WM_COMMAND:
            {
                int i,j;
                
                i=LOWORD(wParam);
                j=HIWORD(wParam);

                if (i==IDOK || i==IDCANCEL)
                {
lblend:
                    for(i=0; i<3; i++)
                        SetWindowLong(OldProcs[i].hWnd,
                            GWL_WNDPROC,OldProcs[i].OldProcFunc);

                    i=TRUE;
                    if(LOWORD(wParam)==IDCANCEL)
                    {
                        i=FALSE;
                        MoveMemory(lpChar, CopyChar,sizeof(CopyChar));
                    }

                    EndDialog(hDlg, i);
                    return TRUE;
                }

                if(j==EN_CHANGE)
                {
                    HWND	hWnd;
                    char	c;

                    hWnd=(HWND)lParam;
                    if((i=SendMessage(hWnd,HKM_GETHOTKEY,0,0))&
                        ((HOTKEYF_ALT|HOTKEYF_CONTROL|HOTKEYF_SHIFT)<<8))
                    {
                        i=i>>8;
                        if(i&HOTKEYF_CONTROL) c=VK_CONTROL;
                        else
                            if(i&HOTKEYF_ALT) c=VK_MENU;
                            else
                                if(i&HOTKEYF_SHIFT) c=VK_SHIFT;

                        SendMessage(hWnd,HKM_SETHOTKEY,
                            MAKEWORD(c,0),0);
                    }

                    goto lbltab;
                }

                break;


        case WM_APP:
            SendMessage(hKeys[lParam&0xff],HKM_SETHOTKEY,
                MAKEWORD(wParam,(lParam&0x1000000)>>21),0);
lbltab:
            HWND	foc;
            WORD	w;
            BYTE	v,sh;

            foc=GetFocus();
            for(i=0; i<3; i++)
                if(OldProcs[i].hWnd==foc)
                {
                    w=(WORD)SendMessage(hKeys[i],HKM_GETHOTKEY,0,0);
                    v=w&0xff;
                    sh=(w>>8)&0xff;

                    for(j=0; j<3; j++)
                        if(i!=j && v==lpChar[j].VirtualCode &&
                            sh==lpChar[j].ShiftStatus)
                        {
                            MessageBeep(0);
                            SendMessage(hKeys[i],HKM_SETHOTKEY,
                                MAKEWORD(lpChar[i].VirtualCode,
                                lpChar[i].ShiftStatus),0);
                            goto lblno;
                        }

                    lpChar[i].VirtualCode=v;
                    lpChar[i].ShiftStatus=sh;

                    if(i==2)
                    {
                        SetFocus(GetDlgItem(hKeyDlg,IDOK));
                    }
                    else
                        SetFocus(OldProcs[i+1].hWnd);
                }
lblno:
            break;


        case WM_APP+1:	//Destroy Window
            wParam=IDCANCEL;
            goto lblend;

            }
    }
    return FALSE;
}





LRESULT CALLBACK KeyDlgSubProc(HWND hWnd,
                            UINT mes,
                            WPARAM wParam,
                            LPARAM lParam)
{
    for(int i=0; i<3; i++)
        if(OldProcs[i].hWnd==hWnd)
        {
            if(mes==WM_KEYDOWN || mes==WM_SYSKEYDOWN)
            {
                switch(wParam)
                {
                case VK_ESCAPE:
                    SendMessage(hKeyDlg,WM_APP+1,0,0);
                    return 0;

                case 0xc0:
                case 0x90:
                case 0x91:
                case 0x13:
                    MessageBeep(0);
                    return 0;

                case VK_BACK:
                case ' ':
                case VK_DELETE:
                case VK_TAB:
                case VK_RETURN:
                    {
                    SendMessage(hWnd,HKM_SETHOTKEY,MAKEWORD(0,0),0);
                    SendMessage(hKeyDlg,WM_APP,wParam,
                        (lParam&0x1000000)|i);
                    return 0;
                    }
                }

                SendMessage(hWnd,HKM_SETHOTKEY,MAKEWORD(0,0),0);
            }


            if(mes==WM_GETDLGCODE)
            {
                return DLGC_WANTALLKEYS|DLGC_WANTARROWS|
                        DLGC_WANTCHARS|DLGC_WANTTAB;
            }

            return CallWindowProc((WNDPROC)OldProcs[i].OldProcFunc,
                                    hWnd,
                                    mes,
                                    wParam,
                                    lParam);
        }

#ifdef _DEBUG
    BEEPZERO(0);
#endif
    return 0;
}





BOOL	writeNewGame(int GameNumber012, LPSTR GameName,
                     LPSTR ProjFile, LPSTR LeftName,
                     LPSTR RightName, DWORD EncrPass)
{
    //If EncrPass=0 => no password

    BATTYREG		Reg;
    LPGAME			lpgm;
    GAME			gm;
    BOOL			res=FALSE;

    int				len,nGN,nPF,nLN,nRN;


    nGN=lstrlen(GameName);
    nPF=lstrlen(ProjFile);
    nLN=lstrlen(LeftName);
    nRN=lstrlen(RightName);

    len=nGN+nPF+nLN+nRN+sizeof(GAME);

    lpgm=(LPGAME)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,len+1);

    if(!readRegKey(0,1,0,&Reg,sizeof(Reg)))
    {
        Reg.NumberGames012=0;
        Reg.TimeInstalled.QuadPart=whatTime().QuadPart;

        if(!writeRegKey(0,1,0,&Reg,sizeof(Reg),TRUE))
            goto end;
    }

    if(GameNumber012>Reg.NumberGames012) goto end;

    ZeroMemory(&gm,sizeof(GAME));

    gm.GameType=(LControl&0xf) |
        ((RControl&0xf)<<8) |
        ((Dif&0xf)<<16) |
        (Encryption?0x1000000:0) |
        ((DoublePlay&TRUE)<<31) |
        (DoubleBat<<25);


    switch(LControl)
    {
    case 0:
        gm.LContr_Left=LJoyPos;
        goto lbl1;
    case 1:
        gm.LContr_Left=LMousePos;
lbl1:
        gm.LContr_Right=0;
        gm.LContr_Fire=0;
        break;
    case 2:
        gm.LContr_Left=LControls[0].VirtualCode;
        gm.LContr_Right=LControls[1].VirtualCode;
        gm.LContr_Fire=LControls[2].VirtualCode;
        break;
    }

    switch(RControl)
    {
    case 0:
        gm.RContr_Left=RJoyPos;
        goto lbl2;
    case 1:
        gm.RContr_Left=RMousePos;
lbl2:
        gm.RContr_Fire=0;
        gm.RContr_Right=0;
        break;
    case 2:
        gm.RContr_Left=RControls[0].VirtualCode;
        gm.RContr_Right=RControls[1].VirtualCode;
        gm.RContr_Fire=RControls[2].VirtualCode;
        break;
    }

    gm.ProjSRC=ProjSRC;
    gm.TimeLastPlayed=gm.TimeSince1Play=
        (DWORD)(CountDwnTime.QuadPart/10000000);

    gm.LeftTimePlayed=LCyc/256;
    gm.RightTimePlayed=RCyc/256;

    gm.HiScore=Hi;
    gm.StartLevel=(WORD)Level;
    gm.ScrRatio=ScrSizRat;
    gm.StartLives=(BYTE)(NewNumLives>5?NewNumLives:5);

    gm.LeftCaughtBombs=NumBombsL;
    gm.RightCaughtBombs=NumBombsR;
    gm.LeftCaughtPrizes=NumPrizL;
    gm.RightCaughtPrizes=NumPrizR;

    gm.LeftLostBalls=LostBallL;
    gm.RightLostBalls=LostBallR;

    //Copy Text Values

    LPSTR	lpData;
    int		i;

    lpData=(LPSTR)lpgm+sizeof(GAME);

    gm.Size=len;
    gm.nGameNameLen=nGN;
    gm.nGameProjLen=nPF;
    gm.nLeftNameLen=nLN;
    gm.nRightNameLen=nRN;

    gm.EncryptCode=EncrPass;

    i=0;
    lstrcpy(lpData,GameName);
    lstrcpy(&lpData[i+=gm.nGameNameLen],ProjFile);
    lstrcpy(&lpData[i+=gm.nGameProjLen],LeftName);
    lstrcpy(&lpData[i+=gm.nLeftNameLen],RightName);


    int		g,dNum,iNum,gNum;
    DWORD	Offs;
    BYTE	aux[7];

    g=GameNumber012/4;
    dNum=g/10;					//Data***
    iNum=g%10;					//Inf*
    gNum=GameNumber012&0x3;		//One of 4 game structs
    dNum++;

    if(!readRegKey(dNum,iNum,0,aux,sizeof(aux)))
    {
        if(GameNumber012!=Reg.NumberGames012 || gNum)
            goto end;

        aux[0]=0;
        *(LPWORD)&aux[1]=len+sizeof(aux);
        *(LPWORD)&aux[3]=0;
        *(LPWORD)&aux[5]=0;

        if(!writeRegKey(dNum,iNum,0,aux,sizeof(aux),TRUE))
            goto end;
    }
    else
        if(GameNumber012==Reg.NumberGames012 && !gNum)
            goto end;

    if(gNum)
        Offs=*(LPWORD)&aux[(gNum-1)*2+1];
    else
        Offs=sizeof(aux);


    MoveMemory(lpgm,&gm,sizeof(GAME));

    if(!writeRegKey(dNum,iNum,Offs,lpgm,len,
        (GameNumber012==Reg.NumberGames012)?TRUE:FALSE))
        goto end;

    if(GameNumber012==Reg.NumberGames012)
    {
        aux[0]++;

        if(gNum!=0 && gNum!=3)
            *(LPWORD)&aux[1+gNum*2]=*(LPWORD)&aux[1+(gNum-1)*2]+len;

        if(!writeRegKey(dNum,iNum,0,aux,sizeof(aux),FALSE))
            goto end;

        Reg.NumberGames012++;
        if(!writeRegKey(0,1,0,&Reg,sizeof(Reg),FALSE))
            goto end;
    }

    res=TRUE;
end:
    HeapFree(hHeap,0,lpgm);
    return res;
}




ULARGE_INTEGER	whatTime(VOID)
{
    SYSTEMTIME		st;
    FILETIME		ft;
    ULARGE_INTEGER	ui;


    GetLocalTime(&st);
    SystemTimeToFileTime(&st,&ft);
    ui.LowPart=ft.dwLowDateTime;
    ui.HighPart=ft.dwHighDateTime;

    return ui;
}




int		getGameName(LPGAME lpGameStruct, LPSTR lpDest)
{
    int n=lpGameStruct->nGameNameLen;

    if(n>0)
        MoveMemory(lpDest,
            (LPBYTE)lpGameStruct+sizeof(GAME),
            n);

    lpDest[n]=0;
    return n;
}




int		getProjFile(LPGAME lpGameStruct, LPSTR lpDest)
{
    int n=lpGameStruct->nGameProjLen;

    if(n>0)
        MoveMemory(lpDest,
            (LPBYTE)lpGameStruct+sizeof(GAME)+
            lpGameStruct->nGameNameLen,
            n);

    lpDest[n]=0;
    return n;
}




int		getLeftName(LPGAME lpGameStruct, LPSTR lpDest)
{
    int n=lpGameStruct->nLeftNameLen;

    if(n>0)
        MoveMemory(lpDest,
            (LPBYTE)lpGameStruct+sizeof(GAME)+
            lpGameStruct->nGameNameLen+
            lpGameStruct->nGameProjLen,
            n);

    lpDest[n]=0;
    return n;
}




int		getRightName(LPGAME lpGameStruct, LPSTR lpDest)
{
    int n=lpGameStruct->nRightNameLen;

    if(n>0)
        MoveMemory(lpDest,
            (LPBYTE)lpGameStruct+sizeof(GAME)+
            lpGameStruct->nGameNameLen+
            lpGameStruct->nGameProjLen+
            lpGameStruct->nLeftNameLen,
            n);

    lpDest[n]=0;
    return n;
}




int		combineWith(int TextIndex, LPSTR lpAppend, HWND hDlg)
{
    HWND	hWnd;
    LPSTR	lpt;
    LPSTR	lpBuffer;
    int		n;
    BOOL	b=FALSE;

    lpt=Text1[(TextIndex>=8)?TextIndex-6:TextIndex];
    lpBuffer=(LPSTR)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,0x100);

    hWnd=GetDlgItem(hDlg,PlayTextIDs[TextIndex]);
    lstrcpy(lpBuffer,lpt);

    if(DblPl || TextIndex<8 || TextIndex>13)
    {
        if((TextIndex==7 || TextIndex==13) && 
            (!DblPl || (DblPl && Dbl))) goto lbl1;

        lstrcat(lpBuffer, " ");
        lstrcat(lpBuffer, lpAppend);
        b=TRUE;
    }

lbl1:
    if(TextIndex!=1)
        EnableWindow(hWnd,b);

    SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)lpBuffer);

    n=lstrlen(lpBuffer);
    HeapFree(hHeap,0,lpBuffer);

    return n;
}





BOOL	readProjFile(LPSTR lpFile, DWORD FileOffs, 
                     LPVOID lpWhere, DWORD NumBytes,
                     int *SRCorNULL)
{
    //USES 'buffer' array!!!

    BOOL	res=FALSE;
    HANDLE	hFile;
    LPBYTE	lptr;
    DWORD	siz,s;
    int		src;

    hFile=CreateFile(getfilename(lpFile),
            GENERIC_READ,FILE_SHARE_READ,0,
            OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,
            NULL);

    if(hFile!=INVALID_HANDLE_VALUE)
    {
        siz=GetFileSize(hFile,NULL);

        lptr=(LPBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,siz);
        if(lptr && siz>8 && (signed)siz>0)
        {
            if(ReadFile(hFile,lptr,siz,&s,0) && s==siz)
            {
                src=(char)getSRC(&lptr[8],siz-8);

                if(SRCorNULL)
                    *SRCorNULL=src;

                if(src==*(int*)lptr &&
                    FileOffs<siz && (FileOffs+NumBytes)<=siz &&
                    *(LPDWORD)&lptr[4]==0x15e37a)
                {
                    decodeSegment(lptr+FileOffs,FileOffs-0x8,NumBytes);

                    MoveMemory(lpWhere,lptr+FileOffs,NumBytes);
                    res=TRUE;
                }
            }

            HeapFree(hHeap,0,lptr);
        }

        CloseHandle(hFile);
    }

    return res;
}



BOOL	getProjText(LPSTR lpProjFile, LPSTR lpWhere, 
                    BOOL TrueForProjName)
{
    //You CAN use 'buffer' for each value!!!

    LPSTR	lpt;
    LPBYTE	lpDat;
    BOOL	res=FALSE;
    DWORD	l;

    lpt=(LPSTR)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,MAX_PATH);
    if(lpt)
    {
        lstrcpy(lpt,lpProjFile);
        if(readProjFile(lpt,0x8,&lpt[MAX_PATH-24],4*0x4+0x2,0))
        {
            l=*(LPDWORD)&lpt[MAX_PATH-24+
                    (TrueForProjName?1*0x4:2*0x4)];

            if(l)
            {
                lpDat=(LPBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,l+1);
                if(lpDat)
                {
                    if(readProjFile(lpt,
                        TrueForProjName?0x4*6+0x2:0x4*6+0x2+
                        *(LPDWORD)&lpt[MAX_PATH-24+1*0x4],
                        lpDat,l,0))
                    {
                        lpDat[l]=0;
                        MoveMemory(lpWhere,lpDat,l+1);
                        res=TRUE;
                    }

                    HeapFree(hHeap,0,lpDat);
                }
            }
            else
                lpWhere[0]=0;
        }

        HeapFree(hHeap,0,lpt);
    }

    return res;
}




BOOL	deleteGame(int GameNumber012)
{
    HCURSOR	hCur;
    LPBYTE	lpHead;
    LPGAME	lpGame;
    LPBYTE	lpMem[4];
    BOOL	pre=TRUE,res=FALSE;
    int		n;						//Number of all game struct's
    int		g,dNum,iNum;
    int		a,s,r,w,k,p,q;


    hCur=SetCursor(LoadCursor(NULL,(LPSTR)IDC_WAIT));

    if(readRegKey(0,1,0,&n,sizeof(n)))
        if(GameNumber012<n)
        {
            r=w=GameNumber012&~0x3;		//r=w=GAME, begin of Inf key
            k=GameNumber012;			//k=GAME to remove

            lpHead=(LPBYTE)HeapAlloc(hHeap,0,7);
            if(lpHead)
            {
lbl1:
                if(w&0x3) pre=FALSE;

                g=r/4;
                dNum=1+g/10;				//Data***
                iNum=g%10;					//Inf*

                q=0;
                while(q<4)			//Read
                {
                    if(r>=n) break;

                    if(r!=k)
                    {
                        if(lpGame=getGameStruct(1+r/40,
                            (r/4)%10,r&0x3))
                        {
                            lpMem[q]=(LPBYTE)HeapAlloc(hHeap,0,
                                            lpGame->Size);
                            if(lpMem[q])
                            {
                                MoveMemory(lpMem[q],lpGame,
                                    lpGame->Size);
                                q++;
                            }
                            else
                                pre=FALSE;

                            BEEPZERO(HeapFree(hHeap,0,lpGame));
                        }
                        else
                            pre=FALSE;
                    }

                    r++;
                }

                //write

                lpHead[0]=q;
                *(LPWORD)&lpHead[1]=
                    (q>0?(s=(LPGAME(lpMem[0])->Size+7)):0);
                *(LPWORD)&lpHead[3]=
                    (q>1?(s+=LPGAME(lpMem[1])->Size):0);
                *(LPWORD)&lpHead[5]=
                    (q>2?(s+LPGAME(lpMem[2])->Size):0);

                g=w/4;
                dNum=1+g/10;				//Data***
                iNum=g%10;					//Inf*

                if(!writeRegKey(dNum,iNum,0,lpHead,
                    q?7:0,TRUE))
                    pre=FALSE;

                p=7;
                for(a=0; a<q; a++)
                {
                    if(lpGame=(LPGAME)lpMem[a])
                    {
                        if(!writeRegKey(dNum,iNum,p,lpGame,
                            lpGame->Size,TRUE))
                            pre=FALSE;

                        p+=lpGame->Size;
                        BEEPZERO(HeapFree(hHeap,0,lpMem[a]));
                    }

                    w++;
                }

                if(r<n) goto lbl1;

                //Delete last key
                if((n&0x3)==1)
                    if(!writeRegKey(1+n/40,(n/4)%10,0,0,0,TRUE))
                        pre=FALSE;

                //Change total Games

                n--;
                if(!writeRegKey(0,1,0,&n,sizeof(n),FALSE))
                    pre=FALSE;

                res=pre;
                HeapFree(hHeap,0,lpHead);
            }
        }

    SetCursor(hCur);
    PlaySel=0;
    return res;
}





LRESULT CALLBACK InputProc(HWND hDlg,
                          UINT message,
                          WPARAM wParam,
                          LPARAM lParam)
{
    static	HWND	hEdit;

    switch (message)
    {
        case WM_INITDIALOG:
            {
                LONG	s;
                LPINPUT	lpi;
                lpi=(LPINPUT)lParam;

                SetWindowText(hDlg,lpi->lpTitle);
                SendMessage(GetDlgItem(hDlg,IDC_STATICF),WM_SETTEXT,
                    0,(LPARAM)lpi->lpText);

                hEdit=GetDlgItem(hDlg,IDC_EDIT1);
                s=GetWindowLong(hEdit,GWL_STYLE);
                s=(s&0xffff0000)|(lpi->EditStyle&~ES_PASSWORD);
                SetWindowLong(hEdit,GWL_STYLE,s);

                if(lpi->EditStyle&ES_PASSWORD)
                    SendMessage(hEdit,EM_SETPASSWORDCHAR,'*',0);

                SendMessage(hEdit,EM_LIMITTEXT,lpi->LimitText,0);
                SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpi->lpInitEdit);

                SetFocus(hEdit);
                return FALSE;
            }

        case WM_COMMAND:
            {
                int i;
                
                i=LOWORD(wParam);

                if (i==IDOK || i==IDCANCEL) 
                {
                    *(LPWORD)buffer=sizeof(buffer);
                    buffer[SendMessage(hEdit,EM_GETLINE,0,
                            (LPARAM)buffer)]=0;
                    EndDialog(hDlg,i==IDCANCEL?0:(int)buffer);
                    return TRUE;
                }

                break;
            }
    }
    return FALSE;
}





DWORD	countCodeFromText(LPSTR lpText)
{
    DWORD	c=0;
    int		l=lstrlen(lpText);

    if(l>0)
    {
        __asm
        {
            mov		ebx,[lpText]
            mov		edi,[l]
            mov		esi,[c]
lbl1:
            movsx	eax,byte ptr [ebx]
            mov		edx,esi
            xor		edx,[PasswordMask]
            not		edx
            xor		eax,edx
            movzx	edx,ah
            movsx	ecx,al
            add		edx,eax
            sbb		edx,ecx
            mov		ecx,edx
            and		ecx,0x31
            ror		eax,cl
            xor		eax,edx
            sub		eax,edi
            mov		ecx,eax
            shr		ecx,16
            adc		al,ah
            adc		ah,cl
            sbb		al,ch
            rcl		eax,13
            add		esi,eax

            inc		ebx
            dec		edi
            jnz		short	lbl1

            mov		[c],esi
        }
    }

    return c;
}



BOOL	deleteSelectedProject(HWND hDlg)
{
    HCURSOR	hCur;
    LPSTR	lpFile;
    LPBGP	lpbg;
    HGLOBAL	hGlob;
    BOOL	res=FALSE, pre=TRUE, mask=TRUE;
    int		n;

    static	INPUT	InpPas={"Code Number",
                            "Enter authenticity Code Number:",
                            "",ES_PASSWORD,4};


    n=SendMessage(hDlgWnds[5],CB_GETCURSEL,0,0);
    if(n!=CB_ERR)
        if(hGlob=(HGLOBAL)SendMessage(hDlgWnds[5],CB_GETITEMDATA,n,0))
            if(lpbg=(LPBGP)GlobalLock(hGlob))
            {
                if(!(lpbg->Flags&0x1))
                {
                    //Check Deletion Code Number

                    if(lpbg->DelPassword)
                    {
                        LPSTR	lpPass;
                        int		y,w;

                        y=0;
                        lpPass=(LPSTR)DialogBoxParam(hInst,
                                    (LPSTR)IDD_INPUT,hDlg,
                                    (DLGPROC)InputProc,
                                    (LPARAM)&InpPas);
                        if(!lpPass || !lstrlen(lpPass)) goto bad0;

                        for(w=3; w>=0; w--)
                            y|=hexFromChar(lpPass[w])<<((3-w)*4);

                        if(y!=lpbg->DelPassword)
                        {
                            SendMessage(hDlg,WM_APP+3,0,0);
bad0:
                            res=TRUE;
                            goto bad;
                        }
                    }

                    //Delete all associate games

                    if(!readRegKey(0,1,0,&n,sizeof(n))) goto bad;
                    if(!(lpFile=(LPSTR)HeapAlloc(hHeap,
                                            HEAP_ZERO_MEMORY,
                                            lstrlen(lpbg->FileName)+
                                            8))) goto bad;

                    hCur=SetCursor(LoadCursor(NULL,(LPSTR)IDC_WAIT));

                    lstrcpy(lpFile,lpbg->FileName);
                    lstrcat(lpFile,".bgp");

                    //Check for encrypted Games
                    if(addOrCheck(n,lpFile,&mask,TRUE))
                    {
                        HeapFree(hHeap,0,lpFile);
                        SetCursor(hCur);

                        MessageBox(hDlg,
                            "The Play Log contains some protected "
                            "references to the games played with this "
                            "game project. Remove these game "
                            "records first!",
                            TextCantRemove,
                            MB_OK|MB_ICONWARNING);
                        goto bad0;
                    }

                    //Deleting, then
                    addOrCheck(n,lpFile,&mask,FALSE);

                    //Delete Game Project file
                    if(!DeleteFile(getfilename(lpFile))) mask=0;

                    HeapFree(hHeap,0,lpFile);
                    SetCursor(hCur);
                    res=pre&mask;
                }
bad:
                GlobalUnlock(hGlob);
            }

    return res;
}




int		hexFromChar(char Letter)
{
    int		i;
    char	l=Letter;

    if(l>='0' && l<='9') i=l-'0';
    else
        i=10+(l|0x20)-'a';

    return i;
}




LRESULT CALLBACK ShowProc(HWND hWnd,
                            UINT mes,
                            WPARAM wParam,
                            LPARAM lParam)
{
    if(mes==WM_PAINT)
    {
        HDC			hdc, hdc1, hDcMem;
        PAINTSTRUCT	ps;
        HBITMAP		hBkg;
        HBRUSH		hBr;
        RECT		Crec;
        int			w,h;

        hdc=BeginPaint(hWnd,&ps);
        hdc1=CreateCompatibleDC(hdc);
        GetClientRect(hWnd,&Crec);
        w=Crec.right-Crec.left;
        h=Crec.bottom-Crec.top;
        hBkg=CreateCompatibleBitmap(hdc,w,h);
        SelectObject(hdc1,hBkg);

        //Set background color

        hBr=CreateSolidBrush(RGB(250,250,250));
        FillRect(hdc1,&Crec,hBr);
        BEEPZERO(DeleteObject(hBr));

        //Drawing

        #define		SHADOW	2
        #define		BORDER	8
        BITMAP		bm;
        RECT		rec, SHrec;


        hDcMem=CreateCompatibleDC(hdc);
        SelectObject(hdc1,hBoldFont);

        SHrec.left=(rec.left=Crec.left+BORDER)+SHADOW;
        SHrec.right=(rec.right=Crec.right-BORDER)+SHADOW;
        SHrec.top=(rec.top=110)+SHADOW;
        SHrec.bottom=(rec.bottom=Crec.bottom-40)+SHADOW;

        //Draw "Just Try Wit It!"
        SelectObject(hDcMem,hJust);
        GetObject(hJust,sizeof(bm),&bm);
        BitBlt(hdc1,(w-bm.bmWidth)/2,h-bm.bmHeight-BORDER,
            bm.bmWidth,bm.bmHeight,hDcMem,0,0,SRCAND);

        //Draw "Batty"
        SelectObject(hDcMem,hBatty);
        GetObject(hBatty,sizeof(bm),&bm);
        BitBlt(hdc1,(w-bm.bmWidth)/2,40,
            bm.bmWidth,bm.bmHeight,hDcMem,0,0,SRCAND);

        SetBkMode(hdc1,TRANSPARENT);

        if(!(ShowMode&0x1))		//Rendition
        {
            LoadString(hInst,textIDs[ShowMode/2],buffer,
                        sizeof(buffer));

            SetTextColor(hdc1,RGB(ShadCol,ShadCol,ShadCol));
            DrawText(hdc1,buffer,-1,&SHrec,DT_NOPREFIX|
                DT_TOP|DT_WORDBREAK|DT_CENTER);

            SetTextColor(hdc1,RGB(MainCol,MainCol,MainCol));
            DrawText(hdc1,buffer,-1,&rec,DT_NOPREFIX|
                DT_TOP|DT_WORDBREAK|DT_CENTER);
        }
        else		//Hall Of Fame
        {
            LPENTRY		lpEnt;
            RECT		Nrec;
            TEXTMETRIC	tm;
            int			y,th,cl;

            SelectObject(hdc1, hBigBoldFont);
            SetTextColor(hdc1,RGB(0,0,255));

            DrawText(hdc1,"Hall Of Fame",-1,&rec,DT_NOPREFIX|
                DT_TOP|DT_WORDBREAK|DT_CENTER);

            y=rec.top+30;
            SelectObject(hdc1, hBoldFont);
            GetTextMetrics(hdc1,&tm);
            th=tm.tmHeight*3/2;

            Nrec.left=rec.left+tm.tmAveCharWidth*3;
            Nrec.right=Nrec.left+104;

            cl=HallColor;
            for(i=0; i<9; i++)
            {
                buffer[0]='1'+i;
                buffer[1]='.';

                SetTextColor(hdc1,cl&0xffffff);
                cl^=RGB(0,164,41^187);

                SelectObject(hdc1, hBoldFont);
                TextOut(hdc1,BORDER,y,buffer,2);

                //Names
                Nrec.top=y;
                Nrec.bottom=y+2*th;
                SelectObject(hdc1, hFont);

                lpEnt=(LPENTRY)(lpHallOfFame+
                                *(LPWORD)&lpHallOfFame[0x2+i*2]);

                lstrcpy(buffer,(LPSTR)lpEnt+sizeof(ENTRY));
                if(lpEnt->Flags&0x1)
                {
                    int	l;

                    l=lstrlen(buffer)+1;
                    lstrcat(buffer," & ");
                    lstrcat(buffer,(LPSTR)lpEnt+sizeof(ENTRY)+l);
                }

                DrawText(hdc1,buffer,-1,&Nrec,DT_NOPREFIX|
                DT_LEFT|DT_TOP|DT_SINGLELINE|DT_WORD_ELLIPSIS);

                //Score
                wsprintf(buffer,"%d",lpEnt->Score);
                TextOut(hdc1,Nrec.right+4,y,buffer,lstrlen(buffer));

                y+=th;
            }

        }

        BEEPZERO(DeleteDC(hDcMem));

        //transfer bitmap

        BitBlt(hdc,0,0,w,h,hdc1,0,0,SRCCOPY);

        BEEPZERO(DeleteDC(hdc1));
        BEEPZERO(DeleteObject(hBkg));

        EndPaint(hWnd,&ps);
        return 0;
    }

    return CallWindowProc((WNDPROC)OldShowProc,
                            hWnd,
                            mes,
                            wParam,
                            lParam);
}




LPVOID	getHallOfFame(VOID)
{
    //Returns: ZERO if error, or
    //		   -Pointer to the Hall of Fame in mem.
    //			[RELEASE it with HeapFree()!!!]

    static	char	HitPak[]="Hit Pak";
    LPBYTE			lpHall=NULL;
    LPBYTE			lpMem;
    LPENTRY			lpEnt;
    int				i,siz;
    WORD			keysize;

    if(!readRegKey(0,2,0,&keysize,sizeof(keysize)))
    {
        //Set new Hall Of Fame

        siz=0x2+9*0x2+9*sizeof(ENTRY)+9*sizeof(HitPak);

        lpHall=(LPBYTE)HeapAlloc(hHeap,0,siz);
        if(!lpHall) return 0;

        *(LPWORD)lpHall=siz;

        lpMem=lpHall+9*0x2+0x2;
        for(i=0; i<9; i++)
        {
            lpEnt=(LPENTRY)lpMem;

            *(LPWORD)&lpHall[0x2+i*2]=lpMem-lpHall;
            lpEnt->Score=10000*(10-i);
            lpEnt->Flags=0x0;

            lstrcpy((LPSTR)lpMem+sizeof(ENTRY),HitPak);

            lpMem+=sizeof(ENTRY)+sizeof(HitPak);
        }

        if(!writeRegKey(0,2,0,lpHall,siz,TRUE))
        {
            HeapFree(hHeap,0,lpHall);
            return 0;
        }

        keysize=siz;
        HeapFree(hHeap,0,lpHall);
    }

    //Open existing Hall of Fame

    lpHall=(LPBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,keysize);
    if(!lpHall) return 0;

    if(!readRegKey(0,2,0,lpHall,keysize))
    {
        HeapFree(hHeap,0,lpHall);
        return 0;
    }

    return lpHall;
}





int		addHallOfFameRecord(int Score, LPGAME lpGame, int Flags)
{
    //ENTRY:	Flags==...
    //				0 - to check to fit only;
    //				1 - to add the left player's name;
    //				2 - to add the right player's name;
    //				3 - to add both names

    //RETURN: -1, if Score is not enough, or
    //		  -2, if error, ro
    //		 Position where New Record was added [0-8]

    WORD	siz;
    LPBYTE	lpHall, lpMem;
    LPENTRY	lpEnt;
    int		i,j,l;
    int		allsiz, oldsiz, plussiz, newsiz;
    int		res=-2;
    int		nLN;
    BOOL	d;

    if(lpGame || Flags==0)
        if(readRegKey(0,2,0,&siz,sizeof(siz)))
            if(lpHall=(LPBYTE)HeapAlloc(hHeap,0,siz))
            {
                if(readRegKey(0,2,0,lpHall,siz))
                {
                    res=-1;
                    for(i=0; i<9; i++)
                    {
                        lpEnt=(LPENTRY)(lpHall+*(LPWORD)&lpHall[2+i*2]);
                        if((signed)lpEnt->Score<Score)
                        {
                            if(!Flags)
                            {
                                res=i;
                                break;
                            }

                            //Set New record
                            res=-2;

                            if(Flags==3)
                                d=((lpGame->GameType&0x82000000)==0x80000000);
                            else
                                d=FALSE;

                            if(Flags==2)
                                nLN=lpGame->nRightNameLen;
                            else
                                nLN=lpGame->nLeftNameLen;

                            allsiz=*(LPWORD)&lpHall[2+8*2];
                            oldsiz=*(LPWORD)&lpHall[2+i*2];
                            plussiz=sizeof(ENTRY)+
                                nLN+1+(d?lpGame->nRightNameLen+1:0);

                            newsiz=plussiz+allsiz;

                            lpMem=(LPBYTE)HeapAlloc(hHeap,
                                            HEAP_ZERO_MEMORY, newsiz);
                            if(lpMem)
                            {
                                MoveMemory(lpMem,lpHall,oldsiz);
                                
                                lpEnt=(LPENTRY)&lpMem[oldsiz];
                                lpEnt->Score=Score;
                                lpEnt->Flags=d;

                                if(Flags==2)
                                    l=1+getRightName(lpGame,
                                        (LPSTR)
                                        &lpMem[oldsiz+sizeof(ENTRY)]);
                                else
                                    l=1+getLeftName(lpGame,
                                        (LPSTR)
                                        &lpMem[oldsiz+sizeof(ENTRY)]);

                                if(d)
                                    getRightName(lpGame,
                                    (LPSTR)
                                    &lpMem[oldsiz+sizeof(ENTRY)+l]);

                                if(allsiz>oldsiz)
                                    MoveMemory(&lpMem[oldsiz+plussiz],
                                        &lpHall[oldsiz],
                                        allsiz-oldsiz);

                                *(LPWORD)lpMem=newsiz;
                                for(j=8; j>i; j--)
                                {
                                    *(LPWORD)&lpMem[2+2*j]=
                                        *(LPWORD)&lpHall[2+2*(j-1)]+
                                        plussiz;
                                }

                                if(writeRegKey(0,2,0,lpMem,newsiz,
                                    TRUE))
                                    res=i;

                                HeapFree(hHeap,0,lpMem);
                                break;
                            }
                        }
                    }
                }

                HeapFree(hHeap,0,lpHall);
            }

    return res;
}




LPSTR	getProjPath(VOID)
{
    //Returns ZERO also as error!
    //USES 'buffer' array!

    char	stenc[9+4];
    LPSTR	lpt=NULL;
    LPGAME	lpGame;
    int		l;

    if(NewGame)
    {
        lstrcpy(stenc,PlayFile.FileName);
        lstrcat(stenc,".bgp");
        lpt=getfilename(stenc);
    }
    else
    {
        if(lpGame=getGameStruct(PlayStruct.Data012etc,
                    PlayStruct.Inf01_9, PlayStruct.Game0123))
        {
            l=lstrlen(getfilename(""));
            getProjFile(lpGame,&buffer[l]);
            lpt=buffer;
            BEEPZERO(HeapFree(hHeap,0,lpGame));
        }
    }

    return lpt;
}




BOOL	getRound(int level012, int round012)
{
    BOOL	res=FALSE;
    int		Offs;
    LPSTR	lpt, lpFile;
    LPBYTE	lpMem, lpDat;
    DWORD	s[2];
    int		l,i;

    lpFile=(LPSTR)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,MAX_PATH);
    if(lpFile)
    {
        lpMem=(LPBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,340);
        if(lpMem)
        {
            if(lpt=getProjPath())
            {
                lstrcpy(lpFile,lpt);
                l=lstrlen(lpFile);
                while(lpFile[--l]!='\\')
                    if(l<=0) break;

                if(readProjFile(&lpFile[l+1],3*0x4,s,0x4*2,0))
                {
                    Offs=6*0x4+0x2+s[0]+s[1];
                    if(readProjFile(&lpFile[l+1],Offs+
                        (level012*16+round012*4+(rnd()&0x3))*333,
                        lpMem,333,0))
                    {
                        //Set up the new Round
                        lpDat=lpMem;

                        //Alien Number
                        BUcoef=*lpDat++;
                        if(BUcoef>100) BUcoef=100;

                        //8 Squeakers
                        DWORD	b,c;

                        for(i=0; i<8; i++)
                        {
                            b=*(LPDWORD)lpDat;
                            if(b&0x8000)
                                adddynobjN(4+i,0x3,
                                    16+b&0x7fff, 
                                    16+((b&0x7fff0000)>>16),
                                    0x00030000,
                                    0x32000032|((b>>31)&0x1),
                                    0x09);
                            else
                                adddynobjN(4+i,0,0,0,0,0,0x58);

                            lpDat+=4;
                        }

                        //Cells
                        int		bit;

                        bit=0;
                        for(i=0; i<MAXCELL; i++)
                        {
                            b=(*(LPDWORD)&lpDat[bit/8])>>((bit%8)&31);
                            if((b&0x30)==0x30)
                            {
                                c=0x20;
                            }
                            else
                            {
                                c=b&0xf;
                                if(c<8) c*=2;
                                else
                                    c=1+(c-8)*2;

                                c|=(c&0x1)?39<<8:38<<8;
                                c|=(b&0x30)<<2;
                                c|=(b&0x3c0)<<17;
                                c|=0x10;
                                if(c&0x80) c|=0x10000000;
                            }

                            lpCell[i]=c;
                            bit+=10;
                        }

                        //Count beatable Cells
                        b=0;
                        for(i=0; i<MAXCELL; i++)
                        {
                            c=lpCell[i];
                            if(!(c&0x60)) b++;
                        }

                        NumCellsOut=b;
                        res=TRUE;

                        //Set up Alien Number
                        BUCellsOutRatio=
                            BUcoef?(NumCellsOut*BUcoef/100):-1;

                        //Set other information
                        b=round012&0x3;

                        lpBkg=lpGrxStart+
                            *(LPDWORD)&lpGrxStart[8*b];
                        lpBkgLast=(LPBYTE)lpBkg+BkgSize;

                        BatColor=BatCols[b];
                        ShadeColor=ShadeCols[b];
                        PureColor=PureCols[b];
                        FillColor=PureColor^0x1;

                        SqNoChange=FALSE;
                        preVar=0;
                        PrestartDraw=TRUE;
                        BatInactive=TRUE;

                        BallsFreez=FALSE;
                        NoAuto=FALSE;
                        ShitMode=FALSE;
                        ShitModeSwitch=FALSE;
                        OverHon=FALSE;
                        nodraw=FALSE;
                        ImNum=0;
                        GameOver=FALSE;
                        BatKilled=FALSE;
                        SmashOn=FALSE;
                        RockMode=FALSE;
                        AlienKill=FALSE;
                        AKon=FALSE;
                        AKWhichBat=0;
                        BUfreeflight=0;
                        BUreCreate=FALSE;
                        BUexpl_on=FALSE;
                        wbird_ufo=-1;
                        fly400_on=FALSE;
                        auxDraw=0;
                        FlashFreq=0;
                        OverH=DoublePlay?3:4;
                        SmashOn=FALSE;
                        SwapBall1=FALSE;
                        TextOn=FALSE;
                    }
                }
            }

            HeapFree(hHeap,0,lpMem);
        }

        HeapFree(hHeap,0,lpFile);
    }

    return res;
}






LRESULT CALLBACK InfoProc(HWND hDlg,
                          UINT message,
                          WPARAM wParam,
                          LPARAM lParam)
{
    static	HWND	hEditG,hEditL,hEditR,hEditE;
    static	int		GamNum;

    switch (message)
    {
        case WM_INITDIALOG:
            {
                wsprintf(buffer,
                    "Batty Player%s Information",
                    TwoPlayers?"s":"");
                SetWindowText(hDlg,buffer);

                wsprintf(buffer,"%s",
                    TwoPlayers?"The name of left player:":
                    "The player name:");
                SetDlgItemText(hDlg,IDC_STATICL,buffer);

                hEditG=GetDlgItem(hDlg,IDC_EDIT1);
                hEditL=GetDlgItem(hDlg,IDC_EDIT2);
                hEditR=GetDlgItem(hDlg,IDC_EDIT3);
                hEditE=GetDlgItem(hDlg,IDC_EDIT4);

                EnableWindow(GetDlgItem(hDlg,IDC_STATICR),TwoPlayers);
                EnableWindow(hEditR,TwoPlayers);

                EnableWindow(GetDlgItem(hDlg,IDC_STATICE),Encryption);
                EnableWindow(hEditE,Encryption);

                SendMessage(hEditG,EM_LIMITTEXT,0x20*2,0);
                if(!readRegKey(0,1,0,&GamNum,sizeof(GamNum)))
                    GamNum=0;
                wsprintf(buffer,"Game%d",GamNum+1);
                SetWindowText(hEditG,buffer);
                SendMessage(hEditG,EM_SETSEL,0,-1);

                SendMessage(hEditL,EM_LIMITTEXT,0x20,0);
                wsprintf(buffer,"%s",
                    TwoPlayers?"Left":"Batty");
                SetWindowText(hEditL,buffer);

                if(TwoPlayers)
                {
                    SendMessage(hEditR,EM_LIMITTEXT,0x20,0);
                    wsprintf(buffer,"%s",
                        TwoPlayers?"Right":"Batty");
                    SetWindowText(hEditR,buffer);
                }

                if(Encryption)
                    SendMessage(hEditE,EM_LIMITTEXT,40,0);

                SetFocus(hEditG);
                return FALSE;
            }



        case WM_COMMAND:
            {
                int		i,j;
                LPSTR	lpGameName,lpLeftName,lpRightName,lpEnc;
                int		nGN,nLN,nRN,nEP;
                DWORD	e;
                BOOL	res;
                char	Proj[8+1+3+1];
                
                i=LOWORD(wParam);
                j=HIWORD(wParam);

                if(i==IDOK || i==IDCANCEL)
                {
                    if(i==IDOK)
                    {
                        res=FALSE;

                        nGN=SendMessage(hEditG,WM_GETTEXTLENGTH,0,0);
                        nLN=SendMessage(hEditL,WM_GETTEXTLENGTH,0,0);

                        if(TwoPlayers)
                        {
                            nRN=SendMessage(hEditR,WM_GETTEXTLENGTH,0,0);
                            if(!nRN) nRN=sizeof(TextUnkn2);
                        }
                        else
                            nRN=0;

                        nEP=Encryption?SendMessage(hEditE,
                            WM_GETTEXTLENGTH,0,0):0;

                        if(!nGN) nGN=sizeof(TextUnkn);
                        if(!nLN) nLN=sizeof(TextUnkn2);

                        if(lpGameName=(LPSTR)HeapAlloc(hHeap,
                                                HEAP_ZERO_MEMORY,nGN+1))
                        {
                            if(lpLeftName=(LPSTR)HeapAlloc(hHeap,
                                                HEAP_ZERO_MEMORY,nLN+1))
                            {
                                if(lpRightName=(LPSTR)HeapAlloc(hHeap,
                                                HEAP_ZERO_MEMORY,nRN+1))
                                {
                                    if(lpEnc=(LPSTR)HeapAlloc(hHeap,
                                                HEAP_ZERO_MEMORY,nEP+1))
                                    {
                                        if(!SendMessage(hEditG,WM_GETTEXT,
                                            nGN+1,(LPARAM)lpGameName))
                                            lstrcpy(lpGameName,TextUnkn);

                                        if(!SendMessage(hEditL,WM_GETTEXT,
                                            nLN+1,(LPARAM)lpLeftName))
                                            lstrcpy(lpLeftName,TextUnkn2);

                                        if(TwoPlayers)
                                            if(!SendMessage(hEditR,WM_GETTEXT,
                                                nRN+1,(LPARAM)lpRightName))
                                                lstrcpy(lpRightName,TextUnkn2);

                                        if(Encryption)
                                        {
                                            SendMessage(hEditE,WM_GETTEXT,
                                                nEP+1,(LPARAM)lpEnc);
                                            e=countCodeFromText(lpEnc);
                                            if(e==0)
                                                Encryption=FALSE;
                                        }
                                        else
                                            e=0;

                                        lstrcpy(Proj,PlayFile.FileName);
                                        lstrcat(Proj,".bgp");

                                        res=writeNewGame(GamNum,
                                            lpGameName,Proj,
                                            lpLeftName,lpRightName,
                                            e);

                                        HeapFree(hHeap,0,lpEnc);
                                    }
                                    HeapFree(hHeap,0,lpRightName);
                                }
                                HeapFree(hHeap,0,lpLeftName);
                            }
                            HeapFree(hHeap,0,lpGameName);
                        }

                        if(!res)
                        {
                            MessageBox(hDlg,
                                "Information was not copied to Play Log.",
                                TextFatal,
                                MB_OK | MB_ICONERROR);
                            i=IDCANCEL;
                        }
                        else
                        {
                            PlayStruct.Data012etc=1+GamNum/40;
                            PlayStruct.Inf01_9=(GamNum/4)%10;
                            PlayStruct.Game0123=GamNum&0x3;
                            NewGame=FALSE;
                        }
                    }

                    EndDialog(hDlg, i);
                    return TRUE;
                }

                break;
            }
    }
    return FALSE;
}





BOOL	ifToAddToLog(VOID)
{
    BOOL	r1,r2;

    if(NewGame)
    {
        r1=(addHallOfFameRecord(OneUp,NULL,0)>=0);
        r2=(addHallOfFameRecord(TwoUp,NULL,0)>=0);

        if(Level || r1 || (r2 && DoublePlay))
            if(DialogBoxParam(hInst,(LPSTR)IDD_INFO,hDesk,
                    (DLGPROC)InfoProc,0)==IDOK)
            {
                if(r1 || r2)
                    addRecord();
            }
    }
    else
    {
        if(!writeOldGame(((PlayStruct.Data012etc-1)*10+
                            PlayStruct.Inf01_9)*4+
                            PlayStruct.Game0123))
            MessageBox(hDesk,"Unable to write to the Play Log.",
                TextFatal,MB_OK|MB_ICONERROR);
        else
            addRecord();
    }

    return TRUE;
}





BOOL	writeOldGame(int GameNumber012)
{
    BOOL	res=FALSE;
    char	q[7];
    int		n,g,dNum,iNum,gNum;
    DWORD	offs;
    LPGAME	lpgm;

    if(!readRegKey(0,1,0,&n,sizeof(n))) return FALSE;
    if(GameNumber012>=n) return FALSE;

    g=GameNumber012/4;
    dNum=g/10;					//Data***
    iNum=g%10;					//Inf*
    gNum=GameNumber012&0x3;		//One of 4 game structs
    dNum++;

    if(!readRegKey(dNum,iNum,0,&q,7)) return FALSE;
    if(gNum>=q[0]) return FALSE;

    if(gNum)
        offs=*(LPWORD)&q[1+(gNum-1)*2];
    else
        offs=1+3*0x2;

    if(lpgm=(LPGAME)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,sizeof(GAME)))
    {
        if(readRegKey(dNum,iNum,offs,lpgm,sizeof(GAME)))
        {
            //Updating Play records
            DWORD	c;

            lpgm->LeftLostBalls=LostBallL;
            lpgm->RightLostBalls=LostBallR;
            lpgm->LeftCaughtBombs=NumBombsL;
            lpgm->RightCaughtBombs=NumBombsR;
            lpgm->LeftCaughtPrizes=NumPrizL;
            lpgm->RightCaughtPrizes=NumPrizR;

            c=lpgm->LeftTimePlayed;
            lpgm->LeftTimePlayed+=LCyc/256;
            if(lpgm->LeftTimePlayed<c) lpgm->LeftTimePlayed=-1;

            c=lpgm->RightTimePlayed;
            lpgm->RightTimePlayed+=RCyc/256;
            if(lpgm->RightTimePlayed<c) lpgm->RightTimePlayed=-1;

            lpgm->TimeLastPlayed=(DWORD)(CountDwnTime.QuadPart/10000000);
            lpgm->TimeSince1Play+=lpgm->TimeLastPlayed;

            lpgm->StartLevel=(WORD)Level;
            lpgm->HiScore=Hi;
            lpgm->StartLives=(BYTE)(NewNumLives>5?NewNumLives:5);

            if(writeRegKey(dNum,iNum,offs,lpgm,sizeof(GAME),FALSE))
                res=TRUE;
        }

        HeapFree(hHeap,0,lpgm);
    }

    return res;
}





VOID	addRecord(VOID)
{
    LPGAME	lpgm;

    lpgm=getGameStruct(PlayStruct.Data012etc,
            PlayStruct.Inf01_9,PlayStruct.Game0123);
    if(lpgm)
    {
        if(DoublePlay)
        {
            if(TwoPlayers)
            {
                if(OneUp==TwoUp)
                    addHallOfFameRecord(OneUp,lpgm,3);
                else
                {
                    addHallOfFameRecord(OneUp,lpgm,1);
                    addHallOfFameRecord(TwoUp,lpgm,2);
                }
            }
            else
                addHallOfFameRecord(OneUp>=TwoUp?OneUp:
                    TwoUp,lpgm,1);
        }
        else
            addHallOfFameRecord(OneUp,lpgm,1);

        HeapFree(hHeap,0,lpgm);
    }

    return;
}





BOOL	addOrCheck(int NumProjs, LPSTR lpFile, 
                   BOOL *mask, BOOL TrueToCheck)
{
    int		a,b;
    int		n=NumProjs;
    int		dNum,iNum,gNum;
    char	ch1,ch2;
    LPGAME	lpgm;
    LPBYTE	lpMem;
    BOOL	res=FALSE;

    for(a=0; a<n; a++)
    {
        dNum=1+a/40;		//Data***
        iNum=(a/4)%10;		//Inf*
        gNum=a&0x3;			//One of 4 game structs

        if(lpgm=getGameStruct(dNum,iNum,gNum))
        {
            lpMem=(LPBYTE)lpgm+sizeof(GAME)+
                        lpgm->nGameNameLen;

            for(b=0; b<lpgm->nGameProjLen; b++)
            {
                ch1=lpFile[b];
                ch2=lpMem[b];
                if(ch1>='a' && ch1<='z') ch1-='a'-'A';
                if(ch2>='a' && ch2<='z') ch2-='a'-'A';

                if(ch1!=ch2) goto next;
            }

            if(!lpFile[b])
            {
                if(!TrueToCheck)
                {
                    if(!deleteGame(a)) mask=0;
                    else
                    {
                        a--;
                        if((--n)<0)
                        {
                            n=0;
                            mask=0;
                        }
                    }
                }
                else
                    if(lpgm->GameType&0x1000000) res=TRUE;
            }
next:
            BEEPZERO(HeapFree(hHeap,0,lpgm));
        }
    }

    return res;
}





VOID	inGameText(LPSTR lpText)
{
    TextOn=TRUE;
    DialogBoxParam(hInst,(LPSTR)IDD_TEXT,hMainWnd,
        (DLGPROC)TextProc,(LPARAM)lpText);
    TextOn=FALSE;
    takeCursAway();
    return;
}





LRESULT CALLBACK TextProc(HWND hDlg,
                          UINT message,
                          WPARAM wParam,
                          LPARAM lParam)
{
            HDC		hdc;
    static	RECT	rec;
    static	LPSTR	lpText;
    static	int		len;
    static	HFONT	hFnt;
            RECT	wrec,clip;

    switch (message)
    {
        case WM_INITDIALOG:
            {
                int		r=160,y,w,h,tw,th,tp,c;
                int		iter;
                BOOL	itsit=FALSE;

                hdc=GetDC(hDlg);
                hTextDlg=hDlg;

                hFnt=(HFONT)SendMessage(hDlg,WM_GETFONT,0,0);

                GetWindowRect(hMainWnd,&wrec);
                tw=wrec.right-wrec.left;
                th=wrec.bottom-wrec.top;
                tp=wrec.top;

                iter=th/10;

                SelectObject(hdc,hFnt);
                len=lstrlen((LPSTR)lParam);
again:
                rec.left=rec.top=0;
                rec.right=r;
                rec.bottom=80;

                DrawText(hdc,(LPSTR)lParam,len,&rec,
                    DT_CALCRECT|DT_CENTER|DT_NOPREFIX|
                    DT_TOP|DT_WORDBREAK|DT_WORD_ELLIPSIS);

                iter--;
                if(!itsit && iter>0)
                {
                    y=rec.right-rec.left;
                    if(y>=(tw-40*2))
                    {
                        r=tw-40*2;
                        itsit=TRUE;
                        goto again;
                    }

                    if(y<=((rec.bottom-rec.top)*2))
                    {
                        r+=10;
                        goto again;
                    }
                }

                if((rec.right-rec.left)<160)
                {
                    rec.left=0;
                    rec.right=160;
                }

                w=(rec.right-rec.left)+10*2+
                    GetSystemMetrics(SM_CXFIXEDFRAME)*2;
                h=(rec.bottom-rec.top)+10*2+
                    GetSystemMetrics(SM_CYFIXEDFRAME)*2;
                wrec.left+=((wrec.right-wrec.left)-w)/2;
                wrec.top+=((wrec.bottom-wrec.top)-h)/2;

                y=rec.right-rec.left;
                rec.left=(w-y)/2-GetSystemMetrics(SM_CXFIXEDFRAME);
                rec.right=rec.left+y;
                
                y=rec.bottom-rec.top;
                rec.top=(h-y)/2-GetSystemMetrics(SM_CYFIXEDFRAME);
                rec.bottom=rec.top+y;

                lpText=(LPSTR)lParam;

                c=GetSystemMetrics(SM_CYCAPTION);
                if(h>=(th-6*2-20-c))
                {
                    h=th-6*2-20-c;
                    rec.top=6;
                    wrec.top=tp+6+c;
                }

                MoveWindow(hDlg,wrec.left,wrec.top,
                    w,h,TRUE);

                ReleaseDC(hDlg,hdc);

                clip.right=clip.left=wrec.left+w/2;
                clip.bottom=clip.top=wrec.top+h/2;
                SetCursorPos(clip.left,clip.right);
                ClipCursor(&clip);

                SetTimer(hDlg,0x111,1000*60,NULL);
                SetFocus(hDlg);
                return FALSE;
            }


        case WM_PAINT:
            PAINTSTRUCT		ps;
            HGDIOBJ			hOld;

            hdc=BeginPaint(hDlg,&ps);
            hOld=SelectObject(hdc,hFnt);
            SetBkMode(hdc,TRANSPARENT);

            DrawText(hdc,lpText,len,&rec,
                DT_CENTER|DT_NOPREFIX|
                DT_TOP|DT_WORDBREAK);

            SelectObject(hdc,hOld);
            EndPaint(hDlg,&ps);
            break;


        case WM_GETDLGCODE:
            return DLGC_WANTALLKEYS|DLGC_WANTARROWS|
                DLGC_WANTTAB|DLGC_WANTCHARS;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_KEYDOWN:
        case WM_TIMER:
        case WM_SYSKEYDOWN:
        case WM_CHAR:
        case WM_APP:
end:
            KillTimer(hDlg,0x111);
            EndDialog(hDlg, 0);
            break;

        case WM_COMMAND:
            if(LOWORD(wParam)==IDCANCEL) goto end;
            break;

    }
    return FALSE;
}





