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
    printf("=====================================文件系统（by-1553534-李帅）=====================================\n");
   
	while (cmd != "exit"){
		if (cmd == "help"){
			printf("exit                               |退出\n");
			printf("help                               |指令简介\n");
			printf("test                               |实例分析\n");
			printf("write     [文件名] [内容字符串]    |向文件写内容  |示例: write /A/B Hello World\n");
			printf("                                                  |示例: write B Hello World\n");
            printf("read      [文件名]                 |读文件的内容  |示例：read /A/B\n");
            printf("                                                  |示例: read B\n");
            printf("ls        [目录名]                 |显示目录的内容|示例：ls /A\n");
            printf("                                                  |示例: ls\n");
            printf("mk        [文件名]                 |创建文件      |示例：mk /A/B\n");
            printf("                                                  |示例: mk B\n");
            printf("mkdir     [目录名]                 |创建目录      |示例：mkdir /A\n");
            printf("                                                  |示例: mkdir A\n");
            printf("rm        [文件或目录名]           |删除文件或目录|示例：rm /A/B\n");
            printf("                                                  |示例: rm B\n");
            printf("fin       [外部文件名] [内部文件名]|从外部系统输入|示例：fin input.txt /A/B\n");
            printf("                                                  |示例: fin input.txt B\n");
            printf("fout      [内部文件名] [外部文件名]|输出到外部系统|示例：fout /A/B output.txt\n");
            printf("                                                  |示例: fout B output.txt\n");
            printf("cd        [目录名或绝对路径或'..'] |更改工作路径  |示例：cd /A\n");
            printf("                                                  |示例: cd A\n");
            printf("                                                  |示例: cd ..\n");
            printf("=====================================文件系统（by-1553534-李帅）=====================================\n");
		}
	
	
		else if (cmd == "test"){
			 char arg[2048];
	         cout << endl;
	         cout << "=======================实例分析开始=======================" << endl;
	         cout<<"输出目前目录的情况"<<endl;
	         strcpy(arg,"/");
	         ls(arg);
	         cout << "---------------------------" <<endl;
	         cout<<"在目录下新建/test/Jerry"<<endl;
           	 strcpy(arg,"/test");
	         fcreat(arg,IFDIR);
	         strcpy(arg,"/test/Jerry");
	         fcreat(arg,0);
	         cout<<"输出目前目录的情况"<<endl;
	         strcpy(arg,"/");
	         ls(arg);
	         cout << "---------------------------" <<endl;
	         cout << "外部读取test.txt写进/test/Jerry文件" <<endl;
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
                cout<<"成功读取"<<size<<"字节的文件进入/test/Jerry文件"<<endl;
                cout << "---------------------------" <<endl;
                char content[2048];
    
	            memset(content, 0, sizeof(content));

	            strcpy(arg,"/test/Jerry");
	            int fd = fopen(arg, IREAD);

	            if (fd >= 0) {
	              cout<<"test/Jerry文件现在的所有内容是:\n";
	              cout<<endl;
	              
	             fread(fd,content,2048);
	             cout<<content<<endl;
	             cout<<endl;
	             cout<<"将文件读写指针定位到500字节，读出20字节如下:"<<endl;
	             cout<<endl;
	             // 将文件读写指针定位到第500字节；
	             char content[2048];
    
	             memset(content, 0, sizeof(content));
	             
	             fseek(fd,500);
	             //读出20个字节
                 fread(fd, content, 20);
                 fclose(fd);
                 cout << content << endl;
	           }else{
                  cout << "ERROR：访问非法！" << endl;
         	    }
         	cout << "=======================实例分析结束=======================" << endl;
         	 
                
                
	         
	         
	        
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
                    cout << "已经是根目录了！" << endl;
            }else{
                nnext = nowdirpath + arg1c;
                strcpy(ckt,nnext.c_str());
                if (ck(ckt))
                    nowdirpath = nnext + "/";
            }
		}
		else{
            cout << "ERROR：指令不存在,请重新输入" << endl;
		}
		cout << "filesystem(1553534-李帅) "<< nowdirpath << ">";
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



