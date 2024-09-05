#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASTABLE -1
#define OVERFLOW -2
#define INCREASEMENT 100
#define EXIST 0

typedef int status;


/*�����Ӿ�������ṹ����*/
typedef struct Clause{
    int literal;//��¼�Ӿ��е�����
    int flag;
	//��Ǹ������Ƿ��ѱ�ɾ����δɾ��ʱֵΪ0(EXIST)������ֵΪʹ֮ɾ���ı�Ԫ��ű��ڻظ� 
    struct Clause *nextl;//ָ����Ӿ�����һ���ֵ�ָ��
    struct Clause *litline;
	//ָ������CNF��ʽ�ڽ���������һ��������ͬ���Ӿ��㣬���ڻظ��뷽����� 
}Clause;

/*����CNF��ʽ�����㣨���Ӿ�����ͷ��㣩�ṹ����*/
typedef struct Paradigm{
    int number;//�Ӿ�����ʾ���������������ҵ��Ӿ� 
    int flag;
	//��Ǹ��Ӿ��Ƿ��ѱ�ɾ����δɾ��ʱֵΪ0(EXIST)������ֵΪʹ֮ɾ���ı�Ԫ��ű��ڻظ� 
    struct Paradigm *nextc;//ָ����һ�Ӿ��ͷ���
    struct Clause *sentence;//�Ӿ�ͷָ��
}Paradigm;

/*����CNF��ʽ����ͷ������ͣ��洢CNF��ʽ��Ϣ*/
typedef struct Root{
    int litsize;//�洢��������
    int parasize;//�洢�Ӿ�����
    Paradigm *first;//ָ���һ���Ӿ�
}Root;
 
//Dpll���õ��������ṹ�����ڸ���Ѱ�������ֺ͸����� 
/*����ָ���Ӿ�����ͷ����ָ��������ṹ����*/
typedef struct Paraline{
    Paradigm *claline;//ָ���Ӿ�����ͷ���Paradigm
    struct Paraline *next;//ָ����һ������
} Paraline;

/*�������������Ϣ����ṹ����*/
typedef struct LitTrabverse{
    Paraline *Tra_cla;//ָ���и������ֻ����ֵ��Ӿ�ͷ��������ͷ���
    Clause *Tra_lit;//ָ��������ֻ����ֵ����ֽ��
}LitTrabverse;

/*������������ļ�ʱ����Ĵ洢��Ԫ��Ϣ�ṹ����*/
typedef struct sudokusolver{
    int x;//�洢����Ϣ
    int y;//�洢����Ϣ
    int z;//�洢��Ԫ��Ӧ1��9��ֵ��Ϣ
} sudokusolver;

/*����洢��Ԫ��Ϣ�ı�Ԫ���Ա���ṹ����*/
typedef struct ArgueValue{
    int Value;//��Ԫ����ֵ
    int IsInit;//��Ԫ�Ƿ��Ѹ�ֵ
    int Occur_times;//��Ԫ�������Ӿ��г��ֵ��ܴ��������ڷ������ߵ�ѡ�� 
    LitTrabverse Pos;//��Ԫ���������ֵ������Ϣ�ṹ
    LitTrabverse Neg;//��Ԫ���и����ֵ������Ϣ�ṹ
    sudokusolver xyz;//��������ļ�ʱ����ı�Ԫ��Ϣ
} ArgueValue;


//ȫ�ֱ���
char filename[1000]; //�ļ��� 
long instacksize;//��¼ջstack�Ĵ�С
ArgueValue *ValueList;//��Ԫ��ֵ�� 
Root *r;//���� 
int *Frequency;//��Ԫ���ִ����������� 
int *stack;//DPLL����ջ 
int sudoku_table[9][9];//����ȫ�ֱ���int���Ͷ�ά����洢������
int users_sudoku[9][9];//�洢����ĺ��ո�����������
int sum_op;//��ѡ�� 
/*------------------------CNF�ļ�����------------------------*/

status CreateParadigm(FILE **fp);//����CNF��ʽ�ڽ�������Ԫ��
int CreateClause(FILE **fp,Clause **sentence,Paradigm *ClausHead,int first);//�����Ӿ�������������
status DestroyParadigm(Root *r);//���������������Ա�ṹ
int HasUnitClause(Root *r);//�ж�CNF��ʽ���Ƿ񻹺��е��Ӿ�
Clause * HasUnitClause_Before(Root *r);//�ж�CNF��ʽ���Ƿ񻹺��е��Ӿ䣨�Ż�ǰ�汾��
status isUnitClause(Paradigm *c);//�ж�ָ��cָ����Ӿ������Ƿ�Ϊ���Ӿ�����

/*������CNF��ʽ��ȡһ������*/
int FindLiteral1(Root *r);//ȡÿ��DPLL�����ʽ��Occur_Times��������
int FindLiteral2(Root *r);//ȡԭ��ʽ��Occur_Times��������
int FindLiteral3(Root *r);//ȡ�Ӿ��е�һ��δ��ɾ����flagΪ0��������
int FindLiteral4(Root *r);//��Ԫ�����������һ��δ����ֵ��IsInit=0���ı�Ԫ��������
Clause * FindLiteral_Before(Root *r);//������CNF��ʽ��ȡһ�����֣��Ż�ǰ�汾��

status DeleteClause(Root *r,int l);//ɾ������������l�����е��Ӿ�
status AddClause(Root *r,int l);//��CNF��ʽ�ڽ������ͷ���ֻ��������l�ĵ��Ӿ�����
status RemoveHeadClaus(Root *r,int l);//ɾ��CNF��ʽ�ڽ������дӱ�ͷ��ʼ��һ��ֻ��������l�ĵ��Ӿ�����
status DeleteLiteral(Root *r,int l);//ɾ����������Ϊ-l���Ӿ�������
status RecoverCNF(Root *r,int l);//�ָ���Ϊ����lΪ��ʱ��CNF��ʽ�ڽ����������Ĳ���
void ParadigmTrabverse(Root *r);//����CNF��ʽ�ڽ�����
status SaveValue(ArgueValue *ValueList,int solut,int time);//����CNF��ʽ�Ľ⼰���ʱ����Ϣ

/*------------------------DPLL�㷨------------------------*/

status SAT(void);
status DPLL1(int num,int op,int times);//���õ�һ�ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
status DPLL2(int num,int op,int times);//���õڶ��ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
status DPLL3(int num,int op,int times);//���õ����ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
status DPLL4(int num,int op,int times);//���õ����ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
status DPLL_Before(Root *r,int op);//�Ż�ǰ�汾DPLL
void AnswerComplete(void);//����SAT�����
status AnswerCheck(int solut);//���SAT�������ȷ��

/*------------------------SuDoKu------------------------*/
status Sudoku(void);
FILE * CreateSudokuFile(void);//������������ת��ΪSAT������cnf�ļ�
status CreateSudoku(void);//�����µ����9*9��������
status SolveSudoku(void);//�������cnf�ļ���ת��Ϊ����������ʽ���
status DigHole(int num);//����֪����������num����������Ψһ�����������
status SudokuComplete(void);//������������
FILE * SetCNFfile(void);//������txt�ļ�ת��Ϊcnf 
status CNFSudokuTableTransform(void);//������ӦSAT��Ԫ��ת��Ϊ��ά����
status SudokuTablePrint(void);//���������



/*------------------------CNF�ļ�����------------------------*/

/*����CNF��ʽ�ڽ�������Ԫ��
 ������(FILE**)open���ļ���ָ���ַ*/
status CreateParadigm(FILE **fp) {
	char readfile[20];//�����ַ����������¼���ļ��ж���������
	int l,j;
	Paradigm *p,*q;
	r=(Root*)malloc(sizeof(Root));
	r->first=NULL;
	while(fscanf(*fp,"%s",readfile)!=EOF) { //ѭ�����ļ�
		if (strcmp(readfile,"p")==0) //������p��˵����ʼ���ļ�
			break;
	}
	while (fscanf(*fp, "%s",readfile)!=EOF) {
		if (strcmp(readfile,"cnf")==0) {//���ļ��ж����ַ�����cnf��
			fscanf(*fp, "%d",&l);
			r->litsize=l;//��ȡCNF�ļ� ��Ԫ�� ����r->litsize
			fscanf(*fp, "%d",&l);
			r->parasize=l;//��ȡCNF�ļ� �Ӿ����� ����r->parasize
			break;
		}
	}
	int i;
	if(sum_op==1) printf("������%d ���Ӿ���%d��\n",r->litsize,r->parasize);
	//������Ԫ����ʼ����
	ValueList=(ArgueValue*)malloc((r->litsize+1)*sizeof(ArgueValue));
	if (ValueList==NULL) return OVERFLOW;//û����ɹ�
	for (i=0; i<=r->litsize; i++) {
		ValueList[i].IsInit=0;
		ValueList[i].Occur_times=0;
		ValueList[i].Pos.Tra_cla=NULL;
		ValueList[i].Pos.Tra_lit=NULL;
		ValueList[i].Neg.Tra_cla=NULL;
		ValueList[i].Neg.Tra_lit=NULL;
	}

	Frequency=(int *)malloc((r->litsize)*sizeof(int));

	i=2;
	if (r->parasize!=0) {
		//������һ���Ӿ�ͷ���������
		if (fscanf(*fp, "%d",&l)!=EOF&&l!=0) {
			p=(Paradigm*)malloc(sizeof(Paradigm));
			if (p==NULL) return OVERFLOW;
			r->first=p;//ͷ�ڵ�
			q=p;
			p->number=CreateClause(fp, &p->sentence, p, l);//�������Ӧ�Ӿ�����
			p->flag=EXIST;
			p->nextc=NULL;

			//����CNF��ʽ����
			while (i<=r->parasize) {
				i++;
				fscanf(*fp, "%d",&l);
				p=(Paradigm*)malloc(sizeof(Paradigm));
				if (p==NULL) return OVERFLOW;
				p->number=CreateClause(fp, &p->sentence, p, l);
				p->flag=0;
				p->nextc=NULL;
				q->nextc=p;
				q=q->nextc;
			}
		}
	}

	//����ջstack��¼DPLL��������Ϊ1�����ּ���֮�����ĵ��Ӿ�����(������DPLL)
	stack=(int*)malloc(20000*sizeof(int));
	stack[0]=0;
	instacksize=0;

	/*����Ԫ�����ִ����ɶ�������Frequency������˳������*/
	for (i=0; i<r->litsize; i++)
		Frequency[i]=i+1;
	for (i=0; i<r->litsize; i++) {
		for (j=i+1; j<=r->litsize; j++) {
			if (ValueList[Frequency[i]].Occur_times<ValueList[Frequency[j-1]].Occur_times) {
				l=Frequency[i];
				Frequency[i]=Frequency[j-1];
				Frequency[j-1]=l;
			}
		}
	}
	fclose(*fp);
	return OK;
}

/*�����Ӿ�������������
 ������(FILE**)open���ļ���ָ���ַ
       (Clause **)sentence:�Ӿ��ͷ��� Clause����ָ���ַ
       (Paradigm *)ClausHead���Ӿ�����ͷ�ڵ�ָ��ֵ
	   (int )first���������Ӿ�ĵ�һ�����ֵ�ֵ
 ����ֵ��i���Ӿ������ֵ�����
        ERROR�����Ӿ�Ϊ���Ӿ�
        OVERFLOW���ռ䲻�����*/
int CreateClause(FILE **fp,Clause **sentence,Paradigm *ClausHead,int first) {
	Clause *p,*q;
	Paraline *pline;
	int l,l1,i=0;//i��¼�����Ӿ�����������
	if (first==0) return ERROR;//������û�����ֵ��Ӿ�

	p=(Clause*)malloc(sizeof(Clause));
	if (p==NULL) return OVERFLOW;
	p->literal=first;
	p->flag=0;
	p->nextl=NULL;
	p->litline=NULL;
	q=p;
	*sentence=p;
	i++;

	l1=abs(first);
	ValueList[l1].Occur_times++;
	if (first>0) { //����DPLLʱѰ����ͬ���ֵĲ�ͬ�Ӿ��Լ�ɾ����ظ�
		p->litline=ValueList[l1].Pos.Tra_lit;
		ValueList[l1].Pos.Tra_lit=p;
		pline=(Paraline*)malloc(sizeof(Paraline));
		pline->claline=ClausHead;
		pline->next=ValueList[l1].Pos.Tra_cla;
		ValueList[l1].Pos.Tra_cla=pline;
	} else { //ͬ��
		p->litline=ValueList[l1].Neg.Tra_lit;
		ValueList[l1].Neg.Tra_lit=p;
		pline=(Paraline*)malloc(sizeof(Paraline));
		pline->claline=ClausHead;
		pline->next=ValueList[l1].Neg.Tra_cla;
		ValueList[l1].Neg.Tra_cla=pline;
	}

	fscanf(*fp, "%d",&l);
	while (l!=0) {
		// ��������������
		p=(Clause*)malloc(sizeof(Clause));
		if (p==NULL) return OVERFLOW;
		p->literal=l;
		p->flag=0;
		p->nextl=NULL;
		q->nextl=p;
		q=q->nextl;
		i++;

		// ������Ԫ����
		l1=abs(l);
		ValueList[l1].Occur_times++;
		if (l>0) {
			p->litline=ValueList[l1].Pos.Tra_lit;
			ValueList[l1].Pos.Tra_lit=p;
			pline=(Paraline*)malloc(sizeof(Paraline));
			pline->claline=ClausHead;
			pline->next=ValueList[l1].Pos.Tra_cla;
			ValueList[l1].Pos.Tra_cla=pline;
		} else {
			p->litline=ValueList[l1].Neg.Tra_lit;
			ValueList[l1].Neg.Tra_lit=p;
			pline=(Paraline*)malloc(sizeof(Paraline));
			pline->claline=ClausHead;
			pline->next=ValueList[l1].Neg.Tra_cla;
			ValueList[l1].Neg.Tra_cla=pline;
		}
		fscanf(*fp, "%d",&l);//��ȡ��һ����
	}

	return i;
}

/*����CNF��ʽ�ڽ�����*/
status DestroyParadigm(Root *r) {
	Paradigm *p;
	Clause *t;
	Paraline *pline;
	int i;
	if (r->first!=NULL) {
		p=r->first;
		//����CNF��ʽ����
		while (p!=NULL) {
			if (p->sentence!=NULL) {
				t=p->sentence;
				//���ٵ����Ӿ�����
				while (t!=NULL) {
					p->sentence=t->nextl;
					free(t);//�ͷŴ洢���ֵĵ����Ӿ�������ռ�
					t=p->sentence;
				}
			}
			r->first=p->nextc;
			free(p);//�ͷ��Ӿ�����ͷ���洢�ռ�
			p=r->first;
		}
	}
	//�ͷű�Ԫ����������Ϣ����洢�ռ�
	for (i=0; i<=r->litsize; i++) {
		for (pline=ValueList[i].Pos.Tra_cla; pline!=NULL; pline=ValueList[i].Pos.Tra_cla) {
			ValueList[i].Pos.Tra_cla=pline->next;
			free(pline);
		}
		ValueList[i].Pos.Tra_lit=NULL;
		for (pline=ValueList[i].Neg.Tra_cla; pline!=NULL; pline=ValueList[i].Neg.Tra_cla) {
			ValueList[i].Neg.Tra_cla=pline->next;
			free(pline);
		}
		ValueList[i].Neg.Tra_lit=NULL;
	}

	free(Frequency);
	free(r);
	return OK;
}

/*�ж�CNF��ʽ���Ƿ���ڵ��Ӿ�
 �������ͣ�int
 ����ֵ�����ڵ��Ӿ䣺�õ��Ӿ�������Ψһ����
       �����ڵ��Ӿ䣺0*/
int HasUnitClause(Root *r) {
	Paradigm *p;
	Clause *t;
	for(p=r->first; p!=NULL; p=p->nextc) {
		if(p->flag==0) { //�Ӿ�û�б�ɾ��
			if(p->number==1)//�Ӿ�������Ϊ1;
				for (t=p->sentence; t!=NULL; t=t->nextl) {
					if (t->flag==0)//����û�б�ɾ��
						return t->literal;
				}

		}
	}
	return 0;
}

/*�ж�CNF��ʽ���Ƿ񻹺��е��Ӿ䣨�Ż�ǰ�汾��
 �������ͣ�Clause *
 ����ֵ�����ڵ��Ӿ䣺�õ��Ӿ�������ΨһClause����ָ��
       �����ڵ��Ӿ䣺NULL*/
Clause * HasUnitClause_Before(Root *r) {
	Paradigm *p;
	Clause *t;
	for(p=r->first; p!=NULL; p=p->nextc) {
		if(p->flag==0) { //�Ӿ�û�б�ɾ��
			if(p->number==1)//�Ӿ�������Ϊ1;
				for (t=p->sentence; t!=NULL; t=t->nextl) {
					if (t->flag==EXIST)//����û�б�ɾ��
						return t;
				}

		}
	}
return NULL;
}

/*�жϲ���ָ��cָ����Ӿ�ͷ����Ӧ�Ӿ��Ƿ�Ϊ���Ӿ䣻
 ����ֵ��TRUE���ǵ��Ӿ䣻
       ERROR�����ǵ��Ӿ�*/
status isUnitClause(Paradigm *c) {
	if (c->number==1)
		return TRUE;
	else return FALSE;
}

/*������CNF��ʽ��ȡһ������*/

/*way1: ȡÿ��DPLL�����ʽ�г��ִ�����ࣨOccur_Timesֵ��󣩵�����
 �������ͣ�int
 ����ֵ����ʽ�ǿգ�ѡ�����ֵ�ֵ
       ��ʽ��û��ʣ�����֣�0*/
int FindLiteral1(Root *r) {
	int i,num=0,flag=0;
	for (i=1; i<=r->litsize; i++) {
		if (ValueList[i].IsInit==0) {
			num=i;//�ҵ���Ԫ�������һ��δ������ֵ�ı�Ԫ��num��¼�����
			flag=1;//��ʾ��Ԫ���ڴ���δ������ֵ�ı�Ԫ
			break;
		}
	}
	if (flag==1) {
		for (i++; i<=r->litsize; i++) {
			if (ValueList[i].IsInit==0) {
				if (ValueList[i].Occur_times>ValueList[num].Occur_times)
					num=i;
			}
		}
	}
	return num;//���ر�Ԫ�����������δ��ֵ��Ԫ���
}

/*way2:ȡԭ��ʽ��δ����ֵ�ı�Ԫ�г��ִ�����ࣨOccur_Times��󣩵ı�Ԫ������
 �������ͣ�int
 ����ֵ����ʽ�ǿգ�ѡ�����ֵ�ֵ
       ��ʽ��û��ʣ�����֣�0*/
int FindLiteral2(Root *r) {//frequency  �����¼���ִ����Ķ�����
	int i=0;
	for (i=0; i<r->litsize; i++) {
		if (ValueList[Frequency[i]].IsInit==0) {
			return Frequency[i];
		}
	}
	return 0;
}

/*way3:ȡ�Ӿ��е�һ��δ��ɾ����flagΪ0��������
 �������ͣ�int
 ����ֵ����ʽ�ǿգ�ѡ�����ֵ�ֵ
 ��ʽ��û��ʣ�����֣�0*/
int FindLiteral3(Root *r){
    Paradigm *p;
    Clause *c;
    for (p=r->first; p!=NULL; p=p->nextc)
        if (p->flag==0) {//�Ӿ�δ��ɾ��
            for (c=p->sentence; c!=NULL; c=c->nextl) {
                if (c->flag==0) {//����δ��ɾ��
                    return c->literal;
                }
            }
        }
    return 0;
}

/*way4: ȡ��Ԫ�����������һ��δ����ֵ��IsInit=0���ı�Ԫ��������
 �������ͣ�int
 ����ֵ����ʽ�ǿգ�ѡ�����ֵ�ֵ
       ��ʽ��û��ʣ�����֣�0*/
int FindLiteral4(Root *r){
    int i=0;
    for (i=1; i<=r->litsize; i++) {
        if (ValueList[i].IsInit==0) {
            return i;
        }
    }
    return 0;
}

/*������CNF��ʽ��ȡһ�����֣��Ż�ǰ�汾����ȡ�Ӿ��е�һ��δ��ɾ����flagΪ0��������
 �������ͣ�Clause *
 ����ֵ����ʽ�ǿգ�ѡ�����ֵĽ��ָ��
       ��ʽ��û��ʣ�����֣�NULL*/
Clause * FindLiteral_Before(Root *r) {
	Paradigm *p;
	Clause *c;
	for (p=r->first; p!=NULL; p=p->nextc) {
		if (p->flag==0) {
			for (c=p->sentence; c!=NULL; c=c->nextl) {
				if (c->flag==0) {
					return c;
				}
			}
		}
	}
	return NULL;
}

/*ɾ��������l���Ӿ�
 ����l����ֵΪ1�����֣�int��
  (�˺�����Ӧʹ��ʽ�е��Ӿ��������٣�*///paradigm��num�����
status DeleteClause(Root *r,int l) {//�ı������е�flag���Ӿ�ͷ�ڵ��num��Valuelist��ĳ�����Ŀ
	int l1;
	Paradigm *p;
	Clause *c;
	Paraline *pline;
	l1=abs(l);
	if(l>0) {
		//lΪ���������������l��Ӧ��Ԫ����������Ϣ����
		for(pline=ValueList[l1].Pos.Tra_cla; pline; pline=pline->next) {
			p=pline->claline;
			if(p->flag==EXIST) { //pָ����Ӿ�ͷ����Ӧ�Ӿ�δ��ɾ��
				p->flag=l1;//��pָ�����flagֵ���Ϊl����ʾ���Ӿ�������l��ֵΪ1����ɾ��
				r->parasize--;
				for (c=p->sentence; c!=NULL; c=c->nextl)//�����޸�pָ���Ӿ��flag���ֵ
					if (c->flag==EXIST) {
						c->flag=l1;
						ValueList[abs(c->literal)].Occur_times--;
						p->number--;
					}
			}
		}
	} else {
		//lΪ���������������l��Ӧ��Ԫ�ĸ�������Ϣ����
		for (pline=ValueList[l1].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
			p=pline->claline;
			if (p->flag==0) {
				p->flag=l1;
				r->parasize--;
				for (c=p->sentence; c!=NULL; c=c->nextl)
					if (c->flag==0) {
						c->flag=l1;
						ValueList[abs(c->literal)].Occur_times--;
						p->number--;
					}
			}
		}
	}
	return OK;
}

/*ɾ����������Ϊ-l���Ӿ�������
 ����l��(int)��ֵΪ1�����֣�
 ����ֵ��OK���ɹ�ɾ�����֣�
       FALSE����ʽ�޽�
 (�˺�����Ӧʹ��ʽ�е��Ӿ��������٣�*/
status DeleteLiteral(Root *r,int l) {
	Paradigm *p;
	Clause *c;
	Paraline *pline;
	int l1,l_op;
	l1=abs(l);
	l_op=0-l;//Ҫɾ��������
	if (l>0) {
		for (pline=ValueList[l1].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
			p=pline->claline;
			if (p->flag==EXIST) {//pָ����Ӿ�����ͷ����Ӧ���Ӿ�δ��ɾ��
				for (c=p->sentence; c!=NULL; c=c->nextl) {
					if (c->literal==l_op) {
						if (c->flag==EXIST) {//cָ���Ӿ�������������δ��ɾ��
							if (p->number==1) return FALSE;//Ҫɾ��������Ϊ��ǰ�Ӿ��ڵ�Ψһ���֣����Ӿ���ֵΪ0����ʽ�޽�
							c->flag=l1;//��Ǹý����flagֵΪl1����ʾ��l��ֵΪ1����ɾ��
							ValueList[abs(c->literal)].Occur_times--;//��Ԫ���ڸñ�Ԫ���ֵĴ�����һ
							p->number--;
						}
					}
				}
			}
		}
	} else {
		for (pline=ValueList[l1].Pos.Tra_cla; pline!=NULL; pline=pline->next) {
			p=pline->claline;
			if (p->flag==EXIST) {
				for (c=p->sentence; c!=NULL; c=c->nextl) {
					if (c->literal==l_op) {
						if (c->flag==EXIST) {
							if (p->number==1) return FALSE;
							c->flag=l1;
							ValueList[abs(c->literal)].Occur_times--;
							p->number--;
						}
					}
				}
			}
		}
	}
	return OK;
}

/*��CNF��ʽ�ڽ������ͷ���ֻ��������l�ĵ��Ӿ�����
 ����l��(int)���ӵĵ��Ӿ��ڵ�Ψһ����
 ����ֵ��OVERFLOW���ռ䲻�������
        OK���Ӿ����ӳɹ�*/
status AddClause(Root *r,int l) {
	Paradigm *p;
	Clause *c;
	Paraline *pline;
	int l1;
	p=(Paradigm*)malloc(sizeof(Paradigm));
	if (p==NULL) return OVERFLOW;
	p->nextc=r->first;
	c=(Clause*)malloc(sizeof(Clause));
	if (c==NULL) return OVERFLOW;
	c->literal=l;
	c->flag=0;
	c->nextl=NULL;
	l1=abs(l);
	if (l>0) {
		//lΪ�����֣��ڱ�Ԫ����l��Ӧ��Ԫ����������Ϣ�����ͷ�������������Ϣ
		c->litline=ValueList[l1].Pos.Tra_lit;
		ValueList[l1].Pos.Tra_lit=c;
		pline=(Paraline*)malloc(sizeof(Paraline));
		pline->claline=p;
		pline->next=ValueList[l1].Pos.Tra_cla;
		ValueList[l1].Pos.Tra_cla=pline;
	} else {
		//lΪ�����֣��ڱ�Ԫ����l��Ӧ��Ԫ�ĸ�������Ϣ�����ͷ�������������Ϣ
		c->litline=ValueList[l1].Neg.Tra_lit;
		ValueList[l1].Neg.Tra_lit=c;
		pline=(Paraline*)malloc(sizeof(Paraline));
		pline->claline=p;
		pline->next=ValueList[l1].Neg.Tra_cla;
		ValueList[l1].Neg.Tra_cla=pline;
	}
	p->sentence=c;
	p->number=1;
	p->flag=0;
	r->first=p;
	r->parasize++;
	return OK;
}

/*ɾ��CNF��ʽ�ڽ������дӱ�ͷ��ʼ��һ��ֻ��������l�ĵ��Ӿ�����
 ����l��(int)Ҫɾ���ĵ��Ӿ��Ψһ����ֵ��
 ����ֵ��OK��ɾ���ɹ�*/
status RemoveHeadClaus(Root *r,int l) {
	Paradigm *p,*q;
	Clause *c;
	Paraline *pline;
	int l1;
	l1=abs(l);
	if (l>0) {
		//lΪ�����֣�Ҫɾ���Ľ����Ϣ���ڱ�Ԫ����l��Ӧ��Ԫ����������Ϣ�����ͷ
		pline=ValueList[l1].Pos.Tra_cla;
		p=pline->claline;
		ValueList[l1].Pos.Tra_cla=pline->next;
		ValueList[l1].Pos.Tra_lit=ValueList[l1].Pos.Tra_lit->litline;
	} else {
		//lΪ�����֣�Ҫɾ���Ľ����Ϣ���ڱ�Ԫ����l��Ӧ��Ԫ�ĸ�������Ϣ�����ͷ
		pline=ValueList[l1].Neg.Tra_cla;
		p=pline->claline;
		ValueList[l1].Neg.Tra_cla=pline->next;
		ValueList[l1].Neg.Tra_lit=ValueList[l1].Neg.Tra_lit->litline;
	}
	if (r->first==p) {
		r->first=p->nextc;
	} else {
		for (q=r->first; q!=NULL; q=q->nextc) {//Ѱ�Ҵӱ�ͷ��ʼ��һ��ֻ������l�ĵ��Ӿ�
			if (q->nextc==p)
				break;
		}
		q->nextc=p->nextc;
	}
	c=p->sentence;
	free(c);
	free(p);
	free(pline);
	r->parasize--;
	return  OK;
}

/*�ָ���Ϊ����lΪ��ǰ��CNF�ڽ�����
 ������(int)����l:��ֵΪ1ʱ������
 ����ֵ��OK���ڽ�����ָ��ɹ�*/
status RecoverCNF(Root *r,int l) {
	Paradigm *p;
	Clause *c;
	Paraline *pline;
	int l1;
	l1=abs(l);
	if (l>0) {
		//lΪ�����֣��Ա�Ԫl1����������Ϣ�������������Ѱ�ұ�ɾ�����Ӿ�
		for(pline=ValueList[l1].Pos.Tra_cla; pline; pline=pline->next) {
			p=pline->claline;
			if(p->flag==l1) {
				p->flag=0;
				r->parasize++;
				for(c=p->sentence; c; c=c->nextl) {
					if(c->flag==l1) {
						c->flag=0;
						p->number++;
					}
				}
			}
		}
		//lΪ�����֣��Ա�Ԫl1�ĸ�������Ϣ�������������Ѱ�ұ�ɾ��������
		for (pline=ValueList[l1].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
			p=pline->claline;
			for (c=p->sentence; c!=NULL; c=c->nextl) {
				if (c->flag==l1) {
					c->flag=0;
					p->number++;
				}
			}
		}
	} else {
		//lΪ�����֣��Ա�Ԫl1�ĸ�������Ϣ�������������Ѱ�ұ�ɾ�����Ӿ�
		for (pline=ValueList[l1].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
			p=pline->claline;
			if (p->flag==l1) {
				p->flag=0;
				r->parasize++;
				for (c=p->sentence; c!=NULL; c=c->nextl) {
					if (c->flag==l1) {
						c->flag=0;
						p->number++;
					}
				}
			}
		}
		//lΪ�����֣��Ա�Ԫl1����������Ϣ�������������Ѱ�ұ�ɾ��������
		for (pline=ValueList[l1].Pos.Tra_cla; pline!=NULL; pline=pline->next) {
			p=pline->claline;
			for (c=p->sentence; c!=NULL; c=c->nextl) {
				if (c->flag==l1) {
					c->flag=0;
					p->number++;
				}
			}
		}
	}
	return OK;
}

/*����CNF��ʽ�ڽ�����*/
void ParadigmTrabverse(Root *r) {
	Paradigm *p;
	Clause *c;
	int l,value,flag,i=1;
	for (p=r->first; p!=NULL; p=p->nextc) {
		flag=0;
		printf("��%4d��  ",i++);
		for (c=p->sentence; c!=NULL; c=c->nextl) {
			printf("%4d  ",c->literal);
		}
		printf("\n");
	}
	printf("������ϣ�\n");
}

/*����CNF��ʽ�Ľ⼰���ʱ����Ϣ
 ������(int)solut��CNF��ʽ�Ľ⣬�н�Ϊ1���޽�Ϊ0��
      (int)time:DPLL���ʱ�䣻
 ����ֵ��ERROR���ļ���ʧ�ܣ�
        OK�����ʱ��洢�ɹ�*/
status SaveValue(ArgueValue *ValueList,int solut,int time) {
	int i=0;
	FILE *save;
	char filenamesave[1000];
	for (i=0; filename[i]!='.'; i++)
		filenamesave[i]=filename[i];
	filenamesave[i++]='.';
	filenamesave[i++]='r';
	filenamesave[i++]='e';
	filenamesave[i++]='s';
	filenamesave[i++]='\0';
	save=fopen(filenamesave, "wb");
	if (save==NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}
	if(solut==1) {
		fprintf(save,"s %d\n", solut);
		fprintf(save,"v ");
		for (i=1; i<=r->litsize; i++) {
			if (ValueList[i].IsInit==1) {
				if (ValueList[i].Value==1) {
					fprintf(save, "%-7d",i);
				} else fprintf(save, "%-7d",0-i);
				fprintf(save, " ");
			}
			if(i%10==0) fprintf(save,"\n");
		}
		fprintf(save, "\nt %d ms\n", time);
	}
	else if(solut ==0){
		fprintf(save,"s %d\n", solut);
		fprintf(save,"v ");
		fprintf(save, "\nt %d ms\n", time);
	}
	fclose(save);
	return OK;
}

/*------------------------DPLL�㷨------------------------*/




status SAT(void) {
	int op=1;//��¼ѡ��Ĳ������
	int choose;//��¼ѡ����㷨���
	int i=0,solut=0;//solut��¼CNF��ʽ�Ƿ��н⣬�н�Ϊ1���޽�Ϊ0
	clock_t start = 0,finish = 0;//��¼DPLL�������õ���ʼ����ֹʱ��
	int duration=0;//��¼SAT���ʱ��
	FILE *open;
	printf("������������cnf�ļ�����");
	scanf("%s",filename);
	open=fopen(filename, "r");
	if (open==NULL) {
		printf("�ļ��򿪴���\n");
		getchar();
		getchar();
		return ERROR;
	}
	if (CreateParadigm(&open)==ERROR) {//����CNF��ʽ���ݴ洢�ṹ
		getchar();
		getchar();
		return ERROR;
	}
    getchar(); getchar();
	op=1;
	solut=5;
	while (op) {
		system("cls");
		printf("\t\tChoose an option please.\n");
		printf("---------------------------------------------------------\n");
		printf("  1.��������   2.����CNF�ļ�   3.���������\n");
		printf("  0.�˳�\n");
		printf("---------------------------------------------------------\n");
		scanf("%d",&op);
		switch (op) {
			case 1:
				if (ValueList[1].IsInit==1) {
					printf("�ѶԹ�ʽ��⣡");
					getchar();
					getchar();
					break;
				}
				printf(" ʹ�ã�1.�Ż�1�㷨       2.�Ż�2�㷨        3.�Ż�ǰ�㷨\n");
				scanf("%d",&choose);
				switch (choose) {
					case 1:
						start=clock();
						solut=DPLL3(FindLiteral1(r),1,1);
						finish=clock();
						duration=(finish-start);
						break;
					case 2:
						start=clock();
						solut=DPLL4(FindLiteral2(r),1,1);
						finish=clock();
						duration=(finish-start);
						break;
					case 3:
						start=clock();
						solut=DPLL_Before(r, 1);
						finish=clock();
						duration=(finish-start);
						break;
					default:
						printf("�������\n");
						break;
				}
				if (solut) {
					AnswerComplete();
					for (i=1; i<=r->litsize; i++) {
						if (ValueList[i].IsInit==1) {
							if (ValueList[i].Value==1) {
								printf("%-7d",i);
							} else printf("%-7d",0-i);
						}
						if(i%10==0) printf("\n");
					}
				} else  printf("�ù�ʽ�޽⣡\n");
				printf("\n����ʱ��Ϊ��%d ms\n",(finish-start));
				getchar();
				getchar();
				break;
			case 2:
				if (solut==5) {
					printf("�������CNF��ʽ��\n");
					getchar();
					getchar();
					break;
				}
				ParadigmTrabverse(r);
				if (AnswerCheck(solut)==OK) {
					printf("��ʽ������ɣ�\n");
				} else {
					printf("ERROR��\n");
				}
				getchar();
				getchar();
				break;
			case 3:
				if (solut==5) {
					printf("�������CNF��ʽ��\n");
					getchar();
					getchar();
					break;
				}
				if (SaveValue(ValueList,solut,duration))
					printf("�ɹ������ļ���");
				getchar();
				getchar();
				break;
			case 0:
				break;
			default:
				printf("�������");
				getchar();
				getchar();
				break;
		}
	}
	DestroyParadigm(r);//����CNF��ʽ���ݴ洢�ṹ
	return OK;
}


/*���õ�һ�ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
 ������(int)num���ôν��뺯����ֵ��Ϊ1�ı�Ԫ��ţ�
      (int)op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2��
      (int)timesofDPLL����һ�ν���DPLL����Ϊ1�����Ϊ2��
 ����ֵ��FALSE����ʽ�޽⣻
       TRUE����ʽ�н�*/
status DPLL1(int num,int op,int timesofDPLL) {
	int l,i;
	Clause *c;
	Paradigm *p=r->first;
	Paraline *pline;
	i=0;
	/*���Ӿ����*/
	if (timesofDPLL==1) {
		l=0;//��һ�ν���DPLL�����������뵥�Ӿ�ѭ��
	} else {
		if (op==1)
			l=num;
		else l=0;
	}

	while (l!=0) {
		stack[++instacksize]=l;
		DeleteClause(r, l);
		if (DeleteLiteral(r, l)==FALSE) { //ɾ���Ӿ�������l�ķ�����
			//��̽���ʽ�޽⣬�ָ��ݹ���뱾�κ���ǰ���ڽ�����״̬
			for (; stack[instacksize]!=num; instacksize--) {
				RecoverCNF(r, stack[instacksize]);
				ValueList[abs(stack[instacksize])].IsInit=0;
			}
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
			instacksize--;
			return FALSE;
		}
		if (l>0) {
			ValueList[l].Value=1;//��ֵ����Ϊ1
			ValueList[l].IsInit=1;//����Ѿ���ֵ
		} else {
			ValueList[0-l].Value=0;//��ֵ����Ϊ0
			ValueList[0-l].IsInit=1;//����Ѿ���ֵ
		}
		if (FindLiteral1(r)==0)
			return TRUE;//��ʽ��û��ʣ���Ϊ����ֵ�ı�Ԫ�����ɹ�
		l=HasUnitClause(r);//Ѱ�ҹ�ʽ�еĵ��Ӿ䲢��������ֵ����l
	}
	if (op==1) {
		l=FindLiteral1(r);
	} else {
		do {
			l=rand()%729+1;
			for (pline=ValueList[l].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
				p=pline->claline;
				if (p->number!=0)
					break;
			}
		} while (ValueList[l].IsInit==1);
		for (c=p->sentence; c!=NULL; c=c->nextl) {
			if (c->flag==0) {
				l=c->literal;
				break;
			}
		}
	}
	if (l==0)
		return TRUE;//��ʽ��û��ʣ���Ϊ����ֵ�ı�Ԫ�����ɹ�
	if (DPLL1(l, 1,2)) {
		return TRUE;
	}

	l=0-l;//��lΪ��ֵΪ1ʱ̽�����ʧ�ܣ���l��ֵֻ��Ϊ0�����䷴������ֵΪ1
	if (DPLL1(l,1,2)==FALSE) {
		if (instacksize==0)
			return FALSE; //l���Ǳ���DPLL1������һ������̽��ı�Ԫ��̽��ʧ��
		/*�ָ��ݹ���뱾�κ���ǰ���ڽ�����״̬*/
		for (; stack[instacksize]!=num; instacksize--) {
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
		}
		RecoverCNF(r, stack[instacksize]);
		ValueList[abs(stack[instacksize])].IsInit=0;
		instacksize--;
		return FALSE;
	} else
		return TRUE;
}

/*���õڶ��ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
 ������(int)num���ôν��뺯����ֵ��Ϊ1�ı�Ԫ��ţ�
      (int)op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2��
      (int)timesofDPLL����һ�ν���DPLL����Ϊ1�����Ϊ2��
 ����ֵ��FALSE����ʽ�޽⣻
       TRUE����ʽ�н�*/
status DPLL2(int num,int op,int timesofDPLL) {
	int l,i;
	Clause *c;
	Paradigm *p=r->first;
	Paraline *pline;
	i=0;
	/*���Ӿ����*/
	if (timesofDPLL==1) {
		l=0;
	} else {
		if (op==1)
			l=num;
		else l=0;
	}
	while (l!=0) {
		stack[++instacksize]=l;//��¼ѭ����������ֵ��Ϊ1������
		DeleteClause(r, l);//ɾ��������l���Ӿ�
		if (DeleteLiteral(r, l)==FALSE) { //ɾ���Ӿ�������l�ķ�����
			//��̽���ʽ�޽⣬�ָ��ݹ���뱾�κ���ǰ���ڽ�����״̬
			for (; stack[instacksize]!=num; instacksize--) {
				RecoverCNF(r, stack[instacksize]);
				ValueList[abs(stack[instacksize])].IsInit=0;
			}
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
			instacksize--;
			return FALSE;
		}
		if (l>0) {
			ValueList[l].Value=1;
			ValueList[l].IsInit=1;
		} else {
			ValueList[0-l].Value=0;
			ValueList[0-l].IsInit=1;
		}
		if (FindLiteral2(r)==0)
			return TRUE;
		l=HasUnitClause(r);
	}
	if (op==1) {
		l=FindLiteral2(r);
	} else {
		do {
			l=rand()%729+1;
			for (pline=ValueList[l].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
				p=pline->claline;
				if (p->number!=0)
					break;
			}
		} while (ValueList[l].IsInit==1);
		for (c=p->sentence; c!=NULL; c=c->nextl) {
			if (c->flag==0) {
				l=c->literal;
				break;
			}
		}
	}
	if (l==0)
		return TRUE;
	if (DPLL2(l, 1,2)) {
		return TRUE;
	}
	l=0-l;//��lΪ��ֵΪ1ʱ̽�����ʧ�ܣ���l��ֵֻ��Ϊ0�����䷴������ֵΪ1
	if (DPLL2(l,1,2)==FALSE) {
		if (instacksize==0)
			return FALSE;
		for (; stack[instacksize]!=num; instacksize--) {
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
		}
		RecoverCNF(r, stack[instacksize]);
		ValueList[abs(stack[instacksize])].IsInit=0;
		instacksize--;
		return FALSE;
	} else
		return TRUE;
}

/*���õ����ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
 ������(int)num���ôν��뺯����ֵ��Ϊ1�ı�Ԫ��ţ�
      (int)op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2��
      (int)timesofDPLL����һ�ν���DPLL����Ϊ1�����Ϊ2��
 ����ֵ��FALSE����ʽ�޽⣻
       TRUE����ʽ�н�*/
status DPLL3(int num,int op,int timesofDPLL) {
	int l,i;
	Clause *c;
	Paradigm *p=r->first;
	Paraline *pline;
	i=0;
	/*���Ӿ����*/
	if (timesofDPLL==1) {
		l=0;
	} else {
		if (op==1)
			l=num;
		else l=0;
	}
	while (l!=0) {
		stack[++instacksize]=l;//��¼ѭ����������ֵ��Ϊ1������
		DeleteClause(r, l);//ɾ��������l���Ӿ�
		if (DeleteLiteral(r, l)==FALSE) { //ɾ���Ӿ�������l�ķ�����
			//��̽���ʽ�޽⣬�ָ��ݹ���뱾�κ���ǰ���ڽ�����״̬
			for (; stack[instacksize]!=num; instacksize--) {
				RecoverCNF(r, stack[instacksize]);
				ValueList[abs(stack[instacksize])].IsInit=0;
			}
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
			instacksize--;
			return FALSE;
		}
		if (l>0) {
			ValueList[l].Value=1;
			ValueList[l].IsInit=1;
		} else {
			ValueList[0-l].Value=0;
			ValueList[0-l].IsInit=1;
		}
		if (FindLiteral3(r)==0)
			return TRUE;
		l=HasUnitClause(r);
	}
	if (op==1) {
		l=FindLiteral3(r);
	} else {
		do {
			l=rand()%729+1;
			for (pline=ValueList[l].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
				p=pline->claline;
				if (p->number!=0)
					break;
			}
		} while (ValueList[l].IsInit==1);
		for (c=p->sentence; c!=NULL; c=c->nextl) {
			if (c->flag==0) {
				l=c->literal;
				break;
			}
		}
	}
	if (l==0)
		return TRUE;
	if (DPLL3(l, 1,2)) {
		return TRUE;
	}
	l=0-l;//��lΪ��ֵΪ1ʱ̽�����ʧ�ܣ���l��ֵֻ��Ϊ0�����䷴������ֵΪ1
	if (DPLL3(l,1,2)==FALSE) {
		if (instacksize==0)
			return FALSE;
		for (; stack[instacksize]!=num; instacksize--) {
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
		}
		RecoverCNF(r, stack[instacksize]);
		ValueList[abs(stack[instacksize])].IsInit=0;
		instacksize--;
		return FALSE;
	} else
		return TRUE;
}

/*���õ����ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
 ������(int)num���ôν��뺯����ֵ��Ϊ1�ı�Ԫ��ţ�
      (int)op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2��
      (int)timesofDPLL����һ�ν���DPLL����Ϊ1�����Ϊ2��
 ����ֵ��FALSE����ʽ�޽⣻
       TRUE����ʽ�н�*/
status DPLL4(int num,int op,int timesofDPLL) {
	int l,i;
	Clause *c;
	Paradigm *p=r->first;
	Paraline *pline;
	i=0;
	/*���Ӿ����*/
	if (timesofDPLL==1) {
		l=0;
	} else {
		if (op==1)
			l=num;
		else l=0;
	}
	while (l!=0) {
		stack[++instacksize]=l;//��¼ѭ����������ֵ��Ϊ1������
		DeleteClause(r, l);//ɾ��������l���Ӿ�
		if (DeleteLiteral(r, l)==FALSE) { //ɾ���Ӿ�������l�ķ�����
			//��̽���ʽ�޽⣬�ָ��ݹ���뱾�κ���ǰ���ڽ�����״̬
			for (; stack[instacksize]!=num; instacksize--) {
				RecoverCNF(r, stack[instacksize]);
				ValueList[abs(stack[instacksize])].IsInit=0;
			}
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
			instacksize--;
			return FALSE;
		}
		if (l>0) {
			ValueList[l].Value=1;
			ValueList[l].IsInit=1;
		} else {
			ValueList[0-l].Value=0;
			ValueList[0-l].IsInit=1;
		}
		if (FindLiteral4(r)==0)
			return TRUE;
		l=HasUnitClause(r);
	}
	if (op==1) {
		l=FindLiteral4(r);
	} else {
		do {
			l=rand()%729+1;
			for (pline=ValueList[l].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
				p=pline->claline;
				if (p->number!=0)
					break;
			}
		} while (ValueList[l].IsInit==1);
		for (c=p->sentence; c!=NULL; c=c->nextl) {
			if (c->flag==0) {
				l=c->literal;
				break;
			}
		}
	}
	if (l==0)
		return TRUE;
	if (DPLL4(l, 1,2)) {
		return TRUE;
	}
	l=0-l;//��lΪ��ֵΪ1ʱ̽�����ʧ�ܣ���l��ֵֻ��Ϊ0�����䷴������ֵΪ1
	if (DPLL4(l,1,2)==FALSE) {
		if (instacksize==0)
			return FALSE;
		for (; stack[instacksize]!=num; instacksize--) {
			RecoverCNF(r, stack[instacksize]);
			ValueList[abs(stack[instacksize])].IsInit=0;
		}
		RecoverCNF(r, stack[instacksize]);
		ValueList[abs(stack[instacksize])].IsInit=0;
		instacksize--;
		return FALSE;
	} else
		return TRUE;
}


/*�Ż�ǰ�汾DPLL����
 ������(int)op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2��
 ����ֵ��FALSE����ʽ�޽⣻
       TRUE����ʽ�н�*/
status DPLL_Before(Root *r,int op) {
	int l,i;
	Clause *c;
	Paradigm *p=r->first;
	Paraline *pline;
	int *Memory;

	/*����Memoryջ����ÿ��DPLLѭ����������ֵ��Ϊ1������*/
	Memory=(int*)malloc((r->litsize)*sizeof(int));
	for (i=0; i<r->litsize; i++)
		Memory[i]=0;
	i=0;
	/*���Ӿ����*/
	if (op==1) {
		c=HasUnitClause_Before(r);//˳����ҵ��Ӿ�
	} else {
		c=NULL;
	}
	while (c!=NULL) {
		l=c->literal;
		Memory[i++]=l;
		DeleteClause(r, l);//ɾ��������l���Ӿ�
		if (DeleteLiteral(r, l)==FALSE) { //ɾ���־�������l�ĸ�����
			//��̽���ʽ�޽⣬�ָ��ݹ���뱾�κ���ǰ���ڽ�����״̬
			if (Memory[0]!=0)
				RemoveHeadClaus(r, Memory[0]);//ɾ�����ӵĵ��Ӿ�
			for (i=0; Memory[i]!=0; i++) {
				RecoverCNF(r, Memory[i]);
				ValueList[abs(Memory[i])].IsInit=0;
			}
			return FALSE;
		}

		/*�ڱ�Ԫ���м�¼��Ԫl����ֵ*/
		if (l>0) {
			ValueList[l].Value=1;
			ValueList[l].IsInit=1;
		} else {
			ValueList[0-l].Value=0;
			ValueList[0-l].IsInit=1;
		}
		if (FindLiteral_Before(r)==NULL)
			return TRUE;//��ʽ��û��ʣ���Ϊ����ֵ�ı�Ԫ�����ɹ�
		c=HasUnitClause_Before(r);//Ѱ�ҹ�ʽ�еĵ��Ӿ䲢��������ֵ����l
	}

	Memory[i]=0;
	if (op==1) {
		c=FindLiteral_Before(r);
	} else {
		do {
			l=rand()%729+1;
			for (pline=ValueList[l].Neg.Tra_cla; pline!=NULL; pline=pline->next) {
				p=pline->claline;
				if (p->number!=0)
					break;
			}
		} while (ValueList[l].IsInit==1);
		for (c=p->sentence; c!=NULL; c=c->nextl) {
			if (c->flag==0)
				break;
		}
	}
	if (c==NULL)
		return TRUE;
	l=c->literal;
	AddClause(r, l);//������l��ֵΪ1����CNF��ʽ�ڽ������ͷ���Ӻ�����l�ĵ��Ӿ�����
	if (DPLL_Before(r, 1)) {
		return TRUE;
	}

	l=0-l;//��lΪ��ֵΪ1ʱ̽�����ʧ�ܣ���l��ֵֻ��Ϊ0�����䷴������ֵΪ1
	AddClause(r, l);
	if (DPLL_Before(r,1)==FALSE) {

		if (Memory[0]!=0)
			RemoveHeadClaus(r, Memory[0]);
		for (i=0; Memory[i]!=0; i++) {
			RecoverCNF(r, Memory[i]);
			ValueList[abs(Memory[i])].IsInit=0;
		}
		return FALSE;
	} else
		return TRUE;
}

/*��δ����ֵ�ı�Ԫ���������л�����ȥ������ֵ1*/
void AnswerComplete(void) {
	int i;
	for (i=1; i<=r->litsize; i++) {
		if (ValueList[i].IsInit==0) {
			ValueList[i].IsInit=1;
			ValueList[i].Value=1;
		}
	}
}

/*���SAT�������ȷ��
 ������(int)solut����ʽ��������н�Ϊ1���޽�Ϊ0��
 ����ֵ��TRUE�������ȷ��
        FALSE��������*/
status AnswerCheck(int solut) {
	Paradigm *p;
	Clause *c;
	int flag,l,value,i=0;
	if (solut==1) {//��ʽ�н�
		for (p=r->first; p!=NULL; p=p->nextc) {
			i++;
			flag=0;
			for (c=p->sentence; c!=NULL; c=c->nextl) {
				l=abs(c->literal);
				if (c->literal>0) {
					value=ValueList[l].Value;
				} else value=1-ValueList[l].Value;
				if (value==1) {
					flag=1;//�Ӿ�����������ֵΪ1���Ӿ���ֵΪ1
					break;
				}
			}
			if (flag==0) {

				return FALSE;//�Ӿ�������ֵΪ1�����֣��Ӿ���ֵΪ0��������
			}
		}
		return TRUE;
	} else {//��ʽ�޽�
		for (p=r->first; p!=NULL; p=p->nextc) {
			flag=0;
			for (c=p->sentence; c!=NULL; c=c->nextl) {
				l=abs(c->literal);
				if (c->literal>0) {
					value=ValueList[l].Value;
				} else value=1-ValueList[l].Value;
				if (value==1) flag=1;//�Ӿ�����������ֵΪ1���Ӿ���ֵΪ1
			}
			if (flag==0) return TRUE;//�Ӿ���ֵΪ0�������ȷ
		}
		return FALSE;
	}
}


/*------------------------SuDoKu------------------------*/

status Sudoku(void) {
	int op=1,difficulty=0,i,j,flag=1,choose=1;//op��choose��¼�û�ѡ��Ĳ�����difficulty��¼�û�ѡ������������Ѷ�
	int solut=0;
	char c;
	int x[3];
	srand((unsigned)time(NULL));
	while (op) {
		system("cls");
		printf("\t\tChoose an option please.\n");
		printf("---------------------------------------------------------\n");
		printf("  1.������Ϸ         2.���һ�������ļ�         0.�˳�\n");
		printf("---------------------------------------------------------\n");
		scanf("%d",&op);
		switch (op) {
			case 1:
				solut=CreateSudoku();//���ɻ����������̶�Ӧ��Ԫ��ֵ��
				if (solut) {
					CNFSudokuTableTransform();//�����ı�Ԫ��ֵ��ת��Ϊ��ά������������
					printf("��ѡ�������Ѷȣ�\n1.easy\t\t2.medium\t\t3.Hard\n");
					scanf("%d",&difficulty);
					//SudokuTablePrint();//�����þ䣬��������̴�
					printf("Loading...");
					/*�����û�ѡ�����������Ѷȶ����������������Ӧ�����Ķ���������*/
					switch (difficulty) {
						case 1:
							DigHole(30);
							break;
						case 2:
							DigHole(50);
							break;
						case 3:
							DigHole(64);
							break;
						default:
							printf("�������\n");
							break;
					}
					choose=1;
					while (choose) {
						flag=1;//flag����û�������ȷ�����ȷΪ1������Ϊ0
						printf("��������ֵ����˳������������Ĵ𰸣�������436����������е����еĿո�������6����ÿ����һ�����ûس����Ͽ���������ɺ�������ո񲢰��س�������\n");
						getchar();
						c=getchar();
						while (c!='\n') {
							i=0;
							while (c!=' ') {
								x[i++]=c-'0';
								c=getchar();
							}
							users_sudoku[x[0]][x[1]]=x[2];//��¼�û������
							c=getchar();
						}
						for (i=0; i<9; i++) {
							for (j=0; j<9; j++)
								//�Ա��û����������𰸣��ж��������ȷ��
								if (sudoku_table[i][j]!=users_sudoku[i][j]) {
									printf("�𰸴���\n");
									flag=0;
									break;
								}
							if (flag==0) {
								break;
							}
						}
						if (flag) {
							printf("��ϲ������ȷ��\n");
							SudokuTablePrint();//�����������
							choose=0;
						} else {
							printf("�Ƿ�鿴�𰸣�0.��  1.��\n");
							scanf("%d",&choose);
							switch (choose) {
								case 1:
									break;
								case 0:
									SudokuTablePrint();
									break;
								default:
									printf("�������\n");
									break;
							}
						}
					}
				} else  printf("����ʧ�ܣ�\n");
				getchar();
				getchar();
				break;
			case 2:
				SolveSudoku();
				getchar();
				getchar();
				break;
			case 0:
				break;
			default:
				printf("�������");
				getchar();
				getchar();
				break;
		}
	}

	return OK;
}

/*������������ת��ΪSAT������cnf�ļ�*/
FILE * CreateSudokuFile(void) {
	int x,y,z,i,j,k,l;//x�����������У�y�����������У�zȡ1��9�ֱ����ø�����������ֵΪ1��9����һֵ
	FILE *fp;
	fp=fopen("SudokuTableBase.cnf", "wb");
	if (fp==NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return NULL;
	}
	fprintf(fp, "p cnf 729 10287\n");//����729����Ԫ��9*9�������ո�ÿ�����Ӧ9����Ԫ������1��9��ĳһֵ���Ӧ��ԪΪ�棬����Ϊ��
	for (x=0; x<9; x++) {
		for (y=0; y<9; y++)
			for (z=1; z<9; z++)
				for (i=z+1; i<=9; i++)
					fprintf(fp, "%d %d 0\n",-(81*x+9*y+z),-(81*x+9*y+i));
	}                        //ÿ��cellֻ��ȡ1��9��һ��ֵ
	for (x=0; x<9; x++) {
		for (z=1; z<=9; z++)
			for (y=0; y<8; y++)
				for (i=y+1; i<=8; i++)
					fprintf(fp, "%d %d 0\n",-(81*x+9*y+z),-(81*x+9*i+z));
	}                        //ÿ��1��9ֻ�ܸ�����һ��
	for (y=0; y<9; y++) {
		for (z=1; z<=9; z++)
			for (x=0; x<8; x++)
				for (i=x+1; i<=8; i++)
					fprintf(fp, "%d %d 0\n",-(81*x+9*y+z),-(81*i+9*y+z));
	}                        //ÿ��1��9ֻ�ܸ�����һ��
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++)
			for (z=1; z<=9; z++)
				for (x=0; x<3; x++)
					for (y=0; y<3; y++)
						for (k=x+1; k<3; k++)
							for (l=0; l<3; l++)
								if (y!=l)
									fprintf(fp, "%d %d 0\n",-(81*(3*i+x)+9*(3*j+y)+z),-(81*(3*i+k)+9*(3*j+l)+z));
	}                        //ÿ��region��1��9ֻ�ܸ�����һ��
	for (x=0; x<9; x++) {
		for (y=0; y<9; y++) {
			for (z=1; z<=9; z++)
				fprintf(fp, "%d ",81*x+9*y+z);
			fprintf(fp, "0\n");
		}
	}                        //ÿ��cell����ȡ1��9�е�һ��ֵ
	fclose(fp);
	fp=fopen("SudokuTableBase.cnf", "r");
	return fp;
}

/*�����µ����9*9��������*/
status CreateSudoku(void) {
	int x,y,z,i,d;
	int order[82],randnum;
	FILE *fp;
	for (x=0; x<9; x++)
		for (y=0; y<9; y++)
			sudoku_table[x][y]=0;
	do {
		fp=CreateSudokuFile();
		if (fp==NULL) {
			printf("�ļ���ʧ�ܣ�\n");
			return ERROR;
		}
		CreateParadigm(&fp);//���������ļ���ӦCNF��ʽ�ڽ�����

		/*��1��81�����ֽ����������*/
		for (i=0; i<=81; i++)
			order[i]=i;
		for (i=81; i>2; i--) {
			randnum=rand()%i+1;
			if (i!=randnum) {
				d=order[i];
				order[i]=order[randnum];
				order[randnum]=d;
			}
		}

		for (i=1; i<=10; i++) {                 //�����������ѡ10�������������1~9
			x=(order[i]-1)/9;//˳��Ϊi��cell�����̵�����
			y=(order[i]-1)%9;//��cell�����̵�����
			z=rand()%9+1;//1��9�������

			/*���Ѿ�ȷ����cell��ֵ�����Ԫ��ֵ����*/
			for (d=1; d<=9; d++) {
				ValueList[81*x+9*y+d].IsInit=1;
				if (d==z) {
					ValueList[81*x+9*y+d].Value=1;
					DeleteClause(r, (81*x+9*y+d));
					DeleteLiteral(r, (81*x+9*y+d));
				} else {
					ValueList[81*x+9*y+d].Value=0;
					DeleteClause(r, -(81*x+9*y+d));
					DeleteLiteral(r, -(81*x+9*y+d));
				}
			}

		}
	} while (DPLL2(FindLiteral2(r),2,1)==FALSE);//����SAT�������⣬ֱ���õ���������
	return OK;
}

/*�������cnf�ļ���ת��Ϊ����������ʽ���*/
status SolveSudoku(void) {
	FILE *fp,*open;
	char filename[100];
	char read;
	int i,j,k,x,y,z,d,cham;
	int exitnum[10];
	int table[82];
	printf("������洢�������ļ�����");
	scanf("%s",filename);
	fp=fopen(filename, "r");
	if (fp==NULL) {
		printf("�ļ��򿪴���\n");
		return ERROR;
	}

	/*��ȡ�����ļ��е�����������*/
	for (i=0; i<9; i++) {
		j=0;
		fscanf(fp, "%c",&read);
		while (read!='\n') {
			if (read=='.') {
				sudoku_table[i][j]=0;
			} else if (read<='9'&&read>='1') {
				sudoku_table[i][j]=(int)(read-'0');
			} else j=-1;
			j++;
			fscanf(fp, "%c",&read);
		}
	}
	fclose(fp);
	open=SetCNFfile();
	if (open==NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}
	CreateParadigm(&open);
	i=0;
	DPLL_Before(r,1);//��������ļ�
	for (k=1; k<=r->litsize; k++) {
		if (ValueList[k].Value==1)
			table[i++]=(k%9==0)?9:k%9;
	}//����Ԫ����ֵ��Ϣת��Ϊ��ά����Ϣ*/
	k=0;
	for(i=0; i<9; i++)
		for(j=0; j<9; j++)
			sudoku_table[i][j]=table[k++];
	printf("�����Ľ�Ϊ��\n");
	SudokuTablePrint();//�����������
	return OK;
}
//������txt�ļ�ת��Ϊcnf
FILE * SetCNFfile(void) {
	int i,j,x,y,z,k,l;
	printf("����Ҫת��ΪCNF�ļ����ļ�����\n");
	scanf("%s", filename);
	FILE *fp;
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return NULL;
	}
	int f_sum = 0;//��ʽ��������
	for (i = 0; i < 9; i++) {
		for(j=0; j<9; j++)
			if (sudoku_table[i][j] != 0) f_sum++;
	}
	fprintf(fp,"p cnf 729 %d\n", f_sum + 10287);

	for (i = 0; i < 9; i++) {
		for(j=0; j<9; j++)
			if (sudoku_table[i][j] != 0)
				fprintf(fp, "%d 0\n", 81*i+9*j+sudoku_table[i][j]);
	}

	for (x=0; x<9; x++) {
		for (y=0; y<9; y++)
			for (z=1; z<9; z++)
				for (i=z+1; i<=9; i++)
					fprintf(fp, "%d %d 0\n",-(81*x+9*y+z),-(81*x+9*y+i));
	}                        //ÿ��cellֻ��ȡ1��9��һ��ֵ
	for (x=0; x<9; x++) {
		for (z=1; z<=9; z++)
			for (y=0; y<8; y++)
				for (i=y+1; i<=8; i++)
					fprintf(fp, "%d %d 0\n",-(81*x+9*y+z),-(81*x+9*i+z));
	}                        //ÿ��1��9ֻ�ܸ�����һ��
	for (y=0; y<9; y++) {
		for (z=1; z<=9; z++)
			for (x=0; x<8; x++)
				for (i=x+1; i<=8; i++)
					fprintf(fp, "%d %d 0\n",-(81*x+9*y+z),-(81*i+9*y+z));
	}                        //ÿ��1��9ֻ�ܸ�����һ��
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++)
			for (z=1; z<=9; z++)
				for (x=0; x<3; x++)
					for (y=0; y<3; y++)
						for (k=x+1; k<3; k++)
							for (l=0; l<3; l++)
								if (y!=l)
									fprintf(fp, "%d %d 0\n",-(81*(3*i+x)+9*(3*j+y)+z),-(81*(3*i+k)+9*(3*j+l)+z));
	}                        //ÿ��region��1��9ֻ�ܸ�����һ��
	for (x=0; x<9; x++) {
		for (y=0; y<9; y++) {
			for (z=1; z<=9; z++)
				fprintf(fp, "%d ",81*x+9*y+z);
			fprintf(fp, "0\n");
		}
	}                        //ÿ��cell����ȡ1��9�е�һ��ֵ
	fclose(fp);
	fp=fopen(filename, "r");
	return fp;
}

/*����֪����������num����������Ψһ�����������
������(int)num���ڶ�����Ŀ*/
status DigHole(int num) {
	int x,y,z,i,j,d;
	int rank,origin,result=0,circle;
	int table_diged[9][9];
	int diged_cells[82];
	FILE *fp;

	for (i=0; i<=81; i++)
		diged_cells[i]=0;
	fp=CreateSudokuFile();
	if (fp==NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	}
	CreateParadigm(&fp);

	/*��������Ϣ�����Ӧ��Ԫ��ֵ���У�����CNF��ʽ�ڽ���������Ӧ����*/
	for (i=0; i<9; i++)
		for (j=0; j<9; j++) {
			d=sudoku_table[i][j];
			if (d!=0)
				for (z=1; z<=9; z++) {
					ValueList[81*i+9*j+z].IsInit=1;
					if (d==z)
						ValueList[81*i+9*j+z].Value=1;
					else
						ValueList[81*i+9*j+z].Value=0;
				}
		}
	for (i=1; i<=r->litsize; i++) {
		if (ValueList[i].Value==1) {
			DeleteClause(r, i);
			DeleteLiteral(r, i);
		} else {
			DeleteClause(r, 0-i);
			DeleteLiteral(r, 0-i);
		}
	}

	/*����sudoku_table��ֵ������table_diged*/
	for (i=0; i<9; i++)
		for (j=0; j<9; j++)
			table_diged[i][j]=sudoku_table[i][j];

	/*�ڵ�һ����*/
	rank=rand()%81+1;
	diged_cells[rank]=1;
	x=(rank-1)/9;//˳��Ϊrank��cell�����̵�����
	y=(rank-1)%9;//��cell�����̵�����
	origin=sudoku_table[x][y];
	table_diged[x][y]=0;//��¼�ڶ���λ��

	/*��CNF��ʽ�ڽ�����ָ���δ֪��һ����λ�õ�����ֵ�������Ӧ��Ԫ��ֵʱ��״̬*/
	for (z=1; z<=9; z++) {
		if (z==origin)
			RecoverCNF(r, 81*x+9*y+z);
		else RecoverCNF(r, -(81*x+9*y+z));
		ValueList[81*x+9*y+z].IsInit=0;
		ValueList[81*x+9*y+z].Value=0;
	}
	DestroyParadigm(r);

	for (circle=1; circle<num; circle++) {
		/*ÿ��ѭ�����½�����ǰ������Ϣ��ӦCNF��ʽ�ڽ�������Ԫ���Ա�Ԫ��������Ӧ��Ϣ*/
		fp=CreateSudokuFile();
		if (fp==NULL) {
			printf("�ļ���ʧ�ܣ�\n");
			return ERROR;
		}
		CreateParadigm(&fp);
		for (i=0; i<9; i++)
			for (j=0; j<9; j++) {
				d=table_diged[i][j];
				if (d!=0)
					for (z=1; z<=9; z++) {
						ValueList[81*i+9*j+z].IsInit=1;
						if (d==z)
							ValueList[81*i+9*j+z].Value=1;
						else
							ValueList[81*i+9*j+z].Value=0;
					}
			}
		for (i=1; i<=r->litsize; i++) {
			if (ValueList[i].Value==1) {
				DeleteClause(r, i);
				DeleteLiteral(r, i);
			} else {
				DeleteClause(r, 0-i);
				DeleteLiteral(r, 0-i);
			}
		}

		/*����һ����*/
		rank=rand()%81+1;
		x=(rank-1)/9;//˳��Ϊrank��cell�����̵�����
		y=(rank-1)%9;//��cell�����̵�����
		if (diged_cells[rank]==1) {
			circle--;//�ö��Ѿ����ڹ�
			continue;
		}
		diged_cells[rank]=1;//��¼�ڶ���λ��
		origin=sudoku_table[x][y];
		table_diged[x][y]=0;

		/*���ڶ�λ�������ԭ������ֵ���8����ֵ��������SAT�����������⣬���н���˵���ڶ���ⲻΨһ���ö�������*/
		for (z=1; z<=9; z++) {
			result=0;
			if (z!=origin) {
				/*����CNF��ʽ����*/
				for (d=1; d<=9; d++) {
					ValueList[81*x+9*y+d].IsInit=1;
					if (d==z) {
						ValueList[81*x+9*y+d].Value=1;
						DeleteClause(r, 81*x+9*y+d);
						DeleteLiteral(r, 81*x+9*y+d);
					} else {
						ValueList[81*x+9*y+d].Value=0;
						DeleteClause(r, -(81*x+9*y+d));
						DeleteLiteral(r, -(81*x+9*y+d));
					}
				}
				DPLL1(FindLiteral1(r), 1,1);
				SudokuComplete();
				result=AnswerCheck(1);
				if (result==1) {
					table_diged[x][y]=origin;
					break;
				} else {
					for (d=1; d<=9; d++) {
						if (d==z)
							RecoverCNF(r, 81*x+9*y+d);
						else RecoverCNF(r, -(81*x+9*y+d));
						ValueList[81*x+9*y+d].IsInit=0;
						ValueList[81*x+9*y+d].Value=0;
					}
				}
			}
		}
		if (result==1) {
			circle--;//����ѭ���ڶ����ɹ�
			continue;
		}
		DestroyParadigm(r);
	}

	printf("Complete!\n");
	/*������ɵ�������*/
	printf("   0   1   2   3   4   5   6   7   8  \n");
	printf(" +---+---+---+---+---+---+---+---+---+\n");
	for (i=0; i<9; i++) {
		printf("%d",i);
		for (j=0; j<9; j++) {
			if (j==0||j==3||j==6) {
				printf("|");
			} else printf(" ");
			if (table_diged[i][j]!=0)
				printf(" %d ",table_diged[i][j]);
			else
				printf("   ");
		}
		printf("|\n");
		if (i==2||i==5||i==8) {
			printf(" +---+---+---+---+---+---+---+---+---+\n");
		} else printf(" |           |           |           |\n");
	}
	for (i=0; i<9; i++)
		for (j=0; j<9; j++)
			users_sudoku[i][j]=table_diged[i][j];/*����table_diged��ֵ������users_sudoku*/
	return OK;
}

/*������������*/
status SudokuComplete(void) {
	int i,j,z,y,x,d;
	int complete_table[9][9];
	for (i=0; i<9; i++)
		for (j=0; j<9; j++)
			complete_table[i][j]=0;
	for (i=0; i<9; i++)
		for (j=0; j<9; j++)
			for (z=1; z<=9; z++)
				if (ValueList[81*i+9*j+z].Value==1)
					complete_table[i][j]=z;

	for (i=0; i<9; i++) {
		for (j=0; j<9; j++) {
			if (complete_table[i][j]==0) {
				complete_table[i][j]=1;
				for (d=0; d<9; d++)
					for (y=0; y<9; y++)//����ѭ��ɨ��9�鱣֤����һ��û���ظ�
						if (y!=j)
							if (complete_table[i][j]==complete_table[i][y])
								complete_table[i][j]++;
				for (d=0; d<9; d++)
					for (x=0; x<9; x++)//����ѭ��ɨ��9�鱣֤����һ��û���ظ�
						if (x!=i)
							if (complete_table[x][j]==complete_table[i][j])
								complete_table[i][j]++;
			}
		}
	}
	for (i=0; i<9; i++)
		for (j=0; j<9; j++) {
			d=complete_table[i][j];
			if (d!=0)
				for (z=1; z<=9; z++) {
					ValueList[81*i+9*j+z].IsInit=1;
					if (d==z)
						ValueList[81*i+9*j+z].Value=1;
					else
						ValueList[81*i+9*j+z].Value=0;
				}
		}
	return OK;
}

/*������ӦSAT��Ԫ��ת��Ϊ��ά����*/
status CNFSudokuTableTransform(void) {
	int i,j,z;
	for (i=0; i<9; i++) {
		for (j=0; j<9; j++) {
			for (z=1; z<=9; z++) {
				if (ValueList[81*i+9*j+z].Value==1) {
					sudoku_table[i][j]=z;
				}
			}
		}
	}
	return OK;
}

/*���������*/
status SudokuTablePrint(void) {
	int i,j;
	printf("   0   1   2   3   4   5   6   7   8  \n");//�б��
	printf(" +---+---+---+---+---+---+---+---+---+\n");
	for (i=0; i<9; i++) {
		printf("%d",i);//�б��
		for (j=0; j<9; j++) {
			if (j==0||j==3||j==6) {
				printf("|");//region������
			} else printf(" ");
			printf(" %d ",sudoku_table[i][j]);
		}
		printf("|\n");
		if (i==2||i==5||i==8) {
			printf(" +---+---+---+---+---+---+---+---+---+\n");//region������
		} else printf(" |           |           |           |\n"); //region������
	}
	printf("��ӡ��ɣ�\n");
	return OK;
}

