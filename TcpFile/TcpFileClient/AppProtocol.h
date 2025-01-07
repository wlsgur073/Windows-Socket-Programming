#pragma once

/////////////////////////////////////////////////////////////////////////
//MYCMD ����ü�� nCode ����� ����� �� �ִ� ��
typedef enum CMDCODE {
	CMD_ERROR = 50,			//����
	CMD_GET_LIST = 100,			//C->S: ���� ����Ʈ �䱸
	CMD_GET_FILE,				//C->S: ���� ���� �䱸
	CMD_SND_FILELIST = 200,		//S->C: ���� ����Ʈ ����
	CMD_BEGIN_FILE			//S->C: ���� ���� ������ �˸�.
} CMDCODE;

/////////////////////////////////////////////////////////////////////////
//�⺻���
typedef struct MYCMD
{
	unsigned int nCode;			//����ڵ�
	unsigned int nSize;			//�������� ����Ʈ ���� ũ��
} MYCMD;

/////////////////////////////////////////////////////////////////////////
//Ȯ�����: ���� �޽��� �������
typedef struct ERRORDATA
{
	int	nErrorCode;		//�����ڵ�: ������ Ȯ���� ���� �����.
	char szDesc[256];	//��������
} ERRORDATA;

/////////////////////////////////////////////////////////////////////////
//Ȯ�����: S->C: ���� ����Ʈ ����
typedef struct SEND_FILELIST
{
	unsigned int nCount;	//������ ��������(GETFILE ����ü) ����
} SEND_FILELIST;

/////////////////////////////////////////////////////////////////////////
//Ȯ�����: CMD_GET_FILE
typedef struct GETFILE
{
	unsigned int nIndex;	//���۹������� ������ �ε���
} GETFILE;

/////////////////////////////////////////////////////////////////////////
//Ȯ�����: 
typedef struct FILEINFO
{
	unsigned int nIndex;			//������ �ε���
	char szFileName[_MAX_FNAME];	//�����̸�
	DWORD dwFileSize;				//������ ����Ʈ ���� ũ��
} FILEINFO;

