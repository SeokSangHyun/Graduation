// EnduringLifeServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "cServer.h"
#include "Dump.h"
#include <crtdbg.h>

//#ifndef _DEBUG
//
//#define new new(_CLIENT_BLOCK,__FILE__,__LINE)
//
//#endif

int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	CMiniDump dump;
	dump.Begin();
	cServer* MyServer = new cServer;

	MyServer->Progress();

	delete MyServer;
	dump.End();
	return 0;
}

