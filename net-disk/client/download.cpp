// download.cpp
// 2022.6.18
// author: boiled_fish
#pragma comment(lib, "ws2_32.lib")
#include "../../include/client.h"
using namespace std;

bool write_file(string filename, int wpos, const char* content, int size) {
    if (1) {
        fstream file_tmp;
        file_tmp.open(filename.c_str(), ios::in);
        if (!file_tmp.is_open()) {
			file_tmp.open(filename.c_str(), ios::out);
		}
        file_tmp.close();
    }
	fstream file;
	file.open(filename.c_str(), ios::in | ios::out | ios::binary);
	if (!file.is_open()) {
		cout << "open failed" << endl;
		return false;
	}
	file.seekg(0, file.end);
	int len = file.tellg();
	cout << "file len: " << len << endl;

	file.seekg(wpos, file.beg);
	char buf[50001] = {0};
	file.read(buf, len - wpos);
    
	file.seekp(wpos, file.beg);
	file.write(content, size);
	file.write(buf, len - wpos);
	return true;
}
void replace_all(string& dst, string& old_val, string& new_val) {
	while( true ) {
		string::size_type pos(0);
		if( (pos=dst.find(old_val))!=string::npos) {
			dst.replace(pos,old_val.length(), new_val);
		}
		else {
			break;
		}
	}
}
double get_speed(clock_t start, int size) {
	clock_t end = clock();
	return (double)size / ((double)(end - start) / CLOCKS_PER_SEC);
}
bool parse_recv(data_package* pkg, data_package* pkg_recv, SOCKET sock) {
	string str_paths = pkg_recv->content;
	stringstream ss(str_paths);
    char c = '\n';
    vector<string> paths;
    string path;
    while (getline(ss, path, c)) {
        paths.push_back(path);
    }
    int i;
    for (i = 0; i < paths.size(); ++i) {
    	cout << paths[i] << endl;
	}
	string win_slash = "\\";
	string linux_slash = "/";
	/*���ټ�ʱ*/
	int byte_count = 0;
	clock_t start_time,end_time;
	start_time = clock();
    for (i = 0; i < paths.size(); ++i) {
    	char type = *paths[i].rbegin();
    	/*����pkg���*/
    	string download_path = pkg->username;
    	string pan_path = paths[i].substr(0, paths[i].size() - 2); 
    	cout << "download path: " << download_path << endl;
    	cout << "pan path: " << pan_path << endl;
    	/*�ļ�*/
    	if (type == '1') {
    		cout << "into file process" << endl;
    		/*��ȡmd5��size*/
    		data_package pkg_request;
    		pkg_request.op = PAN_DOWNLOAD_FILE;
    		strcpy(pkg_request.content, pan_path.c_str());
    		cout << "pkg_request content: " << pkg_request.content << endl;
    		int ret = send(sock, (char*)&pkg_request, sizeof(pkg_request), 0);
			if (ret < 0) {
				cout << "send download file pkg failed, error: " << WSAGetLastError() << endl;
				return false;
			}
			cout << "send over" << endl;
			data_package pkg_file_recv;
			ret = 0;
			// while (ret < sizeof(data_package)) {
			// 	ret = recv(sock, (char*)&pkg_file_recv + ret, sizeof(data_package), 0);// MWMO_WAITALL);
			// 	if (ret <= 0) {
			// 		cout << "recv download file ret failed, error: " << WSAGetLastError() << endl;
			// 		return false;
			// 	}
			// }
			ret = recv(sock, (char*)&pkg_file_recv, sizeof(data_package), 0x8);// MWMO_WAITALL);
			if (ret <= 0) {
				cout << "recv download file ret failed, error: " << WSAGetLastError() << endl;
				return false;
			}
			cout << "recv ret: " << ret << endl;
			if (pkg_file_recv.op != PAN_SUCCESS) {
				cout << "download_file ret = PAN_FAILURE" << endl;
				continue;
			}
			byte_count += sizeof(pkg_file_recv);
			cout << "Finished downloading bytes: " << byte_count << endl;
			cout << "Download speed: " << get_speed(start_time, byte_count) << " bytes/s" << endl;
			
			string md5 = pkg_file_recv.username;
			int full_size = atoi(pkg_file_recv.content);
			/*�ж�����*/
			string dir_path = download_path;
			while (*dir_path.rbegin() == '\\') {
				dir_path.pop_back();
			}
			string file_path = download_path;
    		file_path += pan_path;
    		replace_all(file_path, linux_slash, win_slash);
    		cout << "dir_path: " << dir_path << endl;
    		cout << "file_path: " << file_path << endl;
    		if (check_name(md5, full_size, file_path)) {
    			continue;
			}
			/*�����ʱ�ļ��Ƿ����*/
			int file_no = 0;
			string tmp_name = file_path;
    		tmp_name += ".tmp";
    		int tmp_size = 0;
    		if (!access(tmp_name.c_str(), R_OK | W_OK)) {
    			/*��ȡ��ʱ�ļ���С��ȷ���������*/
    			char file_size[BUF_LEN] = { 0 };
				FILE* file = fopen(tmp_name.c_str(), "r");
				if (file) {
					tmp_size = filelength(fileno(file));
					fclose(file);
				}
				else {
					cout << "access tmp file failed" << endl;
					continue;
				}
				file_no = tmp_size / BUF_LEN;
			}
			
			/*��ѭ������*/
			/*���ַ�����1��ÿ�ν��������tmp_size�Ĵ�С2������file_no�����ж�*/
			int complete_no = (full_size % BUF_LEN) ? (full_size / BUF_LEN) : (full_size / BUF_LEN + 1);
			int endure_fail_count = 0;
			cout << "file_no: " << file_no << endl;
			cout << "complete no: " << complete_no << endl;
			while (file_no <= complete_no) {
				data_package pkg_blk;
				pkg_blk.op = PAN_DOWNLOAD_FILE_BLK;
				itoa(file_no, pkg_blk.username, 10);
				string snt_file_path = paths[i].substr(0, paths[i].size() - 2);
				strcpy(pkg_blk.content, snt_file_path.c_str());
				/*����blk��*/
				int ret = send(sock, (char*)&pkg_blk, sizeof(pkg_blk), 0);
				if (ret < 0) {
					cout << "send download file blk pkg failed, error: " << WSAGetLastError() << endl;
					return false;
				}
				/*��������*/ 
				data_package pkg_file_blk;
				ret = 0;
				// while (ret < sizeof(data_package)) {
				// 	ret += recv(sock, (char*)&pkg_file_blk + ret, sizeof(data_package), 0);//MWMO_WAITALL);
				// 	if (ret <= 0) {
				// 		cout << "recv download file blk ret failed, error: " << WSAGetLastError() << endl;
				// 		return false;
				// 	}
				// }
				ret = recv(sock, (char*)&pkg_file_blk, sizeof(data_package), 0x8);//MWMO_WAITALL);
				if (ret <= 0) {
					cout << "recv download file blk ret failed, error: " << WSAGetLastError() << endl;
					return false;
				}
				cout << "recv ret: " << ret << endl;
				cout << "tmp_name: " << tmp_name << endl; 
				if (pkg_file_blk.op == PAN_SUCCESS) {
					int write_size = ((full_size - BUF_LEN * file_no) >= BUF_LEN) ? BUF_LEN : (full_size - BUF_LEN * file_no);
					if (!write_file(tmp_name, file_no * BUF_LEN, pkg_file_blk.content, write_size)) {
						cout << "block " << file_no << " write failed, request again." << endl;
						if (endure_fail_count++ > MAX_FAILURE) {
							cout << "download " << file_path << " failed, exit downloading" << endl;
							break;
						}
						continue;
					}
				}
				else {
					cout << "block " << file_no << " recv failed, request again." << endl;
					if (endure_fail_count++ > MAX_FAILURE) {
						cout << "download " << file_path << " failed, exit downloading" << endl;
						break;
					}
					continue;
				}
				file_no++;
				byte_count += sizeof(pkg_file_blk);
				cout << "Finished downloading bytes: " << byte_count << endl;
				cout << "Download speed: " << get_speed(start_time, byte_count)  << " bytes/s"<< endl;
			}
			char complete_file_size[BUF_LEN] = { 0 };
			FILE* complete_file = fopen(tmp_name.c_str(), "r");
			int local_complete_size = 0;
			if (complete_file) {
				local_complete_size = filelength(fileno(complete_file));
				fclose(complete_file);
			}
			if (local_complete_size == full_size) {
				char cmd[1000] = { 0 };
				sprintf(cmd, "move %s %s", tmp_name.c_str(), file_path.c_str());
				system(cmd);
				cout << "cmd: " << cmd << endl;
			}
		}
		/*�ļ���*/ 
		else if (type == '0') {
			cout << "into dir process" << endl;
			string temp_pan_path = pan_path;
			replace_all(temp_pan_path, linux_slash, win_slash);
			download_path += temp_pan_path;
			cout << "download_path: " << download_path << endl;
			char cmd[1000] = { 0 };
			sprintf(cmd, "mkdir %s", download_path.c_str());
			system(cmd); 
		}
		else {
			cout << "recv type illegal" << endl;
		}
	}
	return true;
}

bool download(data_package* pkg, SOCKET sock) {
	/*local path��Ϊ���ļ���·��*/
	/*�������ļ���*/
	mkdir(pkg->username);
	/*����download�����*/
	data_package pkg_download;
	pkg_download.op = PAN_DOWNLOAD;
	strcpy(pkg_download.content, pkg->content);
	cout << "pkg_download content: " << pkg_download.content << endl;
	int ret = send(sock, (char*)&pkg_download, sizeof(pkg_download), 0);
	if (ret < 0) {
		cout << "send download pkg failed, error: " << WSAGetLastError() << endl;
		return false;
	}
	/*�����ȴ�*/
	data_package pkg_recv;
	ret = recv(sock, (char*)&pkg_recv, sizeof(data_package), 0x8);
	if (ret <= 0) {
		cout << "recv download ret failed, error: " << WSAGetLastError() << endl;
		return false;
	}
	/*�����ļ��ṹ*/
	if (pkg_recv.op == PAN_SUCCESS) {
		return parse_recv(pkg, &pkg_recv, sock);
	}
	else {
		cout << "recv failure op" << endl;
		return false; 
	}
}
