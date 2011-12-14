// 文件名:memcheck.h
// 作者 : Moonwell
// msn : archonzhao@hotmail.com
// 在需要检测内存越界的地方加上#include "memcheck.h"
// 程序运行过程中,出现内存越界访问的代码将会出现中断
#ifndef _MEM_CHECK_H
#define _MEM_CHECK_H
void * operator new (size_t size)
{
	//创建的页面个数
	int page_num = (int) ( size / 4096 + 1 );
	//偏移量
	size_t offset = page_num * 4096 - size;
	//在内存块的后面创建一个额外的保护页面,并且将页面的属性设置为不可读写
	void *p = VirtualAlloc(NULL,page_num*4096+1,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	//定位最后一个保护页面的地址
	void *pchecker = (char *)p + page_num*4096;
	//设置最后一页为不可读写
	DWORD old_value;
	VirtualProtect(pchecker,4096,PAGE_NOACCESS,&old_value);
	return (char *)p + offset;
}

void operator delete (void *p)
{
	//找到VirtualAlloc时候的起始地址
	p = (char *)p - (size_t)p % 4096;
	VirtualFree(p,0,MEM_RELEASE);
}

#endif