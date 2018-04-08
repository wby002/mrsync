#include "download_threadpool.h"
#include <rsync/rsync_client.h>
#include <rsync/rsync_socketio.h>
#include <rsync/rsync_socketutil.h>
#include <rsync/rsync_pathutil.h>
#include <rsync/rsync_log.h>
#include <iostream>

namespace rsync {
	void DownloadThreadpool::run()
	{
		createLocalDir(d_remoteDir, d_localDir);
		SocketIO socketIO;
		socketIO.connect(d_server, 873, d_usr, d_password, d_module);
		Client client(&socketIO, "rsync", 30, &d_cancelFlag);
		char buff[20];
		itoa(reinterpret_cast<uint>(QThread::currentThread()), buff, 16);
		std::string  temporaryFile = PathUtil::join(d_localDir.c_str(), std::string(buff).append("acrosync.part").c_str());
		client.download(d_localDir.c_str(), d_remoteDir.c_str(), temporaryFile.c_str(), 0);
	}

	void DownloadThreadpool::createLocalDir(std::string &remoteDir, std::string &localDir)
	{
		size_t begin_pos = remoteDir.find_first_of("/");
		size_t end_pos = remoteDir.find_last_of("/");
		localDir = localDir + remoteDir.substr(begin_pos, end_pos - begin_pos);
	}

}
