#include "filesystem.h"
#include<iostream>
using namespace std;

int main(){
	sys_init();
	init();
	string cmd = "help", arg1, arg2, nowdirpath = "/";
	string nnext;
	char src[2048], arg1c[2048], arg2c[2048];
	char ckt[2048];
    printf("=====================================�ļ�ϵͳ��by-1553534-��˧��=====================================\n");
   
	while (cmd != "exit"){
		if (cmd == "help"){
			printf("exit                               |�˳�\n");
			printf("help                               |ָ����\n");
			printf("test                               |ʵ������\n");
			printf("write     [�ļ���] [�����ַ���]    |���ļ�д����  |ʾ��: write /A/B Hello World\n");
			printf("                                                  |ʾ��: write B Hello World\n");
            printf("read      [�ļ���]                 |���ļ�������  |ʾ����read /A/B\n");
            printf("                                                  |ʾ��: read B\n");
            printf("ls        [Ŀ¼��]                 |��ʾĿ¼������|ʾ����ls /A\n");
            printf("                                                  |ʾ��: ls\n");
            printf("mk        [�ļ���]                 |�����ļ�      |ʾ����mk /A/B\n");
            printf("                                                  |ʾ��: mk B\n");
            printf("mkdir     [Ŀ¼��]                 |����Ŀ¼      |ʾ����mkdir /A\n");
            printf("                                                  |ʾ��: mkdir A\n");
            printf("rm        [�ļ���Ŀ¼��]           |ɾ���ļ���Ŀ¼|ʾ����rm /A/B\n");
            printf("                                                  |ʾ��: rm B\n");
            printf("fin       [�ⲿ�ļ���] [�ڲ��ļ���]|���ⲿϵͳ����|ʾ����fin input.txt /A/B\n");
            printf("                                                  |ʾ��: fin input.txt B\n");
            printf("fout      [�ڲ��ļ���] [�ⲿ�ļ���]|������ⲿϵͳ|ʾ����fout /A/B output.txt\n");
            printf("                                                  |ʾ��: fout B output.txt\n");
            printf("cd        [Ŀ¼�������·����'..'] |���Ĺ���·��  |ʾ����cd /A\n");
            printf("                                                  |ʾ��: cd A\n");
            printf("                                                  |ʾ��: cd ..\n");
            printf("=====================================�ļ�ϵͳ��by-1553534-��˧��=====================================\n");
		}
	
	
		else if (cmd == "test"){
			 char arg[2048];
	         cout << endl;
	         cout << "=======================ʵ��������ʼ=======================" << endl;
	         cout<<"���ĿǰĿ¼�����"<<endl;
	         strcpy(arg,"/");
	         ls(arg);
	         cout << "---------------------------" <<endl;
	         cout<<"��Ŀ¼���½�/test/Jerry"<<endl;
           	 strcpy(arg,"/test");
	         fcreat(arg,IFDIR);
	         strcpy(arg,"/test/Jerry");
	         fcreat(arg,0);
	         cout<<"���ĿǰĿ¼�����"<<endl;
	         strcpy(arg,"/");
	         ls(arg);
	         cout << "---------------------------" <<endl;
	         cout << "�ⲿ��ȡtest.txtд��/test/Jerry�ļ�" <<endl;
	         FILE *externalFile = fopen("test.txt", "rb");
             fseek(externalFile, 0, SEEK_END);
             int size = ftell(externalFile);
                fseek(externalFile, 0, 0);
                fread(extrabuffer, size, 1, externalFile);
                strcpy(arg,"/test/Jerry");
                int wd = fopen(arg, IWRITE);
                if (wd != -1) {
                    fwrite(wd, extrabuffer, size);
                    fclose(wd);
                }
                fclose(externalFile);
                cout<<"�ɹ���ȡ"<<size<<"�ֽڵ��ļ�����/test/Jerry�ļ�"<<endl;
                cout << "---------------------------" <<endl;
                char content[2048];
    
	            memset(content, 0, sizeof(content));

	            strcpy(arg,"/test/Jerry");
	            int fd = fopen(arg, IREAD);

	            if (fd >= 0) {
	              cout<<"test/Jerry�ļ����ڵ�����������:\n";
	              cout<<endl;
	              
	             fread(fd,content,2048);
	             cout<<content<<endl;
	             cout<<endl;
	             cout<<"���ļ���дָ�붨λ��500�ֽڣ�����20�ֽ�����:"<<endl;
	             cout<<endl;
	             // ���ļ���дָ�붨λ����500�ֽڣ�
	             char content[2048];
    
	             memset(content, 0, sizeof(content));
	             
	             fseek(fd,500);
	             //����20���ֽ�
                 fread(fd, content, 20);
                 fclose(fd);
                 cout << content << endl;
	           }else{
                  cout << "ERROR�����ʷǷ���" << endl;
         	    }
         	cout << "=======================ʵ����������=======================" << endl;
         	 
                
                
	         
	         
	        
		}
		else if (cmd == "write"){
            if (arg1c[0] == '/')
                testWrite(arg1c, arg2c);
            else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                testWrite(ckt, arg2c);
            }
		}
		else if (cmd == "read"){
            if (arg1c[0] == '/')
                testRead(arg1c);
            else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                testRead(ckt);
            }
		}
		else if (cmd == "ls"){
            if (arg1c[0] == '/')
                ls(arg1c);
            else{
                if(nowdirpath != "/" )
                    nnext = nowdirpath.substr(0,nowdirpath.length() - 1);
                else
                    nnext = nowdirpath;
                strcpy(ckt,nnext.c_str());
                ls(ckt);
            }
		}
		else if (cmd == "mk"){
            if (arg1c[0] == '/')
                fcreat(arg1c, 0);
            else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                fcreat(ckt, 0);
            }
		}
		else if (cmd == "mkdir"){
			if (arg1c[0] == '/')
                fcreat(arg1c, IFDIR);
            else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                fcreat(ckt, IFDIR);
            }
		}
		else if (cmd == "rm"){
            if (arg1c[0] == '/')
                fdelete(arg1c);
            else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                fdelete(ckt);
            }
		}
		else if (cmd == "fin"){
            if (arg2c[0] == '/') {
                FILE *externalFile = fopen(arg1c, "rb");
                fseek(externalFile, 0, SEEK_END);
                int size = ftell(externalFile);
                fseek(externalFile, 0, 0);
                fread(extrabuffer, size, 1, externalFile);
                int wd = fopen(arg2c, IWRITE);
                if (wd != -1) {
                    fwrite(wd, extrabuffer, size);
                    fclose(wd);
                }
                fclose(externalFile);
            }else{
                nnext = nowdirpath + arg2c;
                strcpy(ckt,nnext.c_str());
                FILE *externalFile = fopen(arg1c, "rb");
                fseek(externalFile, 0, SEEK_END);
                int size = ftell(externalFile);
                fseek(externalFile, 0, 0);
                fread(extrabuffer, size, 1, externalFile);
                int wd = fopen(ckt, IWRITE);
                if (wd != -1) {
                    fwrite(wd, extrabuffer, size);
                    fclose(wd);
                }
                fclose(externalFile);
            }
		}
		else if (cmd == "fout"){
            if (arg1c[0] == '/') {
                int rd = fopen(arg1c, IREAD);
                if (rd != -1) {
                    int flen = kernel.openFiles[rd].i_size;
                    fread(rd, extrabuffer, flen);
                    FILE *extrafile = fopen(arg2c, "wb");
                    fwrite(extrabuffer, flen, 1, extrafile);
                    fclose(extrafile);
                }
                fclose(rd);
            }else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                int rd = fopen(ckt, IREAD);
                if (rd != -1) {
                    int flen = kernel.openFiles[rd].i_size;
                    fread(rd, extrabuffer, flen);
                    FILE *extrafile = fopen(arg2c, "wb");
                    fwrite(extrabuffer, flen, 1, extrafile);
                    fclose(extrafile);
                }
                fclose(rd);
            }
		}
		else if (cmd == "cd") {
            if (arg1c[0] == '/') {
                if (strlen(arg1c) > 1) {
                    if (ck(arg1c))
                        nowdirpath = string(arg1c) + "/";
                }
                else
                    nowdirpath = "/";
            }else if(arg1c[0] == '.' && arg1c[1] == '.') {
                if (nowdirpath != "/") {
                    int k;
                    for (k = nowdirpath.length() - 2;k > 0;k--) {
                        if(nowdirpath[k] == '/')
                            break;
                    }
                    nowdirpath = nowdirpath.substr(0,k + 1);
                }else
                    cout << "�Ѿ��Ǹ�Ŀ¼�ˣ�" << endl;
            }else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                if (ck(ckt))
                    nowdirpath = nnext + "/";
            }
		}
		else{
            cout << "ERROR��ָ�����,����������" << endl;
		}
		cout << "filesystem(1553534-��˧) "<< nowdirpath << ">";
		cin.getline(src, 2048);
		char *p = src;
		cmd = "";
		arg1 = "";
		arg2 = "";
        while ((*p != ' ') && (*p != 0)){
			cmd += *p;
			++p;
		}
		if (*p!= 0) ++p;
		while ((*p != ' ') && (*p != 0)){
			arg1 += *p;
			++p;
		}
		if (*p!= 0) ++p;
		while ((*p != ' ') && (*p != 0)){
			arg2 += *p;
			++p;
		}
		strcpy(arg1c, arg1.c_str());
		strcpy(arg2c, arg2.c_str());
	}
	releaseBuffers();
}



