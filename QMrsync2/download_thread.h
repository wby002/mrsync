#ifndef	DOWNLOAD_THREAD
#define DOWNLOAD_THREAD

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
	explicit DownloadThread(const char *server, const char *usr, 
							const char* password, std::string remoteDir,
							std::string localDir, const char *module, int cancelFlag);
	~DownloadThread();
	void createLocalDir(std::string &remoteDir, std::string &localDir);
	void stop();
protected:
	void run();
private:
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

