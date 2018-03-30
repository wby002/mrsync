#include "download.h"

using namespace rsync;


int qthreadDownload(const char* server, const char* user,
	const char* password, std::string remoteDir,
	std::string localDir, const char* module, 
	 int g_cancelFlag, QQueue<std::string> downloadFiles)
{


	//const int threadCount = 5;
	//DownloadThread *downloadThread[threadCount];

	while (!downloadFiles.empty())
	{
		DownloadThread downloadThread(server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag);
		DownloadThread downloadThread1(server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag);
		downloadThread.start();
		downloadThread1.start();
	}
	/*while (!downloadFiles.empty()) {
	for (int i = 0; i < 5; i++) {
	downloadThread[i] = new DownloadThread(action, server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag);
	}
	}*/


	//_CrtDumpMemoryLeaks();
	return 0;
}