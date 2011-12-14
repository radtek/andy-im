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

//ʱ��ṹ
typedef struct _otime
{
	WORD  year;//��
	BYTE  month;//��
	BYTE  day;//��
	BYTE  hour;//ʱ
	BYTE  minute;//��
	BYTE  second;//��
	BYTE  msecond10;//0.01��
	
	//���캯��
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
	
	//���ƹ��캯��
	_otime(const _otime& o)
	{
		*this = o; 
	}
	
	//=��������
	_otime& operator=(const _otime& o)
	{
		year = o.year;//��
		month = o.month;//��
		day = o.day;//��
		hour = o.hour;//ʱ
		minute = o.minute;//��
		second = o.second;//��
		msecond10 = o.msecond10;//0.01��)
		return *this;
	}
	
	//���ڱȽϺ���
	//���ڷ���1
	//С�ڷ���-1
	//���ڷ���0
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

//�ֽ���������
class CByteBuff
{
public:
	//ʹ��һ���ֽ�������һ��ʵ��
	CByteBuff(const BYTE* data=NULL,int len=0)
	{
		m_BuffLen=len;
		if( m_BuffLen>0 )
		{
			//û�д�������ڴ�ʧ�ܵ����
			m_pData = new BYTE[m_BuffLen];
			if( NULL!=data   )
			{//��������
				memcpy(m_pData,data,len);
			}
		}
		else
		{
			m_pData=NULL;
		}
	}
	
	//�����������ͷ��ڴ�
	virtual ~CByteBuff()
	{
		if( NULL!=m_pData )
		{
			delete []m_pData;
			m_pData=NULL;
			m_BuffLen=0;
		}
	}
	
	//�����ֽ���
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
	
	//��ȡ���ݳ���(�ֽ�)
	UINT  GetBuffLen() const 
	{
		return m_BuffLen;
	}

	//��ȡ����ָ��
	BYTE* GetBuffData()	const 
	{
		return m_pData;
	}

protected:
	//����ָ��
	BYTE* m_pData;
	//���ݳ���(�ֽ�)
	UINT  m_BuffLen;
};

//������ջ��渨����
class CRecvBuff
{
public:
	//ʹ��һ���ֽ�������һ��ʵ��
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
	
	//�����������ͷ��ڴ�
	~CRecvBuff()
	{
		if( m_bAlocMem )
			delete []m_pBuffer;
	}
	
	//��ȡ�ֽ�������
	UINT  GetBuffLength() const 
	{
		return m_iLength;
	}
	
	//����һ�����ֽ��з��ŵ�������
	//�ֽ�˳��Ϊ���ֽ���ǰ�����ֽ��ں�
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

	//����һ�����ֽ��޷��ŵ�������
	//�ֽ�˳��Ϊ���ֽ���ǰ�����ֽ��ں�
	BOOL GetUInt(UINT& value)
	{		
		//ֱ��ʹ��GetInt����
		return GetInt((int&)value);		
	}
	
	//����һ��˫�ֽڵ�������
	//�ֽ�˳��Ϊ���ֽ���ǰ�����ֽ��ں�
	BOOL GetDword(DWORD& value)
	{
		//ֱ��ʹ��GetInt����
		return GetInt((int&)value);		
	}

	//��ȡһ�����ֽڵĸ�����
	BOOL GetFloat(float& value)
	{
		//Ϊ��ȫ�����������������Ȳ�Ϊ4�ֽڣ��׳�һ��Debug�쳣
		if( sizeof(value)!=4 )
			DebugBreak();
	
		if( m_iCurPos+4>m_iLength )
			return FALSE;
		//ֱ���ڴ渴�ƣ�ʹ��Ĭ�ϵ��ֽ�˳��
		memcpy(&value,m_pBuffer+m_iCurPos,4);
		m_iCurPos+=4;
		
		return TRUE;	
	}
	
	//��ȡһ���ַ�����len����value�Ĵ�С
	BOOL GetString( char* value, int& len)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;
		//���valueΪ�գ���ʾ�����س���
		if( NULL==value )
		{
			//�����س���
			len = m_pBuffer[m_iCurPos];
			return TRUE;
		}
		//�Ȼ�ȡ�ַ������ȣ�ռһ���ֽ�
		int rlen = m_pBuffer[m_iCurPos++];
		if( m_iCurPos+rlen>m_iLength)
			return FALSE;

		//���������ַ������Ȳ���������FALSE
		if( len<rlen ) 
			return FALSE;
		
		//��������
		memcpy(value,m_pBuffer+m_iCurPos,rlen);
		m_iCurPos += rlen;
		value[rlen]='\0';		
		len=rlen;

		return TRUE;
	}

	//��ȡ�ַ���
	BOOL GetString(std::string& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		//�ַ������ȣ�ռһ���ֽ�
		int rlen = m_pBuffer[m_iCurPos++];
		if( m_iCurPos+rlen>m_iLength)
			return FALSE;
		
		//��������
		char* buff = new char[rlen+1];
		if (NULL != buff)
		{
			memcpy(buff,m_pBuffer+m_iCurPos,rlen);
			m_iCurPos += rlen;
			buff[rlen]='\0';
			
			//��std::string��ֵ
			value = std::string(buff);
			//�ͷ��ڴ�
			delete []buff;
		}

		return TRUE;
	}

	//��ȡһ�����ַ�����len����value�Ĵ�С
	BOOL GetLongString( char* value, int& len)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		WORD wl=0;
		//���valueΪ�գ���ʾ�����س���
		if( NULL==value )
		{
			//�����س���			
			if( m_iCurPos+2>m_iLength)
				return FALSE;
			
			wl  = (m_pBuffer[m_iCurPos]<<8) & 0xff00;
			wl |= (m_pBuffer[m_iCurPos+1])    & 0x00ff;
			len = wl;
			return TRUE;
		}
		//�Ȼ�ȡ�ַ������ȣ�ռһ���ֽ�
		if( !GetWord(wl) )
			return FALSE;
		
		if( m_iCurPos+wl>m_iLength)
			return FALSE;

		//���������ַ������Ȳ���������FALSE
		if( len<wl ) 
			return FALSE;
		
		//��������
		memcpy(value,m_pBuffer+m_iCurPos,wl);
		m_iCurPos += wl;
		value[wl]='\0';		
		len=wl;

		return TRUE;
	}

	//��ȡ���ַ���������ռ���ֽ�
	BOOL GetLongString(std::string& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		int rlen=0;
		if( !GetLongString(NULL,rlen) )
			return FALSE;		
		if( m_iCurPos+rlen>m_iLength)
			return FALSE;
		
		//��������
		char* buff = new char[rlen+1];
		if (NULL != buff)
		{
			GetLongString(buff,rlen);
			
			//��std::string��ֵ
			value = std::string(buff);
			//�ͷ��ڴ�
			delete []buff;
		}
		return TRUE;
	}

	//��ȡһ���ַ�
	BOOL GetChar(char& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		value = m_pBuffer[m_iCurPos++];

		return TRUE;
	}

	//��ȡһ���ֽ�
	BOOL GetByte(BYTE& value)
	{
		if( m_iCurPos>=m_iLength)
			return FALSE;

		value = m_pBuffer[m_iCurPos++];

		return TRUE;
	}

	//��ȡָ�����ȵ��ֽ�������
	BOOL GetBytes(BYTE* data, int len)
	{
		if( m_iCurPos+len>m_iLength)
			return FALSE;
		if( data!=NULL )
			memcpy(data,m_pBuffer+m_iCurPos,len);
		m_iCurPos += len;
		return TRUE;
	}

	//��ȡһ��˫�ֽڵ��з�����
	//�ֽ�˳��Ϊ���ֽ���ǰ�����ֽ��ں�
	BOOL GetShort(short& value)
	{
		if( m_iCurPos+2>m_iLength)
			return FALSE;
		
		value  = (m_pBuffer[m_iCurPos++]<<8) & 0xff00;
		value |= (m_pBuffer[m_iCurPos++])    & 0x00ff;

		return TRUE;
	}

	//��ȡһ��˫�ֽڵ��޷�����
	//�ֽ�˳��Ϊ���ֽ���ǰ�����ֽ��ں�
	BOOL GetWord(WORD& value)
	{
		if( m_iCurPos+2>m_iLength)
			return FALSE;
		
		value  = (m_pBuffer[m_iCurPos++]<<8) & 0xff00;
		value |= (m_pBuffer[m_iCurPos++])    & 0x00ff;

		return TRUE;
	}

	//��ȡʱ�䣬8���ֽڣ������ʱ��ṹ
	BOOL GetTime(OTime& time)
	{
		if( m_iCurPos+8>m_iLength)
			return FALSE;
		
		//�꣬2�ֽ�
		GetWord( time.year );
		//�£�1�ֽ�
		GetByte( time.month );
		//�գ�1�ֽ�
		GetByte( time.day );
		//Сʱ��1�ֽ�
		GetByte( time.hour );
		//���ӣ�1�ֽ�
		GetByte( time.minute );
		//�룬1�ֽ�
		GetByte( time.second );
		//10���룬1�ֽ�
		GetByte( time.msecond10 );		

		return TRUE;
	}
	
	//��ȡ����������
	BOOL GetByteBuff(CByteBuff& value)
	{
		if( m_iCurPos+4>m_iLength)
			return FALSE;

		//���������ݳ��ȣ�4�ֽ�
		int len=0;
		if( !GetInt(len) )
			return FALSE;
		
		if( m_iCurPos+len>=m_iLength)
			return FALSE;
		
		//��������
		value.SetData(m_pBuffer+m_iCurPos,len);		
		m_iCurPos += len;

		return TRUE;
	}

	//��ȡû�б�����������
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
	//�ֽ���
	BYTE* m_pBuffer;	
	//�ܳ���
	int   m_iLength;
	//��ǰλ��
	int   m_iCurPos;
	//�Ƿ������·�����ڴ�
	BOOL  m_bAlocMem;
};


//��������ݸ�����
class CSendBuff
{
public: 

	//Ĭ�Ϲ���һ��256���ֽڵĻ���
	CSendBuff(int size=256)
	{
		if( size<256 )
			size = 256;
		m_pBuffer = new BYTE[size];
		m_iTotal = size;
		m_iLength = 0;
	}

	//�ͷ��ڴ�
	~CSendBuff()
	{
		delete [] m_pBuffer;
	}

	//���»����С
	void Resizing(int newsize)
	{	
		//������湻�ã��������
		if( newsize<=m_iTotal)
			return;
		
		//����һ���µĿռ䣬������ԭ�е�����
		BYTE* pTmp = m_pBuffer;
		m_pBuffer = new BYTE[newsize];
		if (NULL != m_pBuffer)
		{
			memcpy(m_pBuffer,pTmp,m_iLength);
			//�ͷ���ǰ������ڴ�
			delete []pTmp;
			//
			m_iTotal = newsize;	
		}
	}
	
	//��ȡ����ָ��
	BYTE* GetBuffer() const 
	{
		return m_pBuffer;
	}
	
	//��ȡ������Ч���ݴ�С(�ֽ�)
	int GetLength() const 
	{
		return m_iLength;
	}

	//���һ���ַ�
	void AddChar(const char value)
	{
		if( m_iLength+1>=m_iTotal )
			Resizing( 2*m_iTotal );

		m_pBuffer[m_iLength++] = value;		
	}

	//���һ���ֽ�
	void AddByte(const BYTE value)
	{
		if( m_iLength+1>=m_iTotal )
			Resizing( 2*m_iTotal );
		m_pBuffer[m_iLength++] = value;		
	}

	//���һ����СΪlen���ֽ���
	void AddBytes(const BYTE* data, const int len)
	{
		if( m_iTotal<m_iLength+len)
			Resizing(m_iLength+len);
		
		memcpy(m_pBuffer+m_iLength,data,len);
		m_iLength += len;

	}

	//���һ��˫�ֽڵ��з�����
	//���ֽ���ǰ�����ֽ��ں�
	void AddShort(const short value)
	{
		if( m_iLength+2>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		m_pBuffer[m_iLength++] = (value&0xff00)>>8;
		m_pBuffer[m_iLength++] = (value&0x00ff);
	}

	//���һ���޷��ŵ�˫�ֽ���
	void AddWord(const WORD value)
	{
		if( m_iLength+2>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		m_pBuffer[m_iLength++] = (value&0xff00)>>8;
		m_pBuffer[m_iLength++] = (value&0x00ff);		
	}

	//���һ�����ֽڵ��з�������
	//���ֽ���ǰ�����ֽ��ں�
	void AddInt(const int value)
	{
		if( m_iLength+4>=m_iTotal )
			Resizing( 2*m_iTotal );
		
		m_pBuffer[m_iLength++] = (value & 0xff000000) >> 24;
		m_pBuffer[m_iLength++] = (value & 0x00ff0000) >> 16;
		m_pBuffer[m_iLength++] = (value & 0x0000ff00) >> 8;
		m_pBuffer[m_iLength++] = (value & 0x000000ff);		
	}

	//���һ�����ֽڵĸ�����	
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
	
	//���һ���ַ���������ռһ���ֽ�
	void AddString(const char* value)
	{
		int len =0;
		if( NULL!=value)
		{
			len=strlen(value);
			if( len>255 )//max size = 255;
			{
				//�ضϳ��ȴ���255�ֽڵ��ַ���
				len=255;
			}
		}
		
		if( m_iLength+len+1>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		m_pBuffer[m_iLength++]=len;	//���ȣ�ռһ�ֽ�
		if( NULL!=value && len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value,len);
			m_iLength += len;	
		}
	}

	//���һ��std::string�ַ���������ռһ���ֽ�
	void AddString(const std::string& value)
	{
		int len = value.size();
		if( len>255 )//max size = 255;
		{
			//�ضϳ��ȴ���255�ֽڵ��ַ���
			len=255;
		}

		if( m_iLength+len+1>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		
		m_pBuffer[m_iLength++]=len;//���ȣ�ռһ�ֽ�
		if( len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value.c_str(),len);
			m_iLength += len;
		}
	}

	//���һ�����ַ���������ռ�����ֽ�
	void AddLongString(const char* value)
	{
		int len =0;
		if( NULL!=value)
		{
			len=strlen(value);
			if( len>65535 )
			{
				//�ضϳ��ȴ���65535�ֽڵ��ַ���
				len=65535;
			}
		}
		
		if( m_iLength+len+2>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		AddWord( WORD(len) );//���ȣ�ռ���ֽ�
		if( NULL!=value && len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value,len);
			m_iLength += len;	
		}
	}

	//���һ��std::string���ַ���������ռ�����ֽ�
	void AddLongString(const std::string& value)
	{
		int len = value.size();
		if( len>65535 )//
		{
			//�ضϳ��ȴ���65535�ֽڵ��ַ���
			len=65535;
		}

		if( m_iLength+len+2>=m_iTotal )
			Resizing( 2*m_iTotal);
		
		AddWord( WORD(len) );//���ȣ�ռ���ֽ�
		if( len>0 )
		{
			memcpy(m_pBuffer+m_iLength,value.c_str(),len);
			m_iLength += len;
		}
	}

	
	//���ʱ�䣬8�ֽڣ������ʱ��ṹ
	void AddTime(const OTime& value)
	{		
		AddWord(value.year);//�꣬2�ֽ�
		AddByte(value.month);//�£�1�ֽ�
		AddByte(value.day);//�գ�1�ֽ�
		AddByte(value.hour);//ʱ��1�ֽ�
		AddByte(value.minute);//�֣�1�ֽ�
		AddByte(value.second);//�룬1�ֽ�
		AddByte(value.msecond10);//10���룬1�ֽ�
	}

	//���һ�����͸�����
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
	
	//���һ���ֽ�����
	void AddByteBuff(const CByteBuff& buff)
	{
		BYTE* pData = buff.GetBuffData();
		int dLen = buff.GetBuffLen() ;
		if( m_iTotal<m_iLength+dLen+4)
			Resizing(m_iLength+dLen+4);

		//����ռ�ĸ��ֽ�
		AddInt(dLen);		
		memcpy(m_pBuffer+m_iLength,pData,dLen);
		m_iLength += dLen;
	}

	//������������ȣ�Ĭ��λ��Ϊ2������Ϊ�ܳ��ȣ�6
	void ResetCmdLength(int cmdlen=0, int iPos=2)
	{
		if( cmdlen==0)
			cmdlen=m_iLength-6;		
		m_pBuffer[iPos]   = (cmdlen & 0xff000000) >> 24;
		m_pBuffer[iPos+1] = (cmdlen & 0x00ff0000) >> 16;
		m_pBuffer[iPos+2] = (cmdlen & 0x0000ff00) >> 8;
		m_pBuffer[iPos+3] = (cmdlen & 0x000000ff) ;		
	}

	//��������Ѽ��������
	void Clear()
	{
		m_iLength=0;
	}

protected:
	//�ֽڻ���
	BYTE* m_pBuffer;
	//�����С(�ֽ�)
	int   m_iTotal;
	//��Ч���ݳ���(�ֽ�)
	int   m_iLength;	
};

//��֯����ͷ����
#define CMD_RST_HEADER( sbuff,  code,  cid, idx, rst)   \
	sbuff.AddShort(CMDFAG); \
	sbuff.AddInt( 0 ); \
	sbuff.AddInt( code); \
	sbuff.AddInt( cid);	\
	sbuff.AddWord( idx ); \
	sbuff.AddWord( rst ); 
	

//��֯����ͷ����
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

//˵����
//��pHeaderList=NULLʱ��ֻ�轫���ݰ�pSendBuff����uClientID��
//��pHeaderList!=NULLʱ����Ҫ�����Ͷ�����ݰ�����ͬ��uClientID
//��Ӧÿ����pHeaderList�еĽṹ��  uClientIDΪ�ͻ�ID
//����������Ϊ�� pHeader��pSendBuff
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