#include "LoginLauncher.h"
#include"fclientmsg.pb.h"




LoginLauncher::LoginLauncher(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle(QString::fromLocal8Bit("��½��"));
	//�������벻����
	ui.passwordLineEdit->setEnabled(false);

	//���ӷ�����
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(connectServer()));
}

LoginLauncher::~LoginLauncher()
{
	//����
	closesocket(clientSock);
	WSACleanup();
	
}

bool LoginLauncher::LoadingSocketLib()
{
	//�����׽��ֿ�
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

	//�����׽���
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
	//��ʼ���������˵�ַ�����
	serverAddr.sin_addr.S_un.S_addr = inet_addr(hostAddr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
}

void LoginLauncher::connectServer()
{
	// �����׽��ֿⲢ����socket
	LoadingSocketLib();
	//��ʼ���������˵�ַ�����
	initServerAddrFamily("192.168.8.237", 30001);
	//���ӷ�����
	if (0 != ::connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR)))
	{
		//cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!" << endl;
		ui.textBrowser->append(QString::fromLocal8Bit("����loginServerʧ��"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����loginServer�ɹ���"));
	}
	
	//1.�ͻ���ͨ�����õ�ip�Ͷ˿�����loginserver����������ϢMsgLoginVerifyC2S
	//����MsLoginVerifyc2s
	MsgLoginVerifyC2S msg;
	msg.set_account(ui.userLineEdit->text().toStdString());
	msg.set_zoneid(237);
	msg.set_gatetype(1);
	//����buffer
	//�������ĳ���
	int packageLength = 6 + msg.ByteSize();
	char* package = new char[packageLength];
	memset(package, 0, packageLength);
	//����
	int index = 0;
	//���ð��峤��
	int bodyLength = msg.ByteSize() + 2;
	////�����峤��д��buffer
	int *ptrBodyLength = ( int *)package;
	*ptrBodyLength = bodyLength;
	//д��id
	index = 4;
	package[index++] = 1;
	package[index++] = 60;
	//д��msg
	msg.SerializeToArray(&package[index], msg.ByteSize());
	//������Ϣ
	if(send(clientSock, package, packageLength, 0)<0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����MsgLoginVerifyC2S��Ϣʧ�ܣ�"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����MsgLoginVerifyC2S�ɹ���loginServer��"));
	}
	//�ͷſռ䣻
	delete[] package;
	package = nullptr;

	//2.����������MsgLoginResGatewayInfoS2C���ͻ���
	//���հ�ͷ�ĸ��ֽ�,��ȡbodylength
	recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
	package = new char[bodyLength];
	//���ܰ��岿��
	recv(clientSock, package, bodyLength, 0);
	//����loginServer����ϢMsgLoginResGatewayInfoS2C
	MsgLoginResGatewayInfoS2C msgInfoS2C;
	msgInfoS2C.ParseFromArray(&package[2], bodyLength - 2);
	//�ͷ�
	delete[] package;
	package = nullptr;

	
	//3.�ͻ�������MsgLoginResGatewayInfoS2C�����ݺͺ�gatewayserver��������
	const string tmp = msgInfoS2C.ip();
	auto port = msgInfoS2C.port();
	initServerAddrFamily(tmp.c_str(),port);
	//�����׽ӿ�
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	//����gatewayServer
	if (::connect(clientSock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR)) != 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����getwayServer�ɹ���"));
	}
	//������ϢMsgLoginGameVerifyAccountC2S��֤
	MsgLoginGameVerifyAccountC2S msgAccoutC2S;
	msgAccoutC2S.set_accid(msgInfoS2C.accid());
	msgAccoutC2S.set_loginpasswd(msgInfoS2C.loginpasswd());
	//����
	packageLength = 6 + msgAccoutC2S.ByteSize();
	package = new char[packageLength];
	memset(package, 0, packageLength);
	//���ð��峤��
	bodyLength = msgAccoutC2S.ByteSize() + 2;
	////�����峤��д��buffer
	ptrBodyLength = (int *)package;
	*ptrBodyLength = bodyLength;
	//д��id
	index = 4;
	package[index++] = 4;
	package[index++] = 60;
	//д��msgAccoutC2S
	msgAccoutC2S.SerializeToArray(&package[index], msgAccoutC2S.ByteSize());
	//���͸�gatewayServer
	if (send(clientSock, package, packageLength, 0) < 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����MsgLoginGameVerifyAccountC2Sʧ�ܣ�"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����MsgLoginGameVerifyAccountC2S�ɹ���gatewayServer��"));
	}
	//�ͷ�
	delete[] package;
	package = nullptr;

	// �������û�������Ϸʱ��, ����Ϣֻ������Ϣ��
	recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
	package = new char[bodyLength];
	recv(clientSock, package, bodyLength, 0);
	MsgTimeGameTimer msgGameTimer;
	msgGameTimer.ParseFromArray(&package[2], bodyLength - 2);
	auto time = msgGameTimer.gametime();
	//�ͷ�
	delete[] package;
	package = nullptr;

	while (true)
	{
		//4.gatewayServer����MsgLoginGameResS2C���ͻ��˽���
		//���հ�ͷ�ĸ��ֽ�,��ȡbodylength
		recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
		package = new char[bodyLength];
		//��ȡ����
		recv(clientSock, package, bodyLength, 0);
		MsgLoginGameResS2C msgResS2C;
		msgResS2C.ParseFromArray(&package[2], bodyLength - 2);
		//�ͷ�
		delete[] package;
		package = nullptr;
		//5�ͻ��˷�����Ϣ
		//charid=0�����ʹ�����ɫ��ϢMsgCreateCharC2S
		if (msgResS2C.charid() == 0)
		{
			ui.textBrowser->append(QString::fromLocal8Bit("�봴����ɫ��"));
			return;
		}
		else
		{
			break;
		}

	}
	//5.���ͽ�����Ϸ��ϢMsgRquestEnterGameC2S
	MsgRquestEnterGameC2S msgGameC2S;
	//����
	packageLength = 6 + msgGameC2S.ByteSize();
	package = new char[packageLength];
	memset(package, 0, packageLength);
	//���ð��峤��
	bodyLength = msgGameC2S.ByteSize() + 2;
	////�����峤��д��buffer
	ptrBodyLength = (int *)package;
	*ptrBodyLength = bodyLength;
	//д����Ϣid
	index = 4;
	package[index++] = 8;
	package[index++] = 60;
	//д��msgAccoutC2S
	msgGameC2S.SerializeToArray(&package[index], msgGameC2S.ByteSize());
	//���͸�gatewayServer
	if (send(clientSock, package, packageLength, 0) < 0)
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����MsgRquestEnterGameC2Sʧ�ܣ�"));
	}
	else
	{
		ui.textBrowser->append(QString::fromLocal8Bit("����MsgRquestEnterGameC2S�ɹ���gatewayServer��"));
	}
	//�ͷ�
	delete[] package;
	package = nullptr;

	//6.����������MsgEnterGameResS2C��sgMainDataS2C
	//���հ�ͷ�ĸ��ֽ�,��ȡbodylength
	recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
	package = new char[bodyLength];
	//��ȡ����
	recv(clientSock, package, bodyLength, 0);
	MsgEnterGameResS2C msgEnterGame;
	msgEnterGame.ParseFromArray(&package[2], bodyLength - 2);
	//�ͷ�
	delete[] package;
	package = nullptr;


	//msgMainDataS2C��Ϣ
	MsgMainDataS2C msgData;
	while (true)
	{
	
		char msg_id = 0;
		char msg_type_id = 0;
		recv(clientSock, (char*)&bodyLength, sizeof(bodyLength), 0);
		package = new char[bodyLength-2];
		recv(clientSock, &msg_id, sizeof(msg_id),0);
		recv(clientSock, &msg_type_id, sizeof(msg_type_id),0);
		//��ȡ����
		recv(clientSock, package, bodyLength-2, 0);
		if (msg_id == 1 && msg_type_id == 61)
		{
			msgData.ParseFromArray(package, bodyLength - 2);
			break;
		}
		//�ͷ�
		delete[] package;
		package = nullptr;
	}
	//�����ɫ��Ϣ��
	//ͷ��
	//��ɫID
	//level = 3;			//�ȼ�
	//name = 4;		//��ɫ����
	//exp = 6;            //����ֵ
	//fcountry = 11;      // ����
	//viplevel     // �ȼ�
   // accid = 14;     	// �˺�id
	
	ui.textBrowser->append(QString::fromLocal8Bit("��ɫID:"));
	QString str = QString::number(msgData.charid());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("��ɫ�ȼ�:"));
	str = QString::number(msgData.level());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("��ɫ����:"));
	ui.textBrowser->append(QString::fromLocal8Bit(msgData.name().c_str()));

	ui.textBrowser->append(QString::fromLocal8Bit("����ֵ:"));
	str = QString::number(msgData.exp());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("����:"));
	str = QString::number(msgData.fcountry());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("vip�ȼ�:"));
	str = QString::number(msgData.viplevel());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

	ui.textBrowser->append(QString::fromLocal8Bit("�˺�ID:"));
	str = QString::number(msgData.accid());
	ui.textBrowser->append(QString::fromLocal8Bit(str.toLatin1()));

}

