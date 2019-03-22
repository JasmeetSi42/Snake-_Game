#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
# define URL_Length 1000
# define Path_Length 300
struct Node
{
	
	struct Node * next;
	struct Node * previous;	
	int key;
	char *URL;
	int isVisited;
	int depth;	
};
struct Node *head;
struct Node *start;
struct Node *Hash[50];
int compare(char BASE_URL[],char *str1)
{
	int NotEqual=0;
	for(int i=0;BASE_URL[i]!='\0';i++)
	{
		if(str1[i]!=BASE_URL[i])
		{
			NotEqual=1;
			break;
		}
	}
	if(NotEqual==0)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
bool checkLink(char *str1)
{
char BASE_URL[]={"www.chitkara.edu.in"};
	int b=compare(BASE_URL,str1);
	if(b==0)
	{
		printf("Wrong Url");
		printf("\nSearch Terminated");
		return false;
	}
	else
	{
		char *ptr=(char*)malloc(1000);
		strcat(ptr,"wget --spider ");
		strcat(ptr,str1);
		if(system(ptr))
		{
			printf("URL Not Found");
			return false;
		}	
	}
return true;
}
bool checkDepth(char *str2)
{
	if(str2[0]>=49&&str2[0]<=53)
	{
		if(str2[1]!='\0')
		{
			return false;
		}
	}
	else
	{
		return false;
	}
return true;
}
bool checkdirectory(char *str3)
{
	DIR* dir= opendir(str3);
	if(dir)
	{
		return true;
	}
	else if(ENOENT == errno)
	{
		return false;
	}
return true;
}
void getPage(char *URL)
{
	char urlbuffer[URL_Length+300]={0};
	strcat(urlbuffer,"wget ");	
	strcat(urlbuffer,URL);	
	strcat(urlbuffer," -O ~/Desktop/temp.txt");
	system(urlbuffer);
}
char* DataTransfer(char *p)
{
	FILE *fp,*fp1;
	fp1=fopen(p,"w");
	int a=0;
	fp=fopen("temp.txt","r");
	char ch=getc(fp);
	char *STR =(char*)malloc(1000000);	
	while(ch!=EOF)
	{
		STR[a]=ch;
		a++;
		ch=getc(fp);
		
	}
	STR[a]='\0';
	fprintf(fp1,"%s",STR);
	return STR;
}
char* PathCreated(char *Path)
{
	char String_File[20];
	 int file_no;
	FILE *File_No,*File_No1;
	File_No=fopen("FileNOs.txt","r");	
	char ch=getc(File_No);
	int sum=0;
	while(ch!=EOF&&ch!=10)
	{
		int a=ch-48;
		sum=sum*10+a;
		ch=getc(File_No);
	}
	file_no=sum;
	fclose(File_No);	
	File_No1=fopen("FileNOs.txt","w");
	fprintf(File_No1,"%d",sum+1);	
	fclose(File_No);
	sprintf(String_File,"%d",file_no);
	char p[Path_Length+100]={0};
	strcat(p,"touch ");		
	strcat(p,Path);
	strcat(p,"/temp");
	strcat(p,String_File);
	strcat(p,".txt");
	printf("%s",p);
	system(p);
	printf("Path Created");
	char *temp=(char*)malloc(100);
	strcat(temp,"temp");
	strcat(temp,String_File);
	strcat(temp,".txt");
	return DataTransfer(temp);
}
void NormalizeWord(char* word) {
  int i = 0;
  while (word[i]) {
      // NEW
    if (word[i] < 91 && word[i] > 64) // Bounded below so this funct. can run on all urls
      // /NEW
      word[i] += 32;
    i++;
  }
}
int NormalizeURL(char* URL) 
{
  int len = strlen(URL);
  if (len <= 1 )
    return 0;
  //! Normalize all URLs.
  if (URL[len - 1] == '/') 
  {
    URL[len - 1] = 0;
    len--;
  }
  int i, j;
  len = strlen(URL);
  //! Safe check.
  if (len < 2)
    return 0;
  //! Locate the URL's suffix.
  for (i = len - 1; i >= 0; i--)
    if (URL[i] =='.')
      break;
  for (j = len - 1; j >= 0; j--)
    if (URL[j] =='/')
      break;
  //! We ignore other file types.
  //! So if a URL link is to a file that are not in the file type of the following
  //! one of four, then we will discard this URL, and it will not be in the URL list.
  if ((j >= 7) && (i > j) && ((i + 2) < len)) 
  {
    if ((!strncmp((URL + i), ".htm", 4))
        ||(!strncmp((URL + i), ".HTM", 4))
        ||(!strncmp((URL + i), ".php", 4))
        ||(!strncmp((URL + i), ".jsp", 4))
        ) 
    {
      len = len; // do nothing.
    } 
    else 
    {
      return 0; // bad type
    }
  }
  return 1;
}

void removeWhiteSpace(char* html) 
{
  int i;
  char *buffer = malloc(strlen(html)+1), *p=malloc (sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++) 
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}
int GetNextURL(char* html, char* urlofthispage, char* result, int pos) 
{
  char c;
  int len, i, j;
  char* p1;  //!< pointer pointed to the start of a new-founded URL.
  char* p2;  //!< pointer pointed to the end of a new-founded URL.

  // NEW
  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // /NEW

  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos])) 
  {
    if ((c=='<') &&
        ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c) 
  {  
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    p1 = strchr(&(html[pos+1]), '=');
    
    if ((!p1) || (*(p1-1) == 'e') || ((p1 - html - pos) > 10)) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*(p1+1) == '\"' || *(p1+1) == '\'')
      p1++;

    p1++;    

    p2 = strpbrk(p1, "\'\">");
    if (!p2) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*p1 == '#') 
    { // Why bother returning anything here....recursively keep going...

      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(p1, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(p1, "http", 4) || !strncmp(p1, "HTTP", 4)) 
    {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, p1, (p2-p1));
      return  (int)(p2 - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (p1[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this. 
	// again...probably good to recursively keep going..
	// NEW
        
        return GetNextURL(html,urlofthispage,result,pos+1);
	// /NEW
      }
      if (p1[0] == '/') {
        //! this means the URL is the absolute path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, p1, (p2 - p1));
        return (int)(p2 - html + 1);        
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, p1, p2 - p1);
            return (int)(p2 - html + 1);
        }
        if ((i <= 6)||(i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, p1, p2 - p1);
          return (int)(p2 - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, p1, p2 - p1);
        return (int)(p2 - html + 1);
      }
    }
  }    
  return -1;
}
int LinkGet(char *Temp_Link[100],char *html,char *str1)
{
	int filesize=strlen(html);
	char *result=(char*)malloc(30000);
	int a=0;	
	int pos=0;
	while(pos<filesize)	
	{
		if(pos==-1)
		{
			printf("Link Completed\n");
			break;
		}
		else
		{
			int Duplicate_Flag=0;
			pos=GetNextURL(html,str1,result,pos);
			for(int i=0;i<a;i++)
			{
				if(strcmp(Temp_Link[i],result)==0)
				{

					Duplicate_Flag=1;
				}
			}
			if(Duplicate_Flag==0)
			{
				strcpy(Temp_Link[a],result);
				a++;
			}
			if(a==100)
			{
				break;
			}
			//	printf("%s\n",Temp_Link[a-1]);
						
		}
	}
	return a;
}
int HashCode(char *URL)
{
	long int Code=0;
	for(int i=0;URL[i]!='\0';i++)
	{
		Code+=URL[i];
	}
	for(Code;Code>50;Code/=10)
	{}
	return Code;
}
void Filling_List(struct Node *head,struct Node *start,int Links_Size,char *Temp_Link[100])
{
	head->previous=NULL;
	//printf("%d\n",Links_Size);	
	for(int i=0;i<Links_Size-1;i++)	
	{
		start->next=NULL;		
		start->URL=(char*)malloc(100000);
		strcpy(start->URL,Temp_Link[i]);
		start->isVisited=0;
		start->depth=0;
		int Code=HashCode(start->URL);
		start->key=Code;	
		struct Node *new=(struct Node*)malloc(sizeof(struct Node));		
		start->next=new;
		struct Node *old;
		old=start;
		//printf("%d\n",Code);		
		start=start->next;
		start->previous=old;
		if(Hash[Code]==NULL)
		{
			Hash[Code]=start;
		}
		/*else
		{
			struct Node * Hash_Addr;
			Hash_Addr=Hash[Code];			
			//printf("%d\n",Hash_Addr->next->key);
			//Hash_Addr->next=start->depth++;	
			struct Node *t;
			t=Hash_Addr->next;
			Hash_Addr->next=new;
			new=t;
			t->previous=new;
		}*/
	}		
	start->next=NULL;
	start->URL=(char*)malloc(100000);
	strcpy(start->URL,Temp_Link[Links_Size-1]);
	int Code=HashCode(start->URL);
	start->key=Code;	
	if(Hash[Code]==NULL)
	{
		Hash[Code]=start;
	}
	/*else
	{
			struct Node * Hash_Addr;
			Hash_Addr=Hash[Code];
			struct Node *t;
			t=Hash_Addr->next;
			Hash_Addr->next=start;
			start=t;
			t->previous=start;		
	}*/
}
int main(int argc,char *argv[])
{
	if(argc!=4)
	{
		printf("Invalid Arguments");
		return 0;
	}
	char *str1=argv[1];
	char *str2=argv[2];
	char *str3=argv[3];
	bool CheckURL=checkLink(str1);
	if(CheckURL==false)
	{	
		return 0;
	}
	bool CheckDepth=checkDepth(str2);
	if(CheckDepth==false)
	{
		printf("Wrong Depth\nSearch terminated");
		return 0;
	}
	bool CheckDirectory=checkdirectory(str3);
	if(CheckDirectory==false)
	{
		printf("Wrong Directory\nSearch Terminated");
		return 0;
	}
	int depth=1;
	while(true)
	{
		if(depth==4){break;}
		depth++;
		getPage(str1);
	char *html=(char*)malloc(10000000);
	html=PathCreated(str3);
	char *Temp_Link[100];
	for(int i=0;i<100;i++)
	{
		*(Temp_Link+i)=(char*)malloc(10000000);
	}
	int Links_Size=LinkGet(Temp_Link,html,str1);	
	head =(struct Node *)malloc(sizeof(struct Node));
	start=head;	
	for(int i=0;i<50;i++)
	{
		Hash[i]=NULL;
	}
	Filling_List(head,start,Links_Size,Temp_Link);	
	struct Node *temp;
	temp=head;
	/*while(temp!=NULL)
	{
		printf("%d\n",temp->key);
		temp=temp->next;	
	}*/
	FILE *fp1;
	fp1=fopen("FileLink.txt","w");
	while(temp!=NULL)
	{
		fprintf(fp1,"%s,%d,%d,%d\n",temp->URL,temp->key,temp->depth,temp->isVisited);
		temp=temp->next;
	}
	fclose(fp1);}

}
