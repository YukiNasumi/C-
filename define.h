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

typedef int status;


/*�����Ӿ�������ṹ����*/
typedef struct Clause{
    int literal;//��¼�Ӿ��е�����
    int flag;
	//��Ǹ������Ƿ��ѱ�ɾ����δɾ��ʱֵΪ0������ֵΪʹ֮ɾ���ı�Ԫ��ű��ڻظ� 
    struct Clause *nextl;//ָ����Ӿ�����һ���ֵ�ָ��
    struct Clause *litline;
	//ָ������CNF��ʽ�ڽ���������һ��������ͬ���Ӿ��㣬���ڻظ��뷽����� 
}Clause;

/*����CNF��ʽ�����㣨���Ӿ�����ͷ��㣩�ṹ����*/
typedef struct Paradigm{
    int number;//�Ӿ�����ʾ���������������ҵ��Ӿ� 
    int flag;
	//��Ǹ��Ӿ��Ƿ��ѱ�ɾ����δɾ��ʱֵΪ0������ֵΪʹ֮ɾ���ı�Ԫ��ű��ڻظ� 
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
