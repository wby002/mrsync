#include "download.h"
#include "download_thread.h"

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

int main(int argc, char* argv[])
{
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
	std::string remoteDir = argv[5];
	std::string localDir = argv[6];
	const char* module = argv[7];
	std::string threadPoolNumber = argv[8];

	int threadpoolCount = std::stoi(threadPoolNumber);
	SocketIO socketio;
	socketio.connect(server, 873, user, password, module);
	Client client(&socketio, "rsync", 30, &g_cancelFlag);
	std::vector<Entry*> remoteFiles;
	client.getRemoteFiles(remoteDir.c_str(), &remoteFiles);
	QQueue<std::string> downloadFiles;
	
	for (int i = 0; i < remoteFiles.size(); ++i)
	{
		if (!remoteFiles[i]->isDirectory()) {
			downloadFiles.enqueue("./" + std::string(remoteFiles[i]->getPath()));
		}
		delete remoteFiles[i];
	}
	if (::strcasecmp(action, "download") == 0)
	{
		    //use thread
		//qthreadDownload(server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag,downloadFiles); 
		    //use threadpool
		qthreadpoolDownload(server, user, password, localDir, module, g_cancelFlag, downloadFiles, threadpoolCount);	
	}
	else if (::strcasecmp(action, "upload") == 0) {
		client.upload(localDir.c_str(), remoteDir.c_str());
	}
	else {
		printf("Invalid action: %s\n", action);
	}
	
	libssh2_exit();
	SocketUtil::cleanup();
	return 0;
}