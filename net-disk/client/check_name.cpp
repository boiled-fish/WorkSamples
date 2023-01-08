#pragma comment(lib, "ws2_32.lib") 
#include "../../include/client.h"
using namespace std;

//��ȡ��������� 
string cmdPopen(char* cmdLine) {
	char buffer[1024] = { '\0' };
	FILE* pf = NULL;
	pf = _popen(cmdLine, "r");
	if (NULL == pf) {
		printf("open pipe failed\n");
		return std::string("");
	}
	std::string ret;
	while (fgets(buffer, sizeof(buffer), pf)) {
		ret += buffer;
	}
	_pclose(pf);
	return ret;
}
//ͨ��windows�����������ȡ�ļ�md5�� 
string getFileMd5(string file_name)
{
	char command[LEN] = { 0 };
	sprintf(command,"certutil -hashfile %s MD5",file_name.c_str());
	string tmp=cmdPopen(command);
	char cmd_out[LEN] = { 0 };
	strcpy(cmd_out,tmp.c_str());
	const char *sep = "\n";
	char *p;
	p = strtok(cmd_out,sep);
	p = strtok(NULL, sep);
	string md5(p);
	return md5;
}
//����sizeû�õ��� 
bool check_name(string md5, int size, string file_name)
{
	bool ans = access(file_name.c_str(), R_OK | W_OK) == 0;
	if(ans == false)
		return ans;
    else
    {
    	string file_md5 = getFileMd5(file_name);
    	if (size == 0) {
    		cout << "���ļ�, �봫�ɹ�" << endl; 
    		return true;
		}
    	if(file_md5 == md5)
    		cout << "md5��ͬ���봫�ɹ�" << endl;
    	else
    		cout << "�ļ���������md5��ͬ" << endl;
    	return ans;
	}
	return ans;
}
