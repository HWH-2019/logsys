#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include<signal.h>
#include <stdlib.h>
#include<time.h>
#include<sys/time.h>
#include <sys/wait.h>
#include<sys/syscall.h>
#include<linux/kernel.h>
 


using namespace std;

string misspath="/mnt/xfs/miss.log";
string timepath="/mnt/xfs/time.log";
string configpath="/mnt/xfs/.config";

    
    
long unsigned int fibonacci(unsigned int n);    /*计算 fibonacci 数的函数*/
/*用于分别记录父，子 1 进程 real time 过的总秒数*/
static long p_realt_secs=0,c1_realt_secs=0;     
/*用于分别记录父，子 1进程 virtual time 过的总秒数*/
static long p_virtt_secs=0, c1_virtt_secs=0;
/*用于分别记录父，子 1 进程 proft time 过的总秒数*/
static long p_proft_secs=0,c1_proft_secs=0;     

/*用于分别取出父，子 1 进程的 real timer 的值*/
static struct itimerval p_realt,c1_realt;     
/*用于分别取出父，子 1，子 2 进程的 virtual timer 的值*/ 
static struct itimerval p_virtt,c1_virtt;     
/*用于分别取出父，子 1进程的 proft timer 的值*/
static struct itimerval p_proft,c1_proft;  


static void par_sig(int signo);    /*父进程的信号处理函数*/
static void c1_sig(int signo);      /*子进程 1 的信号处理函数*/

int syscallwrite(char* buf,char* path); /* 系统调用写入文件 */
int syscallread(char* buf,char* path);  /* 系统调用读取文件 */
long syscallfilelen(char* path); /* 系统调用获取文件大小 */

void timer(){

}
//获取配置文件里的日志路径
void getpath(){
    ifstream file;
    file.open(configpath, std::ios::out | std::ios::app);
    string line;
    while (getline(file, line)) {
        int pos = line.find("=");
        int length = line.length();
        string key = line.substr(0, pos);
        if (key == "misspath") {
            misspath = line.substr(pos + 1, length - pos - 1);
        } else if (key == "timepath") {
            timepath = line.substr(pos + 1, length - pos - 1);
        }
    }
    file.close();
}

//写入配置文件
void writepath(string key,string val){
    ifstream in;
    string data="";
    in.open(configpath,std::ios::out | std::ios::in);
    string line;
    while(getline(in,line)){
    	int pos = line.find("=");
        int length = line.length();
        string k = line.substr(0, pos);
        if(k==key){
        	data+=key+"="+val+"\n";
        }else{
        	data+=line+"\n";
        }
    }
    in.close();
    
    //写入文件
    ofstream out;
    out.open(configpath,ios::out | ios::trunc);
    out.flush();
    out<<data;
    out.close();
    

    
    fstream fs;
    fs.open(val,ios::in);
    
    if(!fs){
    	ofstream f(val);
    	f.close();
    }
}

//更改日志存储位置
void changepath() {
    char ch=' ';
    bool changed=false;
    string path1;
    string path2;
    while(true){
    getpath();
    cout << "当前日志的存储位置为:" << endl;
    cout<<"\t缺页中断日志文件路径:"<<misspath<<endl;
    cout<<"\t进程统计时间日志文件路径:"<<timepath<<endl;
    cout << "输入命令(1更改缺页日志路径，2更改时间日志路径，q退出更改):"<<endl;
     string str;
     cin>>str;
     ch=str.at(0);
     if(ch=='q')break;
     switch(ch){
      case '1':
       cout<<"缺页中断日志文件路径:";
       cin >> path1;
       changed = true;
       if(path1!=misspath){
       writepath("misspath",path1);
       }
       break;
      case '2':
       cout<<"进程统计时间日志文件路径:";
       cin>>path2;
       if(path2!=timepath){
       writepath("timepath",path2);
       }
       break;
      default:
       break;
     }
     
    }
    
    if(changed){
	cout<<"重载模块"<<endl;
    	system("sudo rmmod readpfcount");
    	char buf[120];
    	string path = "sudo insmod ./pagefault/readpfcount.ko path="+misspath;
    	strcpy(buf,path.c_str());
    	system(buf);
   }
    
    
}

//打印缺页日志
void printmiss() {
	
    char *buf; 
    char path[120];
    strcpy(path,misspath.c_str());
    long fsize = syscallfilelen(path);
    buf = (char *)malloc(sizeof(char) * fsize);
    syscallread(buf,path);
    cout << "打印缺页日志" << endl;
    printf("%s",buf);
}

//打印时间日志
void printtime() {
    char *buf; 
    char path[120];
    strcpy(path,timepath.c_str());
    long fsize = syscallfilelen(path);
    buf = (char *)malloc(sizeof(char) * fsize);

    syscallread(buf,path);
    cout << "打印时间日志" << endl;
    
    printf("%s",buf);
}

//打印日志
void printlog() {
    string str;
    cout << "输入1打印缺页日志,2打印进程时间日志:";
    cin >> str;
    char ch = str.at(0);
    switch (ch) {
        case '1':
            printmiss();
            break;
        case '2':
            printtime();
            break;
        default:
            break;
    }
}

//打印帮助
void printhelp() {
    cout << "c:更改日志存储位置" << endl;
    cout << "p:打印日志" << endl;
    cout << "f:创建子进程" << endl;
    cout << "q:退出" << endl;
}
void creatfork(){
    long unsigned fib=0;
    pid_t pid1,pid2;
    unsigned int fibarg=39;
    int status;
    struct itimerval v;
    long moresec,moremsec,t1,t2;
    

        /*设置子进程 1 的信号处理函数和定时器初值*/
        signal(SIGALRM,c1_sig);

        v.it_interval.tv_sec=10;
        v.it_interval.tv_usec=0;
        v.it_value.tv_sec=10;
        v.it_value.tv_usec=0;
        setitimer(ITIMER_REAL,&v,NULL);

		/*
		operation
		*/

        fib=fibonacci(fibarg);    /*计算 fibonacci 数*/



        /*取出子进程 1 的定时器值*/
   
        getitimer(ITIMER_REAL,&c1_realt);

		/*通过定时器的当前值和各信号发出的次数计算子进程 1 总共用的 real time,cpu
		time,user time 和 kernel time。moresec 和 moremsec 指根据定时器的当前值计算
		出的自上次信号发出时过去的 real time,cpu time,user time 和 kernel time。计算
		kernel time 时,moresec 和 moremsec 为 kernel time 的实际秒数+毫秒数*/
        moresec=9-c1_realt.it_value.tv_sec;
		moremsec= (1000000-c1_realt.it_value.tv_usec)/1000;
        printf("Child 1 fib=%ld , real time=%ld sec,%ldmsec\n",fib,c1_realt_secs+moresec,moremsec);
        
        char buf[120]; 
        char path[120];
        
	strcpy(path,timepath.c_str());
        sprintf(buf,"Child 1 fib=%ld , real time=%ld sec,%ldmsec\n",fib,c1_realt_secs+moresec,moremsec);
        syscallwrite(buf,path);
        
        fflush(stdout);
             

}
//主执行函数
void execute(string str) {
    char ch = str.at(0);
    switch (ch) {
        case 'm':
            printhelp();
            break;
        case 'c':
            changepath();
            break;
        case 'p':
            printlog();
            break;
        case 'f':
            creatfork();
            break;
        default:
            break;
    }
}


int main() {
    getpath();
    
    long unsigned fib=0;
    pid_t pid1,pid2;
    unsigned int fibarg=39;
    int status;
    struct itimerval v;
    long moresec,moremsec,t1,t2;
    /*设置子进程 1 的信号处理函数和定时器初值*/
        signal(SIGALRM,c1_sig);

        v.it_interval.tv_sec=10;
        v.it_interval.tv_usec=0;
        v.it_value.tv_sec=10;
        v.it_value.tv_usec=0;
        setitimer(ITIMER_REAL,&v,NULL);

    
    
    while (true) {
        string str;
        cout << "命令(输入m获取帮助)：";
        cin >> str;
        if (str != "\n") {
            if (str == "q") {
                break;
            } else {
                execute(str);
            }
        }
    }
    /*取出子进程 1 的定时器值*/
   
        getitimer(ITIMER_REAL,&c1_realt);

		/*通过定时器的当前值和各信号发出的次数计算子进程 1 总共用的 real time,cputime,user time 和 kernel time。moresec 和 moremsec 指根据定时器的当前值计算出的自上次信号发出时过去的 real time,cpu time,user time 和 kernel time。计算kernel time 时,moresec 和 moremsec 为 kernel time 的实际秒数+毫秒数*/
        moresec=9-c1_realt.it_value.tv_sec;
		moremsec= (1000000-c1_realt.it_value.tv_usec)/1000;
        printf("主进程  real time=%ld sec，%ldmsec\n",c1_realt_secs+moresec,moremsec);
        char buf[120]; 
        char path[120];
        
        

	strcpy(path,timepath.c_str());
        sprintf(buf,"主进程  real time=%ld sec，%ldmsec\n",c1_realt_secs+moresec,moremsec);
        syscallwrite(buf,path);

        fflush(stdout);
            
    return 0;
}

int syscallwrite(char* buf,char* path){
	
	int blength=strlen(buf)+1;
	int plength=strlen(path)+1;
	
       return syscall(444,blength,buf,plength,path);
}


int syscallread(char* buf,char* path){
	
	int plength=strlen(path)+1;
	
       return syscall(443,buf,plength,path);
}

long syscallfilelen(char* path){

	int plength=strlen(path)+1;
	return syscall(442,plength,path);
}

long unsigned fibonacci(unsigned int n)
{
                if(n==0)
                                return 0;
                else if(n==1||n==2)
                                return 1;
                else
                                return (fibonacci(n-1)+fibonacci(n-2));
}

/*父进程信号处理函数；每个 timer 过 10 秒减为 0，激活处理函数一次，相应的计数器加 10*/
static void par_sig(int signo)
{
 
                switch(signo)
                {
                                case SIGALRM:
                                                p_realt_secs+=10;
                                                break;

                }
}
 
/*子进程 1 的信号处理函数，功能与父进程的信号处理函数相同*/
static void c1_sig(int signo)
{
 
                switch(signo)
                {                                case SIGALRM:
                                                c1_realt_secs+=10;
                                                break;

                }
}
