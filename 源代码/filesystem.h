/*参考unix 源码 \unix64\oos\src\include */ 

#include<iostream>
#include <bits/stdc++.h>
using namespace std;
/*
 * 缓存控制块buf定义
 * 记录了相应缓存的使用情况等信息；
 * 同时兼任I/O请求块，记录该缓存
 * 相关的I/O请求和执行结果。
 */
class Buf
{
public:
	enum BufFlag	/* b_flags中标志位 */
	{
		B_WRITE = 0x1,		/* 写操作。将缓存中的信息写到硬盘上去 */
		B_READ	= 0x2,		/* 读操作。从盘读取信息到缓存中 */
		B_DONE	= 0x4,		/* I/O操作结束 */
		B_ERROR	= 0x8,		/* I/O因出错而终止 */
		B_BUSY	= 0x10,		/* 相应缓存正在使用中 */
		B_WANTED = 0x20,	/* 有进程正在等待使用该buf管理的资源，清B_BUSY标志时，要唤醒这种进程 */
		B_ASYNC	= 0x40,		/* 异步I/O，不需要等待其结束 */
		B_DELWRI = 0x80		/* 延迟写，在相应缓存要移做他用时，再将其内容写到相应块设备上 */
	};
	
public:
	unsigned int b_flags;	/* 缓存控制块标志位 */
	
	int		padding;		/* 4字节填充，使得b_forw和b_back在Buf类中与Devtab类
							 * 中的字段顺序能够一致，否则强制转换会出错。 */
	/* 缓存控制块队列勾连指针 */
	Buf*	b_forw;
	Buf*	b_back;
	Buf*	av_forw;
	Buf*	av_back;
	
	short	b_dev;			/* 主、次设备号，其中高8位是主设备号，低8位是次设备号 */
	int		b_wcount;		/* 需传送的字节数 */
	unsigned char* b_addr;	/* 指向该缓存控制块所管理的缓冲区的首地址 */
	int		b_blkno;		/* 磁盘逻辑块号 */
	int		b_error;		/* I/O出错时信息 */
	int		b_resid;		/* I/O出错时尚未传送的剩余字节数 */
};





static const unsigned int IALLOC = 0x8000;/* 文件被使用 */
static const unsigned int IFMT = 0x6000;
static const unsigned int IFDIR = 0x4000;
static const unsigned int IFCHR = 0x2000;
static const unsigned int IFBLK = 0x6000;
static const unsigned int ILARG = 0x1000;
static const unsigned int ISUID = 0x800;
static const unsigned int ISGID = 0x400;
static const unsigned int ISVTX = 0x200;
static const unsigned int IREAD = 0x100;
static const unsigned int IWRITE = 0x80;
static const unsigned int IEXEC = 0x40;
static const unsigned int ITEXT = 0x20;
static const unsigned int IWANT = 0x10;
static const unsigned int IMOUNT = 0x8;
static const unsigned int IACC = 0x4;
static const unsigned int IUPD = 0x2;
static const unsigned int ILOCK = 0x1;

static const char *DISKFILENAME = "myDisk.img";
static const int DISKINODECAPACITY = 2048;
static const int BLOCKSIZE = 512;
static const int DATABLOCKCOUNT = 32768;
const int FILECOUNT = 20;
const int BUFCOUNT = 23;
/*
 * 外存索引节点(DiskINode)的定义
 * 外存Inode位于文件存储设备上的
 * 外存Inode区中。每个文件有唯一对应
 * 的外存Inode，其作用是记录了该文件
 * 对应的控制信息。
 * 外存Inode中许多字段和内存Inode中字段
 * 相对应。外存INode对象长度为64字节，
 * 每个磁盘块可以存放512/64 = 8个外存Inode
 */
 

struct inode_d_type{
	unsigned int d_mode;/* 状态的标志位，定义见enum INodeFlag */
	int d_nlink;/* 文件联结计数，即该文件在目录树中不同路径名的数量 */
	short d_uid;/* 文件所有者的用户标识数 */
	short d_gid;/* 文件所有者的组标识数 */
	int d_size;/* 文件大小，字节为单位 */
	int d_addr[10];/* 用于文件逻辑块好和物理块好转换的基本索引表 */
	int d_atime;/* 最后访问时间 */
	int d_mtime;/* 最后修改时间 */
};

struct fbl_type{
	int n;
	int blkno[110];
};

struct fil_type{
	int n;
	int ino[110];
};

//superblock的结构体 
struct sb_type{
	int s_isize;/* 外存Inode区占用的盘块数 */
	int s_fsize;/* 盘块总数 */

	fbl_type freebl;/* free block list*/
	fil_type freeil;/*free inode list*/

	int s_flock;/* 封锁空闲盘块索引表标志 */
	int s_ilock;/* 封锁空闲Inode表标志 */

	int s_fmod;/* 内存中super block副本被修改标志，意味着需要更新外存对应的Super Block */
	int s_ronly;/* 本文件系统只能读出 */
	int s_time;/* 最近一次更新时间 */
	int padding[27];/* 填充使SuperBlock块大小等于1024字节，占据2个扇区 */
};

static const int DIRENTRYSIZE = 32;

struct de_type{
	int m_ino;
	char m_name[DIRENTRYSIZE - sizeof(int)];
};

/*
 * 内存索引节点(INode)的定义
 * 系统中每一个打开的文件、当前访问目录、
 * 挂载的子文件系统都对应唯一的内存inode。
 * 每个内存inode通过外存inode所在存储设备的设备号(i_dev)
 * 以及该设备外存inode区中的编号(i_number)来确定
 * 其对应的外存inode。
 */
struct inode_type{
	unsigned int i_flag;/* 状态的标志位，定义见enum INodeFlag */
	unsigned int i_mode;/* 文件工作方式信息 */
	int i_size;/* 文件大小，字节为单位 */
	int i_addr[10];/* 用于文件逻辑块好和物理块好转换的基本索引表 */
	int i_number;/* 外存inode区中的编号 */
	int i_count;/* 引用计数 */
	int i_offset;
	int i_nlink;/* 文件联结计数，即该文件在目录树中不同路径名的数量 */
	short i_dev;/* 外存inode所在存储设备的设备号 */
	short i_uid;/* 文件所有者的用户标识数 */
	short i_gid;/* 文件所有者的组标识数 */
	int i_lastr;/* 存放最近一次读取文件的逻辑块号，用于判断是否需要预读 */
};


/*
 * 缓存控制块buf定义
 * 记录了相应缓存的使用情况等信息；
 * 同时兼任I/O请求块，记录该缓存
 * 相关的I/O请求和执行结果。
 */
 
struct buf_type{
	unsigned int b_flags;/* 缓存控制块标志位 */
	int b_blkno;/* 磁盘逻辑块号 */
	/* 缓存控制块队列勾连指针 */
	buf_type *b_forw;
	buf_type *b_back;
	buf_type *av_forw;
	buf_type *av_back;
	char data[BLOCKSIZE];
	short b_dev;/* 主、次设备号，其中高8位是主设备号，低8位是次设备号 */
	int b_wcount;/* 需传送的字节数 */
	unsigned char* b_addr;/* 指向该缓存控制块所管理的缓冲区的首地址 */
	int b_error;/* I/O出错时信息 */
	int b_resid;/* I/O出错时尚未传送的剩余字节数 */
};
enum BufFlag	/* b_flags中标志位 */
	{
		B_WRITE = 0x1,		/* 写操作。将缓存中的信息写到硬盘上去 */
		B_READ	= 0x2,		/* 读操作。从盘读取信息到缓存中 */
		B_DONE	= 0x4,		/* I/O操作结束 */
		B_ERROR	= 0x8,		/* I/O因出错而终止 */
		B_BUSY	= 0x10,		/* 相应缓存正在使用中 */
		B_WANTED = 0x20,	/* 有进程正在等待使用该buf管理的资源，清B_BUSY标志时，要唤醒这种进程 */
		B_ASYNC	= 0x40,		/* 异步I/O，不需要等待其结束 */
		B_DELWRI = 0x80		/* 延迟写，在相应缓存要移做他用时，再将其内容写到相应块设备上 */
	};

/*
 * Kernel类用于封装所有内核相关的全局类实例对象，
 * 例如PageManager, ProcessManager等。
 * 
 * Kernel类在内存中为单体模式，保证内核中封装各内核
 * 模块的对象都只有一个副本。
 */
struct kernel_type{
	FILE *diskfile;/* disk文件*/ 
	sb_type superBlock;/*superblock*/ 
	buf_type buffers[BUFCOUNT];/*缓存控制*/
	 
	buf_type *freeBufHead;/*空闲缓存链表的头指针*/ 
	buf_type *freeBufTail;/*空闲缓存链表的尾指针*/ 
	int freeBufCount;/*空闲缓存控制块的个数*/ 
	inode_type openFiles[FILECOUNT];/*打开的inode文件*/ 
	int openFileCount;/*计数信号*/ 
};
extern kernel_type kernel;

inode_type getDirInode(char *name, char *end);
void ls(char *name); //列出 name 路径下的所有文件
int fopen(char *name, int mode);//以mode模式打开 name 文件
void fcreat(char *name, int mode); //以mode模式创建 name 文件
void fdelete(char *name);//删除 name 文件
void releaseDataBlocks(inode_type inode);
void pushFreeBlockList(int blkno);

void loadSuperBlock();
int alloc();
int ialloc();

void IORecv(void *p, int offset, int size);//发送 IO 读取请求，模拟设备读取
void IOSend(void *p, int offset, int size);//发送 IO 写入请求，模拟设备写入
inode_type getRootInode();
inode_type getInode(int ino);
void setInode(inode_type inode);


void readDisk(void *p, int offset, int size);//读取硬盘的数据，offset 为整个硬盘的绝对位移
void writeDisk(void *p, int offset, int size);//写入硬盘的数据，offset 为整个硬盘的绝对位移
int readBlock(void *p, int blkno, int offset, int size);//根据 blkno 读取某一块的内容，不一定是数据块
int writeBlock(void *p, int blkno, int offset, int size);//根据 blkno 写入某一块的内容，不一定是数据块
buf_type *getBuffer(int blkno, int mode);//获取缓存块
void releaseBuffers();//释放所有缓存块



void readFile(int ino, void *p, int offset, int size);//从 Inode 编号为 ino 的文件中读取数据
void writeFile(inode_type &is, void *p, int offset, int size);//向 is 的文件中写入数据
int openFile(int ino, int mode);//打开文件，返回描述符
int getBlockNum(inode_type *node);//计算某个 offset 对应的数据块编号
int fseek(int fd, int offset);//移动文件指针
int fread(int fd, void *buffer, int len);//从文件中读取
int fwrite(int fd, void *buffer, int len);//向文件中写入
void fclose(int fd);//关闭文件
bool ck(char *name);//查看路径是否存在



inode_type getRootInode(){
	inode_d_type dinode;
	readDisk(&dinode, sizeof(sb_type), sizeof(inode_d_type));
	inode_type inode;
	inode.i_mode = dinode.d_mode;
	inode.i_size = dinode.d_size;
	memcpy(inode.i_addr, dinode.d_addr, sizeof(inode.i_addr));
	inode.i_number = 0;
	inode.i_count = 1;
	return inode;
}

inode_type getInode(int ino){
	inode_d_type dinode;
	readDisk(&dinode, sizeof(sb_type) + (ino * sizeof(inode_d_type)), sizeof(inode_d_type));
	inode_type inode;
	inode.i_mode = dinode.d_mode;
	inode.i_size = dinode.d_size;
	memcpy(inode.i_addr, dinode.d_addr, sizeof(inode.i_addr));
	inode.i_number = ino;
	inode.i_count = 1;
	return inode;
}

void setInode(inode_type inode){
	inode_d_type dinode;
	dinode.d_mode = inode.i_mode;
	dinode.d_size = inode.i_size;
	memcpy(dinode.d_addr, inode.i_addr, sizeof(inode.i_addr));
	writeDisk(&dinode, sizeof(sb_type) + (inode.i_number * sizeof(inode_d_type)), sizeof(inode_d_type));
}

buf_type *getBuffer(int blkno, int mode){
	buf_type *p = kernel.freeBufHead;
	for (int i = 0; i < kernel.freeBufCount; ++i){
		if (p->b_blkno == blkno)
			return p;
		p = p->b_forw;
	}
	if (kernel.freeBufCount == BUFCOUNT){ //如果找不到
	    p = kernel.freeBufHead;//不要最旧的一个
		kernel.freeBufHead = kernel.freeBufHead->b_forw;
		//if (p->b_flags & B_ASYNC)
		IOSend(p->data, p->b_blkno * BLOCKSIZE, BLOCKSIZE);
	}else {//找可用的
		++kernel.freeBufCount;
		for (int i = 0; i < BUFCOUNT; ++i)
			if (!(kernel.buffers[i].b_flags & B_BUSY)){
				p = &kernel.buffers[i];
				break;
			}
	}
    if (kernel.freeBufCount > 1){//连接
		kernel.freeBufTail->b_forw = p;
		p->b_back = kernel.freeBufTail;
		kernel.freeBufTail = p;
	}else{
		kernel.freeBufHead = p;
		kernel.freeBufTail = p;
	}
	//读数据
	p->b_flags = mode;
	p->b_flags |= B_BUSY;
	p->b_blkno = blkno;
	IORecv(p->data, blkno * BLOCKSIZE, BLOCKSIZE);
	return p;
}

void releaseBuffers(){//释放所有缓存
	buf_type *p = kernel.freeBufHead;
	for (int i = 0; i < kernel.freeBufCount; ++i){
		IOSend(p->data, p->b_blkno * BLOCKSIZE, BLOCKSIZE);
		p = p->b_forw;
	}
}

int readBlock(void *p, int blkno, int offset, int size){//读块的数据
	buf_type *buf = getBuffer(blkno, B_READ);
	if (offset + size > BLOCKSIZE)
		size = BLOCKSIZE - offset;
	memcpy(p, buf->data + offset, size);
	return size;
}

int writeBlock(void *p, int blkno, int offset, int size){//块写数据
	buf_type *buf = getBuffer(blkno, B_READ);
	if (offset + size > BLOCKSIZE)
		size = BLOCKSIZE - offset;
    //cout << "wb "  << blkno <<  " " << offset << ' ' << size <<endl;
	memcpy(buf->data + offset, p, size);
	return size;
}

void readDisk(void *p, int offset, int size){
	int rsize;
	while (size > 0){
		rsize = readBlock(p, offset / BLOCKSIZE, offset % BLOCKSIZE, size);
		size -= rsize;
		offset += rsize;
		p = (char *)p + rsize;
	}
}

void writeDisk(void *p, int offset, int size){
	int wsize;
	while (size > 0){
		wsize = writeBlock(p, offset / BLOCKSIZE, offset % BLOCKSIZE, size);
		size -= wsize;
		offset += wsize;
		p = (char *)p + wsize;
	}
}

int openFile(int ino, int mode){
	if (kernel.openFileCount == FILECOUNT)
		cout << "ERROR:文件打开数过多！" << endl;
	int fd = 0;
	for (fd = 0; fd < FILECOUNT; ++fd)
		if (!(kernel.openFiles[fd].i_flag & IALLOC))
			break;
	kernel.openFiles[fd] = getInode(ino);
	kernel.openFiles[fd].i_flag |= IALLOC;
	kernel.openFiles[fd].i_offset = 0;
	return fd;
}

int fseek(int fd, int offset){
	kernel.openFiles[fd].i_offset = offset;
	return offset;
}

int getBlockNum(inode_type* node){//计算块数
	const static int DBS = BLOCKSIZE / sizeof(int);
	int abs = node->i_offset / BLOCKSIZE;
	if (abs < 6){//1级
		return node->i_addr[abs];
	}
	else if (abs < 6 + 2 * DBS){//2级
		int ab = 6 + (abs - 6) / DBS;
		int bb = (abs - 6) % DBS;
		int blkno;
		readBlock(&blkno, node->i_addr[ab], bb * sizeof(int), sizeof(int));
		return blkno;
	}else{//3级
		int t = (abs - 6 - 2 * DBS) / DBS;
		int ab = t / DBS + 8;
		int bb = t % DBS;
		int cb = (abs - 6 - 2 * DBS) % DBS;
		int blkno_, blkno;
		readBlock(&blkno_, node->i_addr[ab], bb * sizeof(int), sizeof(int));
		readBlock(&blkno, blkno_, cb * sizeof(int), sizeof(int));
		return blkno;
	}
	cout << "ERROR : getBlockNum() RE!" << endl;
}

int fread(int fd, void *buffer, int len){
	inode_type *node = &kernel.openFiles[fd];
	if (node->i_size - node->i_offset < len)
		len = node->i_size - node->i_offset;
    
	while (len > 0){
		int blkno = getBlockNum(node);
		int rsize = readBlock(buffer, blkno, node->i_offset % BLOCKSIZE, len);
		buffer = (char *)buffer + rsize;
		node->i_offset += rsize;
		len -= rsize;
	}
	return len;
}

int fwrite(int fd, void *buffer, int len){
	const static int DBS = BLOCKSIZE / sizeof(int);
	
	if (kernel.openFiles[fd].i_size < (kernel.openFiles[fd].i_offset + len)){//不够则分配
	    
		int finalsize = kernel.openFiles[fd].i_offset + len;
		int finalabsblkno = (finalsize - 1) / BLOCKSIZE;
		int currentabsblkno = (kernel.openFiles[fd].i_size - 1) / BLOCKSIZE;
		if (kernel.openFiles[fd].i_size == 0)
			currentabsblkno = -1;
		kernel.openFiles[fd].i_size = finalsize;
		while (currentabsblkno < finalabsblkno){
			if (currentabsblkno < 5){//1级就够
				++currentabsblkno;
			
				kernel.openFiles[fd].i_addr[currentabsblkno] = alloc();
			}
			else if (currentabsblkno < 5 + DBS * 2){ //2级
				int cab = ++currentabsblkno;
				if (cab == 7)
					kernel.openFiles[fd].i_addr[6] = alloc();
				if (cab == 7 + DBS)
					kernel.openFiles[fd].i_addr[7] = alloc();
				int ab = (cab - 6) / DBS;
				int bb = (cab - 6) % DBS;
				int newb = alloc();
				
				writeBlock(&newb, kernel.openFiles[fd].i_addr[ab], bb * sizeof(int), sizeof(int));
			}
			else{//3级
				int cab = ++currentabsblkno;
				if (cab == 6 + DBS * 2)
					kernel.openFiles[fd].i_addr[8] = alloc();
				if (cab == 6 + DBS * 2 + DBS * DBS)
					kernel.openFiles[fd].i_addr[9] = alloc();
				int ab = (cab - 6 - 2 * DBS) / DBS / DBS + 8;
				int bb = (cab - 6 - 2 * DBS) / DBS % DBS;
				int cb = (cab - 6 - 2 * DBS) % DBS;
				int newb;
				if (cb == 0){
					newb = alloc();
					writeBlock(&newb, kernel.openFiles[fd].i_addr[ab], bb * sizeof(int), sizeof(int));
				}
				else
					readBlock(&newb, kernel.openFiles[fd].i_addr[ab], bb * sizeof(int), sizeof(int));
				int newc = alloc();
				
				writeBlock(&newc, newb, cb * sizeof(int), sizeof(int));
			}
		}
	}
	inode_type *node = &kernel.openFiles[fd];
	while (len > 0){ //写数据
		int blkno = getBlockNum(node);
		int wsize = writeBlock(buffer, blkno, node->i_offset % BLOCKSIZE, len);
		buffer = (char *)buffer + wsize;
		node->i_offset += wsize;
		len -= wsize;
	}
	return len;
}

void fclose(int fd){//关闭并更新inode
	kernel.openFiles[fd].i_flag &= !IALLOC;
	inode_type *node = &kernel.openFiles[fd];
	int blkno = getBlockNum(node);
	setInode(kernel.openFiles[fd]);
}

void readFile(int ino, void *p, int offset, int size){//读文件
	int fd = openFile(ino, IREAD);
	//cout << "rdf-fd = " << fd <<endl;
	inode_type *node = &kernel.openFiles[fd];
	//cout << "rdf-ino = " << node->i_number << endl;
	int blkno = getBlockNum(node);
	//cout << "rdf-blkno = " << blkno <<endl;
	fseek(fd, offset);
	fread(fd, p, size);
	fclose(fd);
}

void writeFile(inode_type &is, void *p, int offset, int size){//写文件
	int fd = openFile(is.i_number, IWRITE);
	fseek(fd, offset);
	fwrite(fd, p, size);
    memcpy(&is.i_addr,&kernel.openFiles[0].i_addr,sizeof(int) * 10);
	fclose(fd);
}

inode_type getDirInode(char *name, char *end){//查找目录的inode
 
	if (*name != '/'){//格式检查
		cout <<"ERROR: 格式非法！" << endl;
		return inode_type();
	}
	++name;
	char *tail = name;
	inode_type inode = getRootInode();
	while ((*tail != '/') && (*tail != 0 ) && (tail != end))//计算结尾位置
		++tail;
	char tname[32];
	memset(tname, 0, sizeof(tname));
	de_type direntry;
	direntry.m_ino = inode.i_number;
	while (tail != end){
        
		strncpy(tname, name, tail - name);//提取名称
        tname[tail - name] = '\0';
		int entryCnt = inode.i_size / sizeof(de_type);
		
		bool found = false;
		for (int i = 0; i < entryCnt; ++i){//查找
			readFile(inode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
			
			if ((strcmp(direntry.m_name, tname) == 0) && (direntry.m_ino != -1)){
				found = true;
				break;
			}
		}
		if ((!found) || (direntry.m_ino == -1)){
		    inode_type err;
		    err.i_number = -1;
			cout << "ERROR: 路径不存在！" << endl;
			return err;
		}
		inode = getInode(direntry.m_ino);
		name = ++tail;
		while ((*tail != '/') && (tail != end))
			++tail;
	}
	return getInode(direntry.m_ino);
}

int fopen(char *name, int mode){
	char *tail = name;
	while (*tail != '\0')
		++tail;
    if (strlen(name) > 1) {
        *tail = '/';
        tail++;
        *tail = '/0';
    }
	inode_type pinode = getDirInode(name, tail + 1);
	if (pinode.i_mode != 0 || pinode.i_number == -1)
        return -1;
	
	return openFile(pinode.i_number, mode);
}

void fcreat(char *name, int mode){
	char *tail = name;
	while (*tail != '\0')
		++tail;
	while (*tail != '/')
		--tail;
	++tail;
	inode_type pinode = getDirInode(name, tail);//找父亲的inode
	if (pinode.i_number == -1)
        return;
	de_type direntry;
	int entryCnt = pinode.i_size / sizeof(de_type);
	for (int i = 0; i < entryCnt; ++i){
		readFile(pinode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		if (direntry.m_ino != -1 && (strcmp(direntry.m_name,tail) == 0)){
		    cout << "ERROR：该文件名已经存在！" <<endl;
		    return;
		}
	}
	direntry.m_ino = ialloc();
	inode_type inode = getInode(direntry.m_ino);
	inode.i_size = 0;
	inode.i_mode = mode;
	inode.i_number = direntry.m_ino;
	strcpy(direntry.m_name, tail);
	writeFile(pinode, &direntry, pinode.i_size, sizeof(de_type));
	pinode.i_size += sizeof(de_type);
	setInode(pinode);
	entryCnt = pinode.i_size / sizeof(de_type);
	readFile(pinode.i_number, &direntry, (entryCnt-1)*sizeof(de_type), sizeof(de_type));
	setInode(inode);
}

void fdelete(char *name){
	char *tail = name;
	while (*tail != '\0')
		++tail;
	while (*tail != '/')
		--tail;
	char tname[30];
	strcpy(tname, tail + 1);
	++tail;
	inode_type pinode = getDirInode(name, tail);//找父亲的inode
	if (pinode.i_number == -1)
        return;
	de_type direntry;
	direntry.m_ino = pinode.i_number;
	int entryCnt = pinode.i_size / sizeof(de_type);
	bool found = false;
	int i;
	for (i = 0; i < entryCnt; ++i){//查找entry
		readFile(pinode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		if ((strcmp(direntry.m_name, tname) == 0) && (direntry.m_ino != -1)) {
			found = true;
			break;
		}
	}
	if ((!found) || (direntry.m_ino == -1)){
		cout << "ERROR: 找不到指定路径！" << endl;
		return;
	}
	direntry.m_ino = -1;//DELETED = -1
	writeFile(pinode, &direntry, i * sizeof(de_type), sizeof(de_type));
	inode_type inode = getInode(direntry.m_ino);
	releaseDataBlocks(inode);
	setInode(pinode);
}

bool ck(char *name) {
    char *tail = name;
	while (*tail != '\0')
		++tail;
	while (*tail != '/')
		--tail;
	char tname[30];
	strcpy(tname, tail + 1);
	++tail;
	inode_type pinode = getDirInode(name, tail);//找父亲的inode
	if (pinode.i_number == -1)
        return 0;
	de_type direntry;
	direntry.m_ino = pinode.i_number;
	int entryCnt = pinode.i_size / sizeof(de_type);
	bool found = false;
	int i;
	for (i = 0; i < entryCnt; ++i){//查找entry
		readFile(pinode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		inode_type ss= getInode(direntry.m_ino);
		if ((strcmp(direntry.m_name, tname) == 0) && (direntry.m_ino != -1) && (ss.i_mode == IFDIR)) {
			found = true;
			break;
		}
	}
	if ((!found) || (direntry.m_ino == -1)){
		cout << "ERROR: 找不到指定路径！" << endl;
		return 0;
	}
	return 1;
}

void ls(char *name){
	char *tail = name;
	while (*tail != '\0')
		tail++;
    if (strlen(name) > 1) {
        *tail = '/';
        tail++;
        *tail = '/0';
    }
	inode_type inode = getDirInode(name, tail);
	if (inode.i_number == -1)
        return;
	
	de_type direntry;
	int entryCnt = inode.i_size / sizeof(de_type);
    
	inode_type t;
	for (int i = 0; i < entryCnt; ++i){
		readFile(inode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		
		inode_type ss= getInode(direntry.m_ino);
		
		if (direntry.m_ino != -1){// && strlen(direntry.m_name)> 0) {
		    t = getInode(direntry.m_ino);
            if (t.i_mode == IFDIR)
                cout << '/';
			cout << direntry.m_name << endl;
		}
	}
	int k = 0;
	return;
}

void pushFreeBlockList(int blkno){//加入空块链表
	if (blkno == 0)
		return;
	kernel.superBlock.freebl.blkno[++kernel.superBlock.freebl.n] = blkno;
}

void releaseDataBlocks(inode_type inode){//释放inode用到的全部
	int p[BLOCKSIZE / sizeof(int)];
	for (int i = 0; i < 10; ++i)
		pushFreeBlockList(inode.i_addr[i]);
}

void loadSuperBlock(){
	readDisk(&kernel.superBlock, 0, sizeof(sb_type));
	int k = 0;
}

int alloc(){
	if (kernel.superBlock.freebl.n == 0){
		int blkno = kernel.superBlock.freebl.blkno[0];
		readBlock(&kernel.superBlock.freebl, blkno, 0, sizeof(fbl_type));
		kernel.superBlock.freebl.blkno[++kernel.superBlock.freebl.n] = blkno;
	}
	return kernel.superBlock.freebl.blkno[kernel.superBlock.freebl.n--];
}

int ialloc(){
	if (kernel.superBlock.freeil.n == 0){
		int blkno = kernel.superBlock.freeil.ino[0];
		readBlock(&kernel.superBlock.freeil, blkno, 0, sizeof(fil_type));
		kernel.superBlock.freebl.blkno[++kernel.superBlock.freebl.n] = blkno;
	}
	return kernel.superBlock.freeil.ino[kernel.superBlock.freeil.n--];
}

void IORecv(void *p, int offset, int size){
	kernel.diskfile = fopen(DISKFILENAME, "rb");
	fseek(kernel.diskfile, offset, 0);
	fread(p, size, 1, kernel.diskfile);
	fclose(kernel.diskfile);
}

void IOSend(void *p, int offset, int size){
	kernel.diskfile = fopen(DISKFILENAME, "rb+");
	fseek(kernel.diskfile, offset, 0);
	fwrite(p, size, 1, kernel.diskfile);
	fclose(kernel.diskfile);
}

/***************************************************************/ 
/* 初始化 设置函数*/ 
inode_d_type diskinodes[DISKINODECAPACITY];
de_type rootdirentry[5];
de_type emptydirentry;
char emptydata[(DATABLOCKCOUNT * BLOCKSIZE) - sizeof(de_type)];
FILE *outf;

void superblock_init(sb_type &sb){
	sb.s_isize = DISKINODECAPACITY * sizeof(inode_d_type) / BLOCKSIZE;
	sb.s_fsize = DATABLOCKCOUNT;
	int nextBlkno = ((sizeof(sb_type) + sizeof(diskinodes))) / BLOCKSIZE + 1;
	sb.freeil.n = 0;
	sb.freeil.ino[0] = -1;
	for (int i = 6; i < DISKINODECAPACITY; ++i){
		++sb.freeil.n;
		sb.freeil.ino[sb.freeil.n] = i;
		if (sb.freeil.n == 99){
			fseek(outf, nextBlkno * BLOCKSIZE, 0);
			fwrite(&sb.freeil, sizeof(sb.freeil), 1, outf);
			sb.freeil.n = 0;
			sb.freeil.ino[0] = nextBlkno;
			++nextBlkno;
		}
	}//生成 free inode list
	sb.freebl.n = 0;
	sb.freebl.blkno[0] = -1;
	int lastBlkno = (sizeof(sb_type) + sizeof(diskinodes)) / BLOCKSIZE + DATABLOCKCOUNT;
	for (int i = nextBlkno; i < lastBlkno; ++i){
		++sb.freebl.n;
		sb.freebl.blkno[sb.freebl.n] = i;
		if (sb.freebl.n == 99){
			fseek(outf,  lastBlkno * BLOCKSIZE, 0);
			fwrite(&sb.freebl, sizeof(sb.freebl), 1, outf);
			sb.freebl.n = 0;
			sb.freebl.blkno[0] = lastBlkno;
			--lastBlkno;
		}
	}//生成 free block list;
	sb.s_flock = 12;
	sb.s_ilock = 12;
	sb.s_fmod = 0;
	sb.s_ronly = 0;
	sb.s_time = 123;
}

void root_dir_init(){
	//根目录
	rootdirentry[0].m_ino = 1;
	strcpy(rootdirentry[0].m_name, "home");
	rootdirentry[1].m_ino = 2;
	strcpy(rootdirentry[1].m_name, "user");
	rootdirentry[2].m_ino = 3;
	strcpy(rootdirentry[2].m_name, "DirA");
	rootdirentry[3].m_ino = 4;
	strcpy(rootdirentry[3].m_name, "DirB");
	rootdirentry[4].m_ino = 5;
	strcpy(rootdirentry[4].m_name, "DirC");
	memset(diskinodes, 0, sizeof(diskinodes));
	diskinodes[0].d_addr[0] = (sizeof(sb_type) + sizeof(diskinodes)) / BLOCKSIZE;
	diskinodes[0].d_size = sizeof(rootdirentry);
	diskinodes[0].d_mode = IFDIR;
	for (int i = 1; i <= 5; ++i){
		diskinodes[i].d_size = 0;
		diskinodes[i].d_mode = IFDIR;
	}
}

void sys_init()
{    
    cout<<"初始化设定"<<endl;
	outf = fopen(DISKFILENAME, "wb");
	sb_type sb;
	superblock_init(sb);
	fseek(outf, 0, 0);
    root_dir_init();
	fwrite(&sb, sizeof(sb), 1, outf);//写 SuperBlock
	
	cout << "构建SuperBlock 共写入" << sizeof(sb) << "字节" << endl;
	fwrite(&diskinodes, sizeof(diskinodes), 1, outf);//写Inode
	cout << "构建inode 共写入 " << sizeof(diskinodes) << "字节" << endl;
	fseek(outf, sizeof(sb) + sizeof(diskinodes), 0);
	fwrite(&rootdirentry, sizeof(de_type), 5, outf);//写根目录
	cout << "构建根目录\n/home\n/user\n/DirA\n/DirB\n/DirC\n写入" << sizeof(de_type) * 5 << "字节" << endl;
	fclose(outf);
	cout << "初始化设定结束" << endl;
	
}

void testWrite(char *name, char *content){
	int fd = fopen(name, IWRITE);
	
	if (fd >= 0) {
        fwrite(fd, content, strlen(content));
        fclose(fd);
        cout << "成功写入 " << strlen(content) << "字节" << endl;
	}else{
        cout << "ERROR：访问非法！" << endl;
	}
}

void testRead(char *name){
	char content[2048];
	memset(content, 0, sizeof(content));
	int fd = fopen(name, IREAD);
	if (fd >= 0) {
        fread(fd, content, 2048);
        fclose(fd);
        cout << content << endl;
	}else{
        cout << "ERROR：访问非法！" << endl;
	}
}

void init(){//初始化磁盘
	kernel.openFileCount = 0;
	kernel.freeBufCount = 0;
	for (int i = 0; i < BUFCOUNT; ++i)
		memset(&kernel.buffers[i].b_flags, 0, sizeof(unsigned int));
	for (int i = 0; i < FILECOUNT; ++i)
		memset(&kernel.openFiles[i].i_flag, 0, sizeof(unsigned int));
	loadSuperBlock();
}

const int MAXBUF = 2048 * 1000;
kernel_type kernel;
char extrabuffer[MAXBUF];

