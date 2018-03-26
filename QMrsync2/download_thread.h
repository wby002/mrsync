#ifndef	DOWNLOAD_THREAD
#define DOWNLOAD_THREAD

#ifdef TEST_THREAD
#include "qthread.h"
#include <iostream>
#include <string>

namespace rsync 
{
class Client;
class SocketUtil;
class SocketIO;

class DownloadThread : public QThread 
{
	Q_OBJECT
public:
	explicit DownloadThread(const char *action, const char *server, const char *usr, 
							const char* password, std::string remoteDir,
							std::string localDir, const char *module, int cancelFlag);
	~DownloadThread();
	void stop();
protected:
	void run();
private:
	const char *d_action;
	const char *d_server;
	const char *d_usr;
	const char *d_password;
	std::string d_remoteDir;
	std::string d_localDir;
	const char *d_module;
	int d_cancelFlag;

};

}//namesapce rsync


#endif

#ifdef TEST_THREADPOOLx
#include <qobject.h>
#include <qrunnable.h>
#include <qthreadpool.h>
#include <iostream>
#include <string>

namespace rsync {
	class Client;
	class SocketUtil;
	class SocketIO;

	class DownloadThread : public QRunnable
	{
	public:
		DownloadThread(const char *action, const char *server, const char *usr, const char* password, std::string remoteDir,
			std::string localDir, const char *module, int cancelFlag)
			:d_action(action)
			, d_server(server)
			, d_usr(usr)
			, d_password(password)
			, d_remoteDir(remoteDir)
			, d_localDir(localDir)
			, d_module(module)
			, d_cancelFlag(cancelFlag)
		{
		}
		~DownloadThread()
		{

		}
		void run();
	private:
		const char *d_action;
		const char *d_server;
		const char *d_usr;
		const char *d_password;
		std::string d_remoteDir;
		std::string d_localDir;
		const char *d_module;
		int d_cancelFlag;

	};
}
#endif 
#endif

