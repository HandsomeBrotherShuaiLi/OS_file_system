/*�ο�unix Դ�� \unix64\oos\src\include */ 

#include<iostream>
#include <bits/stdc++.h>
using namespace std;
/*
 * ������ƿ�buf����
 * ��¼����Ӧ�����ʹ���������Ϣ��
 * ͬʱ����I/O����飬��¼�û���
 * ��ص�I/O�����ִ�н����
 */
class Buf
{
public:
	enum BufFlag	/* b_flags�б�־λ */
	{
		B_WRITE = 0x1,		/* д�������������е���Ϣд��Ӳ����ȥ */
		B_READ	= 0x2,		/* �����������̶�ȡ��Ϣ�������� */
		B_DONE	= 0x4,		/* I/O�������� */
		B_ERROR	= 0x8,		/* I/O��������ֹ */
		B_BUSY	= 0x10,		/* ��Ӧ��������ʹ���� */
		B_WANTED = 0x20,	/* �н������ڵȴ�ʹ�ø�buf�������Դ����B_BUSY��־ʱ��Ҫ�������ֽ��� */
		B_ASYNC	= 0x40,		/* �첽I/O������Ҫ�ȴ������ */
		B_DELWRI = 0x80		/* �ӳ�д������Ӧ����Ҫ��������ʱ���ٽ�������д����Ӧ���豸�� */
	};
	
public:
	unsigned int b_flags;	/* ������ƿ��־λ */
	
	int		padding;		/* 4�ֽ���䣬ʹ��b_forw��b_back��Buf������Devtab��
							 * �е��ֶ�˳���ܹ�һ�£�����ǿ��ת������� */
	/* ������ƿ���й���ָ�� */
	Buf*	b_forw;
	Buf*	b_back;
	Buf*	av_forw;
	Buf*	av_back;
	
	short	b_dev;			/* �������豸�ţ����и�8λ�����豸�ţ���8λ�Ǵ��豸�� */
	int		b_wcount;		/* �贫�͵��ֽ��� */
	unsigned char* b_addr;	/* ָ��û�����ƿ�������Ļ��������׵�ַ */
	int		b_blkno;		/* �����߼���� */
	int		b_error;		/* I/O����ʱ��Ϣ */
	int		b_resid;		/* I/O����ʱ��δ���͵�ʣ���ֽ��� */
};





static const unsigned int IALLOC = 0x8000;/* �ļ���ʹ�� */
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
 * ��������ڵ�(DiskINode)�Ķ���
 * ���Inodeλ���ļ��洢�豸�ϵ�
 * ���Inode���С�ÿ���ļ���Ψһ��Ӧ
 * �����Inode���������Ǽ�¼�˸��ļ�
 * ��Ӧ�Ŀ�����Ϣ��
 * ���Inode������ֶκ��ڴ�Inode���ֶ�
 * ���Ӧ�����INode���󳤶�Ϊ64�ֽڣ�
 * ÿ�����̿���Դ��512/64 = 8�����Inode
 */
 

struct inode_d_type{
	unsigned int d_mode;/* ״̬�ı�־λ�������enum INodeFlag */
	int d_nlink;/* �ļ���������������ļ���Ŀ¼���в�ͬ·���������� */
	short d_uid;/* �ļ������ߵ��û���ʶ�� */
	short d_gid;/* �ļ������ߵ����ʶ�� */
	int d_size;/* �ļ���С���ֽ�Ϊ��λ */
	int d_addr[10];/* �����ļ��߼���ú�������ת���Ļ��������� */
	int d_atime;/* ������ʱ�� */
	int d_mtime;/* ����޸�ʱ�� */
};

struct fbl_type{
	int n;
	int blkno[110];
};

struct fil_type{
	int n;
	int ino[110];
};

//superblock�Ľṹ�� 
struct sb_type{
	int s_isize;/* ���Inode��ռ�õ��̿��� */
	int s_fsize;/* �̿����� */

	fbl_type freebl;/* free block list*/
	fil_type freeil;/*free inode list*/

	int s_flock;/* ���������̿��������־ */
	int s_ilock;/* ��������Inode���־ */

	int s_fmod;/* �ڴ���super block�������޸ı�־����ζ����Ҫ��������Ӧ��Super Block */
	int s_ronly;/* ���ļ�ϵͳֻ�ܶ��� */
	int s_time;/* ���һ�θ���ʱ�� */
	int padding[27];/* ���ʹSuperBlock���С����1024�ֽڣ�ռ��2������ */
};

static const int DIRENTRYSIZE = 32;

struct de_type{
	int m_ino;
	char m_name[DIRENTRYSIZE - sizeof(int)];
};

/*
 * �ڴ������ڵ�(INode)�Ķ���
 * ϵͳ��ÿһ���򿪵��ļ�����ǰ����Ŀ¼��
 * ���ص����ļ�ϵͳ����ӦΨһ���ڴ�inode��
 * ÿ���ڴ�inodeͨ�����inode���ڴ洢�豸���豸��(i_dev)
 * �Լ����豸���inode���еı��(i_number)��ȷ��
 * ���Ӧ�����inode��
 */
struct inode_type{
	unsigned int i_flag;/* ״̬�ı�־λ�������enum INodeFlag */
	unsigned int i_mode;/* �ļ�������ʽ��Ϣ */
	int i_size;/* �ļ���С���ֽ�Ϊ��λ */
	int i_addr[10];/* �����ļ��߼���ú�������ת���Ļ��������� */
	int i_number;/* ���inode���еı�� */
	int i_count;/* ���ü��� */
	int i_offset;
	int i_nlink;/* �ļ���������������ļ���Ŀ¼���в�ͬ·���������� */
	short i_dev;/* ���inode���ڴ洢�豸���豸�� */
	short i_uid;/* �ļ������ߵ��û���ʶ�� */
	short i_gid;/* �ļ������ߵ����ʶ�� */
	int i_lastr;/* ������һ�ζ�ȡ�ļ����߼���ţ������ж��Ƿ���ҪԤ�� */
};


/*
 * ������ƿ�buf����
 * ��¼����Ӧ�����ʹ���������Ϣ��
 * ͬʱ����I/O����飬��¼�û���
 * ��ص�I/O�����ִ�н����
 */
 
struct buf_type{
	unsigned int b_flags;/* ������ƿ��־λ */
	int b_blkno;/* �����߼���� */
	/* ������ƿ���й���ָ�� */
	buf_type *b_forw;
	buf_type *b_back;
	buf_type *av_forw;
	buf_type *av_back;
	char data[BLOCKSIZE];
	short b_dev;/* �������豸�ţ����и�8λ�����豸�ţ���8λ�Ǵ��豸�� */
	int b_wcount;/* �贫�͵��ֽ��� */
	unsigned char* b_addr;/* ָ��û�����ƿ�������Ļ��������׵�ַ */
	int b_error;/* I/O����ʱ��Ϣ */
	int b_resid;/* I/O����ʱ��δ���͵�ʣ���ֽ��� */
};
enum BufFlag	/* b_flags�б�־λ */
	{
		B_WRITE = 0x1,		/* д�������������е���Ϣд��Ӳ����ȥ */
		B_READ	= 0x2,		/* �����������̶�ȡ��Ϣ�������� */
		B_DONE	= 0x4,		/* I/O�������� */
		B_ERROR	= 0x8,		/* I/O��������ֹ */
		B_BUSY	= 0x10,		/* ��Ӧ��������ʹ���� */
		B_WANTED = 0x20,	/* �н������ڵȴ�ʹ�ø�buf�������Դ����B_BUSY��־ʱ��Ҫ�������ֽ��� */
		B_ASYNC	= 0x40,		/* �첽I/O������Ҫ�ȴ������ */
		B_DELWRI = 0x80		/* �ӳ�д������Ӧ����Ҫ��������ʱ���ٽ�������д����Ӧ���豸�� */
	};

/*
 * Kernel�����ڷ�װ�����ں���ص�ȫ����ʵ������
 * ����PageManager, ProcessManager�ȡ�
 * 
 * Kernel�����ڴ���Ϊ����ģʽ����֤�ں��з�װ���ں�
 * ģ��Ķ���ֻ��һ��������
 */
struct kernel_type{
	FILE *diskfile;/* disk�ļ�*/ 
	sb_type superBlock;/*superblock*/ 
	buf_type buffers[BUFCOUNT];/*�������*/
	 
	buf_type *freeBufHead;/*���л��������ͷָ��*/ 
	buf_type *freeBufTail;/*���л��������βָ��*/ 
	int freeBufCount;/*���л�����ƿ�ĸ���*/ 
	inode_type openFiles[FILECOUNT];/*�򿪵�inode�ļ�*/ 
	int openFileCount;/*�����ź�*/ 
};
extern kernel_type kernel;

inode_type getDirInode(char *name, char *end);
void ls(char *name); //�г� name ·���µ������ļ�
int fopen(char *name, int mode);//��modeģʽ�� name �ļ�
void fcreat(char *name, int mode); //��modeģʽ���� name �ļ�
void fdelete(char *name);//ɾ�� name �ļ�
void releaseDataBlocks(inode_type inode);
void pushFreeBlockList(int blkno);

void loadSuperBlock();
int alloc();
int ialloc();

void IORecv(void *p, int offset, int size);//���� IO ��ȡ����ģ���豸��ȡ
void IOSend(void *p, int offset, int size);//���� IO д������ģ���豸д��
inode_type getRootInode();
inode_type getInode(int ino);
void setInode(inode_type inode);


void readDisk(void *p, int offset, int size);//��ȡӲ�̵����ݣ�offset Ϊ����Ӳ�̵ľ���λ��
void writeDisk(void *p, int offset, int size);//д��Ӳ�̵����ݣ�offset Ϊ����Ӳ�̵ľ���λ��
int readBlock(void *p, int blkno, int offset, int size);//���� blkno ��ȡĳһ������ݣ���һ�������ݿ�
int writeBlock(void *p, int blkno, int offset, int size);//���� blkno д��ĳһ������ݣ���һ�������ݿ�
buf_type *getBuffer(int blkno, int mode);//��ȡ�����
void releaseBuffers();//�ͷ����л����



void readFile(int ino, void *p, int offset, int size);//�� Inode ���Ϊ ino ���ļ��ж�ȡ����
void writeFile(inode_type &is, void *p, int offset, int size);//�� is ���ļ���д������
int openFile(int ino, int mode);//���ļ�������������
int getBlockNum(inode_type *node);//����ĳ�� offset ��Ӧ�����ݿ���
int fseek(int fd, int offset);//�ƶ��ļ�ָ��
int fread(int fd, void *buffer, int len);//���ļ��ж�ȡ
int fwrite(int fd, void *buffer, int len);//���ļ���д��
void fclose(int fd);//�ر��ļ�
bool ck(char *name);//�鿴·���Ƿ����



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
	if (kernel.freeBufCount == BUFCOUNT){ //����Ҳ���
	    p = kernel.freeBufHead;//��Ҫ��ɵ�һ��
		kernel.freeBufHead = kernel.freeBufHead->b_forw;
		//if (p->b_flags & B_ASYNC)
		IOSend(p->data, p->b_blkno * BLOCKSIZE, BLOCKSIZE);
	}else {//�ҿ��õ�
		++kernel.freeBufCount;
		for (int i = 0; i < BUFCOUNT; ++i)
			if (!(kernel.buffers[i].b_flags & B_BUSY)){
				p = &kernel.buffers[i];
				break;
			}
	}
    if (kernel.freeBufCount > 1){//����
		kernel.freeBufTail->b_forw = p;
		p->b_back = kernel.freeBufTail;
		kernel.freeBufTail = p;
	}else{
		kernel.freeBufHead = p;
		kernel.freeBufTail = p;
	}
	//������
	p->b_flags = mode;
	p->b_flags |= B_BUSY;
	p->b_blkno = blkno;
	IORecv(p->data, blkno * BLOCKSIZE, BLOCKSIZE);
	return p;
}

void releaseBuffers(){//�ͷ����л���
	buf_type *p = kernel.freeBufHead;
	for (int i = 0; i < kernel.freeBufCount; ++i){
		IOSend(p->data, p->b_blkno * BLOCKSIZE, BLOCKSIZE);
		p = p->b_forw;
	}
}

int readBlock(void *p, int blkno, int offset, int size){//���������
	buf_type *buf = getBuffer(blkno, B_READ);
	if (offset + size > BLOCKSIZE)
		size = BLOCKSIZE - offset;
	memcpy(p, buf->data + offset, size);
	return size;
}

int writeBlock(void *p, int blkno, int offset, int size){//��д����
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
		cout << "ERROR:�ļ��������࣡" << endl;
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

int getBlockNum(inode_type* node){//�������
	const static int DBS = BLOCKSIZE / sizeof(int);
	int abs = node->i_offset / BLOCKSIZE;
	if (abs < 6){//1��
		return node->i_addr[abs];
	}
	else if (abs < 6 + 2 * DBS){//2��
		int ab = 6 + (abs - 6) / DBS;
		int bb = (abs - 6) % DBS;
		int blkno;
		readBlock(&blkno, node->i_addr[ab], bb * sizeof(int), sizeof(int));
		return blkno;
	}else{//3��
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
	
	if (kernel.openFiles[fd].i_size < (kernel.openFiles[fd].i_offset + len)){//���������
	    
		int finalsize = kernel.openFiles[fd].i_offset + len;
		int finalabsblkno = (finalsize - 1) / BLOCKSIZE;
		int currentabsblkno = (kernel.openFiles[fd].i_size - 1) / BLOCKSIZE;
		if (kernel.openFiles[fd].i_size == 0)
			currentabsblkno = -1;
		kernel.openFiles[fd].i_size = finalsize;
		while (currentabsblkno < finalabsblkno){
			if (currentabsblkno < 5){//1���͹�
				++currentabsblkno;
			
				kernel.openFiles[fd].i_addr[currentabsblkno] = alloc();
			}
			else if (currentabsblkno < 5 + DBS * 2){ //2��
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
			else{//3��
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
	while (len > 0){ //д����
		int blkno = getBlockNum(node);
		int wsize = writeBlock(buffer, blkno, node->i_offset % BLOCKSIZE, len);
		buffer = (char *)buffer + wsize;
		node->i_offset += wsize;
		len -= wsize;
	}
	return len;
}

void fclose(int fd){//�رղ�����inode
	kernel.openFiles[fd].i_flag &= !IALLOC;
	inode_type *node = &kernel.openFiles[fd];
	int blkno = getBlockNum(node);
	setInode(kernel.openFiles[fd]);
}

void readFile(int ino, void *p, int offset, int size){//���ļ�
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

void writeFile(inode_type &is, void *p, int offset, int size){//д�ļ�
	int fd = openFile(is.i_number, IWRITE);
	fseek(fd, offset);
	fwrite(fd, p, size);
    memcpy(&is.i_addr,&kernel.openFiles[0].i_addr,sizeof(int) * 10);
	fclose(fd);
}

inode_type getDirInode(char *name, char *end){//����Ŀ¼��inode
 
	if (*name != '/'){//��ʽ���
		cout <<"ERROR: ��ʽ�Ƿ���" << endl;
		return inode_type();
	}
	++name;
	char *tail = name;
	inode_type inode = getRootInode();
	while ((*tail != '/') && (*tail != 0 ) && (tail != end))//�����βλ��
		++tail;
	char tname[32];
	memset(tname, 0, sizeof(tname));
	de_type direntry;
	direntry.m_ino = inode.i_number;
	while (tail != end){
        
		strncpy(tname, name, tail - name);//��ȡ����
        tname[tail - name] = '\0';
		int entryCnt = inode.i_size / sizeof(de_type);
		
		bool found = false;
		for (int i = 0; i < entryCnt; ++i){//����
			readFile(inode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
			
			if ((strcmp(direntry.m_name, tname) == 0) && (direntry.m_ino != -1)){
				found = true;
				break;
			}
		}
		if ((!found) || (direntry.m_ino == -1)){
		    inode_type err;
		    err.i_number = -1;
			cout << "ERROR: ·�������ڣ�" << endl;
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
	inode_type pinode = getDirInode(name, tail);//�Ҹ��׵�inode
	if (pinode.i_number == -1)
        return;
	de_type direntry;
	int entryCnt = pinode.i_size / sizeof(de_type);
	for (int i = 0; i < entryCnt; ++i){
		readFile(pinode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		if (direntry.m_ino != -1 && (strcmp(direntry.m_name,tail) == 0)){
		    cout << "ERROR�����ļ����Ѿ����ڣ�" <<endl;
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
	inode_type pinode = getDirInode(name, tail);//�Ҹ��׵�inode
	if (pinode.i_number == -1)
        return;
	de_type direntry;
	direntry.m_ino = pinode.i_number;
	int entryCnt = pinode.i_size / sizeof(de_type);
	bool found = false;
	int i;
	for (i = 0; i < entryCnt; ++i){//����entry
		readFile(pinode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		if ((strcmp(direntry.m_name, tname) == 0) && (direntry.m_ino != -1)) {
			found = true;
			break;
		}
	}
	if ((!found) || (direntry.m_ino == -1)){
		cout << "ERROR: �Ҳ���ָ��·����" << endl;
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
	inode_type pinode = getDirInode(name, tail);//�Ҹ��׵�inode
	if (pinode.i_number == -1)
        return 0;
	de_type direntry;
	direntry.m_ino = pinode.i_number;
	int entryCnt = pinode.i_size / sizeof(de_type);
	bool found = false;
	int i;
	for (i = 0; i < entryCnt; ++i){//����entry
		readFile(pinode.i_number, &direntry, i * sizeof(de_type), sizeof(de_type));
		inode_type ss= getInode(direntry.m_ino);
		if ((strcmp(direntry.m_name, tname) == 0) && (direntry.m_ino != -1) && (ss.i_mode == IFDIR)) {
			found = true;
			break;
		}
	}
	if ((!found) || (direntry.m_ino == -1)){
		cout << "ERROR: �Ҳ���ָ��·����" << endl;
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

void pushFreeBlockList(int blkno){//����տ�����
	if (blkno == 0)
		return;
	kernel.superBlock.freebl.blkno[++kernel.superBlock.freebl.n] = blkno;
}

void releaseDataBlocks(inode_type inode){//�ͷ�inode�õ���ȫ��
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
/* ��ʼ�� ���ú���*/ 
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
	}//���� free inode list
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
	}//���� free block list;
	sb.s_flock = 12;
	sb.s_ilock = 12;
	sb.s_fmod = 0;
	sb.s_ronly = 0;
	sb.s_time = 123;
}

void root_dir_init(){
	//��Ŀ¼
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
    cout<<"��ʼ���趨"<<endl;
	outf = fopen(DISKFILENAME, "wb");
	sb_type sb;
	superblock_init(sb);
	fseek(outf, 0, 0);
    root_dir_init();
	fwrite(&sb, sizeof(sb), 1, outf);//д SuperBlock
	
	cout << "����SuperBlock ��д��" << sizeof(sb) << "�ֽ�" << endl;
	fwrite(&diskinodes, sizeof(diskinodes), 1, outf);//дInode
	cout << "����inode ��д�� " << sizeof(diskinodes) << "�ֽ�" << endl;
	fseek(outf, sizeof(sb) + sizeof(diskinodes), 0);
	fwrite(&rootdirentry, sizeof(de_type), 5, outf);//д��Ŀ¼
	cout << "������Ŀ¼\n/home\n/user\n/DirA\n/DirB\n/DirC\nд��" << sizeof(de_type) * 5 << "�ֽ�" << endl;
	fclose(outf);
	cout << "��ʼ���趨����" << endl;
	
}

void testWrite(char *name, char *content){
	int fd = fopen(name, IWRITE);
	
	if (fd >= 0) {
        fwrite(fd, content, strlen(content));
        fclose(fd);
        cout << "�ɹ�д�� " << strlen(content) << "�ֽ�" << endl;
	}else{
        cout << "ERROR�����ʷǷ���" << endl;
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
        cout << "ERROR�����ʷǷ���" << endl;
	}
}

void init(){//��ʼ������
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

