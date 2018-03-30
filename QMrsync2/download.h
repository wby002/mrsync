#ifndef INCLUDE_DOWNLOAD_H
#define INCLUDE_DOWNLOAD_H

#include <qobject.h>
#include <qthreadpool.h>
#include <qcoreapplication.h>
#include <qqueue.h>
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

//#include <testutil/testutil_assert.h>
//#include <testutil/testutil_newdeletemonitor.h> 

#include "download_thread.h"

//qi: TEST_PROGRAM = 1
//qi: LDFLAGS += -lssh2 -lssl -lcrypto -lz
#include <qi/qi_build.h>
#include <crtdbg.h>

int qthreadDownload(const char* server, const char* user,
	const char* password, std::string remoteDir,
	std::string localDir, const char* module,
	int g_cancelFlag, QQueue<std::string> downloadFiles);

#endif
