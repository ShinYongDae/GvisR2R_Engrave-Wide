
// GvisR2R_LaserView.cpp : CGvisR2R_LaserView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "GvisR2R_Laser.h"
#endif

#include "GvisR2R_LaserDoc.h"
#include "GvisR2R_LaserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MainFrm.h"

#include "Process/DataFile.h"

#include "Dialog/DlgMyPassword.h"
#include "Dialog/DlgMyMsgSub00.h"
#include "Dialog/DlgMyMsgSub01.h"
#include "Dialog/DlgMyMsgSub02.h"
#include "Dialog/DlgProgress.h"

#include "Device/MDX2500DEF.h"
#include "Device/SR1000WDEF.h"

extern CMainFrame* pFrm;
extern CGvisR2R_LaserDoc* pDoc;
CGvisR2R_LaserView* pView;
extern CString PATH_WORKING_INFO;

// CGvisR2R_LaserView

IMPLEMENT_DYNCREATE(CGvisR2R_LaserView, CFormView)

BEGIN_MESSAGE_MAP(CGvisR2R_LaserView, CFormView)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DLG_INFO, OnDlgInfo)
	ON_MESSAGE(WM_MYMSG_EXIT, OnMyMsgExit)
	ON_MESSAGE(WM_CLIENT_RECEIVED_MDX, wmClientReceivedMdx)
	ON_MESSAGE(WM_CLIENT_RECEIVED_SR, wmClientReceivedSr)
	ON_MESSAGE(WM_SERVER_RECEIVED, wmServerReceived)
	//ON_MESSAGE(WM_CLIENT_CLOSED, wmClientClosed)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CGvisR2R_LaserView ����/�Ҹ�

CGvisR2R_LaserView::CGvisR2R_LaserView()
	: CFormView(IDD_GVISR2R_LASER_FORM)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	pView = this;

	m_bShift2Mk = FALSE;

	m_bBufEmpty[0] = FALSE;
	m_bBufEmpty[1] = FALSE;

//	m_nDebugStep = 0;
	m_nWatiDispMain = 0;
	m_nNewLot = 0;


	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;

	m_nWatiDispMain = 0;

	m_bStopFromThread = FALSE;
	m_bBuzzerFromThread = FALSE;

	m_nRtnMyMsgBoxIdx = -1;
	m_bDispMyMsgBox = FALSE;
	m_bDispMain = FALSE;
	m_bProbDn[0] = m_bProbDn[1] = FALSE;

	m_nSelRmap = RMAP_UP;
	m_nSelRmapInner = RMAP_INNER_UP;

	m_bTIM_INIT_VIEW = FALSE;
	m_nStepInitView = 0;
	m_nLotEndSerial = 0;

	m_nDummy[0] = 0;
	m_nDummy[1] = 0;
	m_nAoiLastSerial[0] = 0;
	m_nAoiLastSerial[1] = 0;
	m_bChkLastProcVs = FALSE;

	m_dTempPauseLen = 0.0;

	m_bInit = FALSE;
	m_bDispMsg = FALSE;
	for (int kk = 0; kk < 10; kk++)
	{
		m_bDispMsgDoAuto[kk] = FALSE;
		m_nStepDispMsg[kk] = 0;
	}
	m_sFixMsg[0] = _T("");
	m_sFixMsg[1] = _T("");

	m_bWaitClrDispMsg = FALSE;
	m_bOpenShareUp = TRUE;
	m_bOpenShareDn = TRUE;

	m_bStopFeeding = FALSE;

	// 	m_pMpe = NULL;
	m_pDlgMyMsg = NULL;
	m_pDlgMsgBox = NULL;

	m_pDlgInfo = NULL;
	m_pDlgFrameHigh = NULL;
	m_pDlgMenu01 = NULL;
	m_pDlgMenu02 = NULL;
	m_pDlgMenu03 = NULL;
	m_pDlgMenu04 = NULL;
	m_pDlgMenu05 = NULL;

	m_nCntTowerWinker = 0;
	m_bTimTowerWinker = FALSE;
	m_bTimBtnWinker = FALSE;
	m_bTimBuzzerWarn = FALSE;
	m_bTowerWinker[0] = FALSE; // R
	m_bTowerWinker[1] = FALSE; // G
	m_bTowerWinker[2] = FALSE; // Y
	m_bBtnWinker[0] = FALSE; // Ready
	m_bBtnWinker[1] = FALSE; // Reset
	m_bBtnWinker[2] = FALSE; // Run
	m_bBtnWinker[3] = FALSE; // Stop
	for (int nI = 0; nI < 4; nI++)
	{
		m_nCntBtnWinker[nI] = 0;
		m_nDlyWinker[nI] = 5;
	}

	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;
	m_bAoiTest[0] = FALSE;
	m_bAoiTest[1] = FALSE;


	// H/W Device �ʱ�ȭ.....
	m_pMpe = NULL;
	m_pMotion = NULL;
	m_pLight = NULL;

#ifdef USE_VISION
	m_pVision[0] = NULL;
	m_pVision[1] = NULL;
#endif

	m_bTIM_DISP_STATUS = FALSE;

	m_bThread[0] = FALSE;
	m_dwThreadTick[0] = 0;
	m_bThread[1] = FALSE;
	m_dwThreadTick[1] = 0;
	m_bThread[2] = FALSE;
	m_dwThreadTick[2] = 0;

	m_bTIM_MPE_IO = FALSE;


	m_nStepAuto = 0;
	m_nPrevStepAuto = 0;
	m_nPrevMkStAuto = 0;
	m_nStepMk[0] = 0;
	m_nStepMk[1] = 0;

	m_sShare[0] = _T("");
	m_sBuf[0] = _T("");
	m_sShare[1] = _T("");
	m_sBuf[1] = _T("");

	m_sTick = _T("");
	m_sDispTime = _T("");

	m_bChkMpeIoOut = FALSE;

	m_bMkTmpStop = FALSE;
	m_bAoiLdRun = TRUE;
	m_bAoiLdRunF = FALSE;

	m_dwCycSt = 0;
	m_dwCycTim = 0;

	m_bTHREAD_DISP_DEF = FALSE;
	m_nStepTHREAD_DISP_DEF = 0;

	m_bTHREAD_MK[0] = FALSE;
	m_bTHREAD_MK[1] = FALSE;
	m_bTHREAD_MK[2] = FALSE;
	m_bTHREAD_MK[3] = FALSE;
	m_nMkPcs[0] = 0;
	m_nMkPcs[1] = 0;
	m_nMkPcs[2] = 0;
	m_nMkPcs[3] = 0;

	m_nErrCnt = 0;

	m_bAuto = FALSE;
	m_bManual = FALSE;
	m_bOneCycle = FALSE;

	m_bSwRun = FALSE; m_bSwRunF = FALSE;
	m_bSwStop = FALSE; m_bSwStopF = FALSE;
	m_bSwReset = FALSE; m_bSwResetF = FALSE;
	m_bSwReady = FALSE; m_bSwReadyF = FALSE;

	m_bSwStopNow = FALSE;

	for (int nAxis = 0; nAxis < MAX_AXIS; nAxis++)
		m_dEnc[nAxis] = 0.0;

	for (int i = 0; i < 10; i++)
		m_sDispMsg[i] = _T("");

	m_bNewModel = FALSE;
	m_dTotVel = 0.0; m_dPartVel = 0.0;
	m_bTIM_CHK_TEMP_STOP = FALSE;
	m_bTIM_SAFTY_STOP = FALSE;
	m_bTIM_TCPIP_UPDATE = FALSE;
	m_bTIM_START_UPDATE = FALSE;
	m_bTIM_MENU01_UPDATE_WORK = FALSE;
	m_sMyMsg = _T("");
	m_nTypeMyMsg = IDOK;

	m_dwLotSt = 0; m_dwLotEd = 0;

	m_lFuncId = 0;

	m_bDrawGL = TRUE;
	m_bCont = FALSE;
	m_bCam = FALSE;
	m_bReview = FALSE;

	m_bChkBufIdx[0] = TRUE;
	m_nChkBufIdx[0] = 0;
	m_bChkBufIdx[1] = TRUE;
	m_nChkBufIdx[1] = 0;

	m_dwStMkDn[0] = 0;
	m_dwStMkDn[1] = 0;
	m_nVsBufLastSerial[0] = 0;
	m_nVsBufLastSerial[1] = 0;
	m_bShowModalMyPassword = FALSE;

	// 	m_bFurgeRun = FALSE;
	// 	m_nFurgeRun = 0;

	m_nRstNum = 0;
	m_bBufHomeDone = FALSE;
	m_bReadyDone = FALSE;

	//ResetPriority();
	m_Flag = 0L;
	m_AoiLdRun = 0L;

	m_bDoneDispMkInfo[0][0] = FALSE; // Cam0, Up
	m_bDoneDispMkInfo[0][1] = FALSE; // Cam0, Dn
	m_bDoneDispMkInfo[1][0] = FALSE; // Cam1, Up
	m_bDoneDispMkInfo[1][1] = FALSE; // Cam1, Dn

	m_nShareUpS = 0;
	m_nShareUpSerial[0] = 0;
	m_nShareUpSerial[1] = 0;
	m_nShareUpCnt = 0;

	m_nShareDnS = 0;
	m_nShareDnSerial[0] = 0;
	m_nShareDnSerial[1] = 0;
	m_nShareDnCnt = 0;

	m_nBufSerial[0][0] = 0; // Up-Cam0
	m_nBufSerial[0][1] = 0; // Up-Cam1
	m_nBufSerial[1][0] = 0; // Dn-Cam0
	m_nBufSerial[1][1] = 0; // Dn-Cam0

	m_bReAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bReAlign[1][1] = FALSE; // [nCam][nPos] 

	m_bSkipAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bSkipAlign[1][1] = FALSE; // [nCam][nPos] 

	m_bFailAlign[0][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[0][1] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][0] = FALSE; // [nCam][nPos] 
	m_bFailAlign[1][1] = FALSE; // [nCam][nPos] 

	m_bDoMk[0] = TRUE;
	m_bDoMk[1] = TRUE;
	m_bDoneMk[0] = FALSE;
	m_bDoneMk[1] = FALSE;
	m_bReMark[0] = FALSE;
	m_bReMark[1] = FALSE;

	m_nMonAlmF = 0;
	m_nClrAlmF = 0;

	m_bMkSt = FALSE;
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = 0;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;

	m_bLotEnd = FALSE;
	m_nLotEndAuto = 0;

	m_bLastProc = FALSE;
	m_bLastProcFromUp = TRUE;
	m_nLastProcAuto = 0;

	m_bLoadShare[0] = FALSE;
	m_bLoadShare[1] = FALSE;

	m_sNewLotUp = _T("");
	m_sNewLotDn = _T("");

	m_bAoiFdWrite[0] = FALSE;
	m_bAoiFdWrite[1] = FALSE;
	m_bAoiFdWriteF[0] = FALSE;
	m_bAoiFdWriteF[1] = FALSE;

	m_bCycleStop = FALSE;

	for (int a = 0; a < 2; a++)
	{
		for (int b = 0; b < MAX_STRIP; b++)
		{
			m_nMkStrip[a][b] = 0;
			m_bRejectDone[a][b] = FALSE;
		}
	}

	m_sDispMain = _T("");
	m_bReMk = FALSE;

	m_bWaitPcr[0] = FALSE;
	m_bWaitPcr[1] = FALSE;

	m_bShowMyMsg = FALSE;
	m_pMyMsgForeground = NULL;
	m_bContDiffLot = FALSE;

	// 	m_nMsgShiftX = 0;
	// 	m_nMsgShiftY = 0;

	for (int nAns = 0; nAns < 10; nAns++)
		m_bAnswer[nAns] = FALSE;

	m_bChkLightErr = FALSE;

	// client for SR-1000W
	m_pSr1000w = NULL;

	// client for MD-X2500
	m_pMdx2500 = NULL;

	// server for engrave
	m_pEngrave = NULL;

	m_bDestroyedView = FALSE;
	m_bContEngraveF = FALSE;

	m_bStopF_Verify = FALSE;

	m_bLoadMstInfo = FALSE;
	m_bLoadMstInfoF = FALSE;

	m_pDlgUtil01 = NULL;
	//m_pDlgUtil02 = NULL;
	m_bEngStop = FALSE;

	m_sGetItsCode = _T("");
	m_nGetItsCodeSerial = 0;

	m_bSetSig = FALSE;
	m_bSetSigF = FALSE;
	m_bSetData = FALSE;
	m_bSetDataF = FALSE;

	m_sMonDisp = _T("");
}

CGvisR2R_LaserView::~CGvisR2R_LaserView()
{
	DestroyView();
}

void CGvisR2R_LaserView::DestroyView()
{
	if (!m_bDestroyedView)
	{
		m_bDestroyedView = TRUE;

		ThreadKill();
		Sleep(30);

		DelAllDlg();
		Sleep(100);

		Buzzer(FALSE, 0);
		Buzzer(FALSE, 1);

#ifdef USE_VISION
		if (m_pVision[1])
		{
			delete m_pVision[1];
			m_pVision[1] = NULL;
		}

		if (m_pVision[0])
		{
			delete m_pVision[0];
			m_pVision[0] = NULL;
		}
#endif

		m_bTIM_MPE_IO = FALSE;
		m_bTIM_DISP_STATUS = FALSE;
		m_bTIM_INIT_VIEW = FALSE;
		Sleep(100);


		InitIoWrite();
		SetMainMc(FALSE);

		// H/W Device �Ҹ�.....
		HwKill();

		CloseMyMsg();

		if (m_ArrayMyMsgBox.GetSize() > 0)
		{
			m_ArrayMyMsgBox.RemoveAll();
		}
	}
}

void CGvisR2R_LaserView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CGvisR2R_LaserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CGvisR2R_LaserView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	InitMyMsg();

	pDoc->LoadWorkingInfo();
	pDoc->LoadIoInfo();
	pDoc->LoadSignalInfo();
	pDoc->LoadDataInfo();
	if (!LoadMySpec())
		LoadMySpec();
	pDoc->GetCurrentInfo();

#ifdef USE_CAM_MASTER
	CFileFind finder;
	CString sDir, sMsg;
	sDir = pDoc->WorkingInfo.System.sPathCamSpecDir;
	sDir.Delete(sDir.GetLength() - 1, 1);
	sDir.ReleaseBuffer();

	//TCHAR szFile[MAX_PATH] = { 0, };
	//_stprintf(szFile, _T("%s\\*.*"), sDir);

	//if (!finder.FindFile(sDir)) // Check 1st Spec folder is
	if(!pDoc->DirectoryExists(sDir))
	{
		sMsg.Format(_T("ķ�����Ϳ� ���������� �����ϴ�. : \n 1.SpecFolder : %s"), sDir);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg, MB_ICONSTOP | MB_OK);
		ExitProgram();
		return;
	}
#endif

	if (!m_bTIM_INIT_VIEW)
	{
		m_nStepInitView = 0;
		m_bTIM_INIT_VIEW = TRUE;
		SetTimer(TIM_INIT_VIEW, 300, NULL);
	}

}


// CGvisR2R_LaserView ����

#ifdef _DEBUG
void CGvisR2R_LaserView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGvisR2R_LaserView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGvisR2R_LaserDoc* CGvisR2R_LaserView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGvisR2R_LaserDoc)));
	return (CGvisR2R_LaserDoc*)m_pDocument;
}
#endif //_DEBUG


// CGvisR2R_LaserView �޽��� ó����


void CGvisR2R_LaserView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nInc = 0; int nSrl;
	CString str, sMsg, sPath;
	//BOOL bExist;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (nIDEvent == TIM_INIT_VIEW)
	{
		KillTimer(TIM_INIT_VIEW);

		switch (m_nStepInitView)
		{
		case 0:
			m_nStepInitView++;
			DispMsg(_T("���α׷��� �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
	
			// H/W Device �ʱ�ȭ.....
			HwInit();
			break;
		case 1:
			m_nStepInitView++;
			InitIO();
			break;
		case 2:
			m_nStepInitView++;
			InitIoWrite();
			SetMainMc(TRUE);

			m_nMonAlmF = 0;
			m_nClrAlmF = 0;

			break;
		case 3:
			m_nStepInitView++;
			ThreadInit();
			break;
		case 4:
			m_nStepInitView++;
			break;
		case 5:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 1"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_02);
			break;
		case 6:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.-2"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_01);
			break;
		case 7:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 3"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			break;
		case 8:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 4"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_03);
			break;
		case 9:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 5"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_04);
			break;
		case 10:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 6"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_MENU_05);
			break;
		case 11:
			m_nStepInitView++;
			DispMsg(_T("ȭ�鱸���� �����մϴ�.- 7"), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			ShowDlg(IDD_DLG_FRAME_HIGH);
			if (m_pDlgFrameHigh)
				m_pDlgFrameHigh->ChkMenu01();
			SetDualTest(pDoc->WorkingInfo.LastJob.bDualTest);
			break;
		case 12:
			Init(); // AmpReset()
			Sleep(300);
			m_nStepInitView++;
			break;
		case 13:
			m_nStepInitView++;
			DispMsg(_T("H/W�� �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
			InitAct();
			m_bStopFeeding = TRUE;
#ifdef USE_MPE
			if (m_pMpe)
				m_pMpe->Write(_T("MB440115"), 1); // ��ŷ��Feeding����
#endif
			Sleep(300);
			break;
		case 14:
			if (m_pMotion)
			{
				DispMsg(_T("Homming"), _T("Searching Home Position..."), RGB_GREEN, 2000, TRUE);
				m_pMotion->SearchHome();
				m_nStepInitView++;
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 15:
			if (m_pMotion)
			{
				if (m_pMotion->IsHomeDone())// && m_pMotion->IsHomeDone(MS_MKFD))
				{
					m_nStepInitView++;
					Sleep(300);
				}

				sMsg.Format(_T("X0(%s) , Y0(%s)"), m_pMotion->IsHomeDone(MS_X0) ? _T("Done") : _T("Doing"),
					m_pMotion->IsHomeDone(MS_Y0) ? _T("Done") : _T("Doing"));
				DispMsg(sMsg, _T("Homming"), RGB_GREEN, 2000, TRUE);
			}
			else
			{
				m_bTIM_INIT_VIEW = FALSE;
				AfxMessageBox(_T("Motion is failed."));
				PostMessage(WM_CLOSE);
			}
			break;
		case 16:
			m_nStepInitView++;
			m_bLoadMstInfo = TRUE;
			m_bTIM_START_UPDATE = TRUE;
			SetTimer(TIM_START_UPDATE, 50, NULL);
			break;
		case 17:
			m_nStepInitView++;
			DispMsg(_T("Completed Searching Home Pos..."), _T("Homming"), RGB_GREEN, 2000, TRUE);
			m_pMotion->ObjectMapping();
			break;
		case 18:
			TcpIpInit();
			m_nStepInitView++;
			break;
		case 19:
			if (m_bLoadMstInfo)
			{
				Sleep(100);
				break;
			}

			m_nStepInitView++;
			SetAlignPos();
			break;
		case 20:
			m_nStepInitView++;
			//m_bTIM_START_UPDATE = TRUE;
			//SetTimer(TIM_START_UPDATE, 50, NULL);
			//m_bTIM_TCPIP_UPDATE = TRUE;
			//SetTimer(TIM_TCPIP_UPDATE, 500, NULL);
			break;

		case 21:
			m_nStepInitView++;
			m_bStopFeeding = FALSE;
#ifdef USE_MPE
			if (m_pMpe)
				m_pMpe->Write(_T("MB440115"), 0); // ��ŷ��Feeding����
#endif
			if(m_pDlgMenu02)
				m_pDlgMenu02->SetJogSpd(_tstoi(pDoc->WorkingInfo.LastJob.sJogSpd));
			if (m_pDlgMenu03)
				m_pDlgMenu03->InitRelation();
			//m_pMotion->SetFeedRate(MS_X0Y0, 1.0);
			//Sleep(30);
			//m_pMotion->SetFeedRate(MS_X1Y1, 1.0);
			m_pMotion->SetR2RConf();
			//m_pMotion->SetTorque(AXIS_MKTQ, _tstof(pDoc->WorkingInfo.Motion.sMkTq));
			//m_pMotion->SetTorque(AXIS_AOITQ, _tstof(pDoc->WorkingInfo.Motion.sAoiTq));
			//TowerLamp(RGB_YELLOW, FALSE, TRUE);
			TowerLamp(RGB_YELLOW, TRUE);
			//TowerLamp(RGB_RED, TRUE);

			//if(!SetCollision(-10.0))
			if (!SetCollision(-1.0*_tstof(pDoc->WorkingInfo.Motion.sCollisionLength)))
			{
				DispMsg(_T("Collision"), _T("Failed to Set Collision ..."), RGB_GREEN, 2000, TRUE);
			}

			break;
		case 22:
			m_nStepInitView++;
			//MoveMkInitPos();
			InitPLC();
			SetPlcParam();
			GetPlcParam();
			//TcpIpInit();
			m_bTIM_DISP_STATUS = TRUE;
			SetTimer(TIM_DISP_STATUS, 100, NULL);
			break;
		case 23:
			m_nStepInitView++;
			ClrDispMsg();
			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->SetStripAllMk();
				m_pDlgMenu01->RefreshRmap();
				m_pDlgMenu01->ResetLastProc();
			}
			if (!MemChk())
				AfxMessageBox(_T("Memory Error - Cam Spec Data : PCR[0] or PCR[1] or Reelmap"));
			else
			{
				if (pDoc->m_pReelMap)
				{
#ifndef TEST_MODE
					ReloadRst();
					UpdateRst();
#endif
					UpdateLotTime();
				}
			}

			// Check Encoder
			if (!m_bThread[1])
				m_Thread[1].Start(GetSafeHwnd(), this, ThreadProc1);

			// DispDefImg
			if (!m_bThread[2])
				m_Thread[2].Start(GetSafeHwnd(), this, ThreadProc2);

			//MoveInitPos1();
			//Sleep(30);
			MoveInitPos0();

			SetLotLastShot();
			StartLive();

			pDoc->SetEngraveLastShot(pDoc->GetCurrentInfoEngShotNum());

			if (pDoc->GetTestMode() == MODE_OUTER)
			{
				if (m_pDlgMenu01)
					m_pDlgMenu01->EnableItsMode();
			}

			m_bTIM_MPE_IO = TRUE;
			SetTimer(TIM_MPE_IO, 50, NULL);

			m_bTIM_INIT_VIEW = FALSE;
			break;
		}

		if (m_bTIM_INIT_VIEW)
			SetTimer(TIM_INIT_VIEW, 100, NULL);
	}

	if (nIDEvent == TIM_MPE_IO)
	{
		KillTimer(TIM_MPE_IO);

		DoIO(); // DoAutoEng()

		ChkMyMsg();

		if (m_bTIM_MPE_IO)
			SetTimer(TIM_MPE_IO, 100, NULL);
	}

	if (nIDEvent == TIM_TOWER_WINKER)
	{
		KillTimer(TIM_TOWER_WINKER);
		DispTowerWinker();
		if (m_bTimTowerWinker)
			SetTimer(TIM_TOWER_WINKER, 100, NULL);
	}


	if (nIDEvent == TIM_BUZZER_WARN)
	{
		KillTimer(TIM_BUZZER_WARN);
		m_nCntBz++;
		if (m_nCntBz > BUZZER_DELAY)
		{
			m_bTimBuzzerWarn = FALSE;
			Buzzer(FALSE);
		}
		if (m_bTimBuzzerWarn)
			SetTimer(TIM_BUZZER_WARN, 100, NULL);
	}

	if (nIDEvent == TIM_DISP_STATUS)
	{
		KillTimer(TIM_DISP_STATUS);

		DispStsBar();
		DoDispMain();
		//		DispMyMsgBox();

		if (m_bStopFromThread)
		{
			m_bStopFromThread = FALSE;
			Stop();
		}
		if (m_bBuzzerFromThread)
		{
			m_bBuzzerFromThread = FALSE;
			Buzzer(TRUE, 0);
		}


		//		SetMpeIO();

		ChkEmg();
		ChkSaftySen();
		ChkDoor();

		if (m_bTIM_DISP_STATUS)
			SetTimer(TIM_DISP_STATUS, 100, NULL);
	}


	if (nIDEvent == TIM_SHOW_MENU01)
	{
		KillTimer(TIM_SHOW_MENU01);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu01();
	}

	if (nIDEvent == TIM_SHOW_MENU02)
	{
		KillTimer(TIM_SHOW_MENU02);
		if (m_pDlgFrameHigh)
			m_pDlgFrameHigh->ChkMenu02();
	}

	if (nIDEvent == TIM_CHK_TEMP_STOP)
	{
		KillTimer(TIM_CHK_TEMP_STOP);
#ifdef USE_MPE
		if (!(pDoc->m_pMpeSignal[7] & (0x01 << 3)))	// �Ͻ��������(PC�� On��Ű��, PLC�� Ȯ���ϰ� Off��Ŵ)-20141031
		{
			m_bTIM_CHK_TEMP_STOP = FALSE;
			m_pDlgMenu01->SetTempStop(FALSE);
		}
#endif
		if (m_bTIM_CHK_TEMP_STOP)
			SetTimer(TIM_CHK_TEMP_STOP, 500, NULL);
	}

	if (nIDEvent == TIM_SAFTY_STOP)
	{
		KillTimer(TIM_SAFTY_STOP);
		MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
		m_bTIM_SAFTY_STOP = FALSE;
	}

	if (nIDEvent == TIM_START_UPDATE)
	{
		KillTimer(TIM_START_UPDATE);

		if (m_bLoadMstInfo && !m_bLoadMstInfoF)
		{
			//if (m_bTIM_TCPIP_UPDATE)
			if (!pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() && !pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			{
				m_bLoadMstInfoF = TRUE;
				SetTimer(TIM_TCPIP_UPDATE, 500, NULL);
			}
			else
				m_bLoadMstInfo = FALSE;
		}

		if (m_bSetSig && !m_bSetSigF)
		{
			m_bSetSigF = TRUE;

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
			}

			//if (m_pDlgMenu03)
			//	m_pDlgMenu03->UpdateSignal();

			m_bSetSig = FALSE;
		}
		else if (!m_bSetSig && m_bSetSigF)
		{
			m_bSetSigF = FALSE;
		}

		if (m_bSetData && !m_bSetDataF)
		{
			m_bSetDataF = TRUE;

			if (m_pEngrave->m_bGetOpInfo || m_pEngrave->m_bGetInfo)
			{
				if (m_pDlgInfo)
					m_pDlgInfo->UpdateData();

				if (m_pDlgMenu01)
					m_pDlgMenu01->UpdateData();

				m_pEngrave->m_bGetOpInfo = FALSE;
				m_pEngrave->m_bGetInfo = FALSE;
			}

			if (m_pDlgMenu02)
				m_pDlgMenu02->UpdateData();

			if (m_pDlgMenu03)
				m_pDlgMenu03->UpdateData();

			if (m_pDlgMenu04)
				m_pDlgMenu04->UpdateData();

			m_bSetData = FALSE;
		}
		else if (!m_bSetData && m_bSetDataF)
		{
			m_bSetDataF = FALSE;
		}

		if (m_bTIM_START_UPDATE)
			SetTimer(TIM_START_UPDATE, 100, NULL);
	}

	if (nIDEvent == TIM_TCPIP_UPDATE)
	{
		KillTimer(TIM_TCPIP_UPDATE);
		LoadMstInfo();
		if (m_pDlgMenu01)
			m_pDlgMenu01->UpdateData();
		m_bLoadMstInfoF = FALSE;
		m_bLoadMstInfo = FALSE;
	}

	if (nIDEvent == TIM_MENU01_UPDATE_WORK)
	{
		KillTimer(TIM_MENU01_UPDATE_WORK);
		pDoc->GetMkMenu01();
		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->UpdateData();
			m_pDlgMenu01->UpdateWorking();
			m_pDlgMenu01->DispTotRatio();
			m_pDlgMenu01->DispStripRatio();
			m_pDlgMenu01->DispDef();
			m_pDlgMenu01->DispMkCnt();
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CGvisR2R_LaserView::InitMyMsg()
{
	if (m_pDlgMyMsg)
		CloseMyMsg();

	m_pDlgMyMsg = new CDlgMyMsg(this);
	m_pDlgMyMsg->Create();
}

void CGvisR2R_LaserView::CloseMyMsg()
{
	if (m_pDlgMyMsg)
	{
		delete m_pDlgMyMsg;
		m_pDlgMyMsg = NULL;
	}
}

LRESULT CGvisR2R_LaserView::OnMyMsgExit(WPARAM wPara, LPARAM lPara)
{
	Buzzer(FALSE, 0);
	ClrAlarm();
	return 0L;
}

int CGvisR2R_LaserView::MsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut)
{
	int nRtnVal = -1; // Reply(-1) is None.
	if (m_pDlgMyMsg)
		nRtnVal = m_pDlgMyMsg->SyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);

	return nRtnVal;
}

int CGvisR2R_LaserView::AsyncMsgBox(CString sMsg, int nThreadIdx, int nType, int nTimOut)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		m_pDlgMyMsg->AsyncMsgBox(sMsg, nThreadIdx, nType, nTimOut);
	return nRtn;
}

int CGvisR2R_LaserView::WaitRtnVal(int nThreadIdx)
{
	int nRtn = -1;
	if (m_pDlgMyMsg)
		nRtn = m_pDlgMyMsg->WaitRtnVal(nThreadIdx);
	return nRtn;
}


void CGvisR2R_LaserView::ChkMyMsg()
{
	return;

	//HWND hwndForeground;
	CWnd *pWndForeground;

	if (m_bShowMyMsg && m_pMyMsgForeground)
	{
		pWndForeground = pFrm->GetForegroundWindow();
		if (pWndForeground != m_pMyMsgForeground)
			m_pMyMsgForeground->SetForegroundWindow();
	}
}

void CGvisR2R_LaserView::UpdateLotTime()
{
	m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;

	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateLotTime();
}

void CGvisR2R_LaserView::DispStsBar(CString sMsg, int nIdx)
{
	if (m_sDispMsg[nIdx] != sMsg)
		m_sDispMsg[nIdx] = sMsg;
	sMsg.Empty();
}

void CGvisR2R_LaserView::DispStsBar()
{
	DispStsMainMsg(); // 0
	DispTime(); // 7
	ChkShare(); // 2, 4
	ChkBuf(); // 1, 3
}

BOOL CGvisR2R_LaserView::MemChk()
{
	if (!pDoc->m_pPcr[0] || !pDoc->m_pPcr[1])// || !pDoc->m_pReelMap)
		return FALSE;
	return TRUE;
}

void CGvisR2R_LaserView::SetMainMc(BOOL bOn)
{
	if (bOn)
	{
		if (m_pMpe)
			m_pMpe->Write(_T("MB440159"), 1); // ��ŷ�� Door Interlock ON
			//m_pMpe->Write(_T("MB440159"), 1); // ��ŷ�� MC ON
	}
	else
	{
		if (m_pMpe)
			m_pMpe->Write(_T("MB440158"), 0); // ��ŷ�� Door Interlock OFF
			//m_pMpe->Write(_T("MB440159"), 0); // ��ŷ�� MC OFF
	}
}

void CGvisR2R_LaserView::ExitProgram()
{
	long lParam = 0;
	long lData = 1;
	lParam = lParam | lData;
	lData = 0x00 << 16;
	lParam = lParam | lData;
	lData = 1 << 29;
	lParam = lParam | lData;
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
}

void CGvisR2R_LaserView::Init()
{
	int nAxis;
	if (m_pMotion)
	{
		for (nAxis = 0; nAxis < m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->AmpReset(nAxis);
			Sleep(30);
		}
	}
}

void CGvisR2R_LaserView::InitIO()
{
}

BOOL CGvisR2R_LaserView::InitAct()
{
#ifdef USE_XMP
	if (!m_pMotion)
		return FALSE;
#endif

#ifdef USE_LIGHT
	if (!m_pLight)
		return FALSE;
#endif

#ifdef USE_FLUCK
	if (!m_pFluck)
		return FALSE;
#endif

	int nAxis;

	if (m_pMotion)
	{
		for (nAxis = 0; nAxis < m_pMotion->m_ParamCtrl.nTotAxis; nAxis++)
		{
			m_pMotion->ServoOnOff(nAxis, TRUE);
			Sleep(100);
		}

		double dX[2], dY[2];

		if (pDoc->m_pSpecLocal && IsPinData())
		{
			dX[0] = pDoc->m_pSpecLocal->m_dPinPosX[0];
			dY[0] = pDoc->m_pSpecLocal->m_dPinPosY[0];
			dX[1] = pDoc->m_pSpecLocal->m_dPinPosX[1];
			dY[1] = pDoc->m_pSpecLocal->m_dPinPosY[1];
		}
		else
		{
			dX[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			dY[0] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]);
			dX[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[1]);
			dY[1] = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[1]);
		}
		m_pMotion->SetPinPos(0, dX[0], dY[0]);
		m_pMotion->SetPinPos(1, dX[1], dY[1]);
		m_pMotion->m_dStBufPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);


		CfPoint ptPnt0(dX[0], dY[0]);
		if (pDoc->m_Master[0].m_pPcsRgn)
			pDoc->m_Master[0].m_pPcsRgn->SetPinPos(0, ptPnt0);

		CfPoint ptPnt1(dX[1], dY[1]);
		if (pDoc->m_Master[0].m_pPcsRgn)
			pDoc->m_Master[0].m_pPcsRgn->SetPinPos(1, ptPnt1);

		if (pDoc->m_pSpecLocal)// && IsMkOffsetData())
		{
			pDoc->SetMkPnt(CAM_BOTH);
		}

		double dPos = _tstof(pDoc->WorkingInfo.Motion.sStBufPos);
		SetBufInitPos(dPos);
		double dVel = _tstof(pDoc->WorkingInfo.Motion.sBufHomeSpd);
		double dAcc = _tstof(pDoc->WorkingInfo.Motion.sBufHomeAcc);
		SetBufHomeParam(dVel, dAcc);
		// 		m_pMotion->SetStBufPos(dPos);

	}

	// Light On
	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->SetLight(_tstoi(pDoc->WorkingInfo.Light.sVal[0]));
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::TcpIpInit()
{
#ifdef USE_MDX2500
	if (!m_pMdx2500)
	{
		m_pMdx2500 = new CMdx2500(pDoc->WorkingInfo.System.sIpClient[ID_MDX2500], pDoc->WorkingInfo.System.sIpServer[ID_MDX2500], pDoc->WorkingInfo.System.sPort[ID_MDX2500], this);
		//m_pMdx2500->SetHwnd(this->GetSafeHwnd());
	}
#endif

#ifdef USE_SR1000W
	if (!m_pSr1000w)
	{
		m_pSr1000w = new CSr1000w(pDoc->WorkingInfo.System.sIpClient[ID_SR1000W], pDoc->WorkingInfo.System.sIpServer[ID_SR1000W], pDoc->WorkingInfo.System.sPort[ID_SR1000W], this);
		//m_pSr1000w->SetHwnd(this->GetSafeHwnd());
	}
#endif

#ifdef USE_TCPIP
	if (!m_pEngrave)
	{
		m_pEngrave = new CEngrave(pDoc->WorkingInfo.System.sIpClient[ID_PUNCH], pDoc->WorkingInfo.System.sIpServer[ID_ENGRAVE], pDoc->WorkingInfo.System.sPort[ID_ENGRAVE], this);
		m_pEngrave->SetHwnd(this->GetSafeHwnd());
	}
#endif

	return TRUE;
}

BOOL CGvisR2R_LaserView::HwInit()
{
	if (m_pLight)
	{
		delete m_pLight;
		m_pLight = NULL;
	}

	m_pLight = new CLight(this);
	m_pLight->Init();

	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}
	m_pMotion = new CMotion(this);
	if (!m_pMotion->InitBoard())
	{
		//DoMyMsgBox(_T("XMP ���� �ʱ�ȭ ����, �ٽ� �����ϼ���.!!!"));
		MsgBox(_T("XMP ���� �ʱ�ȭ ����, �ٽ� �����ϼ���.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}

#ifdef USE_MPE
	if (!m_pMpe)
		m_pMpe = new CMpDevice(this);
	if (!m_pMpe->Init(1, 1))
	{
		//DoMyMsgBox(_T("��īƮ�θ�ũ(MC0)�� ������α׷��� ���� ��, �ٽ� �����ϼ���.!!!"));
		MsgBox(_T("��īƮ�θ�ũ(MC0)�� ������α׷��� ���� ��, �ٽ� �����ϼ���.!!!"));
		PostMessage(WM_CLOSE);
		return FALSE;
	}
#endif

	return TRUE;
}

void CGvisR2R_LaserView::HwKill()
{
	if (m_pMotion)
	{
		delete m_pMotion;
		m_pMotion = NULL;
	}

	if (m_pMpe)
	{
		delete m_pMpe;
		m_pMpe = NULL;
	}

	if (m_pLight)
	{
		m_pLight->Close();
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pEngrave)
	{
		m_pEngrave->Close();
		delete m_pEngrave;
		m_pEngrave = NULL;
	}


	if (m_pSr1000w)
	{
		m_pSr1000w->Close();
		delete m_pSr1000w;
		m_pSr1000w = NULL;
	}

	if (m_pMdx2500)
	{
		m_pMdx2500->Close();
		delete m_pMdx2500;
		m_pMdx2500 = NULL;
	}
}

void CGvisR2R_LaserView::GetDispMsg(CString &strMsg, CString &strTitle)
{
	if (m_pDlgMsgBox)
		m_pDlgMsgBox->GetDispMsg(strMsg, strTitle);
}

void CGvisR2R_LaserView::DispMsg(CString strMsg, CString strTitle, COLORREF color, DWORD dwDispTime, BOOL bOverWrite)
{
	if (m_bDispMsg)
		return;

	if (m_bAuto)
	{
		return;
	}

	m_bDispMsg = TRUE;

	if (dwDispTime == 0)
	{
		dwDispTime = 24 * 3600 * 1000;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (bOverWrite)
		{
			if(m_pDlgMsgBox)
			m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
		}
		if (m_pDlgMsgBox)
		m_pDlgMsgBox->ShowWindow(SW_SHOW);
		if (m_pDlgMsgBox)
		m_pDlgMsgBox->SetFocus();
		if (m_pDlgMsgBox)
		((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
	}
	else
	{
		m_pDlgMsgBox = new CDlgMsgBox(this, strTitle, strMsg, dwDispTime, color);
		if (m_pDlgMsgBox->GetSafeHwnd() == 0)
		{
			m_pDlgMsgBox->Create();
			m_pDlgMsgBox->ShowWindow(SW_SHOW);
			m_pDlgMsgBox->SetDispMsg(strMsg, strTitle, dwDispTime, color);
			m_pDlgMsgBox->SetFocus();
			((CButton*)m_pDlgMsgBox->GetDlgItem(IDOK))->SetCheck(TRUE);
		}
	}

	m_bDispMsg = FALSE;
}

void CGvisR2R_LaserView::ClrDispMsg()
{
	OnQuitDispMsg(NULL, NULL);
}

BOOL CGvisR2R_LaserView::WaitClrDispMsg()
{
	m_bWaitClrDispMsg = TRUE;
	MSG message;

	DWORD dwMilliseconds = 0; // 100ms sec sleep
	while (WAIT_OBJECT_0 != ::WaitForSingleObject(m_evtWaitClrDispMsg, dwMilliseconds) && m_pDlgMsgBox != NULL)
	{
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	};
	Sleep(10);
	m_bWaitClrDispMsg = FALSE;
	return TRUE;
}

LONG CGvisR2R_LaserView::OnQuitDispMsg(UINT wParam, LONG lParam)
{
	if (m_pDlgMsgBox)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}

	return 0L;
}

void CGvisR2R_LaserView::ShowDlg(int nID)
{
	HideAllDlg();

	switch (nID)
	{
	case IDD_DLG_INFO:
		if (!m_pDlgInfo)
		{
			m_pDlgInfo = new CDlgInfo(this);
			if (m_pDlgInfo->GetSafeHwnd() == 0)
			{
				m_pDlgInfo->Create();
				m_pDlgInfo->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgInfo->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_FRAME_HIGH:
		if (!m_pDlgFrameHigh)
		{
			m_pDlgFrameHigh = new CDlgFrameHigh(this);
			if (m_pDlgFrameHigh->GetSafeHwnd() == 0)
			{
				m_pDlgFrameHigh->Create();
				m_pDlgFrameHigh->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgFrameHigh->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_01:
		if (!m_pDlgMenu01)
		{
			m_pDlgMenu01 = new CDlgMenu01(this);
			if (m_pDlgMenu01->GetSafeHwnd() == 0)
			{
				m_pDlgMenu01->Create();
				m_pDlgMenu01->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu01->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_02:
		if (!m_pDlgMenu02)
		{
			m_pDlgMenu02 = new CDlgMenu02(this);
			if (m_pDlgMenu02->GetSafeHwnd() == 0)
			{
				m_pDlgMenu02->Create();
				m_pDlgMenu02->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu02->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_03:
		if (!m_pDlgMenu03)
		{
			m_pDlgMenu03 = new CDlgMenu03(this);
			if (m_pDlgMenu03->GetSafeHwnd() == 0)
			{
				m_pDlgMenu03->Create();
				m_pDlgMenu03->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu03->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_04:
		if (!m_pDlgMenu04)
		{
			m_pDlgMenu04 = new CDlgMenu04(this);
			if (m_pDlgMenu04->GetSafeHwnd() == 0)
			{
				m_pDlgMenu04->Create();
				m_pDlgMenu04->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu04->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_MENU_05:
		if (!m_pDlgMenu05)
		{
			m_pDlgMenu05 = new CDlgMenu05(this);
			if (m_pDlgMenu05->GetSafeHwnd() == 0)
			{
				m_pDlgMenu05->Create();
				m_pDlgMenu05->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			m_pDlgMenu05->ShowWindow(SW_SHOW);
		}
		break;

	case IDD_DLG_UTIL_01:
		break;
	case IDD_DLG_UTIL_02:
		break;
	}
}

void CGvisR2R_LaserView::HideAllDlg()
{
	if (m_pDlgUtil01 && m_pDlgUtil01->GetSafeHwnd())
	{
		if (m_pDlgUtil01->IsWindowVisible())
			m_pDlgUtil01->ShowWindow(SW_HIDE);
	}
	if (m_pDlgInfo && m_pDlgInfo->GetSafeHwnd())
	{
		if (m_pDlgInfo->IsWindowVisible())
			m_pDlgInfo->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu01 && m_pDlgMenu01->GetSafeHwnd())
	{
		if (m_pDlgMenu01->IsWindowVisible())
			m_pDlgMenu01->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu02 && m_pDlgMenu02->GetSafeHwnd())
	{
		if (m_pDlgMenu02->IsWindowVisible())
			m_pDlgMenu02->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu03 && m_pDlgMenu03->GetSafeHwnd())
	{
		if (m_pDlgMenu03->IsWindowVisible())
			m_pDlgMenu03->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu04 && m_pDlgMenu04->GetSafeHwnd())
	{
		if (m_pDlgMenu04->IsWindowVisible())
			m_pDlgMenu04->ShowWindow(SW_HIDE);
	}
	if (m_pDlgMenu05 && m_pDlgMenu05->GetSafeHwnd())
	{
		if (m_pDlgMenu05->IsWindowVisible())
			m_pDlgMenu05->ShowWindow(SW_HIDE);
	}

}

void CGvisR2R_LaserView::DelAllDlg()
{

	if (m_pDlgUtil01 != NULL)
	{
		delete m_pDlgUtil01;
		m_pDlgUtil01 = NULL;
	}

	if (m_pDlgInfo != NULL)
	{
		delete m_pDlgInfo;
		m_pDlgInfo = NULL;
	}
	if (m_pDlgMenu05 != NULL)
	{
		delete m_pDlgMenu05;
		m_pDlgMenu05 = NULL;
	}
	if (m_pDlgMenu04 != NULL)
	{
		delete m_pDlgMenu04;
		m_pDlgMenu04 = NULL;
	}
	if (m_pDlgMenu03 != NULL)
	{
		delete m_pDlgMenu03;
		m_pDlgMenu03 = NULL;
	}
	if (m_pDlgMenu02 != NULL)
	{
		delete m_pDlgMenu02;
		m_pDlgMenu02 = NULL;
	}
	if (m_pDlgMenu01 != NULL)
	{
		delete m_pDlgMenu01;
		m_pDlgMenu01 = NULL;
	}
	if (m_pDlgFrameHigh != NULL)
	{
		delete m_pDlgFrameHigh;
		m_pDlgFrameHigh = NULL;
	}

	if (m_pDlgMsgBox != NULL)
	{
		if (m_pDlgMsgBox->GetSafeHwnd())
			m_pDlgMsgBox->DestroyWindow();
		delete m_pDlgMsgBox;
		m_pDlgMsgBox = NULL;
	}
}

LRESULT CGvisR2R_LaserView::OnDlgInfo(WPARAM wParam, LPARAM lParam)
{
	ClrDispMsg();
	CDlgInfo Dlg;
	m_pDlgInfo = &Dlg;
	Dlg.DoModal();
	m_pDlgInfo = NULL;

	if (m_pDlgMenu01)
		m_pDlgMenu01->ChkUserInfo(FALSE);

	return 0L;
}

void CGvisR2R_LaserView::DispIo()
{
}

void CGvisR2R_LaserView::DispDatabaseConnection()
{
}

void CGvisR2R_LaserView::TowerLamp(COLORREF color, BOOL bOn, BOOL bWink)
{
	if (bWink)
	{
		if (color == RGB_RED)
		{
			m_bTowerWinker[0] = bOn;
			if (bOn)
			{
				m_bTowerWinker[1] = FALSE;
				m_bTowerWinker[2] = FALSE;
			}
		}
		if (color == RGB_GREEN)
		{
			m_bTowerWinker[1] = bOn;
			if (bOn)
			{
				m_bTowerWinker[0] = FALSE;
				m_bTowerWinker[2] = FALSE;
			}
		}
		if (color == RGB_YELLOW)
		{
			m_bTowerWinker[2] = bOn;
			if (bOn)
			{
				m_bTowerWinker[1] = FALSE;
				m_bTowerWinker[0] = FALSE;
			}
		}

		if (!m_bTowerWinker[0] && !m_bTowerWinker[1] && !m_bTowerWinker[2])
			m_bTimTowerWinker = FALSE;
		else
		{
			if (!m_bTimTowerWinker)
			{
				m_bTimTowerWinker = TRUE;
				SetTimer(TIM_TOWER_WINKER, 100, NULL);
			}
		}
	}
	else
	{
		m_bTimTowerWinker = FALSE;

#ifdef USE_MPE
		if (!pDoc->m_pMpeIo)
			return;
#endif
	}
}

void CGvisR2R_LaserView::DispTowerWinker()
{
	m_nCntTowerWinker++;
	if (m_nCntTowerWinker > LAMP_DELAY)
	{
		m_nCntTowerWinker = 0;
	}

}

void CGvisR2R_LaserView::BuzzerFromThread(BOOL bOn, int nCh)
{
	m_bBuzzerFromThread = TRUE;
}

void CGvisR2R_LaserView::Buzzer(BOOL bOn, int nCh)
{
	//	return; // PLC����
#ifdef USE_MPE
	if (!m_pMpe)
		return;

	if (!bOn)
	{
		switch (nCh)
		{
		case 0:
			//IoWrite(_T("MB44015E"), 0); // ����1 On  (PC�� ON, OFF) - 20141020
			if (m_pMpe)
				m_pMpe->Write(_T("MB44015E"), 0);
			break;
		case 1:
			//IoWrite(_T("MB44015F"), 0); // ����2 On  (PC�� ON, OFF) - 20141020
			if (m_pMpe)
				m_pMpe->Write(_T("MB44015F"), 0);
			break;
		}
	}
	else
	{
		switch (nCh)
		{
		case 0:
			//IoWrite(_T("MB44015E"), 1); // ����1 On  (PC�� ON, OFF) - 20141020
			pView->m_pMpe->Write(_T("MB44015E"), 0);
			Sleep(300);
			pView->m_pMpe->Write(_T("MB44015E"), 1);
			break;
		case 1:
			//IoWrite(_T("MB44015F"), 1); // ����2 On  (PC�� ON, OFF) - 20141020
			pView->m_pMpe->Write(_T("MB44015E"), 0);
			Sleep(300);
			pView->m_pMpe->Write(_T("MB44015F"), 1);
			break;
		}
	}
#endif
}



void CGvisR2R_LaserView::ThreadInit()
{
	if (!m_bThread[0])
		m_Thread[0].Start(GetSafeHwnd(), this, ThreadProc0);
}

void CGvisR2R_LaserView::ThreadKill()
{
	if (m_bThread[0])
	{
		m_Thread[0].Stop();
		Sleep(20);
		while (m_bThread[0])
		{
			Sleep(20);
		}
	}
	if (m_bThread[1])
	{
		m_Thread[1].Stop();
		Sleep(20);
		while (m_bThread[1])
		{
			Sleep(20);
		}
	}
	if (m_bThread[2])
	{
		m_Thread[2].Stop();
		Sleep(20);
		while (m_bThread[2])
		{
			Sleep(20);
		}
	}
}

UINT CGvisR2R_LaserView::ThreadProc0(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_LaserView* pThread = reinterpret_cast<CGvisR2R_LaserView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[0] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[0].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[0] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;
			pThread->GetCurrentInfoSignal();
			bLock = FALSE;
		}
		Sleep(100);
	}
	pThread->m_bThread[0] = FALSE;

	return 0;
}

UINT CGvisR2R_LaserView::ThreadProc1(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_LaserView* pThread = reinterpret_cast<CGvisR2R_LaserView*>(lpContext);

	BOOL bLock = FALSE, bEStop = FALSE, bCollision = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[1] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[1].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[1] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;
			pThread->GetEnc();
			bLock = FALSE;
		}
		Sleep(100);
	}
	pThread->m_bThread[1] = FALSE;

	return 0;
}

UINT CGvisR2R_LaserView::ThreadProc2(LPVOID lpContext)
{
	// Turn the passed in 'this' pointer back into a CProgressMgr instance
	CGvisR2R_LaserView* pThread = reinterpret_cast<CGvisR2R_LaserView*>(lpContext);

	BOOL bLock = FALSE;
	DWORD dwTick = GetTickCount();
	DWORD dwShutdownEventCheckPeriod = 0; // thread shutdown event check period

	pThread->m_bThread[2] = TRUE;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThread->m_Thread[2].GetShutdownEvent(), dwShutdownEventCheckPeriod))
	{
		pThread->m_dwThreadTick[2] = GetTickCount() - dwTick;
		dwTick = GetTickCount();

		if (!bLock)
		{
			bLock = TRUE;

			if (pThread->m_bTHREAD_DISP_DEF)
			{
				pThread->DispDefImg();
				pThread->UpdateLotTime();
				Sleep(0);
			}
			else
				Sleep(30);

			bLock = FALSE;
		}
	}
	pThread->m_bThread[2] = FALSE;

	return 0;
}


void CGvisR2R_LaserView::DispStsMainMsg(int nIdx)
{
	CString str;
	str = m_sDispMsg[nIdx];
	pFrm->DispStatusBar(str, nIdx);
}

void CGvisR2R_LaserView::SwJog(int nAxisID, int nDir, BOOL bOn)
{
	if (!pView->m_pMotion)// || pDoc->Status.bAuto)
		return;

	double fVel, fAcc, fJerk;
	double dStep;
	if (pDoc->Status.bSwJogFast)
		dStep = 0.5;
	else
		dStep = 0.1;

	if (pDoc->Status.bSwJogStep)
	{
		if (bOn)
		{
			double dPos = pView->m_dEnc[nAxisID]; // m_pMotion->GetActualPosition(nAxisID);
			if (nDir == M_CW)
				dPos += dStep;
			else if (nDir == M_CCW)
				dPos -= dStep;

			if (nAxisID == AXIS_X0)
			{
				if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
				{
					if (nDir == M_CW) // �� Jog ��ư.
						return;
				}

				if (m_pMotion->IsLimit(MS_X0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_X0, dPos, fVel, fAcc, fAcc);
			}
			else if (nAxisID == AXIS_Y0)
			{
				if (m_pMotion->IsLimit(MS_Y0, nDir))
					return;
				m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y0, dStep, fVel, fAcc, fJerk);
				m_pMotion->Move(MS_Y0, dPos, fVel, fAcc, fAcc);
			}
			//else if (nAxisID == AXIS_X1)
			//{
			//	if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			//	{
			//		if (nDir == M_CW) // �� Jog ��ư.
			//			return;
			//	}

			//	if (m_pMotion->IsLimit(MS_X1, nDir))
			//		return;
			//	m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X1, dStep, fVel, fAcc, fJerk);
			//	m_pMotion->Move(MS_X1, dPos, fVel, fAcc, fAcc);
			//}
			//else if (nAxisID == AXIS_Y1)
			//{
			//	if (m_pMotion->IsLimit(MS_Y1, nDir))
			//		return;
			//	m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_Y1, dStep, fVel, fAcc, fJerk);
			//	m_pMotion->Move(MS_Y1, dPos, fVel, fAcc, fAcc);
			//}
		}
	}
	else	// Jog Mode
	{
		if (!m_pDlgMenu02)
			return;

		if (nAxisID == AXIS_Y0)
		{
			if (nDir == M_CCW)		// Up
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP);
			}
			else if (nDir == M_CW)	// Dn
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN);
			}
		}
		else if (nAxisID == AXIS_X0)
		{
			if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
			{
				if (nDir == M_CW) // �� Jog ��ư.
					return;
			}

			if (nDir == M_CW)		// Rt
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT);
			}
			else if (nDir == M_CCW)	// Lf
			{
				if (bOn)
					m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF);
				else
					m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF);
			}
		}
		//else if (nAxisID == AXIS_Y1)
		//{
		//	if (nDir == M_CCW)		// Up
		//	{
		//		if (bOn)
		//			m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_UP2);
		//		else
		//			m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_UP2);
		//	}
		//	else if (nDir == M_CW)	// Dn
		//	{
		//		if (bOn)
		//			m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_DN2);
		//		else
		//			m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_DN2);
		//	}
		//}
		//else if (nAxisID == AXIS_X1)
		//{
		//	if (m_bAuto && m_bTHREAD_MK[0] && m_bTHREAD_MK[1] && IsReview())
		//	{
		//		if (nDir == M_CW) // �� Jog ��ư.
		//			return;
		//	}

		//	if (nDir == M_CW)		// Rt
		//	{
		//		if (bOn)
		//			m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_RT2);
		//		else
		//			m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_RT2);
		//	}
		//	else if (nDir == M_CCW)	// Lf
		//	{
		//		if (bOn)
		//			m_pDlgMenu02->SwMyBtnDown(IDC_BTN_JOG_LF2);
		//		else
		//			m_pDlgMenu02->SwMyBtnUp(IDC_BTN_JOG_LF2);
		//	}
		//}
	}
}


BOOL CGvisR2R_LaserView::ChkShareIdx(int *pBufSerial, int nBufTot, int nShareSerial)
{
	if (nBufTot < 1)
		return TRUE;
	for (int i = 0; i < nBufTot; i++)
	{
		if (pBufSerial[i] == nShareSerial)
			return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkVsShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkVsShareUp(nS0);
	BOOL b1 = ChkVsShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkVsShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkVsShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkShare(int &nSerial)
{
	int nS0, nS1;
	BOOL b0 = ChkShareUp(nS0);
	BOOL b1 = ChkShareDn(nS1);

	if (!b0 || !b1)
	{
		nSerial = -1;
		return FALSE;
	}
	else if (nS0 != nS1)
	{
		nSerial = -1;
		return FALSE;
	}

	nSerial = nS0;
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkShareUp(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareUp + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkShareDn(int &nSerial)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsShareDn + _T("*.pcr"));
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

void CGvisR2R_LaserView::ChkShare()
{
	ChkShareUp();
	ChkShareDn();
}

void CGvisR2R_LaserView::ChkShareUp()
{
	CString str;
	int nSerial;
	if (ChkShareUp(nSerial))
	{
		str.Format(_T("US: %d"), nSerial);
		pDoc->Status.PcrShare[0].bExist = TRUE;
		pDoc->Status.PcrShare[0].nSerial = nSerial;
	}
	else
	{
		pDoc->Status.PcrShare[0].bExist = FALSE;
		pDoc->Status.PcrShare[0].nSerial = -1;
		str.Format(_T("US: "));
	}
	if (pFrm)
	{
		if (m_sShare[0] != str)
		{
			m_sShare[0] = str;
			pFrm->DispStatusBar(str, 4);
		}
	}
}

void CGvisR2R_LaserView::ChkShareDn()
{
	CString str;
	int nSerial;
	if (ChkShareDn(nSerial))
	{
		str.Format(_T("DS: %d"), nSerial);
		pDoc->Status.PcrShare[1].bExist = TRUE;
		pDoc->Status.PcrShare[1].nSerial = nSerial;
	}
	else
	{
		pDoc->Status.PcrShare[1].bExist = FALSE;
		pDoc->Status.PcrShare[1].nSerial = -1;
		str.Format(_T("DS: "));
	}
	if (pFrm)
	{
		if (m_sShare[1] != str)
		{
			m_sShare[1] = str;
			pFrm->DispStatusBar(str, 2);
		}
	}
}

BOOL CGvisR2R_LaserView::ChkBufIdx(int* pSerial, int nTot)
{
	if (nTot < 2)
		return TRUE;

	for (int i = 0; i < (nTot - 1); i++)
	{
		if (pSerial[i + 1] != pSerial[i] + 1)
			return FALSE;
	}
	return TRUE;
}

void CGvisR2R_LaserView::SwapUp(__int64 *num1, __int64 *num2) 	// ��ġ �ٲٴ� �Լ�
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CGvisR2R_LaserView::SortingInUp(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("�Ͻ����� - Failed getting information."));
		//MsgBox(sMsg);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
		nPos = sFileName.ReverseFind('.');
		if (nPos > 0)
		{
			sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

		nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_mtime);
		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
		//m_nBufSerialSorting[0][nIndex] = _tstoi(sFileName);

		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[0][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::SortingOutUp(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// ���� ���� �ҽ� ����
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[0][i] > m_nBufSerialSorting[0][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[0][i + 1], &m_nBufSerialSorting[0][i]);
			}
		}
	}									// ���� ���� �ҽ� ��

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[0][i] % 10000);
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkBufUp(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufUp + _T("*.pcr"));
	if (!bExist)
	{
		pDoc->m_bBufEmpty[0] = TRUE;
		if (!pDoc->m_bBufEmptyF[0])
			pDoc->m_bBufEmptyF[0] = TRUE;		// ���� �ѹ� ���۰� ���������(�ʱ�ȭ�� �ϰ� �� ����) TRUE.

		return FALSE; // pcr������ �������� ����.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	CString sNewName;

	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();
			//nPos = sFileName.ReverseFind('.');
			//if (nPos > 0)
			//	sSerial = sFileName.Left(nPos);

			//nSerial = _tstoi(sSerial);
			//if (nSerial > 0)
			//{
			//	pSerial[nTot] = nSerial;
			//	nTot++;
			//}

			if (!SortingInUp(pDoc->WorkingInfo.System.sPathVrsBufUp + sFileName, nTot))
				return FALSE;

				nTot++;
			}
		}

	BOOL bRtn = SortingOutUp(pSerial, nTot);

	if (nTot == 0)
		pDoc->m_bBufEmpty[0] = TRUE;
	else
		pDoc->m_bBufEmpty[0] = FALSE;

	return (bRtn);
	//return TRUE;
}

BOOL CGvisR2R_LaserView::ChkBufDn(int* pSerial, int &nTot)
{
	CFileFind cFile;
	BOOL bExist = cFile.FindFile(pDoc->WorkingInfo.System.sPathVrsBufDn + _T("*.pcr"));
	if (!bExist)
	{
		pDoc->m_bBufEmpty[1] = TRUE;
		if (!pDoc->m_bBufEmptyF[1])
			pDoc->m_bBufEmptyF[1] = TRUE;
		return FALSE; // pcr������ �������� ����.
	}

	int nPos, nSerial;

	CString sFileName, sSerial;
	nTot = 0;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			sFileName = cFile.GetFileName();
			//nPos = sFileName.ReverseFind('.');
			//if (nPos > 0)
			//	sSerial = sFileName.Left(nPos);

			//nSerial = _tstoi(sSerial);
			//if (nSerial > 0)
			//{
			//	pSerial[nTot] = nSerial;
			//	nTot++;
			//}

			if (!SortingInDn(pDoc->WorkingInfo.System.sPathVrsBufDn + sFileName, nTot))
				return FALSE;

			nTot++;
		}
	}


	BOOL bRtn = SortingOutDn(pSerial, nTot);

	if (nTot == 0)
		pDoc->m_bBufEmpty[1] = TRUE;
	else
		pDoc->m_bBufEmpty[1] = FALSE;

	return (bRtn);
	//return TRUE;
}

void CGvisR2R_LaserView::SwapDn(__int64 *num1, __int64 *num2) 	// ��ġ �ٲٴ� �Լ�
{
	__int64 temp;

	temp = *num2;
	*num2 = *num1;
	*num1 = temp;
}

BOOL CGvisR2R_LaserView::SortingInDn(CString sPath, int nIndex)
{
	struct _stat buf;
	struct tm *t;

	CString sMsg, sFileName, sSerial;
	int nPos, nSerial;
	char filename[MAX_PATH];
	StringToChar(sPath, filename);

	if (_stat(filename, &buf) != 0)
	{
		sMsg.Format(_T("�Ͻ����� - Failed getting information."));
		//MsgBox(sMsg);
		pView->ClrDispMsg();
		AfxMessageBox(sMsg);
		return FALSE;
	}
	else
	{
		sFileName = sPath;
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
		{
				sSerial = sFileName.Left(nPos);
			sSerial = sSerial.Right(4);
		}

			nSerial = _tstoi(sSerial);

		t = localtime(&buf.st_mtime);
		//sFileName.Format(_T("%04d%02d%02d%02d%02d%02d%04d"), t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
		//														t->tm_hour, t->tm_min, t->tm_sec, nSerial);
		//m_nBufSerialSorting[1][nIndex] = _tstoi(sFileName);


		CString sYear, sMonth, sDay, sHour, sMin, sSec;
		sYear.Format(_T("%04d"), t->tm_year + 1900);
		sMonth.Format(_T("%02d"), t->tm_mon + 1);
		sDay.Format(_T("%02d"), t->tm_mday);
		sHour.Format(_T("%02d"), t->tm_hour);
		sMin.Format(_T("%02d"), t->tm_min);
		sSec.Format(_T("%02d"), t->tm_sec);

		__int64 nYear = _tstoi(sYear);
		__int64 nMonth = _tstoi(sMonth);
		__int64 nDay = _tstoi(sDay);
		__int64 nHour = _tstoi(sHour);
		__int64 nMin = _tstoi(sMin);
		__int64 nSec = _tstoi(sSec);

		m_nBufSerialSorting[1][nIndex] = nYear * 100000000000000 + nMonth * 1000000000000 + nDay * 10000000000 +
			nHour * 100000000 + nMin * 1000000 + nSec * 10000 + nSerial;

			}

	return TRUE;
}

BOOL CGvisR2R_LaserView::SortingOutDn(int* pSerial, int nTot)
{
	int i, k;

	for (k = 0; k < nTot; k++) 			// ���� ���� �ҽ� ����
	{
		for (i = 0; i < (nTot - 1) - k; i++)
		{

			if (m_nBufSerialSorting[1][i] > m_nBufSerialSorting[1][i + 1])
			{
				SwapUp(&m_nBufSerialSorting[1][i + 1], &m_nBufSerialSorting[1][i]);
		}
	}
	}									// ���� ���� �ҽ� ��

	for (i = 0; i < nTot; i++)
	{
		pSerial[i] = (int)(m_nBufSerialSorting[1][i] % 10000);
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkYield() // ���� ��ȣ : TRUE , ���� �ҷ� : FALSE
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nGood = 0, nBad = 0, nTot;
	double dRatio;
	CString sMsg;

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->GetPcsNum(nGood, nBad);
		else
		{
			Stop();
			sMsg.Format(_T("�Ͻ����� - Failed ChkYield()."));
			MsgBox(sMsg);
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			return FALSE;
		}
	}
	else
	{
		if (pDoc->m_pReelMapUp)
			pDoc->m_pReelMapUp->GetPcsNum(nGood, nBad);
		else
		{
			Stop();
			sMsg.Format(_T("�Ͻ����� - Failed ChkYield()."));
			MsgBox(sMsg);
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			return FALSE;
		}
	}

	nTot = (nGood + nBad);
	double dTotLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtTotYld);
	if (dTotLmt > 0.0)
	{
		if (nTot > 0)
			dRatio = ((double)nGood / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		if (dRatio < dTotLmt)
		{
			Stop();
			sMsg.Format(_T("�Ͻ����� - ��ü ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dTotLmt, dRatio);
			MsgBox(sMsg);
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			return FALSE;
		}
	}

	// 	int nNodeX = pDoc->m_pPcsRgn->nCol;
	// 	int nNodeY = pDoc->m_pPcsRgn->nRow;
	// 	nTot = (nNodeX*nNodeY);
	// 	int nSerial = m_pDlgMenu01->GetCurSerial();
	// 	int nIdx = pDoc->GetPcrIdx(nSerial);
	// 	int nDef = pDoc->m_pPcr[nIdx]->m_nTotRealDef;

	double dPrtLmt = _tstof(pDoc->WorkingInfo.LastJob.sLmtPatlYld);
	if (dPrtLmt > 0.0)
	{
		// 		if(nDef > 0)
		// 			dRatio = ((double)(nTot-nDef)/(double)nTot) * 100.0;
		// 		else
		// 			dRatio = 0.0;

		if (bDualTest)
		{
			if (pDoc->m_Yield[2].IsValid())
			{
				dRatio = pDoc->m_Yield[2].GetYield();

				if (dRatio < dPrtLmt)
				{
					Stop();
					sMsg.Format(_T("�Ͻ����� - ���� ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dPrtLmt, dRatio);
					MsgBox(sMsg);
					TowerLamp(RGB_RED, TRUE);
					Buzzer(TRUE, 0);
					return FALSE;
				}
			}
		}
		else
		{
			if (pDoc->m_Yield[0].IsValid())
			{
				dRatio = pDoc->m_Yield[0].GetYield();

				if (dRatio < dPrtLmt)
				{
					Stop();
					sMsg.Format(_T("�Ͻ����� - ���� ���� ���ѹ��� : %.1f �̴� ( %.1f )"), dPrtLmt, dRatio);
					MsgBox(sMsg);
					TowerLamp(RGB_RED, TRUE);
					Buzzer(TRUE, 0);
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkSaftySen() // ���� : TRUE , ���� : FALSE
{
	if (pDoc->WorkingInfo.LastJob.bMkSftySen)
	{
		if (pDoc->Status.bSensSaftyMk && !pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = TRUE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-4"), 0);
			DispMain(_T("�� ��"), RGB_RED);			
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
			m_bTIM_SAFTY_STOP = TRUE;//MsgBox(_T("�Ͻ����� - ��ŷ�� ���������� �����Ǿ����ϴ�."));
			SetTimer(TIM_SAFTY_STOP, 100, NULL);
		}
		else if (!pDoc->Status.bSensSaftyMk && pDoc->Status.bSensSaftyMkF)
		{
			pDoc->Status.bSensSaftyMkF = FALSE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
	}

	return (pDoc->Status.bSensSaftyMk);
}

void CGvisR2R_LaserView::ResetMotion()
{
	for (int i = 0; i < MAX_MS; i++)
	{
		ResetMotion(i);

		if (i < MAX_AXIS)
		{
			while (!m_pMotion->IsServoOn(i))
			{
				if (i == MS_X0 || i == MS_Y0)
					pView->m_pMotion->Clear(MS_X0Y0);
				else if (i == MS_X1 || i == MS_Y1)
					pView->m_pMotion->Clear(MS_X1Y1);
				else
					pView->m_pMotion->Clear(i);
				// 				for(int k=0; k<MAX_MS; k++)
				// 					m_pMotion->Clear(k);
				Sleep(30);
				m_pMotion->ServoOnOff(i, TRUE);
				Sleep(30);
			}
		}
	}
}

void CGvisR2R_LaserView::ResetMotion(int nMsId)
{
	if (m_pDlgMenu02)
		m_pDlgMenu02->ResetMotion(nMsId);
}

unsigned long CGvisR2R_LaserView::ChkDoor() // 0: All Closed , Open Door Index : Doesn't all closed. (Bit3: F, Bit2: L, Bit1: R, Bit0; B)
{
	unsigned long ulOpenDoor = 0;

	if (pDoc->WorkingInfo.LastJob.bAoiUpDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-5"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-6"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
	{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-7"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-8"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-9"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_UP] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_UP] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-10"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bAoiDnDrSen)
	{
		if (pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 0);
			pDoc->Status.bDoorAoiF[DOOR_FM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-5"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� �߾� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 1);
			pDoc->Status.bDoorAoiF[DOOR_FL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			//pView->DispStsBar(_T("����-6"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_FR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 2);
			pDoc->Status.bDoorAoiF[DOOR_FR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-7"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BM_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 3);
			pDoc->Status.bDoorAoiF[DOOR_BM_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-8"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� �߾� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BL_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 4);
			pDoc->Status.bDoorAoiF[DOOR_BL_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-9"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && !pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor |= (0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorAoi[DOOR_BR_AOI_DN] && pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN])
		{
			ulOpenDoor &= ~(0x01 << 5);
			pDoc->Status.bDoorAoiF[DOOR_BR_AOI_DN] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-10"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �˻�� �� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bMkDrSen)
	{
		if (pDoc->Status.bDoorMk[DOOR_FL_MK] && !pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FL_MK] && pDoc->Status.bDoorMkF[DOOR_FL_MK])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorMkF[DOOR_FL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-11"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorMk[DOOR_FR_MK] && !pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_FR_MK] && pDoc->Status.bDoorMkF[DOOR_FR_MK])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorMkF[DOOR_FR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-12"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorMk[DOOR_BL_MK] && !pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BL_MK] && pDoc->Status.bDoorMkF[DOOR_BL_MK])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorMkF[DOOR_BL_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-13"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorMk[DOOR_BR_MK] && !pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorMk[DOOR_BR_MK] && pDoc->Status.bDoorMkF[DOOR_BR_MK])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorMkF[DOOR_BR_MK] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-14"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ��ŷ�� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bEngvDrSen)
	{
		if (pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor |= (0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 6);
			pDoc->Status.bDoorEngvF[DOOR_FL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-11"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor |= (0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_FR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_FR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 7);
			pDoc->Status.bDoorEngvF[DOOR_FR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-12"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor |= (0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BL_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BL_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 8);
			pDoc->Status.bDoorEngvF[DOOR_BL_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-13"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && !pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor |= (0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorEngv[DOOR_BR_ENGV] && pDoc->Status.bDoorEngvF[DOOR_BR_ENGV])
		{
			ulOpenDoor &= ~(0x01 << 9);
			pDoc->Status.bDoorEngvF[DOOR_BR_ENGV] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-14"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - ���κ� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bUclDrSen)
	{
		if (pDoc->Status.bDoorUc[DOOR_FL_UC] && !pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor |= (0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FL_UC] && pDoc->Status.bDoorUcF[DOOR_FL_UC])
		{
			ulOpenDoor &= ~(0x01 << 10);
			pDoc->Status.bDoorUcF[DOOR_FL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-15"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorUc[DOOR_FR_UC] && !pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor |= (0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_FR_UC] && pDoc->Status.bDoorUcF[DOOR_FR_UC])
		{
			ulOpenDoor &= ~(0x01 << 11);
			pDoc->Status.bDoorUcF[DOOR_FR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-16"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorUc[DOOR_BL_UC] && !pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor |= (0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BL_UC] && pDoc->Status.bDoorUcF[DOOR_BL_UC])
		{
			ulOpenDoor &= ~(0x01 << 12);
			pDoc->Status.bDoorUcF[DOOR_BL_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-17"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorUc[DOOR_BR_UC] && !pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor |= (0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorUc[DOOR_BR_UC] && pDoc->Status.bDoorUcF[DOOR_BR_UC])
		{
			ulOpenDoor &= ~(0x01 << 13);
			pDoc->Status.bDoorUcF[DOOR_BR_UC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-18"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	if (pDoc->WorkingInfo.LastJob.bRclDrSen)
	{
		// 		if(pDoc->Status.bDoorRe[DOOR_FL_RC] && !pDoc->Status.bDoorReF[DOOR_FL_RC])
		// 		{
		// 			ulOpenDoor |= (0x01<<14);
		// 			pDoc->Status.bDoorReF[DOOR_FL_RC] = TRUE;
		// 			Stop();
		// 			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
		// 			TowerLamp(RGB_RED, TRUE);
		// 			Buzzer(TRUE, 0);
		// 		}
		// 		else if(!pDoc->Status.bDoorRe[DOOR_FL_RC] && pDoc->Status.bDoorReF[DOOR_FL_RC])
		// 		{
		// 			ulOpenDoor &= ~(0x01<<14);
		// 			pDoc->Status.bDoorReF[DOOR_FL_RC] = FALSE;
		// 			Buzzer(FALSE, 0);
		// 			Sleep(300);
		// 		}

		if (pDoc->Status.bDoorRe[DOOR_FR_RC] && !pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor |= (0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_FR_RC] && pDoc->Status.bDoorReF[DOOR_FR_RC])
		{
			ulOpenDoor &= ~(0x01 << 15);
			pDoc->Status.bDoorReF[DOOR_FR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-19"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorRe[DOOR_S_RC] && !pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor |= (0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_S_RC] && pDoc->Status.bDoorReF[DOOR_S_RC])
		{
			ulOpenDoor &= ~(0x01 << 16);
			pDoc->Status.bDoorReF[DOOR_S_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-20"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorRe[DOOR_BL_RC] && !pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor |= (0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BL_RC] && pDoc->Status.bDoorReF[DOOR_BL_RC])
		{
			ulOpenDoor &= ~(0x01 << 17);
			pDoc->Status.bDoorReF[DOOR_BL_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-21"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}

		if (pDoc->Status.bDoorRe[DOOR_BR_RC] && !pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor |= (0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = TRUE;
			Buzzer(FALSE, 0);
			Sleep(300);
		}
		else if (!pDoc->Status.bDoorRe[DOOR_BR_RC] && pDoc->Status.bDoorReF[DOOR_BR_RC])
		{
			ulOpenDoor &= ~(0x01 << 18);
			pDoc->Status.bDoorReF[DOOR_BR_RC] = FALSE;
			m_bSwStopNow = TRUE;
			m_bSwRunF = FALSE;
			Stop();
			pView->DispStsBar(_T("����-22"), 0);
			DispMain(_T("�� ��"), RGB_RED);
			MsgBox(_T("�Ͻ����� - �����Ϸ��� �ĸ� ���� ���� Open"));
			TowerLamp(RGB_RED, TRUE);
			Buzzer(TRUE, 0);
		}
	}

	return ulOpenDoor;
}

void CGvisR2R_LaserView::ChkEmg()
{
	if (pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-23"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_F_AOI_UP])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-24"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_UP] && pDoc->Status.bEmgAoiF[EMG_B_AOI_UP])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_UP] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-23"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� ���� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_F_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_F_AOI_DN])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_F_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && !pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-24"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �˻�� �� �ĸ� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgAoi[EMG_B_AOI_DN] && pDoc->Status.bEmgAoiF[EMG_B_AOI_DN])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgAoiF[EMG_B_AOI_DN] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_M_MK] && !pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgMkF[EMG_M_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-25"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ��ŷ�� ���� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgMk[EMG_M_MK] && pDoc->Status.bEmgMkF[EMG_M_MK])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgMkF[EMG_M_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgMk[EMG_B_MK] && !pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgMkF[EMG_B_MK] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-26"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ��ŷ�� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgMk[EMG_B_MK] && pDoc->Status.bEmgMkF[EMG_B_MK])
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgMkF[EMG_B_MK] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgUc && !pDoc->Status.bEmgUcF)
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgUcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-27"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgUc && pDoc->Status.bEmgUcF)
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgUcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgRc && !pDoc->Status.bEmgRcF)
	{
		//SwAoiEmg(TRUE);
		pDoc->Status.bEmgRcF = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-28"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - �����Ϸ��� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgRc && pDoc->Status.bEmgRcF)
	{
		//SwAoiEmg(FALSE);
		pDoc->Status.bEmgRcF = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[0] && !pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-29"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ���κ� �����"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgEngv[0] && pDoc->Status.bEmgEngvF[0])
	{
		pDoc->Status.bEmgEngvF[0] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}

	if (pDoc->Status.bEmgEngv[1] && !pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = TRUE;
		m_bSwStopNow = TRUE;
		m_bSwRunF = FALSE;
		Stop();
		pView->DispStsBar(_T("����-30"), 0);
		DispMain(_T("�� ��"), RGB_RED);
		MsgBox(_T("������� - ���κ� ����ġ"));
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
	}
	else if (!pDoc->Status.bEmgEngv[1] && pDoc->Status.bEmgEngvF[1])
	{
		pDoc->Status.bEmgEngvF[1] = FALSE;
		Buzzer(FALSE, 0);
		Sleep(300);
		ResetMotion();
	}
}

int CGvisR2R_LaserView::ChkSerial() // 0: Continue, -: Previous, +: Discontinue
{
	int nSerial0 = GetBuffer0();
	int nSerial1 = GetBuffer1();
	int nLastShot = pDoc->GetLastShotMk();

	if (nSerial0 == nLastShot + 1 || nSerial1 == nLastShot + 2)
		return 0;
	if (pDoc->WorkingInfo.LastJob.bLotSep || pDoc->m_bDoneChgLot)
	{
		if (nLastShot == pDoc->m_nLotLastShot && (nSerial0 == 1 || nSerial1 == 2))
			return 0;
	}
	if (m_bLastProc && nSerial0 == m_nLotEndSerial)
		return 0;

	return (nSerial0 - nLastShot);
}

void CGvisR2R_LaserView::ChkBuf()
{
	ChkBufUp();
	ChkBufDn();
}

void CGvisR2R_LaserView::DoIO()
{
	MonPlcAlm();
	MonMsgBox();
	MonDispMain();

	if (m_bCycleStop)
	{
		m_bCycleStop = FALSE;
		if (pDoc->m_sAlmMsg != pDoc->m_sIsAlmMsg)
		{
			pDoc->m_sIsAlmMsg = pDoc->m_sAlmMsg;
			MsgBox(pDoc->m_sAlmMsg);
			pDoc->m_sAlmMsg = _T("");
			pDoc->m_sIsAlmMsg = _T("");
			pDoc->m_sPrevAlmMsg = _T("");
		}
		else if (pDoc->m_sMsgBox != pDoc->m_sIsMsgBox)
		{
			pDoc->m_sIsMsgBox = pDoc->m_sMsgBox;
			MsgBox(pDoc->m_sMsgBox, 0, pDoc->m_nTypeMsgBox);
			pDoc->m_sMsgBox = _T("");
			pDoc->m_sIsMsgBox = _T("");
			pDoc->m_sPrevMsgBox = _T("");
		}

		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}

	if (pDoc->Status.bAuto)
		DoAutoEng();

	if (IsRun())
	{
		if (m_pDlgMenu01)
		{
			if (m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(FALSE);
		}
	}
	else
	{
		if (m_pDlgMenu01)
		{
			if (!m_pDlgMenu01->IsEnableBtn())
				m_pDlgMenu01->EnableBtn(TRUE);
		}
	}
}

void CGvisR2R_LaserView::DoSens()
{
}

void CGvisR2R_LaserView::DoBoxSw()
{
}

void CGvisR2R_LaserView::DoEmgSw()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)// || !pDoc->m_pMpeIoF)
		return;
#endif
}

void CGvisR2R_LaserView::GetEnc()
{
	if (!m_pMotion)
		return;

	m_dEnc[AXIS_X0] = m_pMotion->GetActualPosition(AXIS_X0);
	m_dEnc[AXIS_Y0] = m_pMotion->GetActualPosition(AXIS_Y0);
}

void CGvisR2R_LaserView::ChkMRegOut()
{
	m_bChkMpeIoOut = TRUE;
}

void CGvisR2R_LaserView::GetMpeIO()
{
}

void CGvisR2R_LaserView::GetMpeSignal()
{
}

void CGvisR2R_LaserView::GetMpeData()
{
}

int CGvisR2R_LaserView::GetTime(int nSel)
{

	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	switch (nSel)
	{
	case YEAR:
		return Tim.GetYear();
	case MONTH:
		return Tim.GetMonth();
	case DAY:
		return Tim.GetDay();
	case HOUR:
		return Tim.GetHour();
	case MINUTE:
		return Tim.GetMinute();
	case SEC:
		return Tim.GetSecond();
	}

	return 0;
}

CString CGvisR2R_LaserView::GetTime()
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	int nHour = Tim.GetHour();
	int nMinute = Tim.GetMinute();
	int nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

CString CGvisR2R_LaserView::GetTime(stLotTime &LotTime)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	LotTime.nYear = Tim.GetYear();
	LotTime.nMonth = Tim.GetMonth();
	LotTime.nDay = Tim.GetDay();
	LotTime.nHour = Tim.GetHour();
	LotTime.nMin = Tim.GetMinute();
	LotTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), LotTime.nYear, LotTime.nMonth, LotTime.nDay,
		LotTime.nHour, LotTime.nMin, LotTime.nSec);
	return strVal;
}


CString CGvisR2R_LaserView::GetTime(int &nHour, int &nMinute, int &nSec)
{
	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);		// Get the current time from the 
							// operating system.
	CTime Tim(osBinTime);

	int nYear = Tim.GetYear();
	int nMonth = Tim.GetMonth();
	int nDay = Tim.GetDay();
	nHour = Tim.GetHour();
	nMinute = Tim.GetMinute();
	nSec = Tim.GetSecond();

	strVal.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMinute, nSec);
	return strVal;
}

void CGvisR2R_LaserView::DispTime()
{
	stLotTime LotTime;
	CString str;
	str = GetTime(LotTime);
	if (m_sDispTime != str)
	{
		m_sDispTime = str;
		pFrm->DispStatusBar(str, 7);

		pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
		pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
		pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
		pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
		pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
		pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

		if (m_pDlgMenu01)
			m_pDlgMenu01->DispRunTime();
	}
}

// System Input IO .......................................................................
void CGvisR2R_LaserView::DoSaftySens()
{
}

void CGvisR2R_LaserView::DoDoorSens()
{
}

void CGvisR2R_LaserView::DoModeSel()
{
}

void CGvisR2R_LaserView::DoMainSw()
{
}

void CGvisR2R_LaserView::DoMkSens()
{
}

void CGvisR2R_LaserView::DoAoiBoxSw()
{
}

void CGvisR2R_LaserView::DoEmgSens()
{
}

void CGvisR2R_LaserView::DoSignal()
{
}

void CGvisR2R_LaserView::DoUcBoxSw()
{
}

void CGvisR2R_LaserView::DoUcSens1()
{
}

void CGvisR2R_LaserView::DoUcSens2()
{
}

void CGvisR2R_LaserView::DoUcSens3()
{
}

void CGvisR2R_LaserView::DoRcBoxSw()
{
}

void CGvisR2R_LaserView::DoRcSens1()
{
}

void CGvisR2R_LaserView::DoRcSens3()
{
}

void CGvisR2R_LaserView::SetAoiFdPitch(double dPitch)
{
	pDoc->SetAoiFdPitch(dPitch);
	// 	m_pMotion->SetLeadPitch(AXIS_AOIFD, dPitch);
	long lData = long(dPitch*1000.0);
#ifdef USE_MPE
	pView->m_pMpe->Write(_T("ML45012"), lData); // �˻�� Feeding �ѷ� Lead Pitch
	pView->m_pMpe->Write(_T("ML45020"), lData); // ���κ� Feeding �ѷ� Lead Pitch
#endif
}

void CGvisR2R_LaserView::SetMkFdPitch(double dPitch)
{
	pDoc->SetMkFdPitch(dPitch);
}

void CGvisR2R_LaserView::SetBufInitPos(double dPos)
{
}

void CGvisR2R_LaserView::SetBufHomeParam(double dVel, double dAcc)
{
	long lVel = long(dVel*1000.0);
	long lAcc = long(dAcc*1000.0);
}

LRESULT CGvisR2R_LaserView::OnBufThreadDone(WPARAM wPara, LPARAM lPara)
{
	return 0L;
}

//.........................................................................................

BOOL CGvisR2R_LaserView::WatiDispMain(int nDelay)
{
	if (m_nWatiDispMain % nDelay)
	{
		m_nWatiDispMain++;
		return TRUE;
	}

	m_nWatiDispMain = 0;
	m_nWatiDispMain++;
	return FALSE;
}

void CGvisR2R_LaserView::DispMain(CString sMsg, COLORREF rgb)
{
	m_sMonDisp = sMsg;

	m_csDispMain.Lock();
	m_bDispMain = FALSE;
	stDispMain stData(sMsg, rgb);
	m_ArrayDispMain.Add(stData);
	m_bDispMain = TRUE;

	if (sMsg == _T("�� ��"))
		m_bStopF_Verify = TRUE;

	sMsg.Empty();
	m_csDispMain.Unlock();
}

int CGvisR2R_LaserView::DoDispMain()
{
	int nRtn = -1;

	if (!m_bDispMain)
		return nRtn;

	int nCount = m_ArrayDispMain.GetSize();
	if (nCount > 0)
	{
		stDispMain stDispMsg;

		m_csDispMain.Lock();
		stDispMsg = m_ArrayDispMain.GetAt(0);
		m_ArrayDispMain.RemoveAt(0);
		m_csDispMain.Unlock();

		if (m_pDlgMenu01)
		{
			CString sMsg = stDispMsg.strMsg;
			COLORREF rgb = stDispMsg.rgb;
			m_sDispMain = sMsg;
			m_pDlgMenu01->DispMain(sMsg, rgb);
			return 0;
		}
	}

	return nRtn;
}

BOOL CGvisR2R_LaserView::IsReady()
{
	if (m_pDlgMenu03)
		return m_pDlgMenu03->GetReady();

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsAuto()
{
	if (pDoc->Status.bAuto)
		return TRUE;
	return FALSE;
}

void CGvisR2R_LaserView::Shift2Buf()
{
	if (m_nShareUpS > 0)
	{
		m_bLoadShare[0] = TRUE;
		pDoc->m_ListBuf[0].Push(m_nShareUpS);

	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (m_nShareDnS > 0)
		{
			m_bLoadShare[1] = TRUE;
			pDoc->m_ListBuf[1].Push(m_nShareDnS);

			if (m_nShareDnS == m_nAoiLastSerial[0] - 3 && m_nAoiLastSerial[0] > 0)
			{
				if (IsVsDn())
				{
					SetDummyDn();
					Sleep(30);
					SetDummyDn();
					Sleep(30);
					SetDummyDn();
					Sleep(30);
				}
			}
		}
	}

	pDoc->CopyPcrAll();
	pDoc->DelSharePcr();
}

void CGvisR2R_LaserView::Shift2Mk()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;

	if (bDualTest)
	{
		if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
		{
			nSerial = m_nBufDnSerial[0];
			if (nSerial > 0 && (nSerial % 2))
			{
				pDoc->UpdateYield(nSerial);
				pDoc->Shift2Mk(nSerial);	// Cam0
				if (m_pDlgFrameHigh)
					m_pDlgFrameHigh->SetMkLastShot(nSerial);
			}
		}
		else
		{
			nSerial = m_nBufDnSerial[0];
			if (!m_bCont)
			{
				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
			else
			{
				if (nSerial > 0)
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
		}
	}
	else
	{
		if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
		{
			nSerial = m_nBufUpSerial[0];
			if (!m_bCont)
			{
				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial);
				}
				else
				{
					Stop();
				}
			}
			else
			{
				if (nSerial > 0)
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial);
				}
				else
				{
					Stop();
				}
			}
		}
		else
		{
			nSerial = m_nBufUpSerial[0];
			if (!m_bCont)
			{
				if (nSerial > 0 && (nSerial % 2)) // First Shot number must be odd.
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
			else
			{
				if (nSerial > 0)
				{
					pDoc->UpdateYield(nSerial);
					pDoc->Shift2Mk(nSerial);	// Cam0
					pDoc->UpdateYield(nSerial + 1);
					pDoc->Shift2Mk(nSerial + 1);	// Cam1
					if (m_pDlgFrameHigh)
						m_pDlgFrameHigh->SetMkLastShot(nSerial + 1);
				}
				else
				{
					Stop();
				}
			}
		}
	}
}

void CGvisR2R_LaserView::SetTestSts(int nStep)
{
}

void CGvisR2R_LaserView::SetTestSts0(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003829"), 1);
	//IoWrite(_T("MB003829"), 1); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	else
		pView->m_pMpe->Write(_T("MB003829"), 0);
	//IoWrite(_T("MB003829"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
#endif
}

void CGvisR2R_LaserView::SetTestSts1(BOOL bOn)
{
#ifdef USE_MPE
	if (bOn)
		pView->m_pMpe->Write(_T("MB003929"), 1);
	//IoWrite(_T("MB003929"), 1); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
	else
		pView->m_pMpe->Write(_T("MB003929"), 0);
	//IoWrite(_T("MB003929"), 0); // �˻�� �� �˻� ���̺� ���� SOL <-> Y4369 I/F
#endif
}

void CGvisR2R_LaserView::SetAoiStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// �˻�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON,
	m_pDlgMenu03->SwAoiDustBlw(FALSE);
	m_pDlgMenu03->SwAoiTblBlw(FALSE);
	m_pDlgMenu03->SwAoiTblVac(FALSE);
	m_pDlgMenu03->SwAoiFdVac(FALSE);
	// 	m_pDlgMenu03->SwAoiTqVac(FALSE);
}

void CGvisR2R_LaserView::SetAoiFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// �˻�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwAoiFdVac(TRUE);
	// 	m_pDlgMenu03->SwAoiTqVac(TRUE);
	m_pDlgMenu03->SwAoiTblVac(FALSE);
	m_pDlgMenu03->SwAoiTblBlw(TRUE);
	m_pDlgMenu03->SwAoiDustBlw(TRUE);
}

void CGvisR2R_LaserView::SetMkSts(int nStep)
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - TBL�ı� OFF, TBL���� ON, FD/TQ ���� OFF, 
	switch (nStep)
	{
	case 0:
		m_pDlgMenu03->SwMkTblBlw(FALSE);
		m_pDlgMenu03->SwMkFdVac(FALSE);
		m_pDlgMenu03->SwMkTqVac(FALSE);
		break;
	case 1:
		m_pDlgMenu03->SwMkTblVac(TRUE);
		break;
	}
}

void CGvisR2R_LaserView::SetMkStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkTblBlw(FALSE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkFdVac(FALSE);
	// 	m_pDlgMenu03->SwMkTqVac(FALSE);
}

void CGvisR2R_LaserView::SetMkFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkFdVac(TRUE);
	// 	m_pDlgMenu03->SwMkTqVac(TRUE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkTblBlw(TRUE);
}

BOOL CGvisR2R_LaserView::IsMkFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}

void CGvisR2R_LaserView::SetDelay(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt[nId] = GetTickCount();
	m_dwSetDlyEd[nId] = m_dwSetDlySt[nId] + mSec;
}

void CGvisR2R_LaserView::SetDelay0(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt0[nId] = GetTickCount();
	m_dwSetDlyEd0[nId] = m_dwSetDlySt0[nId] + mSec;
}

void CGvisR2R_LaserView::SetDelay1(int mSec, int nId)
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	m_dwSetDlySt1[nId] = GetTickCount();
	m_dwSetDlyEd1[nId] = m_dwSetDlySt1[nId] + mSec;
}

BOOL CGvisR2R_LaserView::WaitDelay(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::WaitDelay0(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::WaitDelay1(int nId) // F:Done, T:On Waiting....
{
	if (nId > 10)
		nId = 9;
	else if (nId < 0)
		nId = 0;

	DWORD dwCur = GetTickCount();
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::GetDelay(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt[nId]);
	if (dwCur < m_dwSetDlyEd[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::GetDelay0(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt0[nId]);
	if (dwCur < m_dwSetDlyEd0[nId])
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::GetDelay1(int &mSec, int nId) // F:Done, T:On Waiting....
{
	DWORD dwCur = GetTickCount();
	mSec = int(dwCur - m_dwSetDlySt1[nId]);
	if (dwCur < m_dwSetDlyEd1[nId])
		return TRUE;
	return FALSE;
}

void CGvisR2R_LaserView::SetAoiFd()
{
	CfPoint OfSt;
	if (GetAoiUpOffset(OfSt))
	{
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->m_dAoiUpFdOffsetX = OfSt.x;
			m_pDlgMenu02->m_dAoiUpFdOffsetY = OfSt.y;
		}
	}

	MoveAoi(-1.0*OfSt.x);
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneAoi();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		//IoWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
		pView->m_pMpe->Write(_T("MB440151"), 1);
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_LaserView::MoveAoi(double dOffset)
{
	//	long lData = (long)0;
	long lData = (long)(dOffset * 1000.0);
	//IoWrite(_T("MB440160"), 1);	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
	pView->m_pMpe->Write(_T("MB440160"), 1);
	//IoWrite(_T("ML45064"), lData);	// �˻�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
	pView->m_pMpe->Write(_T("ML45064"), lData);
}

void CGvisR2R_LaserView::SetMkFd()
{
	CfPoint OfSt;
	GetMkOffset(OfSt);

	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
			MoveMk(-1.0*OfSt.x);
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
				MoveMk(-1.0*OfSt.x);
		}
	}
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneMk();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
		//IoWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
		pView->m_pMpe->Write(_T("MB440151"), 1);

		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_LaserView::MoveMk(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	//IoWrite(_T("MB440161"), 1);		// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF))
	pView->m_pMpe->Write(_T("MB440161"), 1);
	//IoWrite(_T("ML45066"), lData);	// ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
	pView->m_pMpe->Write(_T("ML45066"), lData);
#endif
}

BOOL CGvisR2R_LaserView::IsMkFd()
{
	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
				return TRUE;
#endif
			return FALSE;
		}
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
					return TRUE;
#endif
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::IsAoiFd()
{
#ifdef USE_MPE
	if (pDoc->m_pMpeSignal[5] & (0x01 >> 0))	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

void CGvisR2R_LaserView::SetMkFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	MoveMk(dOffset);
}

void CGvisR2R_LaserView::SetCycTime()
{
	DWORD dCur = GetTickCount();
	if (m_dwCycSt > 0)
	{
		m_dwCycTim = (double)(dCur - m_dwCycSt);
		if (m_dwCycTim < 0.0)
			m_dwCycTim *= (-1.0);
	}
	else
		m_dwCycTim = 0.0;
}

int CGvisR2R_LaserView::GetCycTime()
{
	if (m_dwCycTim < 0)
		m_dwCycTim = 0;

	int nTim = int(m_dwCycTim);
	return nTim;
}

BOOL CGvisR2R_LaserView::IsMkFdDone()
{
	if (m_nShareDnCnt > 0)
	{
		if (m_nShareDnCnt % 2)
			return TRUE;
	}
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 1)))	// ��ŷ�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsAoiFdDone()
{
#ifdef USE_MPE
	if (!(pDoc->m_pMpeSignal[5] & (0x01 << 0)))	// �˻�� �ǵ� ON (PLC�� �ǵ��Ϸ� �� OFF)
		return TRUE;
#endif
	return FALSE;
}

double CGvisR2R_LaserView::GetAoi2InitDist()
{
	// 	double dInitD = _tstof(pDoc->WorkingInfo.LastJob.sDistAoiMk)-_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dInitD = (_tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) - 1.0) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dInitD;
}

double CGvisR2R_LaserView::GetMkInitDist()
{
	double dInitD0 = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dInitD1 = (_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) + dInitD0) - (_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen)*2.0);
	return dInitD1;
}

double CGvisR2R_LaserView::GetMkRemain()
{
#ifdef USE_MPE
	// 	double dRemain = _tstof(pDoc->WorkingInfo.LastJob.sDistAoiMk) - m_dEnc[AXIS_RENC];
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dInitD0 = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) + dInitD0 - dCurPosMkFd;
	return dRemain;
#else
	return 0.0;
#endif
}

void CGvisR2R_LaserView::UpdateWorking()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateWorking();
}

void CGvisR2R_LaserView::StopFromThread()
{
	m_bStopFromThread = TRUE;
}


void CGvisR2R_LaserView::EngStop(BOOL bOn)
{
	m_bEngStop = bOn;

	if (m_pEngrave)
	{
		if (bOn)
		{
			m_pEngrave->SwStop(TRUE);
			Sleep(100);
		}
	}
}

BOOL CGvisR2R_LaserView::IsEngStop()
{
	if (m_sDispMain != _T("������") && m_bEngStop)
		return TRUE;
	else if (m_sDispMain == _T("������") && m_bEngStop)
	{
		m_bEngStop = FALSE;
		return FALSE;
	}

	return m_bEngStop;
}

void CGvisR2R_LaserView::Stop()
{
	CString sMsg;
	if (m_pDlgMenu03)
	{
		m_pDlgMenu03->SwStop();
	}
}

BOOL CGvisR2R_LaserView::IsStop()
{
	if (m_sDispMain == _T("�� ��"))
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsRun()
{
	if (m_sDispMain == _T("������") || m_sDispMain == _T("�ʱ����") || m_sDispMain == _T("�ܸ����") || m_sDispMain == _T("�����غ�")
		|| m_sDispMain == _T("�ܸ�˻�") || m_sDispMain == _T("���˻�") || m_sDispMain == _T("������"))
	{
		if (IsEngStop())
			return FALSE;

		return TRUE;
	}
	return FALSE;
}

void CGvisR2R_LaserView::ShowLive(BOOL bShow)
{
	if (bShow)
	{
		if (!IsShowLive())
			SetTimer(TIM_SHOW_MENU02, 30, NULL);
	}
	else
	{
		if (IsShowLive())
			SetTimer(TIM_SHOW_MENU01, 30, NULL);
	}
}

BOOL CGvisR2R_LaserView::IsShowLive()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->IsWindowVisible())
			return TRUE;
	}

	return FALSE;
}

void CGvisR2R_LaserView::SetLotSt()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.StTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.StTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.StTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.StTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.StTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.StTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.EdTime.nYear = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = 0;
	pDoc->WorkingInfo.Lot.EdTime.nDay = 0;
	pDoc->WorkingInfo.Lot.EdTime.nHour = 0;
	pDoc->WorkingInfo.Lot.EdTime.nMin = 0;
	pDoc->WorkingInfo.Lot.EdTime.nSec = 0;

	m_dwLotSt = GetTickCount();
	pDoc->SaveLotTime(m_dwLotSt);
	DispLotTime();

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->SetLotSt();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetLotSt();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetLotSt();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetLotSt();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetLotSt();
	}
}

void CGvisR2R_LaserView::SetLotEd()
{
	stLotTime LotTime;
	GetTime(LotTime);

	pDoc->WorkingInfo.Lot.EdTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.EdTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.EdTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.EdTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.EdTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.EdTime.nSec = LotTime.nSec;

	pDoc->WorkingInfo.Lot.CurTime.nYear = LotTime.nYear;
	pDoc->WorkingInfo.Lot.CurTime.nMonth = LotTime.nMonth;
	pDoc->WorkingInfo.Lot.CurTime.nDay = LotTime.nDay;
	pDoc->WorkingInfo.Lot.CurTime.nHour = LotTime.nHour;
	pDoc->WorkingInfo.Lot.CurTime.nMin = LotTime.nMin;
	pDoc->WorkingInfo.Lot.CurTime.nSec = LotTime.nSec;

	m_dwLotEd = GetTickCount();

	pDoc->SaveLotTime(pDoc->WorkingInfo.Lot.dwStTick);
	DispLotTime();

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->SetLotEd();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetLotEd();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetLotEd();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetLotEd();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetLotEd();
	}

}

void CGvisR2R_LaserView::DispLotTime()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->DispLotTime();
}

BOOL CGvisR2R_LaserView::IsTest()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���� <-> Y4368 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���� <-> Y4468 I/F

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 || bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsTestUp()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 8) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���� <-> Y4368 I/F
	if (bOn0)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsTestDn()
{
#ifdef USE_MPE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 8) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���� <-> Y4468 I/F
	if (bOn1)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsAoiTblVac()
{
#ifdef USE_MPE
	if (!pDoc->m_pMpeIo)
		return FALSE;

	// MpeIO
	int nInSeg = pDoc->MkIo.MpeIo.nInSeg;
	int nOutSeg = pDoc->MkIo.MpeIo.nOutSeg;

	BOOL bOn0 = (pDoc->m_pMpeIo[nInSeg + 10] & 0x01 << 9) ? TRUE : FALSE; //[34] �˻�� �� �˻� ���̺� ���� SOL <-> Y4469 I/F
	BOOL bOn1 = (pDoc->m_pMpeIo[nInSeg + 14] & 0x01 << 9) ? TRUE : FALSE; //[38] �˻�� �� �˻� ���̺� ���� SOL <-> Y4469 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 || bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsTestDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4328 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 8)) ? TRUE : FALSE;	// �˻�� �� �˻� �Ϸ� <-> X4428 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 && bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd + 10.0 < dTgtFd)//-_tstof(pDoc->WorkingInfo.Motion.sAoiFdDist))
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsAoiTblVacDone()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;	// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;	// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (bOn0 && bOn1)
			return TRUE;
	}
	else
	{
		if (bOn0)
			return TRUE;
	}

	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd + 10.0 < dTgtFd)//-_tstof(pDoc->WorkingInfo.Motion.sAoiFdDist))
	{
		if (bOn0)
			return TRUE;
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsTestDoneUp()
{
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsTestDoneDn()
{
	return TRUE;
}

int CGvisR2R_LaserView::GetSerial()
{
	int nSerial = 0;
	if (m_pDlgMenu01)
		nSerial = m_pDlgMenu01->GetSerial();
	return nSerial;
}

void CGvisR2R_LaserView::SetMkFdLen()
{
	// 	int nLast = pDoc->GetLastSerial();
	int nLast = pDoc->GetLastShotMk();
	pDoc->WorkingInfo.Motion.sMkFdTotLen.Format(_T("%.1f"), (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));

	CString sData, sPath = PATH_WORKING_INFO;
	sData = pDoc->WorkingInfo.Motion.sMkFdTotLen;
	::WritePrivateProfileString(_T("Motion"), _T("MARKING_FEEDING_SERVO_TOTAL_DIST"), sData, sPath);
}

double CGvisR2R_LaserView::GetMkFdLen()
{
	int nLast = pDoc->GetLastShotMk();
	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);

	return dLen;
}

double CGvisR2R_LaserView::GetAoiUpFdLen()
{
	int nLast = pDoc->GetLastShotUp();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_LaserView::GetAoiDnFdLen()
{
	int nLast = pDoc->GetLastShotDn();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_LaserView::GetTotVel()
{
	CString str, sPrev;
	int nDiff;
	int nHour, nMin, nSec;
	int nStYear, nStMonth, nStDay, nStHour, nStMin, nStSec;
	int nCurYear, nCurMonth, nCurDay, nCurHour, nCurMin, nCurSec;
	int nEdYear, nEdMonth, nEdDay, nEdHour, nEdMin, nEdSec;

	nStYear = pDoc->WorkingInfo.Lot.StTime.nYear;
	nStMonth = pDoc->WorkingInfo.Lot.StTime.nMonth;
	nStDay = pDoc->WorkingInfo.Lot.StTime.nDay;
	nStHour = pDoc->WorkingInfo.Lot.StTime.nHour;
	nStMin = pDoc->WorkingInfo.Lot.StTime.nMin;
	nStSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	nCurYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nCurMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nCurDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nCurHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nCurMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nCurSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	nEdYear = pDoc->WorkingInfo.Lot.EdTime.nYear;
	nEdMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth;
	nEdDay = pDoc->WorkingInfo.Lot.EdTime.nDay;
	nEdHour = pDoc->WorkingInfo.Lot.EdTime.nHour;
	nEdMin = pDoc->WorkingInfo.Lot.EdTime.nMin;
	nEdSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	int nTotSec = 0;
	double dMkFdLen = GetMkFdLen();
	if (!nStYear && !nStMonth && !nStDay && !nStHour && !nStMin && !nStSec)
	{
		return 0.0;
	}
	else if (!nEdYear && !nEdMonth && !nEdDay && !nEdHour && !nEdMin && !nEdSec)
	{
		nDiff = (GetTickCount() - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}
	else
	{
		nDiff = (pView->m_dwLotEd - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
	}

	nTotSec = nHour * 3600 + nMin * 60 + nSec;
	double dVel = 0.0;
	if (nTotSec > 0)
		dVel = dMkFdLen / (double)nTotSec; // [mm/sec]
										   // 		dVel = (dMkFdLen*60.0) / ((double)nTotSec*1000.0); // [M/min]

	m_dTotVel = dVel;
	return dVel;
}

double CGvisR2R_LaserView::GetPartVel()
{
	double dLen = _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 2.0;
	double dSec = (double)GetCycTime() / 1000.0;
	double dVel = 0.0;
	if (dSec > 0.0)
		dVel = dLen / dSec; // [mm/sec]
							// 		dVel = (dLen*60.0) / (dSec*1000.0); // [M/min]
	m_dPartVel = dVel;
	return dVel;
}


BOOL CGvisR2R_LaserView::IsBuffer(int nNum)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (bDualTest)
	{
		if (m_nBufTot[0] > nNum && m_nBufTot[1] > nNum) // [0]: AOI-Up , [1]: AOI-Dn
			return TRUE;
	}
	else
	{
		if (m_nBufTot[0] > nNum) // [0]: AOI-Up
			return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferUp()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferDn()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_LaserView::GetBuffer(int *pPrevSerial)
{
	int nS0 = GetBufferUp(pPrevSerial);
	int nS1 = GetBufferDn(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_LaserView::GetBufferUp(int *pPrevSerial)
{
	if (IsBufferUp())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_LaserView::GetBufferDn(int *pPrevSerial)
{
	if (IsBufferDn())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}

BOOL CGvisR2R_LaserView::IsBuffer0()
{
	if (m_nBufTot[0] > 0 && m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferUp0()
{
	if (m_nBufTot[0] > 0)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferDn0()
{
	if (m_nBufTot[1] > 0)
		return TRUE;
	return FALSE;
}

int CGvisR2R_LaserView::GetBuffer0(int *pPrevSerial)
{
	int nS0 = GetBufferUp0(pPrevSerial);
	int nS1 = GetBufferDn0(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_LaserView::GetBufferUp0(int *pPrevSerial)
{
	if (IsBufferUp0())
		return m_pBufSerial[0][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][0];
	return 0;
}

int CGvisR2R_LaserView::GetBufferDn0(int *pPrevSerial)
{
	if (IsBufferDn0())
		return m_pBufSerial[1][0];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][0];
	return 0;
}

BOOL CGvisR2R_LaserView::IsBuffer1()
{
	if (m_nBufTot[0] > 1 && m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferUp1()
{
	if (m_nBufTot[0] > 1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsBufferDn1()
{
	if (m_nBufTot[1] > 1)
		return TRUE;
	return FALSE;
}

int CGvisR2R_LaserView::GetBuffer1(int *pPrevSerial)
{
	int nS0 = GetBufferUp1(pPrevSerial);
	int nS1 = GetBufferDn1(pPrevSerial);
	if (nS0 != nS1)
		return 0;
	return nS0;
}

int CGvisR2R_LaserView::GetBufferUp1(int *pPrevSerial)
{
	if (IsBufferUp1())
		return m_pBufSerial[0][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[0][1];
	return 0;
}

int CGvisR2R_LaserView::GetBufferDn1(int *pPrevSerial)
{
	if (IsBufferDn1())
		return m_pBufSerial[1][1];
	else if (pPrevSerial)
		*pPrevSerial = m_pBufSerial[1][1];
	return 0;
}

BOOL CGvisR2R_LaserView::IsShare()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		if (m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareUp() && IsShareDn())
			{
				m_bWaitPcr[0] = FALSE;
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else if (m_bWaitPcr[0] && !m_bWaitPcr[1])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else if (!m_bWaitPcr[0] && m_bWaitPcr[1])
		{
			if (IsShareDn())
			{
				m_bWaitPcr[1] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp() || IsShareDn())
				return TRUE;
		}

		//	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];	// ��ŷ�� Feeding ���ڴ� ��(���� mm )
		// 	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
		// 	if(dCurPosMkFd < dTgtFd-_tstof(pDoc->WorkingInfo.Motion.sAoiFdDist) + 10.0)
		// 	{
		// 		if(IsShareUp())
		// 			return TRUE;
		// 	}
	}
	else
	{
		if (m_bWaitPcr[0])
		{
			if (IsShareUp())
			{
				m_bWaitPcr[0] = FALSE;
				return TRUE;
			}
		}
		else
		{
			if (IsShareUp())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsShareUp()
{
	return pDoc->Status.PcrShare[0].bExist;
}

BOOL CGvisR2R_LaserView::IsShareDn()
{
	return pDoc->Status.PcrShare[1].bExist;
}

BOOL CGvisR2R_LaserView::IsVsShare()
{
	int nSerial;
	BOOL bExist = ChkVsShare(nSerial);
	return bExist;
}

int CGvisR2R_LaserView::GetShareUp()
{
	return pDoc->Status.PcrShare[0].nSerial;
}

int CGvisR2R_LaserView::GetShareDn()
{
	return pDoc->Status.PcrShare[1].nSerial;
}

BOOL CGvisR2R_LaserView::ChkLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProc);
	return bRtn;
}

BOOL CGvisR2R_LaserView::ChkLastProcFromUp()
{
	BOOL bRtn = TRUE;
	if (m_pDlgMenu01)
		bRtn = (m_pDlgMenu01->m_bLastProcFromUp);
	return bRtn;
}

BOOL CGvisR2R_LaserView::ChkLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.25"));
		return FALSE;
	}

	BOOL bEnd0 = ChkLotEndUp(nSerial);
	BOOL bEnd1 = ChkLotEndDn(nSerial);
	if (bEnd0 || bEnd1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkLotEndUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.26"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
	// 	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsShareUp, nSerial); 
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_LaserView::ChkLotEndDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.27"));
		return 0;
	}

	CString sPath;
	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);
	// 	sPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsShareDn, nSerial); 
	return pDoc->ChkLotEnd(sPath);
}

BOOL CGvisR2R_LaserView::SetSerial(int nSerial, BOOL bDumy)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.28"));
		return 0;
	}

	if (!m_pDlgMenu01)
		return FALSE;

	int nPrevSerial = m_pDlgMenu01->GetCurSerial();

	if (nPrevSerial == nSerial)
		return TRUE;

	BOOL bRtn[2] = {1};
	bRtn[0] = m_pDlgMenu01->SetSerial(nSerial, bDumy);
	//if (pDoc->GetTestMode() == MODE_OUTER)
	//{
	//	if (!m_pDlgMenu06)
	//		bRtn[1] = m_pDlgMenu06->SetSerial(nSerial, bDumy);
	//}

	return (bRtn[0] && bRtn[1]);
}

BOOL CGvisR2R_LaserView::SetSerialReelmap(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Error - SetSerialReelmap : m_pDlgMenu01 is NULL."));
		return FALSE;
	}
	return m_pDlgMenu01->SetSerialReelmap(nSerial, bDumy);
}

BOOL CGvisR2R_LaserView::SetSerialMkInfo(int nSerial, BOOL bDumy)
{
	if (!m_pDlgMenu01)
		return FALSE;
	return m_pDlgMenu01->SetSerialMkInfo(nSerial, bDumy);
}

void CGvisR2R_LaserView::SetListBuf()
{
	pDoc->m_ListBuf[0].Clear();
	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
			pDoc->m_ListBuf[0].Push(m_pBufSerial[0][i]);
	}

	pDoc->m_ListBuf[1].Clear();
	if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
	{
		for (int i = 0; i < m_nBufTot[1]; i++)
			pDoc->m_ListBuf[1].Push(m_pBufSerial[1][i]);
	}
}

void CGvisR2R_LaserView::DispLotStTime()
{
	//char szData[MAX_PATH];
	TCHAR szData[MAX_PATH];
	CString sPath = PATH_WORKING_INFO;
	// [Lot]
	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Tick"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.dwStTick = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.dwStTick = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Year"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nYear = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nYear = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Month"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMonth = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMonth = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Day"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nDay = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nDay = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Hour"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nHour = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nHour = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Minute"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nMin = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nMin = 0;

	if (0 < ::GetPrivateProfileString(_T("Lot"), _T("Start Second"), NULL, szData, sizeof(szData), sPath))
		pDoc->WorkingInfo.Lot.StTime.nSec = _tstoi(szData);
	else
		pDoc->WorkingInfo.Lot.StTime.nSec = 0;

	m_dwLotSt = (DWORD)pDoc->WorkingInfo.Lot.dwStTick;
	DispLotTime();
}

void CGvisR2R_LaserView::ClrMkInfo()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pDoc->ClrPcr();
	if (pDoc->m_pReelMap)
		pDoc->m_pReelMap->Clear();
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Clear();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Clear();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Clear();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Clear();
	}

	if (m_pDlgMenu01)
	{
		m_pDlgMenu01->ResetMkInfo();
		m_pDlgMenu01->SetPnlNum();
		m_pDlgMenu01->SetPnlDefNum();
		m_pDlgMenu01->RefreshRmap();
		m_pDlgMenu01->UpdateRst();
		m_pDlgMenu01->UpdateWorking();
	}
}

void CGvisR2R_LaserView::ModelChange(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn 
{
	if (nAoi == 0)
	{
		pDoc->SetModelInfoUp();
		pView->OpenReelmapUp(); // At Start...
		pView->SetPathAtBufUp();
		if (pView->m_pDlgMenu01)
		{
			pView->m_pDlgMenu01->UpdateData();
			if (pView->m_nSelRmap == RMAP_UP || pView->m_nSelRmap == RMAP_ALLUP)
				pView->m_pDlgMenu01->OpenReelmap(pView->m_nSelRmap);
		}

		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[0].sModel, pDoc->Status.PcrShare[0].sLayer);

		if (pDoc->GetTestMode() == MODE_OUTER)
		{
			OpenReelmapInner();
		}
	}
	else if (nAoi == 1)
	{
		pDoc->SetModelInfoDn();
		pView->OpenReelmapDn(); // At Start...
		pView->SetPathAtBufDn();
		if (pView->m_pDlgMenu01)
		{
			pView->m_pDlgMenu01->UpdateData();
			if (pView->m_nSelRmap == RMAP_DN || pView->m_nSelRmap == RMAP_ALLDN)
				pView->m_pDlgMenu01->OpenReelmap(pView->m_nSelRmap);
		}
		pDoc->m_pSpecLocal->MakeDir(pDoc->Status.PcrShare[1].sModel, pDoc->Status.PcrShare[1].sLayer);
	}
}

void CGvisR2R_LaserView::ResetMkInfo(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bDualTestInner;
	CString sLot, sLayerUp, sLayerDn;
	pDoc->GetCurrentInfo();

	// CamMst Info...
	pDoc->GetCamPxlRes();

	//if(!pDoc->m_pReelMap)
	//	InitReelmap();

	if (nAoi == 0 || nAoi == 2)
	{
		if (!bDualTest)
		{
			m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();
		}

		//if (pDoc->GetItsSerialInfo(1, bDualTestInner, sLot, sLayerUp, sLayerDn, 3))
		//{
		//	//if (pDoc->GetTestMode() == MODE_OUTER)
		//	if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
		//	{
		//		if (!bDualTestInner)
		//		{
		//			if (m_pDlgMenu06)
		//				m_pDlgMenu06->ResetMkInfo();
		//		}
		//	}
		//}

		if (IsLastJob(0)) // Up
		{
			pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				pDoc->WorkingInfo.LastJob.sModelUp,
				pDoc->WorkingInfo.LastJob.sLayerUp);
			pDoc->m_Master[0].LoadMstInfo();
			//pDoc->m_Master[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);

			if (pDoc->GetItsSerialInfo(1, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
			{
				//if (pDoc->GetTestMode() == MODE_OUTER)
				if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
				{
					pDoc->m_MasterInner[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
						pDoc->WorkingInfo.LastJob.sModelUp,
						sLayerUp);
					pDoc->m_MasterInner[0].LoadMstInfo();
					//pDoc->m_MasterInner[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->m_sEngLotNum);

					if (bDualTestInner)
					{
						//GetCurrentInfoEng();
						pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
							pDoc->WorkingInfo.LastJob.sModelUp,
							sLayerDn);
						pDoc->m_MasterInner[1].LoadMstInfo();
					}
				}
			}
		}
		else
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - IsLastJob(0)..."));
		}

		InitReelmapUp();

		if (pDoc->GetItsSerialInfo(1, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
		{
			if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
			{
				InitReelmapInnerUp();
				if (bDualTestInner)
					InitReelmapInnerDn();
			}
		}

		OpenReelmap();
		SetAlignPosUp();

		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->ChgModelUp();

			if (bDualTest)
				m_pDlgMenu02->ChgModelDn();
		}
		
		if (m_pDlgMenu01)
		{
			m_pDlgMenu01->InitCadImgUp();

			//if (!bDualTest && pDoc->GetTestMode() != MODE_OUTER)
			//{
			//	m_pDlgMenu01->InitGL();
			//	m_bDrawGL = TRUE;
			//	m_pDlgMenu01->RefreshRmap();
			//}
		}

		//if (pDoc->GetItsSerialInfo(1, bDualTestInner, sLot, sLayerUp, sLayerDn, 0))
		//{
		//	//if (pDoc->GetTestMode() == MODE_OUTER)
		//	if (pDoc->m_Master[0].IsMstSpec(pDoc->WorkingInfo.System.sPathCamSpecDir, pDoc->WorkingInfo.LastJob.sModelUp, sLayerUp))
		//	{
		//		if (m_pDlgMenu06)
		//		{
		//			m_pDlgMenu06->InitCadImgUp();
		//			if (bDualTestInner)
		//				m_pDlgMenu06->InitCadImgDn();

		//			m_pDlgMenu06->InitGL();
		//			m_pDlgMenu06->RefreshRmap();
		//		}

		//	}
		//}

		if (m_pDlgMenu01)
		{
			if (!bDualTest)
			{
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}
		}
	}


	if (bDualTest)
	{
		if (nAoi == 1 || nAoi == 2)
		{
			m_bDrawGL = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->ResetMkInfo();

			//ResetReelmap();

			if (IsLastJob(1)) // Dn
			{
				pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
					pDoc->WorkingInfo.LastJob.sModelUp,
					//pDoc->WorkingInfo.LastJob.sModelDn,
					pDoc->WorkingInfo.LastJob.sLayerDn,
					pDoc->WorkingInfo.LastJob.sLayerUp);

				pDoc->m_Master[1].LoadMstInfo();
				//pDoc->m_Master[1].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotDn);

				//if (pDoc->GetTestMode() == MODE_OUTER)
				//{
				//	//GetCurrentInfoEng();
				//	pDoc->m_MasterInner[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
				//		pDoc->WorkingInfo.LastJob.sModelUp,
				//		pDoc->m_sEngLayerDn);
				//	pDoc->m_MasterInner[1].LoadMstInfo();
				//}
			}
			else
			{
				AfxMessageBox(_T("Error - IsLastJob(1)..."));
			}

			InitReelmapDn();
			SetAlignPosDn();

			if (m_pDlgMenu02)
				m_pDlgMenu02->ChgModelDn();

			if (m_pDlgMenu01)
			{
				m_pDlgMenu01->InitCadImgDn();
				m_pDlgMenu01->InitGL();
				m_bDrawGL = TRUE;
				m_pDlgMenu01->RefreshRmap();
			}

			// 		m_bDrawGL = TRUE;
		}
	}
	// 	else
	// 	{
	// 		if(m_pDlgMenu01)
	// 			m_pDlgMenu01->ResetMkInfo();
	// 	}
}

void CGvisR2R_LaserView::SetAlignPos()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[0][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[0];
		//m_pMotion->m_dAlignPosX[0][1] = pDoc->m_Master[0].m_stAlignMk.X1+ pView->m_pMotion->m_dPinPosX[0];
		//m_pMotion->m_dAlignPosY[0][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[0];

		m_pMotion->m_dAlignPosX[1][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[1];
		//m_pMotion->m_dAlignPosX[1][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[1];
		//m_pMotion->m_dAlignPosY[1][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[1];
	}
}

void CGvisR2R_LaserView::SetAlignPosUp()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[0][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[0];
		m_pMotion->m_dAlignPosY[0][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[0];
		//m_pMotion->m_dAlignPosX[0][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[0];
		//m_pMotion->m_dAlignPosY[0][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[0];
	}
}

void CGvisR2R_LaserView::SetAlignPosDn()
{
	if (m_pMotion)
	{
		m_pMotion->m_dAlignPosX[1][0] = pDoc->m_Master[0].m_stAlignMk.X0 + pView->m_pMotion->m_dPinPosX[1];
		m_pMotion->m_dAlignPosY[1][0] = pDoc->m_Master[0].m_stAlignMk.Y0 + pView->m_pMotion->m_dPinPosY[1];
		//m_pMotion->m_dAlignPosX[1][1] = pDoc->m_Master[0].m_stAlignMk.X1 + pView->m_pMotion->m_dPinPosX[1];
		//m_pMotion->m_dAlignPosY[1][1] = pDoc->m_Master[0].m_stAlignMk.Y1 + pView->m_pMotion->m_dPinPosY[1];
	}
}

int CGvisR2R_LaserView::GetErrCode(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.29"));
		return 0;
	}

#ifndef	TEST_MODE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_LaserView::GetErrCodeUp(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.30"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCodeDn(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.31"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCode0(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.32"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifndef	TEST_MODE
	int nErr[2];
	nErr[0] = GetErrCodeUp0(nSerial);
	if (nErr[0] != 1)
		return nErr[0];
	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn0(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_LaserView::GetErrCodeUp0(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.33"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCodeDn0(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.34"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
	}
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCode1(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.35"));
		return 0;
	}

#ifndef	TEST_MODE
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nErr[2];
	nErr[0] = GetErrCodeUp1(nSerial);
	if (nErr[0] != 1)
		return nErr[0];

	if (bDualTest)
	{
		nErr[1] = GetErrCodeDn1(nSerial);
		if (nErr[1] != 1)
			return nErr[1];
	}
#endif

	return 1;
}

int CGvisR2R_LaserView::GetErrCodeUp1(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.36"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nErrCode = pDoc->m_pPcr[0][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetErrCodeDn1(int nSerial) // 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 1;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.37"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nErrCode = 1;

#ifndef	TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nErrCode = pDoc->m_pPcr[1][nIdx]->m_nErrPnl;
		else
			return 2;	// Code Setting Error.
	}
	else
		return 2;	// Code Setting Error.
#endif

	return nErrCode;
}

int CGvisR2R_LaserView::GetTotDefPcs(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.38"));
		return 0;
	}

	int nTotDef = 0;
#ifndef TEST_MODE
	nTotDef = GetTotDefPcsUp(nSerial) + GetTotDefPcsDn(nSerial);
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsUp(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.39"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.40"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcs0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.41"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])
		{
			if (pDoc->m_pPcr[2][nIdx])
				nTotDef = pDoc->m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])
		{
			if (pDoc->m_pPcr[0][nIdx])
				nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsUp0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.42"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsDn0(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.43"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcs1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.44"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])
		{
			if (pDoc->m_pPcr[2][nIdx])
				nTotDef = pDoc->m_pPcr[2][nIdx]->m_nTotDef;
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])
		{
			if (pDoc->m_pPcr[0][nIdx])
				nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
		}
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsUp1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.45"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[0])
	{
		if (pDoc->m_pPcr[0][nIdx])
			nTotDef = pDoc->m_pPcr[0][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

int CGvisR2R_LaserView::GetTotDefPcsDn1(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.46"));
		return 0;
	}

	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nTotDef = 0;

#ifndef TEST_MODE
	if (pDoc->m_pPcr[1])
	{
		if (pDoc->m_pPcr[1][nIdx])
			nTotDef = pDoc->m_pPcr[1][nIdx]->m_nTotDef;
	}
#else
	nTotDef = 1;
#endif

	return nTotDef;
}

CfPoint CGvisR2R_LaserView::GetMkPnt(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt(nMkPcs); // Cam0�� Mk ����Ʈ.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_LaserView::GetMkPnt0(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nMkPcs); // Cam0�� Mk ����Ʈ.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_LaserView::GetMkPnt1(int nMkPcs)
{
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	if (pDoc->m_Master[0].m_pPcsRgn)
		ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nMkPcs); // Cam1�� Mk ����Ʈ.
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

CfPoint CGvisR2R_LaserView::GetMkPnt0(int nSerial, int nMkPcs)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.47"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	int nDefPcsId = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0�� Mk ����Ʈ.
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt0(nDefPcsId); // Cam0�� Mk ����Ʈ.
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CGvisR2R_LaserView::GetMkStripIdx0(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pDoc->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / nMaxStrip);
	int nStripIdx = 0;//nRow, nNum, nMode, 

#ifndef TEST_MODE
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// Ȧ��.
		nRow = nNodeY - (nMode + 1);
	else		// ¦��.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CGvisR2R_LaserView::GetMkStripIdx1(int nDefPcsId) // 0 : Fail , 1~4 : Strip Idx
{
	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pDoc->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / nMaxStrip);
	int nStripIdx = 0;//nNum, nMode, nRow, 

#ifndef TEST_MODE
	int nRow = 0, nNum = 0, nMode = 0;
	nNum = int(nDefPcsId / nNodeY);
	nMode = nDefPcsId % nNodeY;
	if (nNum % 2) 	// Ȧ��.
		nRow = nNodeY - (nMode + 1);
	else		// ¦��.
		nRow = nMode;

	nStripIdx = int(nRow / nStripY) + 1;
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

int CGvisR2R_LaserView::GetMkStripIdx0(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.48"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx0(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;

	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

CfPoint CGvisR2R_LaserView::GetMkPnt1(int nSerial, int nMkPcs)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.50"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	//int nDefPcsId;
	CfPoint ptPnt;
	ptPnt.x = -1.0;
	ptPnt.y = -1.0;

#ifndef TEST_MODE
	int nDefPcsId = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[2][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1�� Mk ����Ʈ.
						}
					}
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					if (pDoc->m_pPcr[0][nIdx]->m_pMk[nMkPcs] != -2) // -2 (NoMarking)
					{
						nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
						if (pDoc->m_Master[0].m_pPcsRgn)
						{
							ptPnt = pDoc->m_Master[0].m_pPcsRgn->GetMkPnt1(nDefPcsId); // Cam1�� Mk ����Ʈ.
						}
					}
				}
			}
		}
	}
#else
	ptPnt.x = 1.0;
	ptPnt.y = 1.0;
#endif

	return ptPnt;
}

int CGvisR2R_LaserView::GetMkStripIdx1(int nSerial, int nMkPcs) // 0 : Fail , 1~4 : Strip Idx
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.51"));
		return 0;
	}

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nIdx = pDoc->GetPcrIdx1(nSerial);
	int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
	int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
	int nStripY = int(nNodeY / MAX_STRIP_NUM);
	int nStripIdx = 0;

#ifndef TEST_MODE
	int nDefPcsId = 0, nNum = 0, nMode = 0, nRow = 0;
	if (bDualTest)
	{
		if (pDoc->m_pPcr[2])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[2][nIdx])
			{
				if (pDoc->m_pPcr[2][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[2][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
	else
	{
		if (pDoc->m_pPcr[0])	// [0]:AOI-Up , [1]:AOI-Dn , [2]:AOI-AllUp , [3]:AOI-AllDn
		{
			if (pDoc->m_pPcr[0][nIdx])
			{
				if (pDoc->m_pPcr[0][nIdx]->m_pDefPcs)
				{
					nDefPcsId = pDoc->m_pPcr[0][nIdx]->m_pDefPcs[nMkPcs];
					nNum = int(nDefPcsId / nNodeY);
					nMode = nDefPcsId % nNodeY;
					if (nNum % 2) 	// Ȧ��.
						nRow = nNodeY - (nMode + 1);
					else		// ¦��.
						nRow = nMode;

					nStripIdx = int(nRow / nStripY) + 1;
				}
			}
		}
	}
#else
	nStripIdx = 1;
#endif

	return nStripIdx;
}

void CGvisR2R_LaserView::Move0(CfPoint pt, BOOL bCam)
{
	double fLen, fVel, fAcc, fJerk;
	double pPos[2];

	if (pDoc->WorkingInfo.System.bNoMk || bCam)
	{
		pPos[0] = pt.x;
		pPos[1] = pt.y;
	}
	else
	{
		pPos[0] = pt.x + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetX);
		pPos[1] = pt.y + _tstof(pDoc->WorkingInfo.Vision[0].sMkOffsetY);
	}

	if (pPos[0] < 0.0)
		pPos[0] = 0.0;
	if (pPos[1] < 0.0)
		pPos[1] = 0.0;

	double dCurrX = pView->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_dEnc[AXIS_Y0];
	fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		m_pMotion->Move0(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

BOOL CGvisR2R_LaserView::IsMoveDone()
{
	if (!m_pMotion)
		return FALSE;

	if (IsMoveDone0())
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsMoveDone0()
{
	if (!m_pMotion)
		return FALSE;

	if (m_pMotion->IsMotionDone(MS_X0) && m_pMotion->IsMotionDone(MS_Y0))
	{
		//	Sleep(50);
		return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::LoadPcrUp(int nSerial, BOOL bFromShare)
{
	return TRUE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.52"));
		return 0;
	}

	int nHeadInfo = pDoc->LoadPCR0(nSerial); // 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR0()"));
		return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::LoadPcrDn(int nSerial, BOOL bFromShare)
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return 0;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.53"));
		return 0;
	}

	int nHeadInfo = pDoc->LoadPCR1(nSerial); // 2(Failed), 1(����), -1(Align Error, �뱤�ҷ�), -2(Lot End)
	if (nHeadInfo >= 2)
	{
		MsgBox(_T("Error-LoadPCR1()"));
		return FALSE;
	}
	return TRUE;
}

BOOL CGvisR2R_LaserView::UpdateReelmap(int nSerial)
{
	return TRUE;

	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.54"));
		return 0;
	}

	if (!pDoc->MakeMkDir())
		return FALSE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString str;
	CString sPathRmap[4], sPathPcr[2]; //[Up/Dn]
	if (pDoc->m_pReelMap)
	{
		stModelInfo stInfo;
		sPathPcr[0].Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufUp, nSerial);
		if (bDualTest)
			sPathPcr[1].Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVrsBufDn, nSerial);

		if (!pDoc->GetPcrInfo(sPathPcr[0], stInfo)) // Up
		{
			pView->DispStsBar(_T("E(4)"), 5);
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error-GetPcrInfo(4)"));
			return FALSE;
		}

		if (!pDoc->MakeMkDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
			return FALSE;

		str = _T("ReelMapDataUp.txt"); // [0]:AOI-Up
		sPathRmap[0].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			stInfo.sModel,
			stInfo.sLot,
			stInfo.sLayer,
			str);

		if (bDualTest)
		{
			str = _T("ReelMapDataAll.txt"); // [2]:AOI-AllUp
			sPathRmap[2].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				stInfo.sModel,
				stInfo.sLot,
				stInfo.sLayer,
				str);

			if (!pDoc->GetPcrInfo(sPathPcr[1], stInfo)) // Dn
			{
				pView->DispStsBar(_T("E(5)"), 5);
				pView->ClrDispMsg();
				AfxMessageBox(_T("Error-GetPcrInfo(5)"));
				return FALSE;
			}

			if (!pDoc->MakeMkDir(stInfo.sModel, stInfo.sLot, stInfo.sLayer))
				return FALSE;

			str = _T("ReelMapDataDn.txt"); // [1]:AOI-Dn
			sPathRmap[1].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				stInfo.sModel,
				stInfo.sLot,
				stInfo.sLayer,
				str);
			str = _T("ReelMapDataAll.txt"); // [3]:AOI-AllDn
			sPathRmap[3].Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
				stInfo.sModel,
				stInfo.sLot,
				stInfo.sLayer,
				str);
		}

		m_nSerialRmapUpdate = nSerial;
		m_sPathRmapUpdate[0] = sPathRmap[0];
		m_sPathRmapUpdate[1] = sPathRmap[1];
		m_sPathRmapUpdate[2] = sPathRmap[2];
		m_sPathRmapUpdate[3] = sPathRmap[3];

		m_bTHREAD_UPDATE_REELMAP_UP = TRUE;
		if (bDualTest)
		{
			m_bTHREAD_UPDATE_REELMAP_DN = TRUE;
			m_bTHREAD_UPDATE_REELMAP_ALLUP = TRUE;
			m_bTHREAD_UPDATE_REELMAP_ALLDN = TRUE;
		}

		Sleep(100);

		return TRUE;
	}
	else
		MsgBox(_T("Error-ReelMapWrite()"));

	return FALSE;
}

void CGvisR2R_LaserView::InitInfo()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateData();

	if (m_pDlgMenu05)
	{
		m_pDlgMenu05->InitModel();
		if (m_pDlgMenu05->IsWindowVisible())
			m_pDlgMenu05->AtDlgShow();
	}
}

void CGvisR2R_LaserView::InitReelmap()
{
	pDoc->InitReelmap();
	pDoc->SetReelmap(ROT_NONE);
	pDoc->UpdateData();
}

void CGvisR2R_LaserView::InitReelmapUp()
{
	pDoc->InitReelmapUp();
	pDoc->SetReelmap(ROT_NONE);
	pDoc->UpdateData();
}

void CGvisR2R_LaserView::InitReelmapDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	pDoc->InitReelmapDn();
	pDoc->SetReelmap(ROT_NONE);
	pDoc->UpdateData();
}

 void CGvisR2R_LaserView::LoadMstInfo()
 {
	 BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

#ifdef TEST_MODE
	 pDoc->GetCamPxlRes();
	 if (IsLastJob(0)) // Up
	 {
		 pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			 pDoc->WorkingInfo.LastJob.sModelUp,
			 pDoc->WorkingInfo.LastJob.sLayerUp);
		 pDoc->m_Master[0].LoadMstInfo();
	 }
	 if (IsLastJob(1)) // Dn
	 {
		 pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			 pDoc->WorkingInfo.LastJob.sModelDn,
			 pDoc->WorkingInfo.LastJob.sLayerDn,
			 pDoc->WorkingInfo.LastJob.sLayerUp);
		 pDoc->m_Master[1].LoadMstInfo();
	 }
#else
	 pDoc->GetCamPxlRes();

	 if (IsLastJob(0)) // Up
	 {
		 pDoc->m_Master[0].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			 pDoc->WorkingInfo.LastJob.sModelUp,
			 pDoc->WorkingInfo.LastJob.sLayerUp);
		 pDoc->m_Master[0].LoadMstInfo();
		//pDoc->m_Master[0].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotUp);
	 }

	 if (IsLastJob(1)) // Dn
	 {
		 pDoc->m_Master[1].Init(pDoc->WorkingInfo.System.sPathCamSpecDir,
			 pDoc->WorkingInfo.LastJob.sModelUp,
			 //pDoc->WorkingInfo.LastJob.sModelDn,
			 pDoc->WorkingInfo.LastJob.sLayerDn,
			 pDoc->WorkingInfo.LastJob.sLayerUp);
		 pDoc->m_Master[1].LoadMstInfo();
		 //pDoc->m_Master[1].WriteStripPieceRegion_Text(pDoc->WorkingInfo.System.sPathOldFile, pDoc->WorkingInfo.LastJob.sLotDn);
	 }

#endif
	 // Reelmap ���� Loading.....
	 InitReelmap(); // Delete & New

	 if (m_pDlgMenu01)
	 {
		 m_pDlgMenu01->InitGL();
		 m_bDrawGL = TRUE;
		 m_pDlgMenu01->RefreshRmap();
		 m_pDlgMenu01->InitCadImg();
		 m_pDlgMenu01->SetPnlNum();
		 m_pDlgMenu01->SetPnlDefNum();
	 }

	 if (m_pDlgMenu02)
	 {
		 m_pDlgMenu02->ChgModelUp(); // PinImg, AlignImg�� Display��.
	 }

#ifndef TEST_MODE
	 if (m_pDlgMenu01)
		 m_pDlgMenu01->RedrawWindow();

	 DispMsg(_T("������ �ʱ�ȭ�մϴ�."), _T("�˸�"), RGB_GREEN, DELAY_TIME_MSG);
	 OpenReelmap();
#endif
	 SetPathAtBuf(); // Reelmap path�� ������.
	 LoadPcrFromBuf();

#ifdef TEST_MODE
#else
	 //nSrl = pDoc->GetLastSerial();
	 int nSrl = pDoc->GetLastShotMk();
	 SetMkFdLen();
	 if (nSrl >= 0)
	 {
		 if (bDualTest)
			 m_pDlgMenu01->SelMap(ALL);
		 else
			 m_pDlgMenu01->SelMap(UP);
	 }
#endif

 }

BOOL CGvisR2R_LaserView::IsPinMkData()
{
	if (pDoc->IsPinMkData())
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsPinData()
{
	return pDoc->IsPinData();
}

BOOL CGvisR2R_LaserView::CopyDefImg(int nSerial)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.55"));
		return 0;
	}

	return pDoc->CopyDefImg(nSerial);
}

BOOL CGvisR2R_LaserView::CopyDefImg(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.56"));
		return 0;
	}

	return pDoc->CopyDefImg(nSerial, sNewLot);
}

BOOL CGvisR2R_LaserView::CopyDefImgUp(int nSerial, CString sNewLot)
{
	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.57"));
		return 0;
	}

	return pDoc->CopyDefImgUp(nSerial, sNewLot);
}

BOOL CGvisR2R_LaserView::CopyDefImgDn(int nSerial, CString sNewLot)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (nSerial <= 0)
	{
		AfxMessageBox(_T("Serial Error.58"));
		return FALSE;
	}

	return pDoc->CopyDefImgDn(nSerial, sNewLot);
}

BOOL CGvisR2R_LaserView::MovePinPos()
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->MovePinPos();
}

BOOL CGvisR2R_LaserView::OnePointAlign(CfPoint &ptPnt)
{
	if (!m_pDlgMenu02)
		return FALSE;

	return m_pDlgMenu02->OnePointAlign(ptPnt);
}

BOOL CGvisR2R_LaserView::IsHomeDone(int nMsId)
{
	if (!m_pMotion)
		return FALSE;

	return m_pMotion->IsHomeDone(nMsId);
}

BOOL CGvisR2R_LaserView::GetAoiUpInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.59"));
		return 0;
	}

	return pDoc->GetAoiUpInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CGvisR2R_LaserView::GetAoiDnInfo(int nSerial, int *pNewLot, BOOL bFromBuf)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.60"));
		return 0;
	}

	return pDoc->GetAoiDnInfo(nSerial, pNewLot, bFromBuf);
}

BOOL CGvisR2R_LaserView::LoadMySpec()
{
	return pDoc->LoadMySpec();
}

CString CGvisR2R_LaserView::GetProcessNum()
{
	return pDoc->GetProcessNum();
}

BOOL CGvisR2R_LaserView::GetAoiUpOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiUpOffset(OfSt);
}

BOOL CGvisR2R_LaserView::GetAoiDnOffset(CfPoint &OfSt)
{
	return pDoc->GetAoiDnOffset(OfSt);
}

BOOL CGvisR2R_LaserView::GetMkOffset(CfPoint &OfSt)
{
	if (m_pDlgMenu02)
	{
		OfSt.x = m_pDlgMenu02->m_dMkFdOffsetX[0][0]; // -: ��ǰ ����, +: ��ǰ ����.
		OfSt.y = m_pDlgMenu02->m_dMkFdOffsetY[0][0]; // -: ��ǰ ����, +: ��ǰ ����.
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::IsAoiLdRun()
{
	return TRUE;

	BOOL bRtn = FALSE;
	if (m_pDlgMenu03)
		bRtn = m_pDlgMenu03->IsAoiLdRun();

	return bRtn;
}

BOOL CGvisR2R_LaserView::IsInitPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pTgtPos[0] - 2.0 || dCurrX > pTgtPos[0] + 2.0)
		return FALSE;
	if (dCurrY < pTgtPos[1] - 2.0 || dCurrY > pTgtPos[1] + 2.0)
		return FALSE;

	return TRUE;
}

void CGvisR2R_LaserView::MoveInitPos0(BOOL bWait)
{
	if (!m_pMotion)
		return;

	double pTgtPos[2];
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if(bWait)
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, WAIT);
		else
			m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel, fAcc, fAcc, ABS, NO_WAIT);
	}
}

void CGvisR2R_LaserView::LotEnd()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->LotEnd();
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiLotEnd(TRUE);

	m_bCont = FALSE;
	SetLotEd();

	MakeResultMDS();
}

void CGvisR2R_LaserView::TimWinker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	m_bBtnWinker[nId] = TRUE;
	if (!m_bTimBtnWinker)
	{
		m_bTimBtnWinker = TRUE;
		m_nDlyWinker[nId] = nDly;
		SetTimer(TIM_BTN_WINKER, 100, NULL);
	}
}

void CGvisR2R_LaserView::StopTimWinker(int nId) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
	m_bBtnWinker[nId] = FALSE;
	m_bTimBtnWinker = FALSE;
	for (int i = 0; i < 4; i++)
	{
		if (m_bBtnWinker[i])
			m_bTimBtnWinker = TRUE;
	}
}

void CGvisR2R_LaserView::Winker(int nId, int nDly) // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	if (nId == MN_RUN)
	{
		if (pView->m_pMpe)
		{
			m_bBtnWinker[nId] = TRUE;
			//IoWrite(_T("MB44015D"), 1); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
			pView->m_pMpe->Write(_T("MB44015D"), 1); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
		}
	}
	// 	DispBtnWinker(nDly);
#endif
}

void CGvisR2R_LaserView::ResetWinker() // 0:Ready, 1:Reset, 2:Run, 3:Stop
{
#ifdef USE_MPE
	//IoWrite(_T("MB44015D"), 0); // �ڵ� �ʱ� ��������(PC�� On/Off ��Ŵ, PLC�� ���������� ��ũ����, on->off�� �������� on, �ٽ� ��������ġ�� �������� off) - 20141017
	pView->m_pMpe->Write(_T("MB44015D"), 0);
#endif
	for (int i = 0; i<4; i++)
	{
		m_bBtnWinker[i] = FALSE;
		m_nCntBtnWinker[i] = FALSE;
	}
}

void CGvisR2R_LaserView::SetOrigin()
{
}

BOOL CGvisR2R_LaserView::IsSetLotEnd()
{
	if (m_nLotEndSerial > 0)
		return TRUE;
	return FALSE;
}

void CGvisR2R_LaserView::SetLotEnd(int nSerial)
{
	if (nSerial <= 0)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Serial Error.61"));
		return;
	}
	m_nLotEndSerial = nSerial;

	CString str;
	str.Format(_T("%d"), m_nLotEndSerial);
	DispStsBar(str, 0);
}

int CGvisR2R_LaserView::GetLotEndSerial()
{
	return m_nLotEndSerial; // ���̺�� �����ϴ� Serial.
}

BOOL CGvisR2R_LaserView::StartLive()
{
	if (StartLive0())	// && StartLive1())
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_LaserView::StartLive0()
{
	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StartLive();
#endif
	if (bRtn0)
		return TRUE;

	return  FALSE;
}

BOOL CGvisR2R_LaserView::StopLive()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	if (StopLive0())	// && StopLive1())
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_LaserView::StopLive0()
{
#ifdef TEST_MODE
	return TRUE;
#endif

	BOOL bRtn0 = FALSE;

#ifdef USE_VISION
	if (m_pVision[0])
		bRtn0 = m_pVision[0]->StopLive();
#endif
	if (bRtn0)
		return TRUE;

	return FALSE;
}

void CGvisR2R_LaserView::UpdateRst()
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->UpdateRst();
}

BOOL CGvisR2R_LaserView::IsChkTmpStop()
{
	if (pDoc->WorkingInfo.LastJob.bTempPause)
	{
		double dFdLen = GetMkFdLen();
		if (dFdLen >= _tstof(pDoc->WorkingInfo.LastJob.sTempPauseLen)*1000.0)
		{
			// 			m_dTempPauseLen += m_dTempPauseLen;
			pDoc->WorkingInfo.LastJob.bTempPause = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsVerify()
{
	BOOL bVerify = FALSE;
	int nSerial0 = m_nBufUpSerial[0];
	int nSerial1 = m_nBufUpSerial[1];
	int nPeriod = pDoc->WorkingInfo.LastJob.nVerifyPeriod;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
	{
		nSerial0 = m_nBufDnSerial[0];
		nSerial1 = m_nBufDnSerial[1];
	}


	if (pDoc->WorkingInfo.LastJob.bVerify)
	{
		double dFdLen = GetMkFdLen();
		double dVerifyLen = _tstof(pDoc->WorkingInfo.LastJob.sVerifyLen)*1000.0;
		if (dFdLen < dVerifyLen)
		{
			if (nSerial0 == 1 || nPeriod == 0 || nPeriod == 1 || nPeriod == 2 || m_bStopF_Verify)
			{
				m_bStopF_Verify = FALSE;
				bVerify = TRUE;
			}
			else
			{
				if (!(nSerial0 % nPeriod) || !(nSerial1 % nPeriod))
					bVerify = TRUE;
			}
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bVerify = FALSE;
			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();
		}
	}

	return bVerify;
}

int CGvisR2R_LaserView::GetVsBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotMk();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_LaserView::GetVsUpBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotUp();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

int CGvisR2R_LaserView::GetVsDnBufLastSerial()
{
	int nLastShot = pDoc->GetLastShotDn();
	if (nLastShot > 0 && m_bCont)
		return (nLastShot + 4);

	return 4;
}

BOOL CGvisR2R_LaserView::IsFixPcsUp(int nSerial)
{
	if (!pDoc->m_pReelMapUp)
		return FALSE;

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot;

	if (pDoc->m_pReelMapUp->IsFixPcs(nSerial, pCol, pRow, nTot))
	{
		int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / MAX_STRIP_NUM;

		sMsg.Format(_T("��� �����ҷ� �߻�"));
		for (int i = 0; i < nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			if (!(i % 5))
				str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			else
				str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);

			sMsg += str;
		}
		m_sFixMsg[0] = sMsg;
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsFixPcsDn(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return FALSE;

	if (!pDoc->m_pReelMapUp)
		return FALSE;

	int nMaxStrip;
#ifdef USE_CAM_MASTER
	nMaxStrip = pDoc->m_Master[0].GetStripNum(); // �� ��Ʈ���� ����
#else
	nMaxStrip = MAX_STRIP;
#endif

	CString sMsg = _T(""), str = _T("");
	int nStrip, pCol[2500], pRow[2500], nTot;

	if (pDoc->m_pReelMapDn->IsFixPcs(nSerial, pCol, pRow, nTot))
	{
		int nNodeX = pDoc->m_Master[0].m_pPcsRgn->nCol;
		int nNodeY = pDoc->m_Master[0].m_pPcsRgn->nRow;
		int nStPcsY = nNodeY / nMaxStrip;

		sMsg.Format(_T("�ϸ� �����ҷ� �߻�"));
		for (int i = 0; i < nTot; i++)
		{
			nStrip = int(pRow[i] / nStPcsY);
			if (!(i % 5))
				str.Format(_T("\r\n[%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);
			else
				str.Format(_T(" , [%d:%c-%d,%d]"), nSerial, 'A' + nStrip, pCol[i] + 1, (pRow[i] % nStPcsY) + 1);

			sMsg += str;
		}
		// 		TowerLamp(RGB_RED, TRUE);
		// 		Buzzer(TRUE, 0);
		// 		MsgBox(sMsg);
		m_sFixMsg[1] = sMsg;
		return TRUE;
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsReview()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_LaserView::IsReview0()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_LaserView::IsReview1()
{
	return (pDoc->WorkingInfo.LastJob.bReview);
}

BOOL CGvisR2R_LaserView::IsJogRtDn()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_LaserView::IsJogRtUp()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_LaserView::IsJogRtDn0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? TRUE : FALSE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

BOOL CGvisR2R_LaserView::IsJogRtUp0()
{
	BOOL bOn = FALSE;
#ifdef USE_MPE
	bOn = pDoc->m_pMpeIb[4] & (0x01 << 10) ? FALSE : TRUE;	// ��ŷ�� JOG ��ư(��)
#endif
	return bOn;
}

void CGvisR2R_LaserView::OpenShareUp(BOOL bOpen)
{
	m_bOpenShareUp = bOpen;
}

BOOL CGvisR2R_LaserView::IsOpenShareUp()
{
	return m_bOpenShareUp;
}

void CGvisR2R_LaserView::OpenShareDn(BOOL bOpen)
{
	m_bOpenShareDn = bOpen;
}

BOOL CGvisR2R_LaserView::IsOpenShareDn()
{
	return m_bOpenShareDn;
}

void CGvisR2R_LaserView::SwAoiEmg(BOOL bOn)
{
	if (m_pDlgMenu03)
		m_pDlgMenu03->SwAoiEmg(bOn);
	if (bOn)
	{
	}
	else
	{
	}
}

BOOL CGvisR2R_LaserView::IsVs()
{
	if (!m_bChkLastProcVs)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (GetAoiDnVsStatus())
				return TRUE;
			else if (GetAoiUpVsStatus())
				return TRUE;
		}
		else
		{
			if (GetAoiUpVsStatus())
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsVsUp()
{
	return GetAoiUpVsStatus();
}

BOOL CGvisR2R_LaserView::IsVsDn()
{
	return GetAoiDnVsStatus();
}

void CGvisR2R_LaserView::SetDummyUp()
{
	m_nDummy[0]++;
	if (m_nDummy[0] == 3)
	{
		if (!MakeDummyUp(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-2)"));
	}
	}
	else
	{
		if (!MakeDummyUp(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyUp(-1)"));
	}
	}
}

void CGvisR2R_LaserView::SetDummyDn()
{
	m_nDummy[1]++;
	if (m_nDummy[1] == 3)
	{
		if (!MakeDummyDn(-2))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-2)"));
	}
	}
	else
	{
		if (!MakeDummyDn(-1))
		{
			pView->ClrDispMsg();
			AfxMessageBox(_T("Error - MakeDummyDn(-1)"));
	}
	}
}

BOOL CGvisR2R_LaserView::MakeDummyUp(int nErr) // AOI ��� ����.
{
	int nSerial = m_nAoiLastSerial[0] + m_nDummy[0];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		m_nAoiLastSerial[0]);

	CDataFile *pDataFile = new CDataFile;

	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);
	char* pRtn = NULL;
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiUpVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareUp, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if(pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		pDoc->WorkingInfo.LastJob.sLotUp);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

BOOL CGvisR2R_LaserView::MakeDummyDn(int nErr) // AOI ��� ����.
{
	m_nAoiLastSerial[1] = m_nAoiLastSerial[0] - 3;
	int nSerial = m_nAoiLastSerial[0] + m_nDummy[1];
	FILE *fpPCR = NULL;

	CString sMsg, strRstPath, strRstPath2, sDummyRst;
	strRstPath.Format(_T("%s%s\\%s\\\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		m_nAoiLastSerial[1]);

	CDataFile *pDataFile = new CDataFile;
	char* pRtn = NULL;
	if (!pDataFile->Open(strRstPath))
	{
		sMsg.Format(_T("%s File not found."), strRstPath);
		pView->MsgBox(sMsg);
		delete pDataFile;
		return FALSE;
	}
	CString sLine, sRemain, sNewLine;
	sLine = pDataFile->GetLineString(1);
	int nPos = sLine.Find(',', 0);
	sLine.Delete(0, nPos);
	sNewLine.Format(_T("%d%s"), 0, sLine);
	pDataFile->ReplaceLine(1, sNewLine);

	strRstPath2.Format(_T("%s%s\\%s\\\\%s\\%04d.RST"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);
	fpPCR = fopen(pRtn = StringToChar(strRstPath2), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}
	fprintf(fpPCR, pRtn = StringToChar(pDataFile->GetAllString())); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR != NULL)
		fclose(fpPCR);

	delete pDataFile;


	CString sDummyPath;
	sDummyPath.Format(_T("%s%s\\%s\\\\%s\\%04d.PCR"), pDoc->WorkingInfo.System.sPathAoiDnVrsData,
		pDoc->WorkingInfo.LastJob.sModelUp,
		//pDoc->WorkingInfo.LastJob.sModelDn,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp,
		//pDoc->WorkingInfo.LastJob.sLotDn,
		nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); if (pRtn) delete pRtn; pRtn = NULL;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);



	sDummyPath.Format(_T("%s%04d.pcr"), pDoc->WorkingInfo.System.sPathVsShareDn, nSerial);

	fpPCR = fopen(pRtn = StringToChar(sDummyPath), "w+"); delete pRtn;
	if (fpPCR == NULL)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("TROUBLE_SAVE_PIECEOUT_VRS"), MB_ICONWARNING | MB_OK);
	}

	fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		pDoc->WorkingInfo.LastJob.sLotUp);
	//fprintf(fpPCR, "%04d,%s,%s,%s\n", nErr, pDoc->WorkingInfo.LastJob.sModelDn,
	//	pDoc->WorkingInfo.LastJob.sLayerDn,
	//	pDoc->WorkingInfo.LastJob.sLotDn);

	fprintf(fpPCR, "%04d\n", 0);

	fprintf(fpPCR, "%s", pRtn = StringToChar(_T("���ϸ�����"))); if (pRtn) delete pRtn; pRtn = NULL;
	fprintf(fpPCR, "\n");

	if (fpPCR != NULL)
		fclose(fpPCR);

	return TRUE;
}

void CGvisR2R_LaserView::SetAoiDummyShot(int nAoi, int nDummy)
{
	pDoc->AoiDummyShot[nAoi] = nDummy;
	switch (nAoi)
	{
	case 0:
		if (m_pMpe)
			m_pMpe->Write(_T("ML45068"), (long)nDummy);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
		break;
	case 1:
		if (m_pMpe)
			m_pMpe->Write(_T("ML45070"), (long)nDummy);	// �˻�� (��) �۾����� ���� Shot�� - 20141104
		break;
	}
}

int CGvisR2R_LaserView::GetAoiUpDummyShot()
{
	int nDummy = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CGvisR2R_LaserView::GetAoiDnDummyShot()
{
	int nDummy = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Dummy"), _T("Shot"), NULL, szData, sizeof(szData), sPath))
		nDummy = _tstoi(szData);
	else
		nDummy = 0;

	return nDummy;
}

int CGvisR2R_LaserView::GetAoiUpSerial()
{
	int nSerial = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CGvisR2R_LaserView::GetAoiUpVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiUpCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

int CGvisR2R_LaserView::GetAoiDnSerial()
{
	int nSerial = 0;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current Serial"), NULL, szData, sizeof(szData), sPath))
		nSerial = _tstoi(szData);
	else
		nSerial = 0;
	if (nSerial > 1)
		nSerial--;

	return nSerial;
}

BOOL CGvisR2R_LaserView::GetAoiDnVsStatus()
{
	BOOL bVsStatus = FALSE;
	//char szData[200];
	TCHAR szData[200];
	CString sPath = pDoc->WorkingInfo.System.sPathAoiDnCurrInfo;
	if (0 < ::GetPrivateProfileString(_T("Infomation"), _T("Current VS Status"), NULL, szData, sizeof(szData), sPath))
		bVsStatus = _tstoi(szData) > 0 ? TRUE : FALSE;

	return bVsStatus;
}

BOOL CGvisR2R_LaserView::IsDoneDispMkInfo()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsDoneDispMkInfo();
	return bRtn;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CGvisR2R_LaserView::DispDefImg()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nBreak = 0;

	switch (m_nStepTHREAD_DISP_DEF)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF++;
		m_nBufUpSerial[0] = m_nBufDnSerial[0] = _ttoi(pView->GetMkMenu01(_T("DispDefImg"), _T("SerialL")));
		m_nBufUpSerial[1] = m_nBufDnSerial[1] = _ttoi(pView->GetMkMenu01(_T("DispDefImg"), _T("SerialR")));

		if (bDualTest)
		{
			nSerial = m_nBufDnSerial[0];
			sNewLot = m_sNewLotDn;
		}
		else
		{
			nSerial = m_nBufUpSerial[0];
			sNewLot = m_sNewLotUp;
		}
		break;
	case 1:
		Sleep(300);
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 2:
		if (bDualTest)
			nSerial = m_nBufDnSerial[0];
		else
			nSerial = m_nBufUpSerial[0];

		if (IsDoneDispMkInfo())	 // Check �ҷ��̹��� Display End
		{
			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmap(nSerial, TRUE);	// Reelmap(��) Display Start
				SetSerialMkInfo(nSerial, TRUE);		// �ҷ��̹���(��) Display Start
			}
			else if (ChkLastProc() && nSerial < 1)
			{
				SetSerialReelmap(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
				SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
			}
			else
			{
				SetFixPcs(nSerial);
				SetSerialReelmap(nSerial);	// Reelmap(��) Display Start
				SetSerialMkInfo(nSerial);	// �ҷ��̹���(��) Display Start
			}

			m_nStepTHREAD_DISP_DEF++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfo())	 // Check �ҷ��̹���(��) Display End
		{
			if (bDualTest)
				nSerial = m_nBufDnSerial[1];
			else
				nSerial = m_nBufUpSerial[1];

			if (nSerial > 0)
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
					SetSerialMkInfo(nSerial, TRUE);	// �ҷ��̹���(��) Display Start
				else
					SetSerialMkInfo(nSerial);		// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (ChkLastProc())
					SetSerialMkInfo(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			m_nStepTHREAD_DISP_DEF++;
		}
		break;
	case 4:
		if (bDualTest)
			nSerial = m_nBufDnSerial[1];
		else
			nSerial = m_nBufUpSerial[1];

		if (nSerial > 0)
		{
			m_nStepTHREAD_DISP_DEF++;

			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmap(nSerial, TRUE);	// Reelmap(��) Display Start
			}
			else
			{
				SetFixPcs(nSerial);
				SetSerialReelmap(nSerial);			// Reelmap(��) Display Start
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF++;
				SetSerialReelmap(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			else
			{
				m_nStepTHREAD_DISP_DEF++;
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF++;
		break;

	case 11:
		if (IsDoneDispMkInfo() && IsRun())	 // Check �ҷ��̹���(��) Display End
			m_nStepTHREAD_DISP_DEF++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF = FALSE;
		break;
		// CopyDefImg End ============================================
	}

}

BOOL CGvisR2R_LaserView::IsSameUpDnLot()
{
	if (pDoc->Status.PcrShare[0].sLot == pDoc->Status.PcrShare[1].sLot)
		return TRUE;

	return FALSE;
}

int CGvisR2R_LaserView::GetAutoStep()
{
	return m_nStepAuto;
}

void CGvisR2R_LaserView::MoveMkInitPos()
{
	MoveMk0InitPos();
}

void CGvisR2R_LaserView::MoveMk0InitPos()
{
	double pTgtPos[2];
	pTgtPos[1] = _tstof(pDoc->WorkingInfo.Motion.sStPosY[0]);
	pTgtPos[0] = _tstof(pDoc->WorkingInfo.Motion.sStPosX[0]);
	double dCurrX = pView->m_dEnc[AXIS_X0];
	double dCurrY = pView->m_dEnc[AXIS_Y0];

	double fLen, fVel, fAcc, fJerk;
	fLen = sqrt(((pTgtPos[0] - dCurrX) * (pTgtPos[0] - dCurrX)) + ((pTgtPos[1] - dCurrY) * (pTgtPos[1] - dCurrY)));
	if (fLen > 0.001)
	{
		pView->m_pMotion->GetSpeedProfile0(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
		if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
		{
			if (!pView->m_pMotion->Move0(MS_X0Y0, pTgtPos, fVel / 2.0, fAcc / 2.0, fAcc / 2.0))
				AfxMessageBox(_T("Move X0Y0 Error..."));
		}
	}
}


BOOL CGvisR2R_LaserView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message != WM_KEYDOWN)
		return CFormView::PreTranslateMessage(pMsg);

	if ((pMsg->lParam & 0x40000000) == 0)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_ESCAPE:
			//m_bEscape = TRUE;
			return TRUE;
		case 'S':
		case 's':
			if (GetKeyState(VK_CONTROL) < 0) // Ctrl Ű�� ������ ����
			{
				WINDOWPLACEMENT wndPlace;
				AfxGetMainWnd()->GetWindowPlacement(&wndPlace);
				wndPlace.showCmd |= SW_MAXIMIZE;
				AfxGetMainWnd()->SetWindowPlacement(&wndPlace);
			}
			break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}


int CGvisR2R_LaserView::MyPassword(CString strMsg, int nCtrlId)
{
	CDlgMyPassword dlg1(this);
	dlg1.SetMsg(strMsg, nCtrlId);
	dlg1.DoModal();
	return (dlg1.m_nRtnVal);

}

BOOL CGvisR2R_LaserView::ReloadRst()
{
	return TRUE;

	double dRatio = 0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On Reloading Reelmap."));
	dlg.Create(sVal);

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	BOOL bRtn[5];
	if (pDoc->m_pReelMap)
		bRtn[0] = pDoc->m_pReelMap->ReloadRst();
	//dlg.SetPos(1);
	if (pDoc->m_pReelMapUp)
		bRtn[1] = pDoc->m_pReelMapUp->ReloadRst();
	//dlg.SetPos(2);
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			bRtn[2] = pDoc->m_pReelMapDn->ReloadRst();
		//dlg.SetPos(3);
		if (pDoc->m_pReelMapAllUp)
			bRtn[3] = pDoc->m_pReelMapAllUp->ReloadRst();
		//dlg.SetPos(4);
		if (pDoc->m_pReelMapAllDn)
			bRtn[4] = pDoc->m_pReelMapAllDn->ReloadRst();
		//dlg.SetPos(5);
	}

	int nRatio[5] = { 0 };
	BOOL bDone[5] = { 0 };
	int nTo = 0;
	if (bDualTest)
		nTo = 500; //[%]
	else
		nTo = 200; //[%]

	dlg.SetRange(0, nTo);

	for (int nProc = 0; nProc < nTo;)
	{
		if (pDoc->m_pReelMap)
		{
			nRatio[0] = pDoc->m_pReelMap->GetProgressReloadRst();
			bDone[0] = pDoc->m_pReelMap->IsDoneReloadRst();
		}
		else
			bDone[0] = TRUE;
		if (!bRtn[0])
			bDone[0] = TRUE;

		if (pDoc->m_pReelMapUp)
		{
			nRatio[1] = pDoc->m_pReelMapUp->GetProgressReloadRst();
			bDone[1] = pDoc->m_pReelMapUp->IsDoneReloadRst();
		}
		else
			bDone[1] = TRUE;
		if (!bRtn[1])
			bDone[1] = TRUE;

		if (bDualTest)
		{
			if (pDoc->m_pReelMapDn)
			{
				nRatio[2] = pDoc->m_pReelMapDn->GetProgressReloadRst();
				bDone[2] = pDoc->m_pReelMapDn->IsDoneReloadRst();
			}
			else
				bDone[2] = TRUE;
			if (!bRtn[2])
				bDone[2] = TRUE;

			if (pDoc->m_pReelMapAllUp)
			{
				nRatio[3] = pDoc->m_pReelMapAllUp->GetProgressReloadRst();
				bDone[3] = pDoc->m_pReelMapAllUp->IsDoneReloadRst();
			}
			else
				bDone[3] = TRUE;
			if (!bRtn[3])
				bDone[3] = TRUE;

			if (pDoc->m_pReelMapAllDn)
			{
				nRatio[4] = pDoc->m_pReelMapAllDn->GetProgressReloadRst();
				bDone[4] = pDoc->m_pReelMapAllDn->IsDoneReloadRst();
			}
			else
				bDone[4] = TRUE;
			if (!bRtn[4])
				bDone[4] = TRUE;

		}
		else
		{
			bDone[2] = TRUE;
			bDone[3] = TRUE;
			bDone[4] = TRUE;
		}

		nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4];

		if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4])
			break;
		else
		{
			dlg.SetPos(nProc);
			Sleep(30);
		}
	}

	dlg.DestroyWindow();

	if (bDualTest)
	{
		for (int i = 0; i < 5; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		return ReloadRstInner();
	}

	return TRUE;
}

void CGvisR2R_LaserView::ReloadRstUp()
{
}

void CGvisR2R_LaserView::ReloadRstAllUp()
{
}

void CGvisR2R_LaserView::ReloadRstDn()
{
}

void CGvisR2R_LaserView::ReloadRstAllDn()
{
}

BOOL CGvisR2R_LaserView::ReloadRst(int nSerial)
{
	return TRUE;
}

void CGvisR2R_LaserView::ReloadRstUpInner()
{
}

void CGvisR2R_LaserView::ReloadRstAllUpInner()
{
}

void CGvisR2R_LaserView::ReloadRstDnInner()
{
}

BOOL CGvisR2R_LaserView::OpenReelmapFromBuf(int nSerial)
{
	return TRUE;
}

void CGvisR2R_LaserView::OpenReelmap()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->Open();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->Open();
	}
}

void CGvisR2R_LaserView::OpenReelmapUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->Open();
	}

	if (pDoc->m_pReelMap)
	{
		if (bDualTest)
		{
			if (pDoc->m_pReelMap->m_nLayer == RMAP_UP || pDoc->m_pReelMap->m_nLayer == RMAP_ALLUP)
				pDoc->m_pReelMap->Open();
		}
		else
		{
			if (pDoc->m_pReelMap->m_nLayer == RMAP_UP)
				pDoc->m_pReelMap->Open();
		}
	}
}

void CGvisR2R_LaserView::OpenReelmapDn()
{
	return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->Open();
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->Open();
}

void CGvisR2R_LaserView::EStop()
{
	if (m_pMotion)
	{
		m_pMotion->EStop(MS_X0Y0);
		Sleep(30);
		ResetMotion(MS_X0Y0);
		Sleep(30);
		AfxMessageBox(_T("X�� �浹 ������ ���� �����Դϴ�."));
	}
}

BOOL CGvisR2R_LaserView::IsRunAxisX()
{
	if (m_pMotion->IsMotionDone(MS_X0))// && m_pMotion->IsMotionDone(MS_X1))
		return FALSE;
	return TRUE;
}

void CGvisR2R_LaserView::StopAllMk()
{
	// Mk0
	m_nStepMk[2] = MK_END;

	// Mk1
	m_nStepMk[3] = MK_END;
}

void CGvisR2R_LaserView::InitPLC()
{
#ifdef USE_MPE
	long lData;
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sTotalReelDist) * 1000.0);
	m_pMpe->Write(_T("ML45000"), lData);	// ��ü Reel ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sSeparateDist) * 1000.0);
	m_pMpe->Write(_T("ML45002"), lData);	// Lot �и� ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sCuttingDist) * 1000.0);
	m_pMpe->Write(_T("ML45004"), lData);	// Lot �и� �� ������ġ (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Lot.sStopDist) * 1000.0);
	m_pMpe->Write(_T("ML45006"), lData);	// �Ͻ����� ���� (���� M * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiTq) * 1000.0);
	m_pMpe->Write(_T("ML45042"), lData);	// �˻�� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkTq) * 1000.0);
	m_pMpe->Write(_T("ML45044"), lData);	// ��ŷ�� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sEngraveTq) * 1000.0);
	m_pMpe->Write(_T("ML45050"), lData);	// ���κ� Tension ���� ��ũ�� (���� Kgf * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen) * 1000.0);
	m_pMpe->Write(_T("ML45032"), lData);	// �� �ǳ� ���� (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdVel) * 1000.0);
	m_pMpe->Write(_T("ML45034"), lData);	// �� �ǳ� Feeding �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkJogVel) * 1000.0);
	m_pMpe->Write(_T("ML45038"), lData);	// ���Ӱ��� �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiJogAcc) * 1000.0);
	m_pMpe->Write(_T("ML45040"), lData);	// ���Ӱ��� ���ӵ� (���� mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdAcc) * 1000.0);
	m_pMpe->Write(_T("ML45036"), lData);	// �� �ǳ� Feeding ���ӵ� (���� mm/s^2 * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sAoiFdLead) * 1000.0);
	m_pMpe->Write(_T("ML45012"), lData);	// �˻�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	m_pMpe->Write(_T("ML45020"), lData);	// ���κ� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sMkFdLead) * 1000.0);
	m_pMpe->Write(_T("ML45014"), lData);	// ��ŷ�� Feeding �ѷ� Lead Pitch (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sStBufPos) * 1000.0);
	pView->m_pMpe->Write(_T("ML45016"), lData);	// ���� ���� ���� �ѷ� �ʱ���ġ(���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist) * 1000.0);
	pView->m_pMpe->Write(_T("ML45008"), lData);	// AOI(��)���� ��ŷ���� �Ÿ� (���� mm * 1000)
	lData = (long)(_tstoi(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * 1000);
	pView->m_pMpe->Write(_T("ML45010"), lData);	// AOI(��)���� AOI(��) Shot�� (���� Shot�� * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) * 1000.0);
	pView->m_pMpe->Write(_T("ML45018"), lData);	// ���κο��� AOI(��)���� �Ÿ� (���� mm * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetVel) * 1000.0);
	m_pMpe->Write(_T("ML45060"), lData);	// 2D ���ڵ� ��������ġ���� Feeding �ӵ� (���� mm/sec * 1000)
	lData = (long)(_tstof(pDoc->WorkingInfo.Motion.sFdBarcodeOffsetAcc) * 1000.0);
	m_pMpe->Write(_T("ML45062"), lData);	// 2D ���ڵ� ��������ġ���� Feeding ���ӵ� (���� mm/s^2 * 1000)

	lData = (long)(_tstof(pDoc->WorkingInfo.LastJob.sUltraSonicCleannerStTim) * 100.0);
	m_pMpe->Write(_T("MW05940"), lData);	// AOI_Dn (���� [��] * 100) : 1 is 10 mSec.
	m_pMpe->Write(_T("MW05942"), lData);	// AOI_Up (���� [��] * 100) : 1 is 10 mSec.
#endif
}

BOOL CGvisR2R_LaserView::SetCollision(double dCollisionMargin)
{
	BOOL bRtn = FALSE;
	if (m_pMotion)
		bRtn = m_pMotion->SetCollision(dCollisionMargin);
	return bRtn;
}

void CGvisR2R_LaserView::MpeWrite()
{
	for (int i = TOT_M_IO - 1; i >= 0; i--)
	{
		if (pDoc->m_pIo[i].nIdx > -1)
		{
			if (m_pMpe)
				m_pMpe->Write(pDoc->m_pIo[i].sAddr, pDoc->m_pIo[i].lData);
			pDoc->m_pIo[i].nIdx = -1;
			pDoc->m_pIo[i].sAddr = _T("");
			pDoc->m_pIo[i].lData = 0;
		}
	}
}

void CGvisR2R_LaserView::IoWrite(CString sMReg, long lData)
{
	int i = 0;
	for (i = 0; i < TOT_M_IO; i++)
	{
		if (pDoc->m_pIo[i].nIdx < 0)
		{
			pDoc->m_pIo[i].nIdx = i;
			pDoc->m_pIo[i].sAddr = sMReg;
			pDoc->m_pIo[i].lData = lData;
			break;
		}
	}
	if (i >= TOT_M_IO)
	{
		pView->ClrDispMsg();
		AfxMessageBox(_T("Not enought TOT_M_IO Num!!!"));
	}
}

BOOL CGvisR2R_LaserView::IsRdyTest()
{
	BOOL bOn0 = IsRdyTest0();
	BOOL bOn1 = IsRdyTest1();

	if (bOn0 && bOn1)
		return TRUE;
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsRdyTest0()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[10] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X432B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[10] & (0x01 << 9)) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F

	if (bOn0 && bOn1)
		return TRUE;
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsRdyTest1()
{
#ifdef USE_MPE
	BOOL bOn0 = (pDoc->m_pMpeIb[14] & (0x01 << 11)) ? TRUE : FALSE;		// �˻�� �� �ڵ� ���� <-> X442B I/F
	BOOL bOn1 = (pDoc->m_pMpeIb[14] & (0x01 << 9)) ? TRUE : FALSE;		// �˻�� �� ���̺� ���� �Ϸ� <-> X4329 I/F
	double dCurPosMkFd = (double)pDoc->m_pMpeData[0][0];				// ��ŷ�� Feeding ���ڴ� ��(���� mm )
	double dTgtFd = _tstof(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot) * _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist);
	if (dCurPosMkFd > dTgtFd - _tstof(pDoc->WorkingInfo.Motion.sAoiFdDist) / 2.0)
	{
		BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
		if (bDualTest)
		{
			if (bOn0 && bOn1)
				return TRUE;
		}
		else
		{
			if (bOn0)
				return TRUE;
		}
	}
#endif
	return FALSE;
}

void CGvisR2R_LaserView::SetPlcParam()
{
}

void CGvisR2R_LaserView::GetPlcParam()
{
	if (!m_pMpe)
		return;
#ifdef USE_MPE
	// Main
	pDoc->BtnStatus.Main.Ready = m_pMpe->Read(_T("MB005503")) ? TRUE : FALSE;
	pDoc->BtnStatus.Main.Run = m_pMpe->Read(_T("MB005501")) ? TRUE : FALSE;
	pDoc->BtnStatus.Main.Reset = m_pMpe->Read(_T("MB005504")) ? TRUE : FALSE;
	pDoc->BtnStatus.Main.Stop = m_pMpe->Read(_T("MB005502")) ? TRUE : FALSE;
	pDoc->BtnStatus.Main.Auto = m_pMpe->Read(_T("MB005505")) ? TRUE : FALSE;	// ��ŷ�� �ڵ�/���� (ON)
	pDoc->BtnStatus.Main.Manual = m_pMpe->Read(_T("MB005505")) ? FALSE : TRUE;	// ��ŷ�� �ڵ�/���� (OFF)

																				// TorqueMotor
	pDoc->BtnStatus.Tq.Mk = m_pMpe->Read(_T("MB440155")) ? TRUE : FALSE;
	pDoc->BtnStatus.Tq.Aoi = m_pMpe->Read(_T("MB440156")) ? TRUE : FALSE;
	pDoc->BtnStatus.Tq.Eng = m_pMpe->Read(_T("MB440154")) ? TRUE : FALSE;

	// InductionMotor
	pDoc->BtnStatus.Induct.Rc = m_pMpe->Read(_T("MB44017D")) ? TRUE : FALSE;	//pView->SetTwoMetal(FALSE, TRUE);	// One Metal IDC_CHK_68		
	pDoc->BtnStatus.Induct.Uc = m_pMpe->Read(_T("MB44017C")) ? TRUE : FALSE;	//pView->SetTwoMetal(TRUE, TRUE);	// Two Metal IDC_CHK_69

																				// Core150mm
	pDoc->BtnStatus.Core150.Rc = m_pMpe->Read(_T("MB44017E")) ? TRUE : FALSE;	// SetCore150mmRecoiler(TRUE);	// Recoiler IDC_CHK_70	
	pDoc->BtnStatus.Core150.Uc = m_pMpe->Read(_T("MB44017F")) ? TRUE : FALSE;	// SetCore150mmUncoiler(TRUE);	// Uncoiler IDC_CHK_71	

																				// Etc
																				//pDoc->BtnStatus.Etc.EmgAoi = m_pMpe->Read(_T("")) ? TRUE : FALSE;

																				// Recoiler
	pDoc->BtnStatus.Rc.Relation = m_pMpe->Read(_T("MB005801")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.FdCw = m_pMpe->Read(_T("MB00580C")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.FdCcw = m_pMpe->Read(_T("MB00580D")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.ReelChuck = m_pMpe->Read(_T("MB00580B")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.DcRlUpDn = m_pMpe->Read(_T("MB005802")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.ReelJoinL = m_pMpe->Read(_T("MB005805")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.ReelJoinR = m_pMpe->Read(_T("MB005806")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.ReelJoinVac = m_pMpe->Read(_T("MB00580F")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.PprChuck = m_pMpe->Read(_T("MB005808")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.PprCw = m_pMpe->Read(_T("MB005809")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.PprCcw = m_pMpe->Read(_T("MB00580A")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.Rewine = m_pMpe->Read(_T("MB005803")) ? TRUE : FALSE;
	pDoc->BtnStatus.Rc.RewineReelPpr = m_pMpe->Read(_T("MB005804")) ? TRUE : FALSE;

	// Punch
	pDoc->BtnStatus.Mk.Relation = m_pMpe->Read(_T("MB005511")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.FdCw = m_pMpe->Read(_T("MB005513")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.FdCcw = m_pMpe->Read(_T("MB005514")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.FdVac = m_pMpe->Read(_T("MB005515")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.PushUp = m_pMpe->Read(_T("MB005516")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.TblBlw = m_pMpe->Read(_T("MB005512")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.TblVac = m_pMpe->Read(_T("MB005517")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.FdClp = m_pMpe->Read(_T("MB005519")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.TqClp = m_pMpe->Read(_T("MB00551A")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.LsrPt = m_pMpe->Read(_T("MB005518")) ? TRUE : FALSE;
	pDoc->BtnStatus.Mk.DcRSol = m_pMpe->Read(_T("MB00551B")) ? TRUE : FALSE;

	// AOIDn
	pDoc->BtnStatus.AoiDn.Relation = m_pMpe->Read(_T("MB005701")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.FdCw = m_pMpe->Read(_T("MB005703")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.FdCcw = m_pMpe->Read(_T("MB005704")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.FdVac = m_pMpe->Read(_T("MB005705")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.PushUp = m_pMpe->Read(_T("MB005706")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.TblBlw = m_pMpe->Read(_T("MB005702")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.TblVac = m_pMpe->Read(_T("MB005707")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.FdClp = m_pMpe->Read(_T("MB005709")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.TqClp = m_pMpe->Read(_T("MB00570A")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.LsrPt = m_pMpe->Read(_T("MB005708")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiDn.VelSonicBlw = m_pMpe->Read(_T("MB44014F")) ? TRUE : FALSE;

	// AOIUp
	pDoc->BtnStatus.AoiUp.Relation = m_pMpe->Read(_T("MB005601")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.FdCw = m_pMpe->Read(_T("MB005603")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.FdCcw = m_pMpe->Read(_T("MB005604")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.FdVac = m_pMpe->Read(_T("MB005605")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.PushUp = m_pMpe->Read(_T("MB005606")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.TblBlw = m_pMpe->Read(_T("MB005602")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.TblVac = m_pMpe->Read(_T("MB005607")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.FdClp = m_pMpe->Read(_T("MB005609")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.TqClp = m_pMpe->Read(_T("MB00560A")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	pDoc->BtnStatus.AoiUp.LsrPt = m_pMpe->Read(_T("MB005608")) ? TRUE : FALSE;

	// Engrave
	//pDoc->BtnStatus.Eng.Relation = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.FdCw = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.FdCcw = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.FdVac = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.PushUp = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.TblBlw = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.TblVac = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.FdClp = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.TqClp = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.MvOne = m_pMpe->Read(_T("MB440151")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.LsrPt = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	//pDoc->BtnStatus.Eng.DcRSol = m_pMpe->Read(_T("")) ? TRUE : FALSE;
	pDoc->BtnStatus.Eng.VelSonicBlw = m_pMpe->Read(_T("MB44014E")) ? TRUE : FALSE;

	// Uncoiler
	pDoc->BtnStatus.Uc.Relation = m_pMpe->Read(_T("MB005401")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.FdCw = m_pMpe->Read(_T("MB00540C")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.FdCcw = m_pMpe->Read(_T("MB00540D")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.ReelChuck = m_pMpe->Read(_T("MB00540B")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.DcRlUpDn = m_pMpe->Read(_T("MB005402")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.ReelJoinL = m_pMpe->Read(_T("MB005405")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.ReelJoinR = m_pMpe->Read(_T("MB005406")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.ReelJoinVac = m_pMpe->Read(_T("MB00540F")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.PprChuck = m_pMpe->Read(_T("MB005408")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.PprCw = m_pMpe->Read(_T("MB005409")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.PprCcw = m_pMpe->Read(_T("MB00540A")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.ClRlUpDn = m_pMpe->Read(_T("MB005403")) ? TRUE : FALSE;
	pDoc->BtnStatus.Uc.ClRlPshUpDn = m_pMpe->Read(_T("MB005404")) ? TRUE : FALSE;
#endif
}

void CGvisR2R_LaserView::InitIoWrite()
{
}


BOOL CGvisR2R_LaserView::MoveAlign0(int nPos)
{
	if (!m_pMotion)
		return FALSE;

	if (m_pDlgMenu02)
		m_pDlgMenu02->SetLight();

	if (m_pMotion->m_dPinPosY[0] > -5.0 && m_pMotion->m_dPinPosX[0] > -5.0)
	{
		double dCurrX = m_dEnc[AXIS_X0];
		double dCurrY = m_dEnc[AXIS_Y0];

		double pPos[2];
		if (nPos == 0)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X0 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y0 + m_pMotion->m_dPinPosY[0];
		}
		else if (nPos == 1)
		{
			pPos[0] = pDoc->m_Master[0].m_stAlignMk.X1 + m_pMotion->m_dPinPosX[0];
			pPos[1] = pDoc->m_Master[0].m_stAlignMk.Y1 + m_pMotion->m_dPinPosY[0];
		}

		double fLen, fVel, fAcc, fJerk;
		fLen = sqrt(((pPos[0] - dCurrX) * (pPos[0] - dCurrX)) + ((pPos[1] - dCurrY) * (pPos[1] - dCurrY)));
		if (fLen > 0.001)
		{
			pView->m_pMotion->GetSpeedProfile(TRAPEZOIDAL, AXIS_X0, fLen, fVel, fAcc, fJerk);
			if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
			{
				if (!pView->m_pMotion->Move(MS_X0Y0, pPos, fVel, fAcc, fAcc, ABS, NO_WAIT))
				{
					pView->ClrDispMsg();
					AfxMessageBox(_T("Error - Move MoveAlign0 ..."));
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

void CGvisR2R_LaserView::SetLastProc()
{

	if (m_pDlgMenu01)
		m_pDlgMenu01->SetLastProc();
}

BOOL CGvisR2R_LaserView::IsLastProc()
{
	BOOL bRtn = FALSE;
	if (m_pDlgMenu01)
		bRtn = m_pDlgMenu01->IsLastProc();
	else
		bRtn = FALSE;

	return bRtn;
}

BOOL CGvisR2R_LaserView::IsLastJob(int nAoi) // 0 : AOI-Up , 1 : AOI-Dn , 2 : AOI-UpDn
{
	switch (nAoi)
	{
	case 0: // AOI-Up
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		break;
	case 1: // AOI-Dn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	case 2: // AOI-UpDn
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerUp.IsEmpty())
			return FALSE;
		if (pDoc->WorkingInfo.System.sPathCamSpecDir.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sModelUp.IsEmpty() ||
			pDoc->WorkingInfo.LastJob.sLayerDn.IsEmpty())
			return FALSE;
		break;
	}

	return TRUE;
}

void CGvisR2R_LaserView::MonDispMain()
{
	CString sDisp = pDoc->GetMonDispMain();

	if (sDisp == _T("������") && m_sMonDisp != _T("������"))
	{		
		DispMain(_T("������"), RGB_GREEN);
	}

	if (sDisp == _T("�����غ�") && m_sMonDisp != _T("�����غ�"))
	{
		DispMain(_T("�����غ�"), RGB_GREEN);
	}

	if (sDisp == _T("������") && m_sMonDisp != _T("������"))
	{
		DispMain(_T("������"), RGB_GREEN);
	}

	if (sDisp == _T("���˻�") && m_sMonDisp != _T("���˻�"))
	{
		DispMain(_T("���˻�"), RGB_GREEN);
	}

	if (sDisp == _T("�ܸ�˻�") && m_sMonDisp != _T("�ܸ�˻�"))
	{
		DispMain(_T("�ܸ�˻�"), RGB_GREEN);
	}

	if (sDisp == _T("�� ��") && m_sMonDisp != _T("�� ��"))
	{
		pView->DispStsBar(_T("����-44"), 0);
		DispMain(_T("�� ��"), RGB_RED);
	}
}

void CGvisR2R_LaserView::MonPlcAlm()
{
	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		if (pDoc->m_sAlmMsg != pDoc->m_sPrevAlmMsg)
		{
			pDoc->m_sPrevAlmMsg = pDoc->m_sAlmMsg;
			CycleStop();
		}
	}
}

void CGvisR2R_LaserView::MonMsgBox()
{
	if (!pDoc->m_sMsgBox.IsEmpty())
	{
		if (pDoc->m_sMsgBox != pDoc->m_sPrevMsgBox)
		{
			pDoc->m_sPrevMsgBox = pDoc->m_sMsgBox;
			CycleStop();
		}
	}
}

void CGvisR2R_LaserView::PlcAlm(BOOL bMon, BOOL bClr)
{
}

void CGvisR2R_LaserView::FindAlarm()
{
}

void CGvisR2R_LaserView::ResetMonAlm()
{
}

void CGvisR2R_LaserView::ClrAlarm()
{
	if (!pDoc->m_sAlmMsg.IsEmpty())
	{
		pDoc->m_sAlmMsg = _T("");
		pDoc->m_sIsAlmMsg = _T("");
		pDoc->m_sPrevAlmMsg = _T("");
	}
}

void CGvisR2R_LaserView::ResetClear()
{
}

void CGvisR2R_LaserView::ChkTempStop(BOOL bChk)
{
	if (bChk)
	{
		if (!m_bTIM_CHK_TEMP_STOP)
		{
			m_bTIM_CHK_TEMP_STOP = TRUE;
			SetTimer(TIM_CHK_TEMP_STOP, 100, NULL);
		}
	}
	else
	{
		m_bTIM_CHK_TEMP_STOP = FALSE;
	}
}

void CGvisR2R_LaserView::ChgLot()
{
	pDoc->WorkingInfo.LastJob.sLotUp = pDoc->Status.PcrShare[0].sLot;
	pDoc->SetModelInfoUp();

	pDoc->WorkingInfo.LastJob.sLotDn = pDoc->Status.PcrShare[1].sLot;
	pDoc->SetModelInfoDn();

	SetPathAtBuf();
}

void CGvisR2R_LaserView::LoadPcrFromBuf()
{
	return;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString str, sTemp;

	if (ChkBufUp(m_pBufSerial[0], m_nBufTot[0]))
	{
		for (int i = 0; i < m_nBufTot[0]; i++)
		{
			LoadPcrUp(m_pBufSerial[0][i]);
			if (!bDualTest)
				UpdateReelmap(m_pBufSerial[0][i]);
		}
	}

	if (bDualTest)
	{
		if (ChkBufDn(m_pBufSerial[1], m_nBufTot[1]))
		{
			for (int i = 0; i < m_nBufTot[1]; i++)
			{
				LoadPcrDn(m_pBufSerial[1][i]);
				UpdateReelmap(m_pBufSerial[1][i]); // After inspect bottom side.
			}
		}
	}
}

void CGvisR2R_LaserView::SetPathAtBuf()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetPathAtBuf();
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetPathAtBuf();
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetPathAtBuf();
		if (pDoc->m_pReelMapAllDn)
			pDoc->m_pReelMapAllDn->SetPathAtBuf();
	}

	if (pDoc->GetTestMode() == MODE_OUTER)
	{
		if (pDoc->m_pReelMapIts)
			pDoc->m_pReelMapIts->SetPathAtBuf();
	}
}

void CGvisR2R_LaserView::SetPathAtBufUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetPathAtBuf();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->SetPathAtBuf();
	}
}

void CGvisR2R_LaserView::SetPathAtBufDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapDn)
		pDoc->m_pReelMapDn->SetPathAtBuf();
	if (pDoc->m_pReelMapAllDn)
		pDoc->m_pReelMapAllDn->SetPathAtBuf();
}


void  CGvisR2R_LaserView::SetLotLastShot()
{
	pDoc->m_nLotLastShot = pDoc->m_nBufLastShot = int(_tstof(pDoc->WorkingInfo.LastJob.sLotSepLen)*1000.0 / _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen));
}

BOOL CGvisR2R_LaserView::IsMkStrip(int nStripIdx)
{
	if (!m_pDlgMenu01 || nStripIdx < 1 || nStripIdx > MAX_STRIP_NUM)
		return TRUE;

	return (m_pDlgMenu01->GetChkStrip(nStripIdx - 1));
}

void CGvisR2R_LaserView::CycleStop()
{
	m_bCycleStop = TRUE;
}

BOOL CGvisR2R_LaserView::ChkLotCutPos()
{
	if (pDoc->WorkingInfo.LastJob.bLotSep && pDoc->m_bDoneChgLot)
	{
		double dFdTotLen = GetMkFdLen();
		double dLotCutPos = _tstof(pDoc->WorkingInfo.LastJob.sLotCutPosLen)*1000.0;
		if (dFdTotLen >= dLotCutPos)
		{
			pDoc->WorkingInfo.LastJob.bLotSep = FALSE;
#ifdef USE_MPE
			pView->m_pMpe->Write(_T("MB440184"), 0);	// ��Ʈ�и����(PC�� On��Ű��, PC�� Ȯ���ϰ� Off��Ŵ)-20141031
														//pView->IoWrite(_T("MB440184"), 1);
#endif
			if (pDoc->m_pReelMap)
				pDoc->m_pReelMap->m_bUseLotSep = FALSE;

			::WritePrivateProfileString(_T("Last Job"), _T("Use Lot seperate"), _T("0"), PATH_WORKING_INFO);

			if (m_pDlgMenu01)
				m_pDlgMenu01->UpdateData();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::ChkStShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && !(m_nShareUpS % 2))
		{
			sMsg.Format(_T("AOI ����� �ø����� ¦���� �����Ͽ����ϴ�.\r\n- �ø��� ��ȣ: %d"), m_nShareUpS);
			MsgBox(sMsg);
			return FALSE;
		}
	}

	if (bDualTest)
	{
		if (pDoc->m_ListBuf[1].nTot == 0)
		{
			if (m_nShareDnS > 0 && !(m_nShareDnS % 2))
			{
				sMsg.Format(_T("AOI �ϸ��� �ø����� ¦���� �����Ͽ����ϴ�.\r\n- �ø��� ��ȣ: %d"), m_nShareDnS);
				MsgBox(sMsg);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ChkContShotNum()
{
	CString sMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (!m_pDlgFrameHigh)
		return FALSE;

	if (pDoc->m_ListBuf[0].nTot == 0)
	{
		if (m_nShareUpS > 0 && m_pDlgFrameHigh->m_nMkLastShot + 1 != m_nShareUpS)
		{
			sMsg.Format(_T("AOI ����� ����Shot(%d)�� ������Shot(%d)�� �ҿ����Դϴ�.\r\n��� �����Ͻðڽ��ϱ�?"), m_nShareUpS, m_pDlgFrameHigh->m_nMkLastShot);
			if (IDNO == MsgBox(sMsg, 0, MB_YESNO))
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_LaserView::SetFixPcs(int nSerial)
{
	return;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->SetFixPcs(nSerial);
	if (bDualTest)
	{
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->SetFixPcs(nSerial);
	}
}

void CGvisR2R_LaserView::MakeResultMDS()
{
	CString sPath, strMsg;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	pDoc->WorkingInfo.LastJob.sProcessNum = pDoc->GetProcessNum();
	pDoc->UpdateProcessNum(pDoc->WorkingInfo.LastJob.sProcessNum);

	if (m_pDlgMenu05)
	{
		m_pDlgMenu05->m_sModel = pDoc->WorkingInfo.LastJob.sModelUp;
		m_pDlgMenu05->m_sLot = pDoc->WorkingInfo.LastJob.sLotUp;
		m_pDlgMenu05->m_sLayer = pDoc->WorkingInfo.LastJob.sLayerUp;
		m_pDlgMenu05->m_sProcessNum = pDoc->WorkingInfo.LastJob.sProcessNum;
		sPath.Format(_T("%s%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp);

		if (bDualTest)
			m_pDlgMenu05->m_sRmapPath.Format(_T("%s\\ReelMapDataAll.txt"), sPath);
		else
			m_pDlgMenu05->m_sRmapPath.Format(_T("%s\\ReelMapDataUp.txt"), sPath);

		m_pDlgMenu05->MakeResult();

		MakeResult();
		MakeSapp3();
	}
	else
	{
		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
	}

	RemakeReelmap();
}

void CGvisR2R_LaserView::MakeResult()
{
	// TODO: Add your control notification handler code here

	// File Save......
	CString strDestPath;
	strDestPath.Format(_T("%s%s\\%s\\%s.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		_T("Result"));

	CFileFind cFile;
	if (cFile.FindFile(strDestPath))
		DeleteFile(strDestPath);

	//////////////////////////////////////////////////////////
	// Directory location of Work file
	CString strData;
	strData = m_pDlgMenu05->TxtDataMDS();
	TCHAR lpszCurDirPathFile[MAX_PATH];
	_stprintf(lpszCurDirPathFile, _T("%s"), strDestPath);

	CFile file;
	CFileException pError;
	if (!file.Open(lpszCurDirPathFile, CFile::modeWrite, &pError))
	{
		if (!file.Open(lpszCurDirPathFile, CFile::modeCreate | CFile::modeWrite, &pError))
		{
			// ���� ���¿� ���н� 
#ifdef _DEBUG
			afxDump << _T("File could not be opened ") << pError.m_cause << _T("\n");
#endif
			return;
		}
	}
	//������ ������ file�� �����Ѵ�.
	char* pRtn = NULL;
	file.SeekToBegin();
	file.Write(pRtn = StringToChar(strData), strData.GetLength());
	file.Close();
	if (pRtn)
		delete pRtn;
}

void CGvisR2R_LaserView::MakeSapp3()
{
	if (pDoc->WorkingInfo.LastJob.sProcessNum.IsEmpty() || pDoc->WorkingInfo.System.sPathSapp3.IsEmpty())
		return;

	FILE *fp = NULL;
	TCHAR FileName[MAX_PATH];
	CString sPath, sVal, strMsg;
	char* pRtn = NULL;

	sPath.Format(_T("%s%9s_%4s_%5s.txt"), pDoc->WorkingInfo.System.sPathSapp3,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sProcessNum,
		pDoc->WorkingInfo.System.sMcName);
	_stprintf(FileName, _T("%s"), sPath);

	fp = fopen(pRtn = TCHARToChar(FileName), "w+");
	if(pRtn) delete pRtn; 
	pRtn = NULL;

	if (fp != NULL)
	{
		fprintf( fp, "%s\n", pRtn = StringToChar(m_pDlgMenu05->Sapp3Data()) );
	}
	else
	{
		strMsg.Format(_T("It is trouble to open file.\r\n%s"), sPath);
		pView->ClrDispMsg();
		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
	}

	if(pRtn)
		delete pRtn;
	fclose(fp);
}

BOOL CGvisR2R_LaserView::RemakeReelmap()
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	CString sReelmapSrc, str;

	str = _T("ReelMapDataUp.txt");
	sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		str);
	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->RemakeReelmap(sReelmapSrc);

	if (bDualTest)
	{
		str = _T("ReelMapDataDn.txt");
		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerDn,
			str);
		if (pDoc->m_pReelMapDn)
			pDoc->m_pReelMapDn->RemakeReelmap(sReelmapSrc);

		str = _T("ReelMapDataAll.txt");
		sReelmapSrc.Format(_T("%s%s\\%s\\%s\\%s"), pDoc->WorkingInfo.System.sPathOldFile,
			pDoc->WorkingInfo.LastJob.sModelUp,
			pDoc->WorkingInfo.LastJob.sLotUp,
			pDoc->WorkingInfo.LastJob.sLayerUp,
			str);
		if (pDoc->m_pReelMapAllUp)
			pDoc->m_pReelMapAllUp->RemakeReelmap(sReelmapSrc);
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::IsDoneRemakeReelmap()
{
	return TRUE;

	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	BOOL bSuccess[3] = { FALSE };
	DWORD dwSt = GetTickCount();

	do
	{
		if (GetTickCount() - dwSt > 600000)
			break;
	} while (!pDoc->m_pReelMapUp->m_bThreadAliveRemakeReelmap && !pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap && !pDoc->m_pReelMapDn->m_bThreadAliveRemakeReelmap);

	if (bDualTest)
	{
		bSuccess[0] = pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap;
		bSuccess[1] = pDoc->m_pReelMapDn->m_bRtnThreadRemakeReelmap;
		bSuccess[2] = pDoc->m_pReelMapAllUp->m_bRtnThreadRemakeReelmap;

		if (!bSuccess[0] || !bSuccess[2] || !bSuccess[1])
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}
	else
	{
		if (!pDoc->m_pReelMapUp->m_bRtnThreadRemakeReelmap)
		{
			MsgBox(_T("ReelMap Converting Failed."));
			return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_LaserView::SetReject()
{
	CfPoint ptPnt;

	if (m_bDoMk[0])
	{
		if (!m_bTHREAD_MK[2])
		{
			m_bDoneMk[0] = FALSE;
			m_nStepMk[2] = 0;
			m_nMkPcs[2] = 0;
			m_bTHREAD_MK[2] = TRUE;
		}
	}

	if (m_bDoMk[1])
	{
		if (!m_bTHREAD_MK[3])
		{
			m_bDoneMk[1] = FALSE;
			m_nStepMk[3] = 0;
			m_nMkPcs[3] = 0;
			m_bTHREAD_MK[3] = TRUE;
		}
	}
}

void CGvisR2R_LaserView::DoInterlock()
{
}

BOOL CGvisR2R_LaserView::ChkLightErr()
{
	int nSerial, nErrCode;
	BOOL bError = FALSE;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	if (bDualTest)
		nSerial = m_nBufDnSerial[0];//GetBuffer0();
	else
		nSerial = m_nBufUpSerial[0];//GetBuffer0();

	if (nSerial > 0)
	{
		if ((nErrCode = GetErrCode0(nSerial)) != 1)
		{
			if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
				bError = TRUE;
		}
	}

	if (bDualTest)
		nSerial = m_nBufDnSerial[1];//GetBuffer0();
	else
		nSerial = m_nBufUpSerial[1];//GetBuffer0();

	if (nSerial > 0)
	{
		if ((nErrCode = GetErrCode1(nSerial)) != 1)
		{
			if (nErrCode == -1) // -1(Align Error, �뱤�ҷ�)
				bError = TRUE;
		}
	}

	if (bError)
	{
		Stop();
		TowerLamp(RGB_RED, TRUE);
		Buzzer(TRUE, 0);
		pView->DispStsBar(_T("����-45"), 0);
		DispMain(_T("�� ��"), RGB_RED);
	}

	return bError;
}

void CGvisR2R_LaserView::CntMk()
{
#ifdef USE_MPE
	if (m_nPrevTotMk[0] != m_nTotMk[0])
	{
		m_nPrevTotMk[0] = m_nTotMk[0];
		pView->m_pMpe->Write(_T("ML45096"), (long)m_nTotMk[0]);	// ��ŷ�� (��) �� ��ŷ�� 
	}
	if (m_nPrevCurMk[0] != m_nMkPcs[0])//m_nCurMk[0])
	{
		m_nPrevCurMk[0] = m_nMkPcs[0];//m_nCurMk[0];
		pView->m_pMpe->Write(_T("ML45098"), (long)m_nMkPcs[0]);	// ��ŷ�� (��) ���� ��ŷ�� ��
	}

	if (m_nPrevTotMk[1] != m_nTotMk[1])
	{
		m_nPrevTotMk[1] = m_nTotMk[1];
		pView->m_pMpe->Write(_T("ML45100"), (long)m_nTotMk[1]);	// ��ŷ�� (��) �� ��ŷ�� 
	}
	if (m_nPrevCurMk[1] != m_nMkPcs[1])//m_nCurMk[1])
	{
		m_nPrevCurMk[1] = m_nMkPcs[1];//m_nCurMk[1];
		pView->m_pMpe->Write(_T("ML45102"), (long)m_nMkPcs[1]);	// ��ŷ�� (��) ���� ��ŷ�� ��
	}
#endif
}

BOOL CGvisR2R_LaserView::IsOnMarking0()
{
	if (m_nMkPcs[0] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		return TRUE;

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsOnMarking1()
{
	if (m_nMkPcs[1] < pDoc->m_Master[0].m_pPcsRgn->nTotPcs)	// ��ŷ�Ϸ�Check
		return TRUE;

	return FALSE;
}

void CGvisR2R_LaserView::SetDualTest(BOOL bOn)
{
#ifdef USE_MPE
	if (pView->m_pMpe)
		pView->m_pMpe->Write(_T("MB44017A"), bOn ? 0 : 1);		// �ܸ� �˻� On
#endif
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetDualTest(bOn);
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetDualTest(bOn);
	if (m_pDlgMenu03)
		m_pDlgMenu03->SetDualTest(bOn);
}

void CGvisR2R_LaserView::SetTwoMetal(BOOL bSel, BOOL bOn)
{
	if (bSel)
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = TRUE;
			m_pMpe->Write(_T("MB44017C"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r������ ON : TRUE	
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bTwoMetal = FALSE;
			m_pMpe->Write(_T("MB44017C"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("Two Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_TWO_METAL - Uncoiler\r������ ON : TRUE	
		}
	}
	else
	{
		if (bOn)
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = TRUE;
			m_pMpe->Write(_T("MB44017D"), 1);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("1"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
		}
		else
		{
			pDoc->WorkingInfo.LastJob.bOneMetal = FALSE;
			m_pMpe->Write(_T("MB44017D"), 0);
			::WritePrivateProfileString(_T("Last Job"), _T("One Metal On"), _T("0"), PATH_WORKING_INFO);// IDC_CHK_ONE_METAL - Recoiler\r������ CW : FALSE
		}
	}
}

void CGvisR2R_LaserView::RestoreReelmap()
{
	return;

	if (pDoc->m_pReelMapUp)
		pDoc->m_pReelMapUp->RestoreReelmap();
}

void CGvisR2R_LaserView::AdjPinPos()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->m_dMkFdOffsetY[0][0] > -2.0 &&
			m_pDlgMenu02->m_dMkFdOffsetY[0][0] < 2.0)
		{
			double dOffsetY = -1.0*(m_pDlgMenu02->m_dMkFdOffsetY[0][0]);// +m_pDlgMenu02->m_dMkFdOffsetY[1][0]) / 2.0;
			dOffsetY *= pDoc->m_dShiftAdjustRatio;

			CfPoint ptPnt[2];
			ptPnt[0].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY;

			m_pDlgMenu02->SetPinPos(0, ptPnt[0]);
		}
	}
}

void CGvisR2R_LaserView::SetEngraveSts(int nStep)
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - TBL�ı� OFF, TBL���� ON, FD/TQ ���� OFF, 
	switch (nStep)
	{
	case 0:
		m_pDlgMenu03->SwMkTblBlw(FALSE);
		m_pDlgMenu03->SwMkFdVac(FALSE);
		m_pDlgMenu03->SwMkTqVac(FALSE);
		break;
	case 1:
		m_pDlgMenu03->SwMkTblVac(TRUE);
		break;
	}
}

void CGvisR2R_LaserView::SetEngraveStopSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkTblBlw(FALSE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkFdVac(FALSE);
}

void CGvisR2R_LaserView::SetEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return;

	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	m_pDlgMenu03->SwMkFdVac(TRUE);
	m_pDlgMenu03->SwMkTblVac(FALSE);
	m_pDlgMenu03->SwMkTblBlw(TRUE);
}

BOOL CGvisR2R_LaserView::IsEngraveFdSts()
{
	if (!m_pDlgMenu03)
		return FALSE;

	BOOL bOn[4] = { 0 };
	// ��ŷ�� - FD/TQ ���� ON, TBL���� OFF, TBL�ı� ON, 
	bOn[0] = m_pDlgMenu03->IsMkFdVac(); // TRUE
	bOn[1] = m_pDlgMenu03->IsMkTqVac(); // TRUE
	bOn[2] = m_pDlgMenu03->IsMkTblVac(); // FALSE
	bOn[3] = m_pDlgMenu03->IsMkTblBlw(); // TRUE

	if (bOn[0] && bOn[1] && !bOn[2] && bOn[3])
		return TRUE;

	return FALSE;
}

void CGvisR2R_LaserView::SetEngraveFd()
{
	CfPoint OfSt;
	GetMkOffset(OfSt);

	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
			MoveMk(-1.0*OfSt.x);
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
				MoveMk(-1.0*OfSt.x);
		}
	}
	if (m_pDlgMenu03)
		m_pDlgMenu03->ChkDoneMk();

	if (!pDoc->WorkingInfo.LastJob.bAoiOnePnl)
	{
#ifdef USE_MPE
		//IoWrite(_T("MB440151"), 1);	// ���ǳ� �̼ۻ��� ON (PC�� ON, OFF)
		pView->m_pMpe->Write(_T("MB440151"), 1);
#endif
		CString sData, sPath = PATH_WORKING_INFO;
		pDoc->WorkingInfo.LastJob.bMkOnePnl = pDoc->WorkingInfo.LastJob.bAoiOnePnl = TRUE;
		sData.Format(_T("%d"), pDoc->WorkingInfo.LastJob.bMkOnePnl ? 1 : 0);
		::WritePrivateProfileString(_T("Last Job"), _T("Marking One Pannel Move On"), sData, sPath);
		::WritePrivateProfileString(_T("Last Job"), _T("AOI One Pannel Move On"), sData, sPath);
	}
}

void CGvisR2R_LaserView::SetEngraveFd(double dDist)
{
	double fLen = pDoc->GetOnePnlLen();
	double dOffset = dDist - (fLen*2.0);
	MoveMk(dOffset);
}

void CGvisR2R_LaserView::MoveEngrave(double dOffset)
{
#ifdef USE_MPE
	long lData = (long)(dOffset * 1000.0);
	//IoWrite(_T("MB440161"), 1);		// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF))
	pView->m_pMpe->Write(_T("MB440161"), 1);
	//IoWrite(_T("ML45066"), lData);	// ��ŷ�� Feeding �ѷ� Offset(*1000, +:�� ����, -�� ����)
	pView->m_pMpe->Write(_T("ML45066"), lData);
#endif
}

BOOL CGvisR2R_LaserView::IsEngraveFd()
{
	if (m_nShareDnCnt > 0)
	{
		if (!(m_nShareDnCnt % 2))
		{
#ifdef USE_MPE
			if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
				return TRUE;
#endif
			return FALSE;
		}
	}
	else
	{
		if (m_nShareUpCnt > 0)
		{
			if (!(m_nShareUpCnt % 2))
			{
#ifdef USE_MPE
				if (pDoc->m_pMpeSignal[5] & (0x01 << 1))	// ��ŷ�� �ǵ� CW ON (PLC�� �ǵ��Ϸ� �� OFF)
					return TRUE;
#endif
				return FALSE;
			}
		}
	}

	return TRUE;
}

double CGvisR2R_LaserView::GetEngraveFdLen()
{
	int nLast = pDoc->GetLastShotEngrave();

	double dLen = (double)nLast * _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dLen;
}

double CGvisR2R_LaserView::GetAoiInitDist()
{
	double dInitD1 = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - _tstof(pDoc->WorkingInfo.LastJob.sOnePnlLen);
	return dInitD1;
}

double CGvisR2R_LaserView::GetAoiRemain()
{
#ifdef USE_MPE
	double dCurPosEngraveFd = (double)pDoc->m_pMpeData[1][0];	// ML44052	,	���κ� Feeding ���ڴ� ��(���� mm)
	double dRemain = _tstof(pDoc->WorkingInfo.Motion.sFdEngraveAoiInitDist) - dCurPosEngraveFd;
	return dRemain;
#else
	return 0.0;
#endif
}

LRESULT CGvisR2R_LaserView::wmClientReceivedMdx(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;
	switch (nCmd)
	{
	case MdxIsReady:
		break;
	case GetCharacterString:
		break;
	default:
		break;
	}

	return (LRESULT)1;
}

LRESULT CGvisR2R_LaserView::wmClientReceivedSr(WPARAM wParam, LPARAM lParam)
{
	int nCmd = (int)wParam;
	CString* sReceived = (CString*)lParam;
	switch (nCmd)
	{
	case SrTriggerInputOn:
		if (m_pDlgMenu02)
		{
			m_pDlgMenu02->Disp2dCode();
		}
		break;
	default:
		break;
	}

	return (LRESULT)1;
}

LRESULT CGvisR2R_LaserView::wmServerReceived(WPARAM wParam, LPARAM lParam)
{
	if(!m_pEngrave)
		return (LRESULT)0;

	int nAcceptId = (int)wParam;
	SOCKET_DATA sSockData;
	SOCKET_DATA *pSocketData = (SOCKET_DATA*)lParam;

	SOCKET_DATA rSockData = *pSocketData;
	int nCmdCode = rSockData.nCmdCode;
	int nMsgId = rSockData.nMsgID;
	switch (nCmdCode)
	{
	case _GetSig:
		break;
	case _GetData:
		break;
	case _SetSig:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysSignal(rSockData);

		pView->m_bSetSig = TRUE;
		break;
	case _SetData:
		if (m_pEngrave && m_pEngrave->IsConnected())
			m_pEngrave->GetSysData(rSockData);

		pView->m_bSetData = TRUE;
		break;
	default:
		break;
	}


	return (LRESULT)1;
}

void CGvisR2R_LaserView::SetEngraveFdPitch(double dPitch)
{
	pDoc->SetEngraveFdPitch(dPitch);
}


BOOL CGvisR2R_LaserView::IsConnected()
{
#ifdef USE_ENGRAVE
	if (m_pEngrave)
	{
		if (m_pEngrave->IsConnected())
		{
			if (!m_bContEngraveF)
			{
				m_bContEngraveF = TRUE;
			}
			return TRUE;
		}
		else
		{
			if (m_bContEngraveF)
			{
				m_bContEngraveF = FALSE;
			}
			return FALSE;
		}
	}
#endif
	return FALSE;
}

BOOL CGvisR2R_LaserView::IsConnectedMdx()
{
	if (m_pMdx2500)
	{
		return m_pMdx2500->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsConnectedSr()
{
	if (m_pSr1000w)
	{
		return m_pSr1000w->IsConnected();
	}

	return FALSE;
}

BOOL CGvisR2R_LaserView::IsConnectedMk()
{
	if (m_pEngrave)
	{
		return m_pEngrave->IsConnected();
	}

	return FALSE;
}


void CGvisR2R_LaserView::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CFormView::OnClose();
}


void CGvisR2R_LaserView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

BOOL CGvisR2R_LaserView::IsPinPos0()
{
	if (!m_pMotion)
		return FALSE;

	double pPos[2];
	pPos[0] = m_pMotion->m_dPinPosX[0];
	pPos[1] = m_pMotion->m_dPinPosY[0];

	double dCurrX = m_dEnc[AXIS_X0];
	double dCurrY = m_dEnc[AXIS_Y0];

	if (dCurrX < pPos[0] - 4.0 || dCurrX > pPos[0] + 4.0)
		return FALSE;
	if (dCurrY < pPos[1] - 4.0 || dCurrY > pPos[1] + 4.0)
		return FALSE;

	return TRUE;
}


void CGvisR2R_LaserView::InitAutoEng()
{
	m_bMkSt = FALSE;
	m_bMkStSw = FALSE;
	m_nMkStAuto = 0;

	m_bEngSt = FALSE;
	m_bEngStSw = FALSE;
	m_nEngStAuto = 0;

	m_bEng2dSt = FALSE;
	m_bEng2dStSw = FALSE;
	m_nEng2dStAuto = 0;

	m_nGetItsCodeSerial = 0;
	pDoc->m_nShotNum = 0;
	pDoc->m_bUploadPinImg = FALSE;
	pDoc->BtnStatus.EngAuto._Init();
	InitAutoEngSignal();

	if (m_pDlgMenu01)
		m_pDlgMenu01->ResetLotTime();
}

// DoAuto
void CGvisR2R_LaserView::DoAutoEng()
{
	if ( !IsAuto() || (MODE_INNER != pDoc->WorkingInfo.LastJob.nTestMode) )
		return;

	CString str;
	str.Format(_T("%d : %d"), m_nStepTHREAD_DISP_DEF, m_bTHREAD_DISP_DEF ? 1 : 0);
	pView->DispStsBar(str, 6);

	// ���κ� ��ŷ���� ��ȣ�� Ȯ��
	DoAtuoGetEngStSignal();

	// ���κ� 2D �ڵ� Reading��ȣ�� Ȯ��
	DoAtuoGet2dReadStSignal();

	// Engrave Auto Sequence
	// ���κ� Marking Start
	DoAutoMarking();

	// ���κ� 2D �ڵ� Reading Start
	DoAuto2dReading();
}

void CGvisR2R_LaserView::DoAtuoGetEngStSignal()
{
	if (!m_bEngSt)
	{
		if ((pDoc->BtnStatus.EngAuto.IsMkSt || m_bMkStSw) && !pDoc->BtnStatus.EngAuto.MkStF)  // AlignTest		// ��ŷ����(PC�� Ȯ���ϰ� Reset��Ŵ.)-20141029
		{
			m_bEngStSw = FALSE;
			pDoc->BtnStatus.EngAuto.MkStF = TRUE;

			m_bEngSt = TRUE;
			m_nEngStAuto = ENG_ST;
				
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(0, TRUE);
		}
	}

	if (!pDoc->BtnStatus.EngAuto.IsMkSt && pDoc->BtnStatus.EngAuto.MkStF)
	{
		pDoc->BtnStatus.EngAuto.MkStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLed(0, FALSE);
	}
}

void CGvisR2R_LaserView::DoAtuoGet2dReadStSignal()
{
	if (!m_bEng2dSt)
	{
		if ((pDoc->BtnStatus.EngAuto.IsRead2dSt || m_bMkStSw) && !pDoc->BtnStatus.EngAuto.Read2dStF)  // 2D(GUI) Reading ���� Start��ȣ(PLC On->PC Off)
		{
			m_bEng2dStSw = FALSE;
			pDoc->BtnStatus.EngAuto.Read2dStF = TRUE;

			m_bEng2dSt = TRUE;
			m_nEng2dStAuto = ENG_2D_ST;
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(3, TRUE);
		}
	}

	if (!pDoc->BtnStatus.EngAuto.IsRead2dSt && pDoc->BtnStatus.EngAuto.Read2dStF)
	{
		pDoc->BtnStatus.EngAuto.Read2dStF = FALSE;
		pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
		if (m_pDlgMenu02)
			m_pDlgMenu02->SetLed(3, FALSE);
	}
}

void CGvisR2R_LaserView::DoAuto2dReading()
{
	if (MODE_INNER == pDoc->WorkingInfo.LastJob.nTestMode || MODE_OUTER == pDoc->WorkingInfo.LastJob.nTestMode)
	{
		Eng2dRead();
	}
}



// DoAutoMarking
void CGvisR2R_LaserView::DoAutoMarking()
{
	if (MODE_INNER == pDoc->WorkingInfo.LastJob.nTestMode)
	{
		MarkingWith1PointAlign();
	}
}

void CGvisR2R_LaserView::MarkingWith1PointAlign()
{
	Eng1PtReady();
	Eng1PtInit();
	Eng1PtAlignPt0();
	Eng1PtDoMarking();
}

void CGvisR2R_LaserView::Eng1PtReady()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST:	// PLC MK ��ȣ Ȯ��	
			m_nEngStAuto++;
			break;
		case ENG_ST + 1:
			pDoc->BtnStatus.EngAuto.IsOnMking = FALSE;
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(1, TRUE);
			m_nEngStAuto = ENG_ST + (Mk1PtIdx::Start);
			break;
		case ENG_ST + (Mk1PtIdx::Start) :	// 2
			if(pDoc->BtnStatus.EngAuto.IsOnMking)
				m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Start) + 1:
			m_nEngStAuto = ENG_ST + (Mk1PtIdx::InitMk);			// InitMk()
			break;
		}
	}
}

void CGvisR2R_LaserView::Eng1PtInit()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST + (Mk1PtIdx::InitMk) :
			m_nEngStAuto++;
			if (nSerial > 2)
			{
				AdjPinPosEng();
				// ���κ� �۾��Ϸ�.(PC�� On, PLC�� Ȯ�� �� Off) - ?
			}
			break;

		case ENG_ST + (Mk1PtIdx::InitMk) + 1:
			m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0);	// Move - Cam1 - Pt0
			break;
		}
	}
}

void CGvisR2R_LaserView::Eng1PtAlignPt0()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST + (Mk1PtIdx::Move0Cam0) :	// Move - Cam1 - Pt0
			MovePinPos();	//if (MoveAlign0(0))
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Move0Cam0) + 1:
			//if (IsRun())
				m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Move0Cam0) + 2:
			if (IsMoveDone())
			{
				Sleep(100);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::Move0Cam0) + 3:
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::Align0_0) :		// 1PtAlign - Cam0 - Pt0
			m_nEngStAuto++;
			if (!m_bSkipAlign[0][0])
			{
				if (OnePointAlign0(0))
					m_bFailAlign[0][0] = FALSE;
				else
					m_bFailAlign[0][0] = TRUE;
			}
			break;
		case ENG_ST + (Mk1PtIdx::Align0_0) + 1:
			if (!IsRun())
				break;

			if (m_bFailAlign[0][0])
			{
				Buzzer(TRUE, 0);
				//if(IDNO == DoMyMsgBox(_T("ī�޶��� ���� ������ �ٽ��Ͻðڽ��ϱ�?"), MB_YESNO))
				if (IDNO == MsgBox(_T("ī�޶��� ���� ������ �ٽ��Ͻðڽ��ϱ�?"), 0, MB_YESNO))
				{
					Buzzer(FALSE, 0);

					//if(IDYES == DoMyMsgBox(_T("ī�޶��� ���� ������ ���� ���Ͻðڽ��ϱ�?"), MB_YESNO))
					if (IDYES == MsgBox(_T("ī�޶��� ���� ������ ���� ���Ͻðڽ��ϱ�?"), 0, MB_YESNO))
					{
						m_bReAlign[0][0] = FALSE;
						m_bSkipAlign[0][0] = TRUE;
						m_bSkipAlign[0][1] = TRUE;
						m_bSkipAlign[0][2] = TRUE;
						m_bSkipAlign[0][3] = TRUE;
						//if(IDNO == DoMyMsgBox(_T("�ǳڿ� ������ 2D ��ŷ�� �Ͻðڽ��ϱ�?"), MB_YESNO))
						if (IDNO == MsgBox(_T("�ǳڿ� ������ 2D ��ŷ�� �Ͻðڽ��ϱ�?"), 0, MB_YESNO))
						{
							m_bDoMk[0] = FALSE;
							m_bDoneMk[0] = TRUE;

							EngStop(TRUE);
							TowerLamp(RGB_YELLOW, TRUE);
						}
						else
						{
							m_bDoMk[0] = TRUE;
							m_bDoneMk[0] = FALSE;
						}
					}
					else
					{
						// ���� ������ �ٽ� ��.
						m_bReAlign[0][0] = TRUE;
						m_bSkipAlign[0][0] = FALSE;
						m_bSkipAlign[0][1] = FALSE;
						m_bSkipAlign[0][2] = FALSE;
						m_bSkipAlign[0][3] = FALSE;
						m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0); // OnePointAlign0(0) ���� ����. - ī�޶� ������
																	 //m_nEngStAuto = MK_ST + (Mk1PtIdx::Move0Cam0); // TwoPointAlign0(0) ���� ����. - ī�޶� ������
																	 //m_nEngStAuto = MK_ST + (Mk1PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
						EngStop(TRUE);
						TowerLamp(RGB_YELLOW, TRUE);
					}
				}
				else
				{ 
					// ���� ������ �ٽ� ��.
					Buzzer(FALSE, 0);

					m_bReAlign[0][0] = TRUE;
					m_bSkipAlign[0][0] = FALSE;
					m_bSkipAlign[0][1] = FALSE;
					m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0); // OnePointAlign0(0) ���� ����. - ī�޶� ������
																 //m_nEngStAuto = MK_ST + (Mk1PtIdx::Move0Cam0); // TwoPointAlign0(0) ���� ����. - ī�޶� ������
																 //m_nEngStAuto = MK_ST + (Mk1PtIdx::Move0Cam1); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
					EngStop(TRUE);
					TowerLamp(RGB_YELLOW, TRUE);
				}
			}

			if (m_bFailAlign[0][0])
			{
				if (!m_bReAlign[0][0])
				{
					if (m_bDoMk[0])
						m_nEngStAuto++; // DoMk
					else
					{
						//MovePinPos(); // 2D �ڵ� ��ġ
						m_nEngStAuto = ENG_ST + (Mk1PtIdx::DoneMk); // Align���� �ʱ�ȭ (Skip 65 : Mk())
					}
				}
				else
				{
					m_nEngStAuto = ENG_ST + (Mk1PtIdx::Move0Cam0); // OnePointAlign0(0) ���� ����. - ī�޶� ������
																 //m_nEngStAuto = MK_ST + (Mk1PtIdx::Move0Cam0); // TwoPointAlign1(0) ���� ����. - ī�޶� ������
				}
			}
			else
				m_nEngStAuto++; // DoMk

			break;
		case ENG_ST + (Mk1PtIdx::Align0_0) + 2:
			if (IsRun())
			{
				//MovePinPos(); // 2D �ڵ� ��ġ
				//pDoc->m_nShotNum++;							// ������ �ø��� ����
				m_nEngStAuto = ENG_ST + (Mk1PtIdx::DoMk);
			}
			break;
		}
	}
}

void CGvisR2R_LaserView::Eng1PtDoMarking()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial = pDoc->GetLastShotEngrave() + 1;

	if (m_bEngSt)
	{
		switch (m_nEngStAuto)
		{
		case ENG_ST + (Mk1PtIdx::DoMk) :				// Mk ��ŷ ����
			if (!IsRun())
				break;

			if (!pDoc->WorkingInfo.System.bNoMk)
			{
				if (!SetMdxLotAndShotNum(pDoc->m_sItsCode, nSerial))
				{
					EngStop(TRUE);
					//MsgBox(_T("SetMdxLotAndShotNum - Failed."));
					//TowerLamp(RGB_RED, TRUE);
					//Buzzer(TRUE, 0);
					break;
				}
			}
			Sleep(100);
			m_nEngStAuto++;
			break;

		case ENG_ST + (Mk1PtIdx::DoMk) + 1:
			if (!IsRun())
				break;

			if (!pDoc->WorkingInfo.System.bNoMk)
			{
				if(m_nGetItsCodeSerial == 0 || (m_nGetItsCodeSerial + 1) == nSerial )
					SetMk(TRUE);	// Mk ��ŷ ����
				else if ((m_nGetItsCodeSerial + 1) > nSerial)
				{
					EngStop(TRUE);
					MsgBox(_T("���� - 2D���ڵ��� ���ε� �ø����� ������ �ø��󺸴� Ů�ϴ�."));
					TowerLamp(RGB_RED, TRUE);
					Buzzer(TRUE, 0);
					break;
				}
			}
			//SetCurrentInfoEngShotNum(pDoc->m_nShotNum);
			if (m_nGetItsCodeSerial == 0 || (m_nGetItsCodeSerial + 1) == nSerial)
				SetCurrentInfoEngShotNum(nSerial);

			Sleep(300);
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::DoMk) + 2:
			if (IsMkDone())
			{
				Sleep(300);
				m_nEngStAuto = ENG_ST + (Mk1PtIdx::DoneMk);	// Mk ��ŷ �Ϸ�
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) :
			pDoc->BtnStatus.EngAuto.IsOnMking = TRUE;
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(1, FALSE);
			m_nEngStAuto++;
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 1:
			if (!pDoc->BtnStatus.EngAuto.IsOnMking)
			{
				pDoc->BtnStatus.EngAuto.IsMkDone = FALSE;
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, TRUE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(2, TRUE);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 2:
			if (pDoc->BtnStatus.EngAuto.IsMkDone)
			{
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(2, FALSE);
				Sleep(300);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 3:
			//if (pDoc->m_pMpeSignal[0] & (0x01 << 2))	// MB440102 - ���κ� Feeding�Ϸ�(PLC�� On��Ű�� PC�� Ȯ���ϰ� Reset��Ŵ.)
			if(pDoc->BtnStatus.EngAuto.IsFdDone)
			{
				if (m_nGetItsCodeSerial == 0 || (m_nGetItsCodeSerial + 1) == nSerial)
					SetLastSerialEng(nSerial); // (_ttoi(pDoc->m_sShotNum));
				//SetLastSerialEng(pDoc->m_nShotNum); // (_ttoi(pDoc->m_sShotNum));
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, TRUE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(6, TRUE);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 4:
			if (!pDoc->BtnStatus.EngAuto.IsFdDone)
			{
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(6, FALSE);
				m_nEngStAuto++;
			}
			break;
		case ENG_ST + (Mk1PtIdx::DoneMk) + 5:
			m_nEngStAuto = 0;
			m_bEngSt = FALSE;
			break;
		}
	}
}

BOOL CGvisR2R_LaserView::IsMkDone()
{
	if (!pView || !pView->m_pMdx2500)
		return FALSE;

	return (!pView->m_pMdx2500->IsRunning());
}

BOOL CGvisR2R_LaserView::SetMk(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_pMdx2500)
		return FALSE;

	return (pView->m_pMdx2500->LaserMarking());
}

BOOL CGvisR2R_LaserView::SetMdxLotAndShotNum(CString sLot, int nSerial)
{
	if (!pView || !pView->m_pMdx2500)
		return FALSE;

	CString sSerial;
	sSerial.Format(_T("%03d"), nSerial);
	return m_pMdx2500->SetMdxOrderShotNum(sLot, sSerial);
}

void CGvisR2R_LaserView::AdjPinPosEng()
{
	if (m_pDlgMenu02)
	{
		if (m_pDlgMenu02->m_dMkFdOffsetY[0][0] > -2.0 &&
			m_pDlgMenu02->m_dMkFdOffsetY[0][0] < 2.0)
		{
			double dOffsetY = -1.0*(m_pDlgMenu02->m_dMkFdOffsetY[0][0]);
			dOffsetY *= pDoc->m_dShiftAdjustRatio;

			CfPoint ptPnt[2];
			ptPnt[0].x = _tstof(pDoc->WorkingInfo.Motion.sPinPosX[0]);
			ptPnt[0].y = _tstof(pDoc->WorkingInfo.Motion.sPinPosY[0]) + dOffsetY;

			m_pDlgMenu02->SetPinPos(0, ptPnt[0]);
		}
	}
}

BOOL CGvisR2R_LaserView::OnePointAlign0(int nPos)
{
	if (!m_pDlgMenu02)
		return FALSE;
	BOOL bRtn;
	CfPoint ptPnt(0.0, 0.0);
	CfPoint _ptPnt(0.0, 0.0);
	bRtn = m_pDlgMenu02->OnePointAlign(ptPnt); // �������� Ȯ���� ������ġ (Motion�� ������ǥ��).
	if(bRtn)
		SetEngOffset(ptPnt);
	else
		SetEngOffset(_ptPnt);

	return bRtn;
}


// DoAutoReading

void CGvisR2R_LaserView::Eng2dRead()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	int nSerial;

	if (m_bEng2dSt)
	{
		switch (m_nEng2dStAuto)
		{
		case ENG_2D_ST:	// PLC MK ��ȣ Ȯ��	
			if (IsRun())
			{
				MoveInitPos0();
				m_nEng2dStAuto++;
			}
			break;
		case ENG_2D_ST + 1:
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(4, TRUE);
			m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::Start);
			break;
		case ENG_2D_ST + (Read2dIdx::Start) :	// 2
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::Start) + 1:
			m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoRead);
			break;
		case ENG_2D_ST + (Read2dIdx::DoRead) :			// 2D Reading ����
			if (!pDoc->WorkingInfo.System.bNoMk)
				Set2dRead(TRUE);							// 2D Reading ����
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::DoRead) + 1:
			Sleep(300);
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::DoRead) + 2:
			if (Is2dReadDone())
			{
				if (!pDoc->WorkingInfo.System.bNoMk)
					Get2dCode(m_sGetItsCode, m_nGetItsCodeSerial);

				Sleep(300);
				m_nEng2dStAuto = ENG_2D_ST + (Read2dIdx::DoneRead);	// 2D Reading �Ϸ�
			}
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) :
			if (IsRun())
			{
				m_nEng2dStAuto++;
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(4, FALSE);
				MovePinPos();
			}
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) + 1:
			pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, TRUE);
			if (m_pDlgMenu02)
				m_pDlgMenu02->SetLed(5, TRUE);
			m_nEng2dStAuto++;
			break;
		case ENG_2D_ST + (Read2dIdx::DoneRead) + 2:
			if (pDoc->BtnStatus.EngAuto.IsRead2dDone)
			{
				m_nEng2dStAuto = 0;
				m_bEng2dSt = FALSE;
				pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
				if (m_pDlgMenu02)
					m_pDlgMenu02->SetLed(5, FALSE);
				Sleep(300);
			}
			break;
		}
	}
}


BOOL  CGvisR2R_LaserView::Is2dReadDone()
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (!pView->m_pSr1000w->IsRunning());
}

BOOL CGvisR2R_LaserView::Set2dRead(BOOL bRun)	// Marking Start
{
	if (!pView || !pView->m_pSr1000w)
		return FALSE;

	return (pView->m_pSr1000w->DoRead2DCode());
}

BOOL CGvisR2R_LaserView::SetEngOffset(CfPoint &OfSt)
{
	// Write Feeding Offset data....
	if(m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetEngOffset(OfSt);
	return pDoc->SetEngOffset(OfSt);
}

void CGvisR2R_LaserView::SetMyMsgYes()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickYes();
		}
	}
}

void CGvisR2R_LaserView::SetMyMsgNo()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub01)
		{
			((CDlgMyMsgSub01*)(m_pDlgMyMsg->m_pDlgMyMsgSub01))->ClickNo();
		}
	}
}

void CGvisR2R_LaserView::SetMyMsgOk()
{
	if (m_pDlgMyMsg)
	{
		if (m_pDlgMyMsg->m_pDlgMyMsgSub02)
		{
			((CDlgMyMsgSub02*)(m_pDlgMyMsg->m_pDlgMyMsgSub02))->ClickOk();
		}
	}
}

void CGvisR2R_LaserView::InitAutoEngSignal()
{
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone, FALSE);
	pDoc->SetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone, FALSE);

	if (m_pDlgMenu02)
	{
		m_pDlgMenu02->SetLed(0, FALSE); // _SigInx::_EngAutoSeqMkSt
		m_pDlgMenu02->SetLed(1, FALSE); // _SigInx::_EngAutoSeqOnMkIng
		m_pDlgMenu02->SetLed(2, FALSE); // _SigInx::_EngAutoSeqMkDone
		m_pDlgMenu02->SetLed(3, FALSE); // _SigInx::_EngAutoSeq2dReadSt
		m_pDlgMenu02->SetLed(4, FALSE); // _SigInx::_EngAutoSeqOnReading2d
		m_pDlgMenu02->SetLed(5, FALSE); // _SigInx::_EngAutoSeq2dReadDone
		m_pDlgMenu02->SetLed(6, FALSE); // _SigInx::_EngAutoSeqFdDone
	}
}

BOOL CGvisR2R_LaserView::GetCurrentInfoSignal()
{
	pDoc->BtnStatus.EngAuto.IsInit = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoInit);
	pDoc->BtnStatus.EngAuto.IsMkSt = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqMkSt);
	pDoc->BtnStatus.EngAuto.IsOnMking = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqOnMkIng);
	pDoc->BtnStatus.EngAuto.IsMkDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqMkDone);
	pDoc->BtnStatus.EngAuto.IsRead2dSt = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadSt);
	pDoc->BtnStatus.EngAuto.IsOnRead2d = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqOnReading2d);
	pDoc->BtnStatus.EngAuto.IsRead2dDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeq2dReadDone);
	pDoc->BtnStatus.EngAuto.IsFdDone = pDoc->GetCurrentInfoSignal(_SigInx::_EngAutoSeqFdDone);

	return TRUE;
}

void CGvisR2R_LaserView::SetLastSerialEng(int nSerial)
{
	if (m_pDlgFrameHigh)
		m_pDlgFrameHigh->SetEngraveLastShot(nSerial);
}

int CGvisR2R_LaserView::GetLastSerialEng()
{
	int nSerial = 0;
	if (pDoc)
		nSerial = pDoc->GetLastSerialEng();

	return nSerial;
}

CString CGvisR2R_LaserView::GetCurrentInfoBufUp()
{
	return pDoc->GetCurrentInfoBufUp();
}

CString CGvisR2R_LaserView::GetCurrentInfoBufDn()
{
	return pDoc->GetCurrentInfoBufDn();
}

void CGvisR2R_LaserView::ChkBufUp()
{
	CString str = GetCurrentInfoBufUp();

	if (pFrm)
	{
		if (m_sBuf[0] != str)
		{
			m_sBuf[0] = str;
			pFrm->DispStatusBar(str, 3);
		}
	}
}

void CGvisR2R_LaserView::ChkBufDn()
{
	CString str = GetCurrentInfoBufDn();

	if (pFrm)
	{
		if (m_sBuf[1] != str)
		{
			m_sBuf[1] = str;
			pFrm->DispStatusBar(str, 1);
		}
	}
}

void CGvisR2R_LaserView::SetCurrentInfoEngShotNum(int nSerial)
{
	pDoc->SetCurrentInfoEngShotNum(nSerial);
}


BOOL CGvisR2R_LaserView::Get2dCode(CString &sLot, int &nSerial)
{
	if (!m_pSr1000w)
		return FALSE;

	CString sData;
	if (m_pSr1000w->Get2DCode(sData))
	{
		int nPos = sData.ReverseFind('-');
		if (nPos != -1)
		{
			pDoc->m_sOrderNum = sData.Left(nPos);
			pDoc->m_sShotNum = sData.Right(sData.GetLength() - nPos - 1);
			pDoc->m_nShotNum = _tstoi(pDoc->m_sShotNum);
			sLot = pDoc->m_sOrderNum;
			nSerial = pDoc->m_nShotNum;
		}
		else
		{
			pView->MsgBox(sData);
			//AfxMessageBox(sData);
		}

		return TRUE;
	}

	return FALSE;
}

void CGvisR2R_LaserView::SetTotOpRto(CString sVal)		// ��ü������
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetTotOpRto(sVal);
}

void CGvisR2R_LaserView::SetTotVel(CString sVal)		// ��ü�ӵ�
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetTotVel(sVal);
}

void CGvisR2R_LaserView::SetPartVel(CString sVal)		// �����ӵ�
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetPartVel(sVal);
}

void CGvisR2R_LaserView::SetMkDoneLen(CString sVal)		// ��ŷ�� : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetMkDoneLen(sVal);
}

void CGvisR2R_LaserView::SetAoiDnDoneLen(CString sVal)	// �˻��(��) : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetAoiDnDoneLen(sVal);
}

void CGvisR2R_LaserView::SetAoiUpDoneLen(CString sVal)	// �˻��(��) : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetAoiUpDoneLen(sVal);
}

void CGvisR2R_LaserView::SetEngDoneLen(CString sVal)	// �˻��(��) : Distance (FdDone) [M]
{
	if (m_pDlgMenu01)
		m_pDlgMenu01->SetEngDoneLen(sVal);
}

void CGvisR2R_LaserView::DispStatusBar(CString strMsg, int nStatusBarID)
{
	if (pFrm)
		pFrm->DispStatusBar(strMsg, nStatusBarID);
}

void CGvisR2R_LaserView::GetMkMenu01()
{
	m_bTIM_MENU01_UPDATE_WORK = TRUE;
	SetTimer(TIM_MENU01_UPDATE_WORK, 500, NULL);
}

CString CGvisR2R_LaserView::GetMkMenu01(CString sMenu, CString sItem)
{
	CString sPath = pDoc->WorkingInfo.System.sPathMkMenu01;
	TCHAR szData[512];

	if (sPath.IsEmpty())
		return _T("");

	if (0 < ::GetPrivateProfileString(sMenu, sItem, NULL, szData, sizeof(szData), sPath))
		return CString(szData);

	return _T("");
}


CString CGvisR2R_LaserView::GetMkDispInfoUp(CString sMenu, CString sItem, int nSerial)
{
	TCHAR szData[512];
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\DefImage\\%d\\Disp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerUp,
		nSerial);

	if (sPath.IsEmpty())
		return _T("");

	if (0 < ::GetPrivateProfileString(sMenu, sItem, NULL, szData, sizeof(szData), sPath))
		return CString(szData);

	return _T("");
}

CString CGvisR2R_LaserView::GetMkDispInfoDn(CString sMenu, CString sItem, int nSerial)
{
	TCHAR szData[512];
	CString sPath;
	sPath.Format(_T("%s%s\\%s\\%s\\DefImage\\%d\\Disp.txt"), pDoc->WorkingInfo.System.sPathOldFile,
		pDoc->WorkingInfo.LastJob.sModelUp,
		pDoc->WorkingInfo.LastJob.sLotUp,
		pDoc->WorkingInfo.LastJob.sLayerDn,
		nSerial);

	if (sPath.IsEmpty())
		return _T("");

	if (0 < ::GetPrivateProfileString(sMenu, sItem, NULL, szData, sizeof(szData), sPath))
		return CString(szData);

	return _T("");
}

CString CGvisR2R_LaserView::GetTimeIts()
{
	stLotTime ItsTime;

	CString strVal;
	time_t osBinTime;			// C run-time time (defined in <time.h>)
	time(&osBinTime);			// Get the current time from the 
								// operating system.
	CTime Tim(osBinTime);

	ItsTime.nYear = Tim.GetYear();
	ItsTime.nMonth = Tim.GetMonth();
	ItsTime.nDay = Tim.GetDay();
	ItsTime.nHour = Tim.GetHour();
	ItsTime.nMin = Tim.GetMinute();
	ItsTime.nSec = Tim.GetSecond();

	strVal.Format(_T("%04d%02d%02d%02d%02d%02d"),
		ItsTime.nYear, ItsTime.nMonth, ItsTime.nDay,
		ItsTime.nHour, ItsTime.nMin, ItsTime.nSec);

	return strVal;
}

BOOL CGvisR2R_LaserView::ReloadRstInner()
{
	double dRatio = 0.0;
	CString sVal = _T("");
	CDlgProgress dlg;
	sVal.Format(_T("On Reloading InnerReelmap."));
	dlg.Create(sVal);
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;


	BOOL bRtn[7];
	if (pDoc->m_pReelMapInner)
		bRtn[0] = pDoc->m_pReelMapInner->ReloadRst();
	if (pDoc->m_pReelMapInnerUp)
		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadRst();

	if (pDoc->m_pReelMapIts)
		bRtn[2] = pDoc->m_pReelMapIts->ReloadRst();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadRst();
		if (pDoc->m_pReelMapInnerAllUp)
			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadRst();
		if (pDoc->m_pReelMapInnerAllDn)
			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadRst();
	}

	int nRatio[7] = { 0 };
	BOOL bDone[7] = { 0 };
	int nTo = 0;
	if (bDualTest)
		nTo = 600; //[%]
	else
		nTo = 300; //[%]

	dlg.SetRange(0, nTo);

	for (int nProc = 0; nProc < nTo;)
	{
		if (pDoc->m_pReelMapInner)
		{
			nRatio[0] = pDoc->m_pReelMapInner->GetProgressReloadRst();
			bDone[0] = pDoc->m_pReelMapInner->IsDoneReloadRst();
		}
		else
			bDone[0] = TRUE;
		if (!bRtn[0])
			bDone[0] = TRUE;

		if (pDoc->m_pReelMapInnerUp)
		{
			nRatio[1] = pDoc->m_pReelMapInnerUp->GetProgressReloadRst();
			bDone[1] = pDoc->m_pReelMapInnerUp->IsDoneReloadRst();
		}
		else
			bDone[1] = TRUE;
		if (!bRtn[1])
			bDone[1] = TRUE;

		bDone[3] = TRUE;

		if (pDoc->m_pReelMapIts)
		{
			nRatio[2] = pDoc->m_pReelMapIts->GetProgressReloadRst();
			bDone[2] = pDoc->m_pReelMapIts->IsDoneReloadRst();
		}
		else
			bDone[2] = TRUE;
		if (!bRtn[2])
			bDone[2] = TRUE;

		if (bDualTest)
		{
			if (pDoc->m_pReelMapInnerDn)
			{
				nRatio[4] = pDoc->m_pReelMapInnerDn->GetProgressReloadRst();
				bDone[4] = pDoc->m_pReelMapInnerDn->IsDoneReloadRst();
			}
			else
				bDone[4] = TRUE;
			if (!bRtn[4])
				bDone[4] = TRUE;

			if (pDoc->m_pReelMapInnerAllUp)
			{
				nRatio[5] = pDoc->m_pReelMapInnerAllUp->GetProgressReloadRst();
				bDone[5] = pDoc->m_pReelMapInnerAllUp->IsDoneReloadRst();
			}
			else
				bDone[5] = TRUE;
			if (!bRtn[5])
				bDone[5] = TRUE;

			if (pDoc->m_pReelMapAllDn)
			{
				nRatio[6] = pDoc->m_pReelMapInnerAllDn->GetProgressReloadRst();
				bDone[6] = pDoc->m_pReelMapInnerAllDn->IsDoneReloadRst();
			}
			else
				bDone[6] = TRUE;
			if (!bRtn[6])
				bDone[6] = TRUE;

		}
		else
		{
			bDone[4] = TRUE;
			bDone[5] = TRUE;
			bDone[6] = TRUE;
		}

		nProc = nRatio[0] + nRatio[1] + nRatio[2] + nRatio[3] + nRatio[4] + nRatio[5] + nRatio[6];

		if (bDone[0] && bDone[1] && bDone[2] && bDone[3] && bDone[4] && bDone[5] && bDone[6])
			break;
		else
		{
			dlg.SetPos(nProc);
			Sleep(30);
		}
	}

	dlg.DestroyWindow();

	if (bDualTest)
	{
		for (int i = 0; i < 7; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	return TRUE;
}

BOOL CGvisR2R_LaserView::ReloadRstInner(int nSerial)
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	BOOL bRtn[7] = { 0 };
	if (pDoc->m_pReelMapInner)
		bRtn[0] = pDoc->m_pReelMapInner->ReloadRst(nSerial);
	if (pDoc->m_pReelMapInnerUp)
		bRtn[1] = pDoc->m_pReelMapInnerUp->ReloadRst(nSerial);

	bRtn[3] = TRUE;

	if (pDoc->m_pReelMapIts)
		bRtn[2] = pDoc->m_pReelMapIts->ReloadRst(nSerial);

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			bRtn[4] = pDoc->m_pReelMapInnerDn->ReloadRst(nSerial);
		if (pDoc->m_pReelMapInnerAllUp)
			bRtn[5] = pDoc->m_pReelMapInnerAllUp->ReloadRst(nSerial);
		if (pDoc->m_pReelMapInnerAllDn)
			bRtn[6] = pDoc->m_pReelMapInnerAllDn->ReloadRst(nSerial);

		for (int i = 0; i < 7; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (!bRtn[i])
				return FALSE;
		}
	}

	return TRUE;
}

void CGvisR2R_LaserView::OpenReelmapInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->Open();

	if (pDoc->m_pReelMapIts)
		pDoc->m_pReelMapIts->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerDn)
			pDoc->m_pReelMapInnerDn->Open();
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Open();
		if (pDoc->m_pReelMapInnerAllDn)
			pDoc->m_pReelMapInnerAllDn->Open();
	}

	if (pDoc->m_pReelMapInner)
	{
		if (pDoc->m_pReelMapInner->m_nLayer < 0)
			pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;
		pDoc->m_pReelMapInner->Open();
	}
}

void CGvisR2R_LaserView::OpenReelmapInnerUp()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (pDoc->m_pReelMapInnerUp)
		pDoc->m_pReelMapInnerUp->Open();

	if (bDualTest)
	{
		if (pDoc->m_pReelMapInnerAllUp)
			pDoc->m_pReelMapInnerAllUp->Open();
	}

	if (pDoc->m_pReelMapInner)
	{
		if (pDoc->m_pReelMapInner->m_nLayer < 0)
			pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;

		if (bDualTest)
		{
			if (pDoc->m_pReelMapInner->m_nLayer == RMAP_UP || pDoc->m_pReelMapInner->m_nLayer == RMAP_ALLUP)
				pDoc->m_pReelMapInner->Open();
		}
		else
		{
			if (pDoc->m_pReelMapInner->m_nLayer == RMAP_UP)
				pDoc->m_pReelMapInner->Open();
		}
	}
}

void CGvisR2R_LaserView::OpenReelmapInnerDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (!bDualTest)
		return;

	if (pDoc->m_pReelMapInnerDn)
		pDoc->m_pReelMapInnerDn->Open();
	if (pDoc->m_pReelMapInnerAllDn)
		pDoc->m_pReelMapInnerAllDn->Open();

	if (pDoc->m_pReelMapInner)
	{
		if (pDoc->m_pReelMapInner->m_nLayer < 0)
			pDoc->m_pReelMapInner->m_nLayer = pView->m_nSelRmapInner;

		if (pDoc->m_pReelMapInner->m_nLayer == RMAP_DN || pDoc->m_pReelMapInner->m_nLayer == RMAP_ALLDN)
			pDoc->m_pReelMapInner->Open();
	}
}


void CGvisR2R_LaserView::UpdateRstInner()
{
}

void CGvisR2R_LaserView::InitReelmapInner()
{
	pDoc->InitReelmapInner();
	pDoc->SetReelmapInner(ROT_NONE);
}

void CGvisR2R_LaserView::InitReelmapInnerUp()
{
	pDoc->InitReelmapInnerUp();
	pDoc->SetReelmapInner(ROT_NONE);
}

void CGvisR2R_LaserView::InitReelmapInnerDn()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	if (bDualTest)
	{
		pDoc->InitReelmapInnerDn();
		pDoc->SetReelmapInner(ROT_NONE);
	}
}

void CGvisR2R_LaserView::DispDefImgInner()
{
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTestInner;

	CString sNewLot;
	BOOL bNewModel = FALSE;
	int nSerial = 0;
	int nNewLot = 0;
	int nBreak = 0;

	switch (m_nStepTHREAD_DISP_DEF_INNER)
	{
		// CopyDefImg Start ============================================
	case 0:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 1:
		Sleep(300);
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 2:
		if (bDualTest)
			nSerial = m_nBufDnSerial[0]; // ���� Camera
		else
			nSerial = m_nBufUpSerial[0]; // ���� Camera

		if (IsDoneDispMkInfoInner())	 // Check �ҷ��̹��� Display End
		{
			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
				SetSerialMkInfoInner(nSerial, TRUE);		// �ҷ��̹���(��) Display Start
			}
			else if (ChkLastProc() && nSerial < 1)
			{
				SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);		// Reelmap(��) Display Start
				SetSerialMkInfoInner(m_nLotEndSerial + 1, TRUE);		// �ҷ��̹���(��) Display Start
			}
			else
			{
				SetSerialReelmapInner(nSerial);	// Reelmap(��) Display Start
				SetSerialMkInfoInner(nSerial);	// �ҷ��̹���(��) Display Start
			}

			m_nStepTHREAD_DISP_DEF_INNER++;
		}

		break;

	case 3:
		if (IsDoneDispMkInfoInner())	 // Check �ҷ��̹���(��) Display End
		{
			if (bDualTest)
				nSerial = m_nBufDnSerial[1]; // ���� Camera
			else
				nSerial = m_nBufUpSerial[1]; // ���� Camera

			if (nSerial > 0)
			{
				if (ChkLastProc() && (nSerial > m_nLotEndSerial))
					SetSerialMkInfoInner(nSerial, TRUE);	// �ҷ��̹���(��) Display Start
				else
					SetSerialMkInfoInner(nSerial);		// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (ChkLastProc())
					SetSerialMkInfoInner(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			m_nStepTHREAD_DISP_DEF_INNER++;
		}
		break;
	case 4:
		if (bDualTest)
			nSerial = m_nBufDnSerial[1]; // ���� Camera
		else
			nSerial = m_nBufUpSerial[1]; // ���� Camera

		if (nSerial > 0)
		{
			m_nStepTHREAD_DISP_DEF_INNER++;

			if (ChkLastProc() && (nSerial > m_nLotEndSerial))
			{
				SetSerialReelmapInner(nSerial, TRUE);	// Reelmap(��) Display Start
			}
			else
			{
				SetSerialReelmapInner(nSerial);			// Reelmap(��) Display Start
			}
		}
		else
		{
			if (ChkLastProc())
			{
				m_nStepTHREAD_DISP_DEF_INNER++;
				SetSerialReelmapInner(m_nLotEndSerial + 1, TRUE);	// �ҷ��̹���(��) Display Start
			}
			else
			{
				if (bDualTest)
				{
					if (m_bLastProc && m_nBufDnSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
				else
				{
					if (m_bLastProc && m_nBufUpSerial[0] == m_nLotEndSerial)
						m_nStepTHREAD_DISP_DEF_INNER++;
					else
					{
						m_nStepTHREAD_DISP_DEF_INNER++;
					}
				}
			}
		}
		break;
	case 5:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 6:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 7:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 8:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 9:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 10:
		m_nStepTHREAD_DISP_DEF_INNER++;
		break;

	case 11:
		if (IsDoneDispMkInfoInner() && IsRun())	 // Check �ҷ��̹���(��) Display End
			m_nStepTHREAD_DISP_DEF_INNER++;
		break;
	case 12:
		m_bTHREAD_DISP_DEF_INNER = FALSE;
		break;
		// Disp DefImg End ============================================
	}

}

BOOL CGvisR2R_LaserView::IsDoneDispMkInfoInner()
{
	return TRUE;
}

BOOL CGvisR2R_LaserView::SetSerialReelmapInner(int nSerial, BOOL bDumy)
{
	return TRUE;
}

BOOL CGvisR2R_LaserView::SetSerialMkInfoInner(int nSerial, BOOL bDumy)
{
	return TRUE;
}
