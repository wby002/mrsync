// Copyright (C) 2015 Acrosync LLC
//
// Unless explicitly acquired and licensed from Licensor under another
// license, the contents of this file are subject to the Reciprocal Public
// License ("RPL") Version 1.5, or subsequent versions as allowed by the RPL,
// and You may not copy or use this file in either source code or executable
// form, except in compliance with the terms and conditions of the RPL.
//
// All software distributed under the RPL is provided strictly on an "AS
// IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, AND
// LICENSOR HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
// LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, QUIET ENJOYMENT, OR NON-INFRINGEMENT. See the RPL for specific
// language governing rights and limitations under the RPL. 

#include <qobject.h>
#include <qthreadpool.h>
#include <qcoreapplication.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include <rsync/rsync_client.h>
#include <rsync/rsync_entry.h>
#include <rsync/rsync_file.h>
#include <rsync/rsync_log.h>
#include <rsync/rsync_pathutil.h>
#include <rsync/rsync_socketutil.h>
#include <rsync/rsync_sshio.h>
#include <rsync/rsync_socketio.h>
#include <rsync/rsync_util.h>

#include <openssl/md5.h>
#include <libssh2.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <qthread.h>

/****************************************************
*监控程序中内存
****************************************************/
#include <testutil/testutil_assert.h>
#include <testutil/testutil_newdeletemonitor.h> 

#include "download_thread.h"

//qi: TEST_PROGRAM = 1
//qi: LDFLAGS += -lssh2 -lssl -lcrypto -lz
#include <qi/qi_build.h>
#include <crtdbg.h>

int g_cancelFlag = 0;


#if defined(WIN32) || defined(__MINGW32__)
#define strcasecmp _stricmp
#include <Windows.h>
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT) {
		g_cancelFlag = 1;
		return TRUE;
	}
	return FALSE;
}

#endif

using namespace rsync;

#ifdef SINGLE_THREAD

int main(int argc, char *argv[])
{
	TESTUTIL_INIT_RAND

		if (argc < 7) {
			::printf("Usage: %s action server user password remote_dir local_dir\n", argv[0]);
			::printf("       where action is either 'download' or 'upload'\n");
			return 0;
		}

#if defined(WIN32) || defined(__MINGW32__)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);//添加cmd  ctr+c 
#endif

	SocketUtil::startup();

	int rc = libssh2_init(0);
	if (rc != 0) {
		LOG_ERROR(LIBSSH2_INIT) << "libssh2 initialization failed: " << rc << LOG_END
			return 0;
	}

	const char *action = argv[1];
	const char *server = argv[2];
	const char *user = argv[3];
	const char *password = argv[4];

	// Make sure remoteDir ends with '/' to indicate it is a directory
	std::string remoteDir = argv[5];
	/*
	if (remoteDir.back() != '/') {
		remoteDir = remoteDir + "/";
	}
	*/
	std::string localDir = argv[6];
	std::string temporaryFile = PathUtil::join(localDir.c_str(), "acrosync.part");


	Log::setLevel(Log::Debug);

	try {
		SocketIO socketio;
		socketio.connect(server, 873, user, password, "common");
		Client client(&socketio, "rsync", 30, &g_cancelFlag);
		/*消除内存泄露
		std::vector<Entry*> remoteFiles;
		client.getRemoteFiles(remoteDir.c_str(), &remoteFiles);
		for (std::vector<Entry*>::iterator a = remoteFiles.begin(); a != remoteFiles.end(); ++a)
		{
		delete(*a);
		}*/
		//std::cout << "sizeofVector: " << remoteFiles.size() << std::endl;
		//client.list(remoteDir.c_str());// show list of directory
		if (::strcasecmp(action, "download") == 0) {
			client.download(localDir.c_str(), remoteDir.c_str(), temporaryFile.c_str(), 0);
		}
		else if (::strcasecmp(action, "upload") == 0) {
			client.upload(localDir.c_str(), remoteDir.c_str());
		}
		else {
			printf("Invalid action: %s\n", action);
		}
	}
	catch (Exception &e) {
		LOG_ERROR(RSYNC_ERROR) << "Sync failed: " << e.getMessage() << LOG_END
	}


	libssh2_exit();
	SocketUtil::cleanup();


	//return app.exec();
	return 0;
}
#endif



#ifdef TEST_THREADPOOLx
int main(int argc, char *argv[])
{
	//_CrtSetBreakAlloc(596);// 定位内存泄露

#if defined(WIN32) || defined(__MINGW32__)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);//添加cmd  ctr+c 
#endif

	SocketUtil::startup();

	int rc = libssh2_init(0);
	if (rc != 0) {
		LOG_ERROR(LIBSSH2_INIT) << "libssh2 initialization failed: " << rc << LOG_END
			return 0;
	}

	const char *action = "download";//argv[1];
	const char *server = "192.168.10.155";//argv[2];
	const char *user = "WBY";//argv[3];
	const char *password = "070122";//argv[4];

	// Make sure remoteDir ends with '/' to indicate it is a directory
	std::string remoteDir = "common";//argv[5];
	if (remoteDir.back() != '/') {
		remoteDir = remoteDir + "/";
	}
	std::string localDir = "E:/RsyncTest233";//argv[6];
	//std::string temporaryFile = PathUtil::join(localDir.c_str(), "acrosync.part");
	const char* module = "common";//argv[7];

	SocketIO socketio;
	socketio.connect(server, 873, user, password, module);
	Client client(&socketio, "rsync", 30, &g_cancelFlag);
	std::vector<Entry*> remoteFiles;
	client.getRemoteFiles(remoteDir.c_str(), &remoteFiles);

	for (int i = 0; i < remoteFiles.size(); ++i)
	{
		std::cout << "Dir: " << remoteFiles[i]->getPath() << std::endl;
	}
	
	QThreadPool* threadpool = QThreadPool::globalInstance();
	threadpool->setMaxThreadCount(16);
	int i = 1;
	for (std::vector<Entry*>::iterator iter = remoteFiles.begin() + 1; iter != remoteFiles.end(); ++iter)
	{
		DownloadThread *downloadThread = new DownloadThread(action, server, user, password, (*iter)->getPath(), localDir, module, g_cancelFlag);
		threadpool->start(downloadThread);
		
	}
	if (threadpool->waitForDone()) {
		for (std::vector<Entry*>::iterator a = remoteFiles.begin(); a != remoteFiles.end(); ++a)
		{
			delete(*a);
		}
	}
	
	libssh2_exit();
	SocketUtil::cleanup();
	//_CrtDumpMemoryLeaks();
	return 0;

}
#endif

#ifdef TEST_THREADx

int main(int argc, char *argv[])
{
	//_CrtSetBreakAlloc(596);// 定位内存泄露

#if defined(WIN32) || defined(__MINGW32__)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);//添加cmd  ctr+c 
#endif

	SocketUtil::startup();

	int rc = libssh2_init(0);
	if (rc != 0) {
		LOG_ERROR(LIBSSH2_INIT) << "libssh2 initialization failed: " << rc << LOG_END
			return 0;
	}

	const char *action = "download";
	const char *server = "192.168.10.155";
	const char *user = "WBY";
	const char *password = "070122";
	// Make sure remoteDir ends with '/' to indicate it is a directory
	std::string remoteDir = "common/";
	
	if (remoteDir.back() != '/') {
		remoteDir = remoteDir + "/";
	}
	
	std::string localDir = "E:/RsyncTest211";//argv[6];
	const char* module = "common";//argv[7];

	/*
	SocketIO socketio;
	socketio.connect(server, 873, user, password, module);
	Client client(&socketio, "rsync", 30, &g_cancelFlag);
	std::vector<Entry*> remoteFiles;
	client.getRemoteFiles(remoteDir.c_str(), &remoteFiles);
	*/
	DownloadThread downloadThread(action, server, user, password, remoteDir, localDir, module, g_cancelFlag);
	downloadThread.start();
	downloadThread.wait();

	libssh2_exit();
	SocketUtil::cleanup();
	//_CrtDumpMemoryLeaks();
	return 0;
}

#endif