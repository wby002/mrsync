#ifndef  DOWNLOAD_THREADPOOL
#define  DOWNLOAD_THREADPOOL

#include <qobject.h>
#include <qrunnable.h>
#include <qthreadpool.h>
#include <qqueue.h>
#include <qmutex.h>

#include <iostream>
#include <string>

namespace rsync {
	class Client;
	class SocketUtil;
	class SocketIO;

	class DownloadThreadpool : public QRunnable
	{
	public:
		DownloadThreadpool( const char *server, const char *usr, const char* password, std::string remoteDir,
			std::string localDir, const char *module, int cancelFlag)
			:d_remoteDir(remoteDir)
			, d_server(server)
			, d_usr(usr)
			, d_password(password)
			, d_localDir(localDir)
			, d_module(module)
			, d_cancelFlag(cancelFlag)
		{
		}
		~DownloadThreadpool()
		{
		}
		void run();
		void createLocalDir(std::string &remoteDir, std::string &localDir);
	private:
		const char *d_server;
		const char *d_usr;
		const char *d_password;
		std::string d_remoteDir;
		std::string d_localDir;
		const char *d_module;
		int d_cancelFlag;
		QMutex mutex;
	};
}

#endif 

