#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PI 3.14159265358979323846
extern int errno;

//deleting string from substring
void DeletePartPath(char *str, char *fnd, char *buf)
{   
    char *p=strstr(str, fnd);   
    if(p==NULL)
    {strcpy(buf, str); return;}   
    char *fnd1=fnd;
    while(str<p)
    {*buf=*str; buf++; str++; *buf='\0';}
    while(*fnd1)
    {str++; fnd1++;} 
    strcpy(buf, str);
    return;
}

//type of error
void mist(char err[]) 
{
        switch(errno)
        {
	    case ENOTTY:{fprintf(stderr,"%s Error: Uknown operation I/O control!\n",err);break;}
            case EACCES:{fprintf(stderr,"%s Error: Access is denied!\n",err);break;}
            case EBADF :{fprintf(stderr,"%s Error: Invalid file descriptor!\n",err);break;}
            case EBUSY :{fprintf(stderr,"%s Error: Resourse is busy!\n",err);break;}
            case EMFILE:{fprintf(stderr,"%s Error: Too many open files!\n",err);break;}
            case EISDIR:{fprintf(stderr,"%s Error: It is directory!\n",err);break;}
            default: {fprintf(stderr,"%s Error:!\n",err);break;}
        }
}

float PowFloat(float value,int in)
{
    int i;
    float result=1;
    for(i=1;i<=abs(in);i++)
      result*=value;
    return result;
}

int PowMinusOne(int in)
{
    if((in%2)==0) return 1;
      else return -1;
}

int TailorFunction(char err[], int K, int N, int FF)
{
    FILE *input,*output; 
    char InPath[]="/tmp/math.txt";  
    char OuPath[]="/tmp/result.txt";

    if((input=fopen(InPath,"w+"))==NULL)
    {
        DeletePartPath(err,"./",err);
        mist(err);
        return 1;
    }

    long int factorial=1;   
    int cnt=1;              //counter
    int sign;               
    float degree;           //count X^i
    float perem=0;	
    pid_t pid;  
    int processes = 0;

    for (int i=0;i<N;i++)
    {           
	for (int j=0;j<K;j++) 
	{
		if (processes == (FF+1))
            	{
                	wait(NULL);
                	processes--;
            	}
            	pid = fork();

            	if (pid == 0)
            	{	
        		//elements 
        		degree=PowFloat((2*PI*i/N),2*j+1);//counting X^(2i+1)
        		sign=PowMinusOne(j+2);            //change sign
        		perem=sign*degree/factorial;  
        		cnt=cnt+2;                        //next element of factorial
        		factorial=factorial*(cnt-1)*cnt;  //counting of next factorial element
        		//counting and ID of process
        		fprintf(stdout,"PID=%d and perem[%d]=%.25f\n",getpid(),i,perem);
        		//forming of string
        		if(fprintf(input, "%d %d %.8lf\n", getpid(), i, perem) ==-1)
        		{
        			DeletePartPath(err,"./",err);
        			mist(err);
        			return 1;
        		}
			exit(0);
		}
		processes++;
	}			
    }

    while (wait(NULL) > 0) { };

    if((output=fopen(OuPath,"w"))==NULL)
    {
        DeletePartPath(err,"./",err);
        mist(err);
        return 1;
    }

    double *result = alloca(sizeof(double)*N);
    memset(result, 0, sizeof(double)*N);
    rewind(input);

    int pidd, f;
    double member_value;
 
    for (int i = 0; i < N*K; i++) {
        if (fscanf(input, "%d %d %lf", &pidd, &f, &member_value) == -1){
        	DeletePartPath(err,"./",err);
        	mist(err);
        	return 1;
        }
        result[f] += member_value;
    }

    for (int i = 0; i < N; i++) 
    {
        if (fprintf(output, "y[%d]=%.8lf\n", i, result[i]) == -1)
	{
        	DeletePartPath(err,"./",err);
        	mist(err);
        	return 1;
        } 
    }

    //trying to close
    if(fclose(input)==-1)
    {
        DeletePartPath(err,"./",err);
  	mist(err);
        return 1;
    }

    //trying to close
    if(fclose(output)==-1)
    {
        DeletePartPath(err,"./",err);
        mist(err);
        return 1;
    }

    return 0;
}

int main(int argc,char *argv[])
{
    int K,N,FF;

    //number of arguments
    if(argc<4)
    {
        fprintf(stderr,"Error:Too few arguments!\n");
        return 1;
    }

    K=atoi(argv[1]);
    N=atoi(argv[2]);
    FF=atoi(argv[3]);
    TailorFunction(argv[0],K,N,FF);
    return 0;
}
