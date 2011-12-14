/**********************************************************************
//

//
//							AndySDK类库 andylib.lib
//作者：
//
//版本：1.0
//

//
//原文件名：andystr.h
//
//说明：
//    字符串操作类 andyStr 实现
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "andylib.h"

void exwsprintf(LPTSTR s,LPCTSTR format,...)
{
	va_list ap;

   if(format)
	{
		va_start(ap, format);		
		_vstprintf(s,format,ap);
		va_end(ap);
	}
}

andyStr::andyStr(LPCTSTR str,DWORD size/*=0*/)
{
   if(str)
   {
      m_len = (DWORD)lstrlen(str);
		m_wSize = 0;
      if(m_len > size)
	      if(m_len < RSTR_INIT)
   	   	m_size = RSTR_INIT;
      	else
      		m_size = m_len + sizeof(TCHAR);
      else
      	m_size = size;

   	m_data = (LPTSTR)New(m_size * sizeof(TCHAR));

      if(m_data)
      	lstrcpy(m_data,str);
      else
      	init();
   }
   else
   {
   	m_len = m_size = m_wSize = 0;
      m_data = NULL;
   }
	m_wData = NULL;
}

andyStr::andyStr(int size)
{
   m_wData = NULL;
	memset(x,0,4);
	
	m_data = NULL;
	m_size = m_len = m_wSize = 0;

   if(size <= RSTR_PCNAME && size >= RSTR_WINPATH)
   	LoadSpecial(size);
	else
	{
		if(size < 0)
			size = 0;

		m_size = size;
		m_len = 0;
 		m_data = (LPTSTR)New(size * sizeof(TCHAR));
		if(!m_data)
   		init();
	}
}

BOOL andyStr::LoadSpecial(int nFlag)
{
	if(m_data && m_size < RSTR_STEP)
		m_data = (LPTSTR)Del(m_data);
	
   if(m_data == NULL)
	{
		m_data = (LPTSTR)New(RSTR_STEP * sizeof(TCHAR));
		m_size = m_data?RSTR_STEP:0;
	}
   else
		Clear();

	m_len = 0;

	if(m_data)
	{		
		switch(nFlag)
		{
		case RSTR_WINPATH:
			m_len = GetWindowsDirectory(m_data,m_size);
			break;
		case RSTR_SYSPATH:
			m_len = GetSystemDirectory(m_data,m_size);
			break;
		case RSTR_TEMPPATH:
			m_len = GetTempPath(m_size,m_data) - 1;
			m_data[m_len] = 0;
			break;
		case RSTR_CURRPATH:
		{
			m_len = GetModuleFileName(GetInstance(),m_data,m_size);
			TCHAR c;
			for(int i=m_len-1;i>1;i--)
			{
				c = m_data[i];
				m_data[i] = 0;
				m_len --;
				if(c == '\\')
					break;
			}
			break;
		}
		case RSTR_MYFILENAME:
			m_len = GetModuleFileName(GetInstance(),m_data,m_size);
			break;
		case RSTR_PCNAME:
			GetComputerName(m_data,&m_len);
			break;
		}
	}
	return (BOOL)m_len;
}

andyStr::andyStr(const andyStr& src)
{
   m_wData = NULL;
	m_wSize = 0;
	if(&src == this || src.m_data == NULL)
   {
		m_data = NULL;
      m_len = 0;
      m_size = 0;
   	return;
	}
   else
   {
		m_data = (LPTSTR)New(sizeof(TCHAR)*src.m_size);
    	if(m_data)
      {
         lstrcpy(m_data,src.m_data);
      	m_len = src.m_len;
         m_size = src.m_size;
      }
   }
}

andyStr::~andyStr()
{
   Release();
}

void andyStr::Release()
{
	m_data = (LPTSTR)Del(m_data);
   m_wData = (WCHAR*)Del(m_wData);
	m_len = m_size = m_wSize = 0;
}

void andyStr::Clear()
{
   if(m_data)
		memset(m_data,0,m_size * sizeof(TCHAR));
   m_len = 0;
}

void andyStr::init()
{
	m_data = (LPTSTR)New(RSTR_INIT * sizeof(TCHAR));
   if(m_data)
   	m_size = RSTR_INIT;
   else
   	m_size = 0;
	m_len = m_wSize = 0;
}

int andyStr::GetLength()
{
	if(m_data)
		m_len = lstrlen(m_data);
	else
		m_len = 0;
	return m_len;
}

TCHAR& andyStr::operator [](int index)
{
	if((DWORD)index < m_size)
		return m_data[index];
	else
	{
		x[0] = '\0';
		return x[0];
	}
}

int andyStr::Load(UINT ident,HINSTANCE hInst)
{
   if(m_data && m_size < RSTR_MAX)
   	m_data = (LPTSTR)Del(m_data);

   if(m_data == NULL)
   {
		m_data = (LPTSTR)New(RSTR_MAX * sizeof(TCHAR));
  	   m_size = RSTR_MAX;
   }
   else
   	Clear();

   m_len = LoadString(hInst,ident,m_data,m_size);

   return m_len;
}

BOOL andyStr::AssignUTF8Str(LPCTSTR str)
{
	if(str)
	{
		int len = lstrlen(str) * sizeof(WCHAR) + 2;
		
		if((int)m_wSize < len)
		{
			Del(m_wData);
			m_wSize = len;
			m_wData = (WCHAR*)New(m_wSize);
		}
		else
			memset(m_wData,0,m_wSize);
		
		if(m_wData)
		{
			len = MultiByteToWideChar(CP_UTF8,0,(LPSTR)str,-1,m_wData,len);
			len = WideCharToMultiByte(CP_ACP,WC_SEPCHARS|WC_COMPOSITECHECK,(LPWSTR)m_wData,-1,NULL,0,NULL,NULL);
		
			SetLength(len+1,TRUE);
			WideCharToMultiByte(CP_ACP,WC_SEPCHARS|WC_COMPOSITECHECK,(LPWSTR)m_wData,-1,(LPSTR)m_data,len,NULL,NULL);
		
			GetLength();
			return TRUE;
		}
	}
	return FALSE;
}

LPCTSTR andyStr::UTF8String(int* nLen)
{
	if(m_data && m_len > 0)
	{
		int len = m_len * sizeof(WCHAR) + 2;
		
		if((int)m_wSize < len)
		{
			Del(m_wData);
			m_wSize = len;
			m_wData = (WCHAR*)New(m_wSize);
		}
		else
			memset(m_wData,0,m_wSize);

		if(m_wData)
			len = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,(LPCSTR)m_data,-1,m_wData,len);
		
		len = WideCharToMultiByte(CP_UTF8,0,(LPWSTR)m_wData,-1,NULL,0,NULL,NULL);
		
		LPTSTR lps = (LPTSTR)New(sizeof(TCHAR)*(len+2));
		if(lps)
		{
			WideCharToMultiByte(CP_UTF8,0,(LPCWSTR)(LPCSTR)m_wData,-1,(LPSTR)lps,len,NULL,NULL);

			if(nLen)
				*nLen = len;

			LPTSTR lpTemp = (LPTSTR)m_wData;
			lstrcpyn(lpTemp,lps,len);
			Del(lps);
			return (LPCTSTR)m_wData;
		}
	}
	return _T("");
}

int andyStr::printf(LPCTSTR format,...)
{
   TCHAR s[RSTR_MAX];
   va_list ap;

   if(format)
	{
		va_start(ap, format);
		_vsntprintf(s,RSTR_MAX-2,format,ap);
		//vsprintf(s, format, ap);
		va_end(ap);
	}

	Clear();
   m_len = lstrlen(s);

   if(m_len < m_size)
   	lstrcpy(m_data,s);
   else
   {
   	Del(m_data);
      if(m_len < RSTR_STEP)
      	m_size = RSTR_STEP;
      else
         m_size = RSTR_MAX;

      m_data = (LPTSTR)New(m_size * sizeof(TCHAR));
      if(m_data)
      	lstrcpy(m_data,s);
      else
      	init();
   }
   return m_len;
}

int andyStr::strcat(LPCTSTR format,...)
{
   TCHAR s[RSTR_MAX];
   va_list ap;

   if(format)
	{
		va_start(ap, format);
		_vsntprintf(s,RSTR_MAX-2,format,ap);
		//vsprintf(s, format, ap);
		va_end(ap);
	}

   m_len += lstrlen(s);

   if(m_len < m_size)
   	::lstrcat(m_data,s);
   else
   {
      LPTSTR temp = m_data;
   	//Del(m_data);
      if(m_len < RSTR_STEP)
      	m_size = RSTR_STEP;
      else if(m_len < RSTR_MAX)
         m_size = RSTR_MAX;
      else
      	m_size = m_len + sizeof(TCHAR);

      m_data = (LPTSTR)New(m_size * sizeof(TCHAR));
      if(m_data)
      	exwsprintf(m_data,_T("%s%s\0"),temp,s);
      else
      	init();

      Del(temp);
   }
   return m_len;
}

//////////////////////////////////////////////////
//
//设置m_data长度
//		bClear:清除数据标志
//		=FALSE：不清除，如果原有m_data长度大于size，不缩减m_data长度，
//				  截断已有数据到size长度
//		=TRUE:清除已有数据
//
//
//////////////////////////////////////////////////
DWORD andyStr::SetLength(DWORD size,BOOL bClear/*=FALSE*/)
{
	if(m_data)
   {
      LPTSTR temp;

   	if(size > m_size)
      {
         temp = m_data;
      	m_data = (LPTSTR)New(size * sizeof(TCHAR));
         if(m_data)
         {
            if(!bClear)
	         	RtlCopyMemory(m_data,temp,m_size);
            else
            	m_len = 0;

				m_size = size;
            Del(temp);
         }
         else
         	m_data = temp;
      }
      else
      {
      	if(bClear)
         	Clear();
			else
				memset(m_data + size,0,(m_size - size)*sizeof(TCHAR));			
      }
   }
   else
   {
		m_data = (LPTSTR)New(size * sizeof(TCHAR));
      if(m_data)
      {
      	m_size = size;
         m_len = 0;
      }
      else
      	init();
   }
   return m_size;
}

void andyStr::resizeBuf(LPCTSTR str)
{
   if(str)
   {
      DWORD len = (DWORD)lstrlen(str);
      if(len < m_size)
      {
         Clear();
      	lstrcpy(m_data,str);
         m_len = len;
      }
      else
      {
         LPTSTR temp = m_data;
   		m_data = (LPTSTR)New((len + 1) * sizeof(TCHAR));
      	if(m_data)
	      {
            m_len = len;
            m_size = len + 1;
            lstrcpy(m_data,str);
            Del(temp);
         }
      	else
         	m_data = temp;
   	}
   }
	else
		Clear();
}

//ANSI与UNICODE通用版本，参数类型根据版本为LPCSTR或LPCWSTR
const andyStr& andyStr::operator=(LPCTSTR str)
{
	if(str != m_data)
		resizeBuf(str);
   return *this;
}

const andyStr& andyStr::operator=(const andyStr& str)
{
	if(this == &str)
   	return *this;

	return operator=(str.m_data);
}

const andyStr& andyStr::operator=(const int number)
{
   if(!m_data)
   	init();
   else if(m_size < RSTR_INIT)
	   SetLength(RSTR_INIT,TRUE);

	wsprintf(m_data,_T("%d\0\0"),number);
   m_len = lstrlen(m_data);
   return *this;
}

//#ifndef _UNICODE
////ANSI版本专用
//const andyStr& andyStr::operator=(LPCWSTR str)
//{
//	if(str)
//	{
//		int z = WideCharToMultiByte(CP_ACP,WC_SEPCHARS|WC_COMPOSITECHECK,(LPWSTR)str,-1,NULL,0,NULL,NULL);
//	
//		SetLength(z+1,TRUE);
//		WideCharToMultiByte(CP_ACP,WC_SEPCHARS|WC_COMPOSITECHECK,(LPWSTR)str,-1,m_data,z,NULL,NULL);
//		
//		GetLength();
//	}
//	else
//		Clear();
//
//   return *this;
//}
//#endif

const andyStr& andyStr::operator=(TCHAR ch)
{
	TCHAR s[2] = {ch,'\0'};
	resizeBuf(s);
	return *this;
}

// const andyStr& andyStr::operator=(const _TUCHAR* psz)
// {
// 	*this = (LPCSTR)psz;
// 	return *this;
// }

const andyStr& andyStr::operator+=(const int number)
{
	TCHAR s[16];
	wsprintf(s,_T("%d\0\0"),number);
	return operator+=(s);
}

const andyStr& andyStr::operator+=(LPCTSTR s)
{
   if(!s)
   	return *this;

   m_len += lstrlen(s);

   if(m_len < m_size)
   	::lstrcat(m_data,s);
   else
   {
      if(m_len < RSTR_STEP)
      	m_size = RSTR_STEP;
      else if(m_len < RSTR_MAX)
         m_size = RSTR_MAX;
      else
      	m_size = m_len + sizeof(TCHAR);

      LPTSTR temp = (LPTSTR)New(sizeof(TCHAR)*m_size);
		if(m_data == NULL)
			lstrcpy(temp,s);
		else
			exwsprintf(temp,_T("%s%s\0"),m_data,s);
		Del(m_data);
      m_data = temp;
   }
   return *this;
}

const andyStr& andyStr::operator+=(const andyStr& str)
{
   return operator+=(str.m_data);
}

const andyStr& andyStr::operator+=(TCHAR ch)
{
	TCHAR s[2] = {ch,'\0'};
	return operator+=(s);
}

BOOL andyStr::operator ==(const andyStr& str) const
{
   if(str.m_data && m_data)
		return (str.m_data == m_data || !lstrcmp(m_data,str.m_data));
   else
		return (!str.m_data && !m_data);   	
}

BOOL andyStr::operator ==(LPCTSTR str) const
{
	if(m_data && str)
		return (m_data == (LPTSTR)str || !lstrcmp(m_data,str));
   else
		return (!m_data && !str);
}

BOOL andyStr::operator !=(const andyStr& str) const
{
	return !operator ==(str);
}

BOOL andyStr::operator !=(LPCTSTR str) const
{
	return !operator ==(str);
}

BOOL andyStr::operator <(const andyStr& str) const
{
	if(str.m_data && m_data)
		return (lstrcmp(m_data,str.m_data) < 0);
   else
		return (BOOL)(str.m_data);   	
}

BOOL andyStr::operator >(const andyStr& str) const
{
	if(str.m_data && m_data)
		return (lstrcmp(m_data,str.m_data) > 0);
   else
		return (BOOL)(m_data);
}

BOOL andyStr::operator <=(const andyStr& str) const
{
   return !operator > (str);
}

BOOL andyStr::operator >=(const andyStr& str) const
{
	return !operator < (str);
}

andyStr andyStr::operator +(const andyStr& rhs) const
{
	andyStr temp = *this;
   temp += rhs.m_data;

   return temp;
}

andyStr andyStr::operator +(TCHAR ch) const
{
	andyStr temp = *this;
   temp += ch;
	
   return temp;
}

int andyStr::cmp(const andyStr& str) const
{
	if(str.m_data && m_data)
		return lstrcmp(m_data,str.m_data);
   else if(m_data)
   	return 1;
   else if(str.m_data)
   	return -1;
   else
   	return 0;
}

int andyStr::cmpi(const andyStr& str) const
{
	if(str.m_data && m_data)
		return lstrcmpi(m_data,str.m_data);
   else if(m_data)
   	return 1;
   else if(str.m_data)
   	return -1;
   else
   	return 0;
}

andyStr andyStr::replace(LPCTSTR str1,LPCTSTR str2,BOOL bAll/* = TRUE*/) const
{
   andyStr temp;
   replace(temp,str1,str2,bAll);
	return temp;
}

BOOL andyStr::replace(andyStr& temp,LPCTSTR str1,LPCTSTR str2,BOOL bAll/* = TRUE*/) const
{
   if(str1 == NULL || str2 == NULL || m_data == NULL || m_len == 0)
	{
		temp = m_data;
		return FALSE;
	}
	
	int l1 = lstrlen(str1);
   int l2 = lstrlen(str2);
   int addstep = l2 - l1;	//如果替换后字串超长，需增加的字串长度
	int i,pos = 0;
   LPTSTR dest,src;
   
   pos = indexOf(str1,pos);
   src = m_data;
   
	if(addstep <= 0)
		temp.SetLength(m_size,TRUE);
	else
		temp.SetLength(m_size + (bAll?count(str1):1) * addstep + 1,TRUE);
	
	dest = temp.m_data;
	
   if(bAll)
   {
		i = 0;
		while(pos >= 0)
      {
			//COPY 匹配字串前面的字符
			lstrcpyn(dest,src,pos - i + 1);
			dest += pos - i;
			src += pos - i;
			//替换 str1 为 str2
			lstrcpyn(dest,str2,l2 + 1);
			dest += l2;
			src += l1;
			i = pos + l1;
         //寻求下一次匹配
         pos = indexOf(str1,i);
      }
   }
   else
   {
		if(pos >= 0)
      {
			//COPY 匹配字串前面的字符
			lstrcpyn(dest,src,pos + 1);
			dest += pos;
			src += pos;
			//替换 str1 为 str2
			lstrcpyn(dest,str2,l2 + 1);
			dest += l2;
			src += l1;			
      }
   }
	//补齐后续字串
	lstrcpyn(dest,src,m_len - (src - m_data) + 1);
	temp.GetLength();
   return TRUE;
}

void andyStr::replace(TCHAR c1,TCHAR c2,BOOL bAll/* = TRUE*/)
{
	LPTSTR tmp = m_data;
	if(tmp)
		if(bAll)
			for(DWORD i=0;i<m_len;i++)
			{
				if(*tmp == c1)
					*tmp = c2;
				tmp ++;
			}
		else
			for(DWORD i=0;i<m_len;i++)
			{
				if(*tmp == c1)
				{
					*tmp = c2;
					return;
				}
				tmp ++;
			}
}

andyStr andyStr::SubString(int start,int count/* = 0*/) const
{
	andyStr temp;

	if(start < 0)
		start = 0;

   if(count <= 0 || (DWORD)(count + start) >= m_len)
   	count = m_len - start;

   if(count <= 0)		//如果start超出范围,m_len - start有可能 <0
   	return temp;

   temp.SetLength(count + sizeof(TCHAR));

   LPTSTR src = m_data + start;
   LPTSTR dest = temp.m_data;

   lstrcpyn(dest,src,count + 1);	
	temp.GetLength();
   return temp;
}

/**************************************************
//
//取匹配的中间字串，
//		例："---{123456}---"
//		SubString("{","}"),返回"123456",
//		nIndex返回最后}的位置，
//		uFlag默认为RSTR_INNER,返回头尾限定字符串的中间字串，
//		如果指定RSTR_OUTER，则连头尾限定字符串一起返回："{123456}"
//
//参数：
//		szStart：为空，从nStart开始返回到szEnd的字符串，有效且未找到，返回NULL
//		szEnd：为空，返回szStart至结尾的字符串，有效且未找到，返回NULL
//
//************************************************/
andyStr andyStr::SubString(LPCTSTR szStart,LPCTSTR szEnd,int nStart/*=0*/,int* nIndex/*=NULL*/,UINT uFlag/*=RSTR_INNER*/) const
{
	if(nStart < 0)
		nStart = 0;

	if((UINT)nStart >= m_len)
	{
		if(nIndex)
			*nIndex = -1;
		return (LPCTSTR)NULL;
	}

	int lens=0,lene=0,s,e=-1,sp;

	if(szStart && *szStart != '\0')
	{
		lens = lstrlen(szStart);
		s = indexOf(szStart,nStart);
		if(s < 0)
		{
			if(nIndex)
				*nIndex = -1;
			return (LPCTSTR)NULL;
		}
		sp = s + lens;
	}
	else
		sp = s = nStart;
	
	if(szEnd && *szEnd != '\0')
	{
		lene = lstrlen(szEnd);
		e = indexOf(szEnd,sp);
		if(e < 0)
		{
			if(MASK_MATCH(uFlag,RSTR_IGNORERIGHT))
				e = m_len;	 //忽略尾限定字符串，返回到结尾
			else
			{
				if(nIndex)
					*nIndex = -1;
				return (LPCTSTR)NULL;
			}
		}
	}
	else
		e = m_len;
	
	if(e - s == lens)
	{
		//头尾同一位置
		if(nIndex)
			*nIndex = e;
		return (LPCTSTR)NULL;
	}
	else	//找到匹配的字串
	{
		if(nIndex)
			*nIndex = e;
		if(MASK_MATCH(uFlag,RSTR_INNER))
			s += lens;
		else if(MASK_MATCH(uFlag,RSTR_OUTER))
			e += lene;

		return SubString(s,e-s);
	}	
}

/*********************************************************
//
//搜索字符串
//
//参数：
//	str:要搜索的字符串
//	start:搜索起始位置，<0：从头开始，>=0：从start位置开始搜索
//	ntimes:第ntimes匹配（从1开始计数）
//
//返回：-1：未找到，否则>=0
//
//*******************************************************/
int andyStr::indexOf(LPCTSTR str,int start/*=-1*/,int ntimes/*=1*/) const
{
   if(!str || *str == '\0' || m_data == NULL || *m_data == '\0' || start > (int)m_len)
   	return -1;
	
	if(start < 0)
		start = 0;

	LPTSTR lpszCur = m_data + start;
	int nCnt = 1,nLen = lstrlen(str);

//修正：原始版本，工程默认定义_MBCS，_tcsstr的_MBCS版本狂慢！
	//while((lpszCur - m_data) < (int)m_len && ((lpszCur = _tcsstr(lpszCur,str)) != NULL))
#ifndef _UNICODE
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = strstr(lpszCur,str)) != NULL))
#else
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = wcsstr(lpszCur,str)) != NULL))
#endif
	{
		if(++nCnt > ntimes)
			return lpszCur - m_data;
		else
			lpszCur += nLen;
	}
	return -1;
}

//ntimes:第ntimes匹配（从1开始计数）
int andyStr::lastIndexOf(LPCTSTR str,int ntimes/*=1*/) const
{
	if(!str || *str == '\0' || m_data == NULL || *m_data == '\0')
		return -1;

	int nLenF = lstrlen(str);
	if(nLenF > (int)m_len)
		return -1;

   LPTSTR lpszEndF = (LPTSTR)str + nLenF -1;

   LPTSTR pS = m_data + m_len - nLenF;
   LPTSTR pFind;
   LPTSTR p;
	int cnt = 1;
   //匹配不成功，被查询串指针将向左移动的字符数
   int nMove;

   while(pS >= m_data)
   {
      pFind = (LPTSTR)str;

      while(pFind <= lpszEndF)
      {
         if(*pS != *pFind)
         {
            //被查询串的当前字符是否在查询串当前字符后面的字串里？
            if(pFind == lpszEndF)
               nMove = nLenF;
            else
            {
            	p = pFind + 1;
               while(p <= lpszEndF)
               {
               	if(*p == *pS)
                  {
		               nMove = p - str;
                     break;
                  }
                  p ++;
               }
               if(p > lpszEndF)
               	nMove = nLenF;
            }
            break;
         }
         pFind ++;
         pS ++;
      }
      if(pFind > lpszEndF)
		{
			cnt ++;
			if(cnt > ntimes)
				return((pS - m_data) - nLenF);
			else
				nMove = nLenF << 1;
		}
      pS -= nMove;
   }
   return -1;
}

//计算含有多少str子串
int andyStr::count(LPCTSTR str) const
{
   if(!str || *str == '\0' || m_data == NULL || *m_data == '\0')
		return 0;
	
	int nCnt = 0,nLen = lstrlen(str);
	LPTSTR lpszCur = m_data;

//修正：原始版本，工程默认定义_MBCS，_tcsstr的_MBCS版本狂慢！
	//while((lpszCur - m_data) < (int)m_len && ((lpszCur = _tcsstr(lpszCur,str)) != NULL))
#ifndef _UNICODE
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = strstr(lpszCur,str)) != NULL))
#else
	while((lpszCur - m_data) < (int)m_len && ((lpszCur = wcsstr(lpszCur,str)) != NULL))
#endif	
	{
		nCnt ++;
		lpszCur += nLen;
	}

	return nCnt;
}

andyStr andyStr::Insert(const andyStr& str, int index)
{
   andyStr temp;
   int i,cnt = str.len();
   LPTSTR src,dest;

	if(cnt == 0 || isClear())
	{
		temp = *this;
		return temp;
	}

   if((cnt + m_len) > m_size)
   	temp.SetLength(cnt + m_len + sizeof(TCHAR),TRUE);
   else
   	temp.SetLength(m_size,TRUE);

   src = m_data;
   dest = temp.m_data;

   for(i=0;i<index;i++)
   	*dest++ = *src++;

   for(i=0;i<cnt;i++)
   	*dest++ = str.m_data[i];

   cnt = m_len - index;

   for(i=0;i<cnt;i++)
   	*dest++ = *src++;

   return temp;
}

int andyStr::Delete(int index,int count)
{
	if(isClear() || index > (int)m_len || count <= 0 || index + count < 0)
		return 0;
	
	if(index < 0)
	{
		count += index;
		index = 0;
	}
	
	if(index + count > (int)m_len)
		count = m_len - index;
	
   LPTSTR src,dest;
	
   dest = m_data + index;
   src = dest + count;

   memmove(dest,src,(m_len - index - count)*sizeof(TCHAR));
	m_len -= count;
	m_data[m_len] = '\0';
	return m_len;
}

void andyStr::toLower()
{
	if(m_data && m_len > 0)
	{
		LPTSTR tmp = m_data + m_len - 1;
		while(tmp >= m_data)
		{
			*tmp = (char)tolower(*tmp);
			tmp --;
		}
	}
}

void andyStr::toUpper()
{
	if(m_data && m_len > 0)
	{
		LPTSTR tmp = m_data + m_len - 1;
		while(tmp >= m_data)
		{
   		*tmp = (TCHAR)toupper(*tmp);
			tmp --;
		}
	}
}

void andyStr::Trim()
{
	RTrim();
	LTrim();   
}

void andyStr::LTrim()
{
   if(m_data == NULL || m_len < 1)
		return;

	LPTSTR start = m_data,tmp = m_data,end = m_data + m_len - 1;

   while(start <= end)
   {
   	if(*start == ' ')
      {
			start ++;
			m_len --;
		}
      else
      	break;
   }
   if(start > m_data)
   {
		while(start <= end)
      {
      	*tmp++ = *start++;
      }
		memset(tmp,0,(end - (tmp - 1)) * sizeof(TCHAR));
	}
}

void andyStr::RTrim()
{
	if(m_data == NULL || m_len < 1)
		return;

	LPTSTR tmp = m_data + m_len - 1;
	while(tmp >= m_data)
   {
   	if(*tmp == ' ')
      {
      	*tmp-- = 0;
         m_len --;
      }
      else
      	break;
   }
}

WCHAR* andyStr::wstring(int* nLen)
{
	int len = (m_len * sizeof(WCHAR)) + 2;
	if((int)m_wSize < len)
	{
		m_wSize = len;
		Del(m_wData);
		m_wData = (WCHAR*)New(len);
	}
	else
		RtlZeroMemory(m_wData,m_wSize);
	
	if(m_wData && m_data)
	{
		len = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,(LPCSTR)m_data,-1,m_wData,len);
		if(nLen)
			*nLen = len;
		return m_wData;
	}
	return L"";
}

andyStr operator +(LPTSTR lhs, const andyStr& rhs)
{
	andyStr temp(lhs);
   temp += rhs.string();
   return temp;
}

//添加count个c字符到串尾
void andyStr::Append(TCHAR c,int count)
{
	LPTSTR s = (LPTSTR)New((count+1) * sizeof(TCHAR));
	LPTSTR p = s;
	if(s)
	{
		for(int i=0;i<count;i++)
			*p++ = c;
		operator+=(s);
	}
	Del(s);
}

//添加lpstr和若干个c字符到串尾，总添加字符数为count
//如果lpstr长度大于count，截断lpstr为count个字符
void andyStr::AppendSize(LPCTSTR lpstr,TCHAR c,int count)
{
	if(count <= 0)
		return;

	int n = lstrlen(lpstr);
	LPTSTR p,s = (LPTSTR)New((count+2) * sizeof(TCHAR));
	
	if(s)
	{
		if(lpstr)
			lstrcpyn(s,lpstr,count+1);	//+1:最后的'\0'

		p = s + lstrlen(s);
		for(int i=p-s;i<count;i++)
			*p++ = c;		
		operator+=(s);
	}
	Del(s);
}


/***********废弃代码，但有参考价值，字符串搜索算法***********************
//ntimes:第ntimes匹配（从1开始计数）
//返回：-1：未找到，否则>=0
int andyStr::indexOf(LPCTSTR str,int start,int ntimes) const
{
   if(!str || m_len < 1)
		return -1;
	
	if(start < 0)
		start = 0;
	
	int nLenF = lstrlen(str);
	if(nLenF < 1)
		return -1;
	
   LPTSTR lpszEndF = (LPSTR)str + nLenF -1;
   LPTSTR lpszEndS = m_data + m_len -1;
	
   LPSTR pS = m_data + start + nLenF -1;
   LPSTR pFind;
   LPSTR p;
   //匹配不成功，被查询串指针将向右移动的字符数
   int nMove;
	int cnt = 1;
   BOOL bEnd = FALSE;
	
   while(pS <= lpszEndS)
   {
		//从右向左匹配。
      pFind = lpszEndF;
      //比较被查询串的当前字符是否和查询串的最右边字
      //符匹配，如果匹配则回溯比较
      if(pS == lpszEndS)
			bEnd = TRUE;
		
      while(pFind >= str)
      {
         if(*pS != *pFind)
         {
            //被查询串的当前字符是否在查询串当前字符前面的字串里？
            if(pFind == str)
               nMove = nLenF;
            else
            {
					p = pFind - 1;
               while(p >= str)
               {
						if(*p == *pS)
                  {
							nMove = lpszEndF - p;
                     break;
                  }
                  p --;
               }
               if(p < str)
						nMove = nLenF;
            }
            break;
         }
         // 匹配成功一个，向左回溯
         pFind --;
         pS --;
      }
      if(pFind < str)
      {
			cnt ++;
			if(cnt > ntimes)
				return((pS - m_data) + 2);
			else
				nMove = nLenF << 1;
		}
		
      if(bEnd)
			return 0;
		
      pS += nMove;
   }
   return 0;
}
**********************************************************/
