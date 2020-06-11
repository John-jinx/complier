#include "scan.h"
#include "global.h"

char* tokenstring();                                ///ת�����ַ���
static void get_nextline(FILE *in);                 ///��ȡ��һ�еĴ���
static void get_token();                            ///��ȡtoken
static int state_change(int state,char ch);         ///״̬ת��
static char *returnstring(int m,int n);             ///��ʶ��ĳ�����token���и��Ʊ��浽������
static tokentype letterchange(char a[]);            ///���ֵ�ʶ��ת�������Ϊ�ؼ��֣� ��洢Ϊ�ؼ��֣����ǹؼ��־���ID
static int is_digit(char ch);                       ///���ֵ��ж�
static int is_letter(char ch);                      ///�ַ����ж�
static int is_keywords(char a[]);                   ///�ؼ��ֵ��ж�
static void print_token();                          ///���
/**�ؼ�������**/
static char keywords[50][20]={ "char", "continue","break",
                                "else","for", "float","if","while",
                                "int","main","double","case",
                                "return","void","continue", "struct",
                                "switch","default"};
int tokenno=0;                      ///token������ �﷨������Ҫ��
tokenrecord token_table[1000];           /**���ڴ��ʶ��ĵ�token**/
char linebuf[1000];                                 /**ÿһ�д��뻺����**/
int lineno=0;                                       /**lineno���ڼ�¼���������;**/
int currentchar;                                    /**��currentcharΪ���뻺�����ļ�����**/
int line_num_table[1000]={0};                       /**line_num_table��¼ÿһ�����һ���Ǻŵ�λ��**/
tokentype token;

/***********************************************************
 * ����:	������
 **********************************************************/
static int transform_table[16][13]={{0,0,0,0,0,0,0,0,0,0,0,0,0},    ///-1 Ϊ����
                        {2,0,0,5,0,6,0,10,8,12,0,0,0},
                        {2,3,-1,0,0,0,0,0,0,0,0,0,0},
                        {4,0,0,0,0,0,0,0,0,0,0,0,0},
                        {4,0,-1,0,0,0,0,0,0,0,0,0,0},
                        {5,0,0,5,-1,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,-1,7,0,0,0,0,0},
                        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                        {0,0,0,0,0,0,-1,9,0,0,0,0,0},
                        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                        {0,0,0,0,0,0,-1,11,0,0,0,0,0},
                        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                        {0,0,0,0,0,0,0,0,0,13,-1,0,0},
                        {0,0,0,0,0,0,0,0,0,0,0,14,15},
                        {0,0,0,0,0,0,0,0,0,0,0,14,15},
                        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};

/***********************************************************
 * ����:	�ʷ���������ں���
 1:�����л��弼�����ж�ȡÿһ�е��ַ�
 **********************************************************/
void scan()
{
    FILE *text;
    text= fopen("../demo/test2.c", "r");/**��ȡ�����ļ�**/
	if(text!=NULL)
	{

		while(!feof(text))                                  /**�ļ�ֱ��ѭ���ж����ļ�βfeof(fp)����������ֵ:��������ļ�����������feof��fp����ֵΪ1������Ϊ0��**/
		{
		    printf("yes");
			currentchar=0;                                  /**ÿ����һ����������**/
			get_nextline(text);                               /**��ȡÿһ�У�������linebuf�У���'/0'����**/
			lineno++;
			if (strcmp(linebuf, " ") == 0) /**�Ƚ�����ǿմ�**/
            {
                continue;
            }
			while(currentchar<strlen(linebuf))              /**ע��currentchar��strlen�ǲ�����'/0'�ģ������л��з���\n��**/
			{
                    get_token();

			}
			line_num_table[lineno]=tokenno-1;
		}
	}
	fclose(text);
	print_token();
}



/***********************************************************
 * ����:	�л���ľ���ʵ��
 **********************************************************/
static void get_nextline(FILE *text)
{
	if (!fgets(linebuf, 9999, text))/**�����һ������ֻ��һ���س����᷵�ش���**/
	{
		strcpy(linebuf, " ");
	}
}


/***********************************************************
 * ����:	ʶ������token
 ͨ���Զ����ͱ�����
 **********************************************************/
static void get_token()
{
	char ch=linebuf[currentchar];;
	int ncurrentchar=currentchar;
	if(ch==' '||ch=='\n'||ch=='\t')
    {
        currentchar++;
        return;
    }
    /**�����ǲ���Ҫ����״̬��������ֱ��һ������ʶ���token: / + - ! { } . [ ]**/
	switch (ch)
	{
	    case '!':
			{
				if(linebuf[currentchar+1]=='=')
				{
					token_table[tokenno].tokenval=NOTEQU;
					currentchar=currentchar+2;
				}
				break;
			}
		case '+':
			{
			    if(linebuf[currentchar+1]=='+')
				{
				    token_table[tokenno].tokenval=pPLUS;
                    currentchar=currentchar+2;
                    break;
				}
				else
                {
                    token_table[tokenno].tokenval=PLUS;
                    currentchar++;
                    break;
                }
			}
		case'-':
			{
			    if(linebuf[currentchar+1]=='-')
				{
				    token_table[tokenno].tokenval=mMINUS;
                    currentchar=currentchar+2;
                    break;
				}
				else
				{
                    token_table[tokenno].tokenval=MINUS;
                    currentchar++;
                    break;
				}
			}
		case '*':
			{
				token_table[tokenno].tokenval=TIMES;
				currentchar++;
				break;
			}
		case '%':
			{
				token_table[tokenno].tokenval=MODE;
				currentchar++;
				break;
			}
		case '{':
			{
				token_table[tokenno].tokenval=L_DA;
				currentchar++;
				break;
			}
		case '}':
			{
				token_table[tokenno].tokenval=R_DA;
				currentchar++;
				break;
			}
		case '[':
			{
				token_table[tokenno].tokenval=L_ZH;
				currentchar++;
				break;
			}
		case ']':
			{
				token_table[tokenno].tokenval=R_ZH;
				currentchar++;
				break;
			}
		case '(':
			{
				token_table[tokenno].tokenval=L_XI;
				currentchar++;
				break;
			}
		case ')':
			{
				token_table[tokenno].tokenval=R_XI;
				currentchar++;
				break;
			}
		case ';':
			{
				token_table[tokenno].tokenval=FENH;
				currentchar++;
				break;
			}
        case ':':
			{
				token_table[tokenno].tokenval=MAOH;
				currentchar++;
				break;
			}
		case ',':
			{
				token_table[tokenno].tokenval=DOUH;
				currentchar++;
				break;
			}
		case '.':
			{
				token_table[tokenno].tokenval=DIAN;
				currentchar++;
				break;
			}
        case '&':
			{
				token_table[tokenno].tokenval=ADE;
				currentchar++;
				break;
			}
        case '|':
			{
				token_table[tokenno].tokenval=HUO;
				currentchar++;
				break;
			}

	default:
	{
            int state=1,newstate;
            while(1)/**����״̬��������ʶ��token����**/
            {
                int col=state_change(state,ch);
                if(col==999)
                {
                    printf("ERROR!!!!!");
                }
                newstate=transform_table[state][col];
                if(newstate == -1) break;
                currentchar++;
                ch=linebuf[currentchar];
                state=newstate;
            }
            switch(state)
            {
            case 2: /**��������**/
                {
                    token_table[tokenno].tokenval=NUM;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    token_table[tokenno].numval=atof(token_table[tokenno].stringval);/**atof()���ַ���ת��Ϊ����**/
                    break;
                }

            case 4:/**������**/
                {
                    token_table[tokenno].tokenval=NUM;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    token_table[tokenno].numval=atof(token_table[tokenno].stringval);
                    break;
                }
            case 5:  /**����Ǳ����֣��򱣴汣���֣������ǣ��򱣴����**/
                {
                        printf("%s",returnstring(ncurrentchar,currentchar-1));
                        printf("%s",token_table[tokenno].stringval);
                        strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                        if(is_keywords(token_table[tokenno].stringval))
                        {
                            token_table[tokenno].tokenval=letterchange(token_table[tokenno].stringval);
                        }

                        else
                            token_table[tokenno].tokenval=ID;
                        break;
                }
            case 6:  /**������ <**/
                {
                    token_table[tokenno].tokenval=SMALLER;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }
            case 7:/**������ <=**/
                {
                    token_table[tokenno].tokenval=SMALLEREQU;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }
             case 8:/**������ >**/
                {
                    token_table[tokenno].tokenval=BIGGER;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }
            case 9:/**������ >=**/
                {
                    token_table[tokenno].tokenval=BIGGEREQU;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }

            case 10:/**������ =**/
                {
                    token_table[tokenno].tokenval=EQUAL;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }
            case 11:/**������ ==**/
                {
                    token_table[tokenno].tokenval=IFEQU;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }
             case 12:/**������ / **/
                {
                    token_table[tokenno].tokenval=DIVIDE;
                    strcpy(token_table[tokenno].stringval,returnstring(ncurrentchar,currentchar-1));
                    break;
                }
            case 15:/**ע��**/
                {
                    token_table[tokenno].tokenval=COMMENT;
                    break;
                }
            default:
                {
                    printf("error");
                }
            }
	}
	}
	tokenno++;
}



/***********************************************************
 * ����:�ؼ��ֵ�ʶ��ת�������Ϊ�ؼ��֣�
 ��洢Ϊ�ؼ��֣����ǹؼ��־���ID
 **********************************************************/
tokentype letterchange(char a[])
{
    if(strcmp(a,"int")==0) return INT;
    if(strcmp(a,"break")==0) return BREAK;
    if(strcmp(a,"case")==0) return CASE;
    if(strcmp(a,"char")==0) return CHAR;
    if(strcmp(a,"continue")==0) return CONTINUE;
    if(strcmp(a,"double")==0) return DOUBLE;
    if(strcmp(a,"float")==0)return FLOAT;
	if(strcmp(a,"char")==0)return CHAR;
	if(strcmp(a,"void")==0)return VOID;
	if(strcmp(a,"main")==0)return MAIN;
	if(strcmp(a,"if")==0)return IF;
	if(strcmp(a,"else")==0)return ELSE;
	if(strcmp(a,"while")==0)return WHILE;
	if(strcmp(a,"printf")==0)return PRINTF;
	if(strcmp(a,"scanf")==0)return SCANF;
	if(strcmp(a,"for")==0)return FOR;
	if(strcmp(a,"return")==0)return RETURN;
	if(strcmp(a,"struct")==0)return STRUCT;
	if(strcmp(a,"default")==0)return DEFAULT;
	if(strcmp(a,"switch")==0) return SWITCH;
	if(strcmp(a,"break")==0) return BREAK;
	printf("error!!!");
}


/***********************************************************
 * ����:	�ж��Ƿ��ǹؼ���
 **********************************************************/
static int is_keywords(char a[])
{
    int i=0;

    for(;i<50;i++)
        if(strcmp(a,keywords[i])==0)/**������Ǳ�����**/
        {
             return 1;
        }

    return 0;
}


/***********************************************************
 * ����:	״̬�仯��ʵ�֣�ͨ��״̬ת����
 **********************************************************/
static int state_change(int state,char ch)
{

    if(state==1)
    {
        if (is_digit(ch)) return 0;
        if (is_letter(ch)) return 3;
        if (ch=='<') return 5;
        if(ch=='>') return 8;
        if(ch=='=') return 7;
        if(ch=='/') return 9;
    }
    if(state==2)
    {
        if(is_digit(ch)) return 0;
        if(ch=='.')  return 1;
        else return 2;
    }

    if(state==3)
    {
        if(is_digit(ch)) return 0;
    }

    if(state==4)
    {
        if(is_digit(ch)) return 0;
        else return 2;
    }

    if(state==5)
    {
        if(is_digit(ch)||is_letter(ch)) return 0;
        return 4;
    }

    if(state==6||state==8||state==10)
    {
        if(ch=='=') return 7;
        else return 6;
    }

     if(state==7||state==9||state==11||state==15)
    {
        return 0;
    }
    if(state==12)
    {
        if(ch=='/') return 9;
        if(ch!='=')return 10;
    }
     if(state==13||state==14)
    {

        if(ch=='\n'){ return 12;}
        else return 11;
    }
    return 999;

}


/***********************************************************
* ����:	��ʶ��ĳ�����token���и��Ʊ��浽������
**********************************************************/

static char* returnstring(int m,int n)
{
    int i=0;
    char s[200];
    for(;i<=(n-m);i++)
    {
        s[i]=linebuf[m+i];
    }
    s[i]='\0';
    char *p =s;
    return p;
}
/***********************************************************
* ����:	1.�ж��Ƿ�Ϊchar
2.�ж��Ƿ�Ϊ����
**********************************************************/

static int is_letter(char ch)
{
    if(('a'<=ch&&ch<='z')||('A'<=ch&&ch<='Z'))
    {
        return 1;
    }

	else
    {
        return 0;
    }
}

static int is_digit(char ch)
{
    if('0'<=ch&&ch<='9')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************
* ����: �����ʱ����е��ַ�ת��
**********************************************************/
char* tokenstring()
{
	switch (token)
	{
	case INT :
	    return "int";
	case FLOAT:
	    return "float";
	case CHAR:
	    return "char";
	case DOUBLE:
	    return "double";
	case VOID:
	    return "void";
	case MAIN:
	    return "main";
	case IF:
	    return "if";
	case ELSE:
	    return "else";
	case WHILE:
	    return "while";
	case PRINTF:
	    return "printf";
	case SCANF:
	    return "scanf";
	case FOR:
	    return "for";
	case EQUAL:
	    return "=";
	case PLUS:
	    return "+";
	case pPLUS:
	    return "++";
	case MINUS:
	    return "-";
	case mMINUS:
	    return "--";
	case TIMES:
	    return "*";
	case DIVIDE:
	    return "/";
	case MODE:
	    return "%";
	case SMALLER:
	    return "<";
	case SMALLEREQU:
	    return "<=";
	case BIGGER:
	    return ">";
	case BIGGEREQU:
	    return ">=";
	case NOTEQU:
	    return "!=";
	case IFEQU:
	    return "==";
    case ADE:
	    return "&";
    case HUO:
	    return "|";
	case L_DA:
	    return "{";
	case R_DA:
	    return "}";
	case L_ZH:
	    return "[";
	case R_ZH:
	    return "]";
	case L_XI:
	    return "(";
	case R_XI:
	    return ")";
	case FENH:
	    return ";";
    case MAOH:
        return ":";
	case DOUH:
	    return ",";
	case DIAN:
	    return ".";
	case NUM:
	    return  "number";
	case ID:
	    return "id";
	case RETURN:
	    return "return";
	case COMMENT:
	    return "ע��";
    case STRUCT:
        return "struct";
    case CASE:
        return "case";
    case DEFAULT:
        return "default";
    case SWITCH:
        return "switch";
    case BREAK:
        return "break";
    default:
        printf("error�������޷�ƥ�䣡����");
	}
}


/***********************************************************
* ����:�������token
**********************************************************/
static void print_token()
{

	int i;
	//printf("tokenno=%d\n",tokenno);
	int j;
	for(j=1;line_num_table[j]!=0;j++)
	{
	    printf("%d: ",j);
		for(i=line_num_table[j-1];i<=line_num_table[j];i++)
		{
		    if(j!=1&&i==line_num_table[j-1])
            {
                i++;
            }
			token=token_table[i].tokenval;
			printf("[%s] ",tokenstring());
		}
		printf("\n");
	}

}
