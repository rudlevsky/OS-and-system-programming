#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define PI 3.14159265358979323846
extern int errno;
FILE *input,*output; 
char InPath[]="/tmp/math.txt";  
char OuPath[]="/tmp/result.txt";  

int *thread_count;            

typedef struct args_st {
    int i;
    int j;
    int N;
    int c;
} args_t;

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

double PowFloat(double value,int in)
{
    int i;
    double result=1;
    for(i=1;i<=abs(in);i++)
      result*=value/i;
    return result;
}

int PowMinusOne(int in)
{
    if((in%2)==0) return 1;
      else return -1;
}

void *element(void *args) 
{  
    args_t *arg = (args_t*)args;
    int sign;               
    double degree;           //count X^i
    double perem=0;
    double per;	

    per = 2*PI*arg->i/arg->N;

    degree=PowFloat((per),2*arg->j+1);//counting X^(2i+1)
    sign=PowMinusOne(arg->j+2);            //change sign
    perem=sign*degree;  
    //counting and ID of process

    fprintf(stdout,"thread=%d and perem[%d]=%.25f\n",(int)pthread_self(),arg->i,perem);

    //forming of string
    if(fprintf(input, "%d %d %.8lf\n", (int)pthread_self(),arg->i, perem) == -1)
    {
 	fprintf(stdout,"thread error found");
    	pthread_exit(NULL);
    }

    thread_count[arg->c] = 0;
    free(arg);
    pthread_exit(NULL);
}

int TailorFunction(char err[], int K, int N, int FF)
{
    if((input=fopen(InPath,"w+"))==NULL)
    {
        DeletePartPath(err,"./",err);
        mist(err);
        return 1;
    }

    pthread_t thread[FF];  
    int processes = 0;
    int perem = 0;

    for(int i = 0; i<FF; i++) {
	thread_count[i] = 0;
    } 
	
    for (int i=0;i<N;i++)
    {    
         
    	for (int j=0;j<K;j++) 
    	{	
		if (processes == FF)
            	{
			int flag = 0;
			while (1) {
			    for(int s = 0; s<FF; s++) {
				if (thread_count[s] == 0) {
				    flag = 1; break;
				}
			    }
			    if (flag != 0) {break;}
			} 
                	processes--;
            	}
	
		for(int s = 0; s<FF; s++) {
		        if (thread_count[s] == 0) 
			{
		            perem = s;
		            thread_count[s] = 1;
			    break;
		        }
	  	}

		args_t *ar = (args_t*)malloc(sizeof(args_t));
		ar->i = i;
		ar->j = j;
		ar->N = N;
		ar->c = perem;

        	if (pthread_create(&thread[perem],NULL,element,ar) != 0) {
			DeletePartPath(err,"./",err);
        		mist(err);
        		return 1;
		}
 
		processes++;			
   	 }
    }

    for(int i = 0; i<FF; i++) 
    {
	pthread_join(thread[i],NULL);
    }
	
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
 
  //  free(result);
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
    thread_count = calloc(FF,sizeof(int));
    TailorFunction(argv[0],K,N,FF);
    free(thread_count);
    return 0;
}
