#include "../stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "win_impl_base.h"
#include "Emotion.h"
#include "chat_dialog.h"
#include "Emotionlist.h"
#include "../AndyLib/Andylib.h"

const TCHAR* const kTabName = _T("tabs");
const TCHAR* const kVLayoutList=_T("emotion_list");




CEmotion::CEmotion(ChatDialog* chat_dialog, POINT ptMouse)
: m_chat_dialog_(chat_dialog)
, based_point_(ptMouse)
{

	Create(NULL, _T("CEmotion"), WS_POPUP, WS_EX_TOOLWINDOW, 0, 0);
	ShowWindow(true);
	//设置文件过滤标志
	m_dwFilter = FILTER_FOLDER|FILTER_PIC|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE;
	m_vec_FileName.clear();
}
tString CEmotion::GetSkinFile()
{
	return _T("emotion_list.xml");
}

tString CEmotion::GetSkinFolder()
{
	return tString(CPaintManagerUI::GetInstancePath()) + _T("skin\\QQRes\\");
}

LPCTSTR CEmotion::GetWindowClassName() const 
{ 
	return _T("ColorWindow");
}
void CEmotion::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CEmotion::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CControlUI* pOne = static_cast<CControlUI*>(paint_manager_.FindControl(msg.ptMouse));
		if (_tcsicmp(pOne->GetClass(), _T("ButtonUI")) == 0)
		{
			 CButtonUI* pbutton =static_cast<CButtonUI*>(pOne);
			 LPCTSTR strname = pbutton->GetNormalImage();
			 CStdString strImage;
			 CStdString FileName;
			 FileName.Format(_T("%s"),strname);
			 FileName = FileName.Right(FileName.GetLength() - 3);
			 strImage.Format(_T("%s\\skin\\%s"),CPaintManagerUI::GetInstancePath().GetData(),FileName.GetData());
			 if (m_chat_dialog_)
			 {
				 m_chat_dialog_->SetInputEditImg(strImage);
				 Close();
			 }
		}
	}
}
void CEmotion::Init()
{
	CStdString str=CPaintManagerUI::GetInstancePath();
	CStdString strface;
	strface.Format(_T("%s\\skin\\Faces\\"),str);
	initFileName(strface);
	int sizeface = m_vec_FileName.size();

	CVerticalLayoutUI* pColorContiner = static_cast<CVerticalLayoutUI*>(paint_manager_.FindControl(kVLayoutList));
	for (int i = 0; (i < 5) && (pColorContiner != NULL); i ++)
	{
		CHorizontalLayoutUI* pLine = new CHorizontalLayoutUI();
		pLine->SetFixedHeight(32);
		pColorContiner->Add(pLine);
		for (int j = 0; j < 10; j++)
		{
			CButtonUI* pOne = new CButtonUI();
			CStdString strAttribute;
			int index=(i+1)*(j+1);
			strAttribute.Format(_T("name=\"img%d\" bordersize=\"1\" bordercolor=\"#FF000000\" width=\"29\" height=\"28\""),index);
			pOne->ApplyAttributeList(strAttribute.GetData());

			
			if (index < sizeface)
			{
				CStdString strPoneface;
				strPoneface.Format(_T("..\\Faces\\%s"),m_vec_FileName[index]);
				pOne->SetNormalImage(strPoneface.GetData());
			}

			//pOne->ApplyAttributeList(_T("bordersize=\"1\" bordercolor=\"#FF000000\" width=\"29\" height=\"28\""));
			pLine->Add(pOne);
			if (i < 5)
			{
				CControlUI* pMargin = new CControlUI();
				pMargin->SetFixedWidth(2);
				pLine->Add(pMargin);
			}
		}
	}

	SIZE size = paint_manager_.GetInitSize();
	MoveWindow(m_hWnd, based_point_.x - static_cast<LONG>(size.cx / 2), based_point_.y - size.cy, size.cx, size.cy, FALSE);
}


LRESULT CEmotion::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	if (m_chat_dialog_)
	{
		//chat_dialog_->SetTextColor(0);
	}
	Close();
	return 0;
}
int CEmotion::initFileName(CStdString folder)
{
	m_vec_FileName.clear();
	//获取选择的目录后列出图象文件
	andyDir dir(folder.GetData());
	andyFile rf;
	andyStr str;
	char szFile[MAX_PATH],szSize[40];
	int index,nSubIdx,nSize,nData;
	FILETIME fTime;
	DWORD dwAttr,dwCheck;
	int nFileCnt=0,nTotalSize=0;
	if(dir.BeginFindFile())
	{
		SHFILEINFO sfi;
		while(dir.FindNext(szFile))
		{
			if(strcmp(szFile,"..") == 0)
				continue;
			rf.SetFile("%s\\%s",dir.Getdir(),szFile);
			str = rf.Extname();
			str.toLower();
			dwAttr = rf.GetAttrib() & (FILTER_FOLDER|FILTER_HIDDEN|FILE_ATTRIBUTE_ARCHIVE);
			dwCheck = m_dwFilter & dwAttr;
			if((dwAttr != 0xFFFFFFFF /*&& dwCheck > 0*/) && (/*MASK_MATCH(dwCheck,FILTER_FOLDER) ||*/
				(/*MASK_MATCH(m_dwFilter,FILTER_PIC) && */(str == "bmp" || str == "jpg" || 
				str == "jpeg" || str == "gif" || str == "pcx" || str == "png")) /*||*/
				/*MASK_MATCH(m_dwFilter,FILTER_OTHER)*/))
			{				
				LPCTSTR filename = rf.Filename();
				CStdString file;
				file.Format(_T("%s"),filename);
				m_vec_FileName.push_back(file);
				
			}
		}

	}
	return 0;

}