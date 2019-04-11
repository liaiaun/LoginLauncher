#include "LoginLauncher.h"
#include"fclientmsg.pb.h"




LoginLauncher::LoginLauncher(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle(QString::fromLocal8Bit("登陆器"));
	//设置密码不可用
	ui.passwordLineEdit->setEnabled(false);

	//连接服务器
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(connectServer()));
}

LoginLauncher::~LoginLauncher()
{
	//清理
	closesocket(clientSock);
	WSACleanup();
	
}

bool LoginLauncher::LoadingSocketLib()
{
	//加载套接字库
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit ("WSAStartup(MAKEWORD(2, 2), &wsaData) execute failed!"));
		return false;
	}
	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		WSACleanup();
		ui.textBrowser->append(QString::fromLocal8Bit("WSADATA version is not correct!"));
		return false;
	}

	//创建套接字
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("clientSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!"));
		return false;
	}
	return true;
}

void LoginLauncher::initServerAddrFamily(const char* hostAddr,u_short port)
{
	//初始化服务器端地址族变量
	serverAddr.sin_addr.S_un.S_addr = inet_addr(hostAddr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
}

void LoginLauncher::connectServer()
{
	// 加载套接字库并创建socket
	LoadingSocketLib();
	//初始化服务器端地址族变量
	initServerAddrFamily("192.168.8.237", 30001);
	//连接服务器
	if (0 != ::connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR)))
	{
		//cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!" << endl;
		ui.textBrowser->append(QString::fromLocal8Bit("连接loginServer失败"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("连接loginServer成功！"));
	}
	
	//1.客户端通过配置的ip和端口连接loginserver，并发送消息MsgLoginVerifyC2S
	//创建MsLoginVerifyc2s
	MsgLoginVerifyC2S msg;
	msg.set_account(ui.userLineEdit->text().toStdString());
	msg.set_zoneid(237);
	msg.set_gatetype(1);
	//创建buffer
	//创建包的长度
	int packageLength = 6 + msg.ByteSize();
	char* package = new char[packageLength];
	memset(package, 0, packageLength);
	//索引
	int index = 0;
	//设置包体长度
	int bodyLength = msg.ByteSize() + 2;
	////将包体长度写入buffer
	int *ptrBodyLength = ( int *)package;
	*ptrBodyLength = bodyLength;
	//写入id
	index = 4;
	package[index++] = 1;
	package[index++] = 60;
	//写入msg
	msg.SerializeToArray(&package[index], msg.ByteSize());
	//发送消息
	if(send(clientSock, package, packageLength, 0)<0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("发送MsgLoginVerifyC2S消息失败！"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("发送MsgLoginVerifyC2S成功给loginServer！"));
	}
	//释放空间；
	delete[] package;
	package = nullptr;

	//2.服务器发送MsgLoginResGatewayInfoS2C给客户端
	//接收包头四个字节,获取bodylength
	recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
	package = new char[bodyLength];
	//接受包体部分
	recv(clientSock, package, bodyLength, 0);
	//接受loginServer的消息MsgLoginResGatewayInfoS2C
	MsgLoginResGatewayInfoS2C msgInfoS2C;
	msgInfoS2C.ParseFromArray(&package[2], bodyLength - 2);
	//释放
	delete[] package;
	package = nullptr;

	
	//3.客户端利用MsgLoginResGatewayInfoS2C的内容和和gatewayserver建立连接
	const string tmp = msgInfoS2C.ip();
	auto port = msgInfoS2C.port();
	initServerAddrFamily(tmp.c_str(),port);
	//创建套接口
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	//连接gatewayServer
	if (::connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR)) != 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("连接getwayServer成功！"));
	}
	//发送消息MsgLoginGameVerifyAccountC2S验证
	MsgLoginGameVerifyAccountC2S msgAccoutC2S;
	msgAccoutC2S.set_accid(msgInfoS2C.accid());
	msgAccoutC2S.set_loginpasswd(msgInfoS2C.loginpasswd());
	//包长
	packageLength = 6 + msgAccoutC2S.ByteSize();
	package = new char[packageLength];
	memset(package, 0, packageLength);
	//设置包体长度
	bodyLength = msgAccoutC2S.ByteSize() + 2;
	////将包体长度写入buffer
	ptrBodyLength = (int *)package;
	*ptrBodyLength = bodyLength;
	//写入id
	index = 4;
	package[index++] = 4;
	package[index++] = 60;
	//写入msgAccoutC2S
	msgAccoutC2S.SerializeToArray(&package[index], msgAccoutC2S.ByteSize());
	//发送给gatewayServer
	if (send(clientSock, package, packageLength, 0) < 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("发送MsgLoginGameVerifyAccountC2S失败！"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("发送MsgLoginGameVerifyAccountC2S成功到gatewayServer！"));
	}
	//释放
	delete[] package;
	package = nullptr;

	// 网关向用户发送游戏时间, 此消息只用于消息戳
	recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
	package = new char[bodyLength];
	recv(clientSock, package, bodyLength, 0);
	MsgTimeGameTimer msgGameTimer;
	msgGameTimer.ParseFromArray(&package[2], bodyLength - 2);
	auto time = msgGameTimer.gametime();
	//释放
	delete[] package;
	package = nullptr;

	while (true)
	{
		//4.gatewayServer发送MsgLoginGameResS2C，客户端接受
		//接收包头四个字节,获取bodylength
		recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
		package = new char[bodyLength];
		//获取包体
		recv(clientSock, package, bodyLength, 0);
		MsgLoginGameResS2C msgResS2C;
		msgResS2C.ParseFromArray(&package[2], bodyLength - 2);
		//释放
		delete[] package;
		package = nullptr;
		//5客户端发送消息
		//charid=0，发送创建角色信息MsgCreateCharC2S
		if (msgResS2C.charid() == 0)
		{
			ui.textBrowser->append(QString::fromLocal8Bit("请创建角色！"));
			return;
		}
		else
		{
			break;
		}

	}
	//5.发送进入游戏信息MsgRquestEnterGameC2S
	MsgRquestEnterGameC2S msgGameC2S;
	//包长
	packageLength = 6 + msgGameC2S.ByteSize();
	package = new char[packageLength];
	memset(package, 0, packageLength);
	//设置包体长度
	bodyLength = msgGameC2S.ByteSize() + 2;
	////将包体长度写入buffer
	ptrBodyLength = (int *)package;
	*ptrBodyLength = bodyLength;
	//写入消息id
	index = 4;
	package[index++] = 8;
	package[index++] = 60;
	//写入msgAccoutC2S
	msgGameC2S.SerializeToArray(&package[index], msgGameC2S.ByteSize());
	//发送给gatewayServer
	if (send(clientSock, package, packageLength, 0) < 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("发送MsgRquestEnterGameC2S失败！"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("发送MsgRquestEnterGameC2S成功到gatewayServer！"));
	}
	//释放
	delete[] package;
	package = nullptr;

	//6.服务器发送MsgEnterGameResS2C和sgMainDataS2C
	//接收包头四个字节,获取bodylength
	recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
	package = new char[bodyLength];
	//获取包体
	recv(clientSock, package, bodyLength, 0);
	MsgEnterGameResS2C msgEnterGame;
	msgEnterGame.ParseFromArray(&package[2], bodyLength - 2);
	//释放
	delete[] package;
	package = nullptr;


	//msgMainDataS2C消息
	MsgMainDataS2C msgData;
	while (true)
	{
	
		char msg_id = 0;
		char msg_type_id = 0;
		recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
		package = new char[bodyLength-2];
		recv(clientSock, &msg_id, sizeof(msg_id),0);
		recv(clientSock, &msg_type_id, sizeof(msg_type_id),0);
		//获取包体
		recv(clientSock, package, bodyLength-2, 0);
		if (msg_id == 1 && msg_type_id == 61)
		{
			msgData.ParseFromArray(package, bodyLength - 2);
			break;
		}
		//释放
		delete[] package;
		package = nullptr;
	}
	//输出角色信息；
	//头像
	//角色ID
	//level = 3;			//等级
	//name = 4;		//角色名称
	//exp = 6;            //经验值
	//fcountry = 11;      // 国家
	//viplevel     // 等级
   // accid = 14;     	// 账号id
	
	ui.textBrowser->append(QString::fromLocal8Bit("角色ID:"));
	QString str = QString::number(msgData.charid());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("角色等级:"));
	str = QString::number(msgData.level());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("角色名称:"));
	ui.textBrowser->append(QString::fromLocal8Bit(msgData.name().c_str()));

	ui.textBrowser->append(QString::fromLocal8Bit("经验值:"));
	str = QString::number(msgData.exp());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("国家:"));
	str = QString::number(msgData.fcountry());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("vip等级:"));
	str = QString::number(msgData.viplevel());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("账号ID:"));
	str = QString::number(msgData.accid());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

}

