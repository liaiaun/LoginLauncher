#pragma once

#include <QtWidgets/QWidget>
#include "ui_LoginLauncher.h"

#include"fclientmsg.pb.h"

//导入FProtoClient
using namespace FProtoClient;
using namespace std;

//socket
#include<WinSock2.h>
//loading lib
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"libprotoc.lib")
#pragma comment(lib,"libprotobuf.lib")

class LoginLauncher : public QWidget
{
	Q_OBJECT

public:
	LoginLauncher(QWidget *parent = Q_NULLPTR);
	~LoginLauncher();
	//加载套接字库
	bool LoadingSocketLib();
	//初始化服务器地址族
	void initServerAddrFamily(const char* hostAddr, u_short port);
public slots:
	void connectServer();

private:
	Ui::LoginLauncherClass ui;
	//客户端
	WSADATA wsaData;
	SOCKET clientSock;
	SOCKADDR_IN serverAddr;

};
