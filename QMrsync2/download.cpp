#include "download.h"
#include <qthreadpool.h>

namespace rsync {
	int qthreadDownload(const char* server, const char* user,
		const char* password, std::string remoteDir,
		std::string localDir, const char* module,
		int g_cancelFlag, QQueue<std::string> downloadFiles)
	{
		std::cout << "qthread: " << std::endl;

		while (!downloadFiles.empty())
		{
			DownloadThread downloadThread(server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag);
			DownloadThread downloadThread1(server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag);
			downloadThread.start();
			downloadThread1.start();
		}

		return 0;
	}


	int qthreadpoolDownload(const char* server, const char* user,
		const char* password, std::string localDir, const char* module,
		int g_cancelFlag, QQueue<std::string> downloadFiles, int threadpoolCount)
	{
		QThreadPool threadpool;
		threadpool.setMaxThreadCount(threadpoolCount);
		for (; !downloadFiles.isEmpty();)
		{
			//必须指针不能对变量取地址
			DownloadThreadpool* downloadThread = new DownloadThreadpool(server, user, password, downloadFiles.dequeue(), localDir, module, g_cancelFlag);
			threadpool.start(downloadThread);
		}
		if (threadpool.activeThreadCount())
		{
			threadpool.waitForDone();
		}				
		return 0;
	}
}

