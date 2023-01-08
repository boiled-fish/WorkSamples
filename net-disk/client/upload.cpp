// upload.cpp
// 2022.6.16
// author: boiled_fish
#pragma comment(lib, "ws2_32.lib")
#include "../../include/client.h"
using namespace std;

/*���鷴ת����*/ 
void reverse(char* a, int len)
{
	int i = 0;
	int t;
	int j;

	for (i = 0; i < len / 2; i++) {
		t = a[i];
		a[i] = a[len - 1 - i];
		a[len - 1 - i] = t;
	}
}
/*intת�����ַ�������*/
void int2string(int b, char* c)
{
	char d[BUF_LEN];
	char temp[2] = { 0 };//ת����������Ҫ�ݴ�����	
	int i;

	while (b % 10) {
		char a = b % 10 + '0';
		temp[0] = a;
		b = b / 10;
		strcat(c, temp);
	}
	reverse(c, strlen(c));
}
/*�ַ���ת����int����*/
int string2int(char b[])
{
	int i;
	int res = 0, temp_res;
	
	for (i = 0; i < strlen(b); i++) {
		temp_res = (b[i] - '0');
		res *= 10;
		res += temp_res;
	}
	return res;
}
/*��servre��·��*/
void pan_path(data_package* pkg, char* path) {
	int len1 = strlen(pkg->username);
	int len2 = strlen(pkg->content);
	int i, pivot = 0;
	for (i = 0; i < len1; ++i) {
		if (pkg->username[i] == '\\') {
			pivot = i;
		}
	}
	strcpy(path, pkg->content);
	if (pivot != 0) {
		strcpy(path + len2, pkg->username + pivot + 1);
	}
	else {
		strcpy(path + len2, pkg->username);	
	}
}

bool upload_file(data_package* pkg, SOCKET sock) {
	/*���check��*/
	/*op��PAN_UPLOAD_FILE_CHECK*/ 
	/*username: md5*/
	/*content: s���ļ�·��-�ļ���С*/
	data_package check_pkg;
	check_pkg.op = PAN_UPLOAD_FILE_CHECK;
	/*����s��·��*/
	pan_path(pkg, check_pkg.content);
	int content_len = strlen(check_pkg.content);
	check_pkg.content[content_len] = '-';
	/*�����ļ���С*/
	char file_size[BUF_LEN] = { 0 };
	int full_size = 0;
	FILE* file = fopen(pkg->username, "r");
	if (file)
	{
		full_size = filelength(fileno(file));
		fclose(file);
	}
	//int2string(size, file_size);
	sprintf(file_size, "%d", full_size);
	strcpy(check_pkg.content + content_len + 1, file_size);
	cout << "file_size: " << file_size << endl;
	/*����md5*/
	if (full_size != 0) {
		if (!get_md5(pkg->username, check_pkg.username)) {
			cout << "cal md5 failed" << endl;
			return false;
		}
	}
	else {
		//char zero[BUF_LEN] = { 0 };
		sprintf(check_pkg.username, "0");
	}
	
	/*����check��*/
	cout << "check pkg md5: "<< check_pkg.username << endl;
	cout << "check pkg content: "<< check_pkg.content << endl;
	int ret = send(sock, (char*)&check_pkg, sizeof(check_pkg), 0);
	if (ret < 0) {
		cout << "send check pkg failed, error: " << WSAGetLastError() << endl;
		return false; 
	}
	/*�����ȴ�*/
	data_package pkg_recv;
	ret = 0;
	// while (ret < sizeof(data_package)) {
	// 	ret += recv(sock, (char*)&pkg_recv + ret, sizeof(data_package), 0); //MWMO_WAITALL);
	// 	if (ret <= 0) {
	// 		cout << "recv failed, error: " << WSAGetLastError() << endl;
	// 		return false;
	// 	}
	// }
	ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0x8); //MWMO_WAITALL);
	if (ret <= 0) {
		cout << "recv failed, error: " << WSAGetLastError() << endl;
		return false;
	}
	cout << "recv ret: " << ret << endl;
	/*���ټ�ʱ*/
	int bytes_count = 0;
	clock_t start_time, end_time;
	start_time = clock();
	if (pkg_recv.op == PAN_SUCCESS) {
		while (1) {
			if (pkg_recv.op == PAN_FAILURE) {
				cout << "upload file failed(PAN_FAILURE)" << endl;
				return false;
			}
			cout << "pkg_recv content: "<< pkg_recv.content << endl;
			if (strcmp(pkg_recv.content, "-1") == 0) {
				cout << "upload file succeed(-1)" << endl;
				return true;
			}
			int file_no = string2int(pkg_recv.content);
			int len = file_no * BUF_LEN;
			bytes_count = len == 0 ? bytes_count : len;
			cout << "Finished Uploading bytes: " << bytes_count << endl;
			cout << "Upload speed: " << get_speed(start_time, bytes_count) << " bytes/s" << endl;
			/*op��PAN_UPLOAD_FILE_CONTENT*/
			/*username�����-md5*/
			/*content: �ļ�����*/
			data_package pkg_snt;
			pkg_snt.op = PAN_UPLOAD_FILE_CONTENT;
			strcpy(pkg_snt.username, pkg_recv.content); //��� 
			pkg_snt.username[strlen(pkg_recv.content)] = '-'; 
			strcpy(pkg_snt.username + strlen(pkg_recv.content) + 1, check_pkg.username); //md5
			/*���ļ�*/
			ifstream infile;
			infile.open(pkg->username, ifstream::binary);
			if (!infile.is_open()) {
				cout << "read file failed" << endl;
				return false;
			}
			infile.seekg(len);
			infile.read(pkg_snt.content, BUF_LEN);
			/*����check��*/
			int ret = send(sock, (char*)&pkg_snt, sizeof(pkg_snt), 0);
			if (ret < 0) {
				cout << "send data pkg failed, error: " << WSAGetLastError() << endl;
				return false;
			}	
			/*�����ȴ�*/
			pkg_recv.op = -1;
			memset(pkg_recv.username, 0, BUF_LEN);
			memset(pkg_recv.content, 0, BUF_LEN);
			ret = 0;
			// while (ret < sizeof(data_package)) {
			// 	ret += recv(sock, (char*)&pkg_recv + ret, sizeof(data_package), 0);//MWMO_WAITALL);
			// 	if (ret <= 0) {
			// 		cout << "recv failed, error: " << WSAGetLastError() << endl;
			// 		return false;
			// 	}
			// }
			ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0x8);//MWMO_WAITALL);
			if (ret <= 0) {
				cout << "recv failed, error: " << WSAGetLastError() << endl;
				return false;
			}
			cout << "recv ret: " << ret << endl;
		}
	}
	else {
		cout << "upload failed" << endl;
		cout << pkg_recv.content << endl;
		return false;
	}
	return true;
}

bool dfs_dir(data_package* pkg, SOCKET sock) {
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string path = pkg->username;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			//�����Ŀ¼,����mkdir��֮�����֮
			//������ļ��������upload_file 
			if ((fileinfo.attrib &  _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					string dir_path = p.assign(path).append("\\").append(fileinfo.name);
					cout << "local path: " << dir_path << endl;
					data_package temp_pkg;
					temp_pkg.op = pkg->op;
					memcpy(temp_pkg.username, pkg->username, BUF_LEN);
					memcpy(temp_pkg.content, pkg->content, BUF_LEN);
					
					strcpy(temp_pkg.username, dir_path.c_str());
					temp_pkg.content[strlen(temp_pkg.content) + 1] = temp_pkg.content[strlen(temp_pkg.content)];
					temp_pkg.content[strlen(temp_pkg.content)] = '/';
					pan_path(&temp_pkg, temp_pkg.content);
					
					/*����mkdir��*/
					data_package pkg_dir;
					pkg_dir.op = PAN_UPLOAD_DIR;
					strcpy(pkg_dir.content, temp_pkg.content);
					int ret = send(sock, (char*)&pkg_dir, sizeof(pkg_dir), 0);
					if (ret < 0) {
						cout << "send mkdir pkg failed, error: " << WSAGetLastError() << endl;
						return false;
					}
					/*�����ȴ�*/ 
					data_package pkg_recv;
					ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
					if (ret <= 0) {
						cout << "mkdir "<< temp_pkg.content << " failed, error: " << WSAGetLastError() << endl;
						return false;
					}
					if (pkg_recv.op == PAN_SUCCESS) {
						cout << "mkdir " << temp_pkg.content << " success" << endl;
						cout << pkg_recv.content << endl;
					}
					else {
						return false;
					}
					dfs_dir(&temp_pkg, sock);
				}
			}
			else {
				string file_path = p.assign(path).append("\\").append(fileinfo.name);
				cout << file_path << endl;
				data_package file_pkg;
				file_pkg.op = PAN_UPLOAD;
				strcpy(file_pkg.username, file_path.c_str());
				cout << "local file path: "<< file_pkg.username << endl;
				strcpy(file_pkg.content, pkg->content);
				
				file_pkg.content[strlen(file_pkg.content) + 1] = file_pkg.content[strlen(file_pkg.content)];
				file_pkg.content[strlen(file_pkg.content)] = '/';
				//pan_path(&file_pkg, file_pkg.content);
				cout << "server file path: "<< file_pkg.content << endl;
				upload_file(&file_pkg, sock);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}
/*upload�ļ���*/
/*����s��·��*/
/*op : PAN_UPLOAD_DIR*/
/*username: ������*/
/*content: s��·��*/
bool upload_dir(data_package* pkg, SOCKET sock) {
	data_package pkg_dir;
	pkg_dir.op = PAN_UPLOAD_DIR;
	pan_path(pkg, pkg_dir.content);
	
	/*����mkdir��*/
	int ret = send(sock, (char*)&pkg_dir, sizeof(pkg_dir), 0);
	if (ret < 0) {
		cout << "send mkdir pkg failed, error: " << WSAGetLastError() << endl;
		return false;
	}
	/*�����ȴ�*/ 
	data_package pkg_recv;
	ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0);
	if (ret <= 0) {
		cout << "mkdir failed, error: " << WSAGetLastError() << endl;
		return false;
	}
	if (pkg_recv.op == PAN_SUCCESS) {
		cout << "mkdir success" << endl;
		cout << pkg_recv.content << endl;
	}
	else {
		return false;
	}
	pan_path(pkg, pkg->content);
	dfs_dir(pkg, sock);
	return true;
}
/*�鿴�ļ�����*/ 
int file_type(char* path) {
	struct stat s;
	if (stat(path, &s) == 0) {
		if (s.st_mode & S_IFDIR) {
			return PAN_DIR;
		}
		else if (s.st_mode & S_IFREG) {
			return PAN_FILE;
		}
		else {
			return PAN_FAILURE;
		}
	}
	else {
		return PAN_FAILURE;
	}
}
bool upload(data_package* pkg, SOCKET sock) {
	switch (file_type(pkg->username)) {
		case PAN_DIR: {
			cout << "into upload dir" << endl;
			return upload_dir(pkg, sock);
		}
		case PAN_FILE: {
			cout << "into upload file" << endl;
			return upload_file(pkg, sock);
		}
		default: {
			return false;
		}
	}
	return true;
}
