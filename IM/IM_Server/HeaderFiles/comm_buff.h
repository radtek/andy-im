// comm_buff.h: interface for the comm_buffs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COMM_BUFF_H__)
#define _COMM_BUFF_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>

//时间结构
typedef struct _otime
{
	WORD  year;//年
	BYTE  month;//月
	BYTE  day;//日
	BYTE  hour;//时
	BYTE  minute;//分
	BYTE  second;//秒
	BYTE  msecond10;//0.01秒
	
	//构造函数
	_otime()
	{
		year = 1900;
		month = 1;
		day = 1;
		hour = 0;
		minute = 0;
		second = 0;
		msecond10 = 0;
	}
	
	//复制构造函数
	_otime(const _otime& o)
	{
		*this = o; 
	}
	
	//=操作符号
	_otime& operator=(const _otime& o)
	{
		year = o.year;//年
		month = o.month;//月
		day = o.day;//日
		hour = o.hour;//时
		minute = o.minute;//分
		second = o.second;//秒
		msecond10 = o.msecond10;//0.01秒)
		return *this;
	}
	
	//日期比较函数
	//大于返回1
	//小于返回-1
	//等于返回0
	int  Compare(const _otime& o)
	{
		if( year>o.year )		return 1;
		if( year<o.year )		return -1;
		if( month>o.month )		return 1;
		if( month<o.month )		return -1;
		if( hour>o.hour )		return 1;
		if( hour<o.hour )		return -1;
		if( minute>o.minute )	return 1;
		if( minute<o.minute )	return -1;
		if( second>o.second )	return 1;
		if( second<o.second )	return -1;
		if( msecond10>o.msecond10 )	return 1;
		if( msecond10<o.msecond10 )	return -1;
		//same
		return 0;
	}
	
} OTime;

//字节流辅助类
class CByteBuff
{
public:
	//使用一串字节流构造一个实例
	CByteBuff(const BYTE* data=NULL,int len=0)
	{
		m_BuffLen=len;
		if( m_BuffLen>0 )
		{
			//没有处理分配内存失败的情况
			m_pData = new BYTE[m_BuffLen];
			if( NULL!=data   )
			{//拷贝数据
				memcpy(m_pData,data,len);
			}
		}
		else
		{
			m_pData=NULL;
		}
	}
	
	//析构函数，释放内存
	virtual ~CByteBuff()
	{
		if( NULL!=m_pData )
		{
			delete []m_pData;
			m_pData=NULL;
			m_BuffLen=0;
		}
	}
	
	//设置字节流
	void SetData(BYTE* data, int len)
	{
		if( NULL!=m_pData )
		{
			delete []m_pData;
			m_pData=NULL;
			m_BuffLen=0;
		}

		m_pData = new BYTE[len];
		if( NULL!=data )
			memcpy(m_pData,data,len);
		m_BuffLen=len;
	}
	
	//获取数据长度(字节)
	UINT  GetBuffLen() const 
	{
		return m_BuffLen;
	}

	//获取数据指针
	BYTE* GetBuffData()	const 
	{
		return m_pData;
	}

protected:
	//数据指针
	BYTE* m_pData;
	//数据长度(字节)
	UINT  m_BuffLen;
};

//命令接收缓存辅助类
class CRecvBuff
{
public:
	//使用一串字节流构造一个实例
	CRecvBuff(const BYTE* buff, int len, BOOL bAlocMem=TRUE)
	{
		if( bAlocMem )
		{
			m_pBuffer = new BYTE[len];
			if (NULL != m_pBuffer)
			{
				memcpy(m_pBuffer,buff,len);
				m_bAlocMem=TRUE;
			}
		}
		else
		{
			m_pBuffer = (BYTE*)buff;
			m_bAlocMem=FALSE;
		}
		m_iLength=len;
		m_iCurPos=0;
		
	}
	
	//析构函数，释放内存
	~CRecvBuff()
	{
		if( m_bAlocMem )
			delete []m_pBuffer;
	}
	
	//获取字节流长度
	UINT  GetBuffLength() const 
	{
		return m_iLength;
	}
	
	//解析一个四字节有符号的整型数
	//字节顺序为高字节在前，低字节在后
	BOOL GetInt(int& value)
	{
		if( m_iCurPos+4>m_iLength)
			return FALSE;
		//
		value  = (m_pBuffer[m_iCurPos++]<<24) & 0xff000000;
		value |= (m_pBuffer[m_iCurPos++]<<16) & 0x00ff0000;
		value |= (m_pBuffer[m_iCurPos++]<<8)  & 0x0000ff00;
		value |= (m_pBuffer[m_iCurPos++])     & 0x000000ff;
		
		return TRUE;
	}

	//解析一个四字节无符号的整型数
	//字节顺序为高字节在前，低字节在后
	BOOL GetUInt(UINT& value)
	{		
		//直接使用GetInt函数
		return GetInt((int&)value);		
	}
	
	//解析一个双字节的整型数
	//字节顺序为高字节在前，低字节在后
	BOOL GetDword(DWORD& value)
	{
		//直接使用GetInt函数
		return GetInt((int&)value);		
	}

	//获取一个四字节的浮点数
	BOOL GetFloat(float& value)
	{
		//为安全起见，如果浮点数长度不为4字节，抛出一个Debug异常
		if( sizeof(value)!=4 )
			DebugBreak();
	
		if( m_iCurPos+4>m_iLength )
			return FALSE;
		//直接内存复制，使用默认的字节顺序
		memcpy(&value,m_pBuffer+m_iCurPos,4);
		m_iCurPos+=4;
		
		return TRUE;	
	}
	
	//获取一个字符串，len传入value的大小
	BOOL GetString( char* value, int& len)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;
		//如果value为空，表示仅返回长度
		if( NULL==value )
		{
			//仅返回长度
			len = m_pBuffer[m_iCurPos];
			return TRUE;
		}
		//先获取字符串长度，占一个字节
		int rlen = m_pBuffer[m_iCurPos++];
		if( m_iCurPos+rlen>m_iLength)
			return FALSE;

		//如果传入的字符串长度不够，返回FALSE
		if( len<rlen ) 
			return FALSE;
		
		//拷贝数据
		memcpy(value,m_pBuffer+m_iCurPos,rlen);
		m_iCurPos += rlen;
		value[rlen]='\0';		
		len=rlen;

		return TRUE;
	}

	//获取字符串
	BOOL GetString(std::string& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		//字符串长度，占一个字节
		int rlen = m_pBuffer[m_iCurPos++];
		if( m_iCurPos+rlen>m_iLength)
			return FALSE;
		
		//拷贝数据
		char* buff = new char[rlen+1];
		if (NULL != buff)
		{
			memcpy(buff,m_pBuffer+m_iCurPos,rlen);
			m_iCurPos += rlen;
			buff[rlen]='\0';
			
			//对std::string赋值
			value = std::string(buff);
			//释放内存
			delete []buff;
		}

		return TRUE;
	}

	//获取一个长字符串，len传入value的大小
	BOOL GetLongString( char* value, int& len)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		WORD wl=0;
		//如果value为空，表示仅返回长度
		if( NULL==value )
		{
			//仅返回长度			
			if( m_iCurPos+2>m_iLength)
				return FALSE;
			
			wl  = (m_pBuffer[m_iCurPos]<<8) & 0xff00;
			wl |= (m_pBuffer[m_iCurPos+1])    & 0x00ff;
			len = wl;
			return TRUE;
		}
		//先获取字符串长度，占一个字节
		if( !GetWord(wl) )
			return FALSE;
		
		if( m_iCurPos+wl>m_iLength)
			return FALSE;

		//如果传入的字符串长度不够，返回FALSE
		if( len<wl ) 
			return FALSE;
		
		//拷贝数据
		memcpy(value,m_pBuffer+m_iCurPos,wl);
		m_iCurPos += wl;
		value[wl]='\0';		
		len=wl;

		return TRUE;
	}

	//获取长字符串，长度占二字节
	BOOL GetLongString(std::string& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		int rlen=0;
		if( !GetLongString(NULL,rlen) )
			return FALSE;		
		if( m_iCurPos+rlen>m_iLength)
			return FALSE;
		
		//拷贝数据
		char* buff = new char[rlen+1];
		if (NULL != buff)
		{
			GetLongString(buff,rlen);
			
			//对std::string赋值
			value = std::string(buff);
			//释放内存
			delete []buff;
		}
		return TRUE;
	}

	//获取一个字符
	BOOL GetChar(char& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		value = m_pBuffer[m_iCurPos++];

		return TRUE;
	}

	//获取一个字节
	BOOL GetByte(BYTE& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		value = m_pBuffer[m_iCurPos++];

		return TRUE;
	}

	//获取指定长度的字节流数据
	BOOL GetBytes(BYTE* data, int len)
	{
		if( m_iCurPos+len>m_iLength)
			return FALSE;
		if( data!=NULL )
			memcpy(data,m_pBuffer+m_iCurPos,len);
		m_iCurPos += len;
		return TRUE;
	}

	//获取一个双字节的有符号数
	//字节顺序为高字节在前，低字节在后
	BOOL GetShort(short& value)
	{
		if( m_iCurPos+2>m_iLength)
			return FALSE;
		
		value  = (m_pBuffer[m_iCurPos++]<<8) & 0xff00;
		value |= (m_pBuffer[m_iCurPos++])    & 0x00ff;

		return TRUE;
	}

	//获取一个双字节的无符号数
	//字节顺序为高字节在前，低字节在后
	BOOL GetWord(WORD& value)
	{
		if( m_iCurPos+2>m_iLength)
			return FALSE;
		
		value  = (m_pBuffer[m_iCurPos++]<<8) & 0xff00;
		value |= (m_pBuffer[m_iCurPos++])    & 0x00ff;

		return TRUE;
	}

	//获取时间，8个字节，含义见时间结构
	BOOL GetTime(OTime& time)
	{
		if( m_iCurPos+8>m_iLength)
			return FALSE;
		
		//年，2字节
		GetWord( time.year );
		//月，1字节
		GetByte( time.month );
		//日，1字节
		GetByte( time.day );
		//小时，1字节
		GetByte( time.hour );
		//分钟，1字节
		GetByte( time.minute );
		//秒，1字节
		GetByte( time.second );
		//10毫秒，1字节
		GetByte( time.msecond10 );		

		return TRUE;
	}
	
	//获取二进制数据
	BOOL GetByteBuff(CByteBuff& value)
	{
		if( m_iCurPos+4>m_iLength)
			return FALSE;

		//二进制数据长度，4字节
		int len=0;
		if( !GetInt(len) )
			return FALSE;
		
		if( m_iCurPos+len>=m_iLength)
			return FALSE;
		
		//设置数据
		value.SetData(m_pBuffer+m_iCurPos,len);		
		m_iCurPos += len;

		return TRUE;
	}

	//获取没有被解析的数据
	int GetRestData(BYTE* buff=NULL,int len=0)
	{
		if( buff==NULL)
		{
			return m_iLength-m_iCurPos;
		}
		
		memcpy(buff,m_pBuffer+m_iCurPos,m_iLength-m_iCurPos);
		return m_iLength-m_iCurPos;
	}

protected:
	//字节流
	BYTE* m_pBuffer;	
	//总长度
	int   m_iLength;
	//当前位置
	int   m_iCurPos;
	//是否是重新分配的内存
	BOOL  m_bAlocMem;
};


//命令发送数据辅助类
class CSendBuff
{
public: 

	//默认构造一个256个字节的缓存
	CSendBuff(int size=256)
	{
		if( size<256 )
			size = 256;
		m_pBuffer = new BYTE[size];
		m_iTotal = size;
		m_iLength = 0;
	}

	//释放内存
	~CSendBuff()
	{
		delete [] m_pBuffer;
	}

	//更新缓存大小
	void Resizing(int newsize)
	{	
		//如果缓存够用，无需更改
		if( newsize<=m_iTotal)
			return;
		
		//分配一个新的空间，并拷贝原有的数据
		BYTE* pTmp = m_pBuffer;
		m_pBuffer = new BYTE[newsize];
		if (NULL != m_pBuffer)
		{
			memcpy(m_pBuffer,pTmp,m_iLength);
			//释放以前分配的内存
			delete []pTmp;
			//
			m_iTotal = newsize;	
		}
	}
	
	//获取缓存指针
	BYTE* GetBuffer() const 
	{
		return m_pBuffer;
	}
	
	//获取缓存有效数据大小(字节)
	int GetLength() const 
	{
		return m_iLength;
	}

	//添加一个字符
	void AddChar(const char value)
	{
		if( m_iLength+1>=m_iTotal )
			Resizing( 2*m_iTotal );

		m_pBuffer[m_iLength++] = value;		
	}

	//添加一个字节
	void AddByte(const BYTE value)
	{
		if( m_iLength+1>=m_iTotal )
			Resizing( 2*m_iTotal );
		m_pBuffer[m_iLength++] = value;		
	}

	//添加一个大小为len的字节流
	void AddBytes(const BYTE* data, const int len)
	{
		if( m_iTotal<m_iLength+len)
			Resizing(m_iLength+len);
		
		memcpy(m_pBuffer+m_iLength,data,len);
		m_iLength += len;

	}

	//添加一个双字节的有符号数
	//高字节在前，低字节在后
	void AddShort(const short value)
	{
		if( m_iLength+2>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		m_pBuffer[m_iLength++] = (value&0xff00)>>8;
		m_pBuffer[m_iLength++] = (value&0x00ff);
	}

	//添加一个无符号的双字节数
	void AddWord(const WORD value)
	{
		if( m_iLength+2>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		m_pBuffer[m_iLength++] = (value&0xff00)>>8;
		m_pBuffer[m_iLength++] = (value&0x00ff);		
	}

	//添加一个四字节的有符号整数
	//高字节在前，低字节在后
	void AddInt(const int value)
	{
		if( m_iLength+4>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		m_pBuffer[m_iLength++] = (value & 0xff000000) >> 24;
		m_pBuffer[m_iLength++] = (value & 0x00ff0000) >> 16;
		m_pBuffer[m_iLength++] = (value & 0x0000ff00) >> 8;
		m_pBuffer[m_iLength++] = (value & 0x000000ff);		
	}

	//添加一个四字节的浮点数	
	void AddFloat(const float value)
	{
		int size = sizeof(value);
		if( size!=4)
			DebugBreak();
		
		if( m_iLength+4>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		memcpy(m_pBuffer+m_iLength,&value,4);
		m_iLength+=4;
	}
	
	//添加一个字符串，长度占一个字节
	void AddString(const char* value)
	{
		int len =0;
		if( NULL!=value)
		{
			len=strlen(value);
			if( len>255 )//max size = 255;
			{
				//截断长度大于255字节的字符串
				len=255;
			}
		}
		
		if( m_iLength+len+1>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		m_pBuffer[m_iLength++]=len;	//长度，占一字节
		if( NULL!=value && len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value,len);
			m_iLength += len;	
		}
	}

	//添加一个std::string字符串，长度占一个字节
	void AddString(const std::string& value)
	{
		int len = value.size();
		if( len>255 )//max size = 255;
		{
			//截断长度大于255字节的字符串
			len=255;
		}

		if( m_iLength+len+1>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		
		m_pBuffer[m_iLength++]=len;//长度，占一字节
		if( len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value.c_str(),len);
			m_iLength += len;
		}
	}

	//添加一个长字符串，长度占二个字节
	void AddLongString(const char* value)
	{
		int len =0;
		if( NULL!=value)
		{
			len=strlen(value);
			if( len>65535 )
			{
				//截断长度大于65535字节的字符串
				len=65535;
			}
		}
		
		if( m_iLength+len+2>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		AddWord( WORD(len) );//长度，占两字节
		if( NULL!=value && len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value,len);
			m_iLength += len;	
		}
	}

	//添加一个std::string长字符串，长度占二个字节
	void AddLongString(const std::string& value)
	{
		int len = value.size();
		if( len>65535 )//
		{
			//截断长度大于65535字节的字符串
			len=65535;
		}

		if( m_iLength+len+2>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		AddWord( WORD(len) );//长度，占两字节
		if( len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value.c_str(),len);
			m_iLength += len;
		}
	}

	
	//添加时间，8字节，含义见时间结构
	void AddTime(const OTime& value)
	{		
		AddWord(value.year);//年，2字节
		AddByte(value.month);//月，1字节
		AddByte(value.day);//日，1字节
		AddByte(value.hour);//时，1字节
		AddByte(value.minute);//分，1字节
		AddByte(value.second);//秒，1字节
		AddByte(value.msecond10);//10毫秒，1字节
	}

	//添加一个发送辅助类
	void AddSendBuff( const CSendBuff& buff)
	{
		BYTE* pData = buff.GetBuffer();
		int dLen = buff.GetLength();
		if( m_iTotal<m_iLength+dLen)
		{
			Resizing(m_iLength+dLen+256);
		}
		
		memcpy(m_pBuffer+m_iLength,pData,dLen);
		m_iLength += dLen;
	}
	
	//添加一个字节流类
	void AddByteBuff(const CByteBuff& buff)
	{
		BYTE* pData = buff.GetBuffData();
		int dLen = buff.GetBuffLen() ;
		if( m_iTotal<m_iLength+dLen+4)
			Resizing(m_iLength+dLen+4);

		//长度占四个字节
		AddInt(dLen);		
		memcpy(m_pBuffer+m_iLength,pData,dLen);
		m_iLength += dLen;
	}

	//重新设置命令长度，默认位置为2，长度为总长度－6
	void ResetCmdLength(int cmdlen=0, int iPos=2)
	{
		if( cmdlen==0)
			cmdlen=m_iLength-6;		
		m_pBuffer[iPos]   = (cmdlen & 0xff000000) >> 24;
		m_pBuffer[iPos+1] = (cmdlen & 0x00ff0000) >> 16;
		m_pBuffer[iPos+2] = (cmdlen & 0x0000ff00) >> 8;
		m_pBuffer[iPos+3] = (cmdlen & 0x000000ff) ;		
	}

	//清除所有已加入的数据
	void Clear()
	{
		m_iLength=0;
	}

protected:
	//字节缓存
	BYTE* m_pBuffer;
	//缓冲大小(字节)
	int   m_iTotal;
	//有效数据长度(字节)
	int   m_iLength;	
};

//组织命令头数据
#define CMD_RST_HEADER( sbuff,  code,  cid, idx, rst)   \
	sbuff.AddShort(CMDFAG); \
	sbuff.AddInt( 0 ); \
	sbuff.AddInt( code); \
	sbuff.AddInt( cid);	\
	sbuff.AddWord( idx ); \
	sbuff.AddWord( rst ); 
	

//组织命令头数据
#define CMD_HEADER(sbuff,code,cid)  \
	sbuff.AddShort(CMDFAG); \
	sbuff.AddInt( 0 ); \
	sbuff.AddInt( code); \
	sbuff.AddInt( cid);	\
	sbuff.AddWord( CCmdPacket::GetNextCmdIndex()); \
	sbuff.AddWord( 0 ); \

struct CID_HEADER
{
	DWORD uClientID;
	CByteBuff* pHeader;
	CID_HEADER(DWORD cid,CByteBuff* header)
	{
		uClientID=cid;
		pHeader=header;
	}
};

typedef std::list<CID_HEADER> CIDHeaderList;

//说明：
//当pHeaderList=NULL时，只需将数据包pSendBuff发给uClientID；
//当pHeaderList!=NULL时，需要将发送多个数据包给不同的uClientID
//对应每个当pHeaderList中的结构：  uClientID为客户ID
//请命令包组成为： pHeader＋pSendBuff
//
struct CSendData 
{
	DWORD uClientID;
	CIDHeaderList*  pHeaderList;
	CByteBuff* pSendBuff;	
	
	CSendData()
	{
		uClientID=0;
		pHeaderList=NULL;
		pSendBuff=NULL;	
	}
	
	CSendData(UINT uCID, BYTE* pData, DWORD dwLen)
	{
		uClientID = uCID;
		pSendBuff = new	CByteBuff(pData,dwLen);
		pHeaderList=NULL;
	}

	~CSendData()
	{
		if( NULL!=pSendBuff)
			delete pSendBuff;

		if( NULL!=pHeaderList )
		{			
			CIDHeaderList::iterator it = pHeaderList->begin();
			
			while( it!=pHeaderList->end() )
			{
				CID_HEADER& dHeader = *it++;
				
				delete dHeader.pHeader;			
				
			}
			
			delete pHeaderList;
		}
	}

};

typedef std::list<CSendData*> CPSendDataList;

#endif // !defined(_COMM_BUFF_H__)