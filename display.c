#include"define.h"

/*-------------------������-------------------*/

int main() {
	char filename[100],copy_filename[100],sudo_filename[100];//filename,sudu_filename���ڴ����ļ�����copy_filename���ڱ���
	char filename_setcnf[100];
	int flag_cnf = 0,flag_sudo=0;//�����ж�cnf�ļ��Ƿ��ȡ�ɹ�
	int op=1,SAT=1,Sudoku=1;//op����ѡ���Ƕ�ȡcnf�ļ�������Sudoku
	while(op) {
		system("cls");
		printf("\n---Menu For Puzzle Or SAT-----\n");
		printf("1.SAT        2.BinaryPuzzle\n");
		printf("0.Exit\n");
		printf("------------------------------\n");
		printf("������0~2��\n");
		printf("----------\n");
		scanf("%d",&op);
		switch(op) {
			case 1:
				while(SAT) {
					system("cls");
					printf("\n--------Menu For SAT--------------\n");
					printf("1.Readfile		    2.Savefile\n");
					printf("3.Traversefile		4.Dpll\n");
					printf("0.Exit\n");
					printf("----------------------------------\n");
					printf("������0~4��\n");
					printf("----------\n");
					scanf("%d",&SAT);
					switch(SAT) {
						case 1:
							printf("�������ȡ��cnf�ļ�����\n");
							scanf("%s", filename);
							strcpy(copy_filename, filename);
							struct Clause * root = Readfile(filename);
							if (root != NULL) {
								printf("�ļ���ȡ�ɹ���\n");
								flag_cnf = 1;
							} else printf("�ļ���ȡʧ�ܣ������ļ�����\n");
							getchar();
							getchar();
							break;
						case 2:
							if(!flag_cnf) printf("�ļ���δ��ȡ�����ȡ�������\n");
							else  {
								if (Savefile(root)) {
									printf("�ļ�����ɹ���\n");
								} else printf("�ļ�����ʧ�ܣ�\n");
							}
							getchar();
							getchar();
							break;
						case 3:
							if (flag_cnf) Traversefile(root);
							else printf("�ļ���δ��ȡ�����ȡ�������\n");
							getchar();
							getchar();
							break;
						case 4:
							if(flag_cnf) {
								printf("��ѡ���Ż���ʽ��\n");
								printf("1.�Ż�ǰ	2.�Ż���\n");
								scanf("%d",&WAY);
								clock_t start,end;//���ڼ�¼����ʱ��time.h
								start=clock();
								if(dpll(root)==OK) {
									end=clock();
									printf("���ļ��н⣡\n");
									printf("���ʱ�䣺%d ms��\n",end-start);
									for(i=1; i<=wordsum; i++) {
										temp=(ValueList[i].value!=0?1:-1)*i;
										printf("%d ",temp);
										if(i%10==0) printf("\n");
									}
									printf("\n");
									writeResult(copy_filename, 1, end-start);
								} else {
									end=clock();
									printf("���ļ��޽⣡\n");
									printf("���ʱ�䣺%d ms��\n",end-start);
									writeResult(copy_filename, 0, end - start);
									printf("\n");
								}
								flag_cnf = 0;
							} else printf("�ļ�δ��ȡ��\n");
							getchar();
							getchar();
							break;
						case 0:
							break;
					}
				}
				getchar();
				getchar();
				break;
			case 2:
				srand((unsigned)time(NULL));
				while(Sudoku) {
					system("cls");
					printf("\n--------Menu For Sudoku--------------\n");
					printf("1.ReadSudoku 	    2.Play\n");
					printf("3.SetCNFfile		4.Dpll For Answer Check\n");
					printf("0.Exit\n");
					printf("----------------------------------\n");
					printf("������0~4��\n");
					printf("----------\n");
					scanf("%d",&Sudoku);
					switch(Sudoku) {
						case 1:
							printf("������Ҫ��ȡ�����������ļ�����\n");
							scanf("%s", sudo_filename);
							Readqipan(sudo_filename);
							SudokuTablePrint();
							flag_sudo = 1;
							getchar();
							getchar();
							break;
						case 2:
							break;
						case 3:
							if (flag_sudo) {
								printf("��ѡ��\n1.ֱ����⣡	2.����Play��Ľ����\n");
								scanf("%d", &way_sudo);
								if (way_sudo == 1) {
									SetCNFfile(qipan);
								} else {
									if (Checkqipan(qipan)) SetCNFfile(qipan);
									else printf("���̻�δ����������������ϣ�\n");
								}
							} else printf("�����ļ���δ��ȡ��\n");
							getchar();
							getchar();
							break;

					}
				}
				getchar();
				getchar();
				break;
			case 0:
				break;
		}
	}

}

/*---------------------------���庯��-------------------------*/

/*------------------------CNF�ļ�����------------------------*/

//�½�����
struct Literal *CreateLiteral() {
	struct Literal * new_literal = malloc(sizeof(struct Literal));
	new_literal->next=NULL;
	new_literal->value=0;
	return new_literal;
}

//�½��Ӿ�
struct Clause * CreateClause() {
	struct Clause * new_clause = malloc(sizeof(struct Clause));
	new_clause->next=NULL;
	new_clause->head=NULL;
	return new_clause;
}

//����cnt�ļ�
struct Clause * Readfile(char * filename) {
	FILE * fp;
	fp = fopen(filename,"r");
	if(fp == NULL) {
		printf("�ļ���ʧ��!\n");
		return NULL;
	}
	char s[100];
	char *c;
	struct Clause * root = NULL, * current_Clause = NULL, * pre_Clause = NULL;
	struct Literal * current_Literal = NULL, * pre_Literal = NULL;
	while(fgets(s, 100, fp)) { //���ж�ȡ
		if(s[0] == 'c') continue;
		else if(s[0] == 'p') {
			sscanf(s, "p cnf %d %d", &wordsum, &forusum);//����ʽ�����������빫ʽ��
			printf("������: %d	��ʽ��: %d\n", wordsum, forusum);
			ValueList=(struct ValueList *)calloc(wordsum + 1, sizeof(struct ValueList));
			for(i = 1; i <= wordsum; i++) ValueList[i].value = -1;//��ʼ�����飬-1��ʾδȷ����ֵ
		} else {
			//�����洢�ṹ
			current_Clause = CreateClause();
			if(root == NULL) root = current_Clause;
			if(pre_Clause != NULL)  pre_Clause->next = current_Clause;
			c = strtok(s, (const char *) " ");//�ָ��ַ���
			while(c != NULL) {
				current_Literal = CreateLiteral();
				temp = atoi(c);
				current_Literal->value = temp;
				if(temp != 0) {
					if(current_Clause->head == NULL) current_Clause->head = current_Literal;
					if(pre_Literal != NULL) pre_Literal->next = current_Literal;
				}
				pre_Literal = current_Literal;
				c = strtok(NULL, (const char *) " ");
			}
			pre_Clause = current_Clause;
		}
	}
	fclose(fp);
	return root;
}

//�������cnf�ļ�
void Traversefile(struct Clause * root) {
	struct Clause* Temp = root;
	i=0;
	while(Temp) {
		struct Literal *temp=Temp->head;
		i++;
		printf("��%d���Ӿ�Ϊ�� ",i);
		while(temp) {
			printf("%d ",temp->value);
			temp=temp->next;
		}
		if(Temp->next!=NULL) printf("\n");
		Temp=Temp->next;
	}
	printf("\n");
	printf("�ļ�������ɣ�\n");
}

//����cnt�ļ�
status Savefile(struct Clause * root) {
	FILE *fp;
	char filename[100];
	printf("��������Ҫ������ļ�����");
	scanf("%s",filename);
	if((fp=fopen(filename,"wb"))==NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ERROR;
	} else {
		fprintf(fp, "%s ", "p cnf");
		fprintf(fp, "%d ", wordsum);
		fprintf(fp, "%d\n", forusum);//��ӡ���ļ���
		struct Clause *T=root;
		while(T) {
			struct Literal *temp=T->head;
			while(temp) {
				fprintf(fp, "%d ", temp->value);
				temp=temp->next;
			}
			if(T->next!=NULL) fprintf(fp, "0\n");
			T=T->next;
		}
	}
	fclose(fp);
	return OK;
}

/*------------------------DPLL�㷨--------------------------*/

//�ж�����
status judgesign(int a) {
	return (a>0)?1:-1;
}

//�жϲ�Ѱ�ҵ��Ӿ�
status judgesimple(struct Clause *root) {
	struct Clause *T=root;
	while(T) {
		if(T->head==NULL) {
			T=T->next;
			continue;
		}
		if(T->head->next==NULL) return T->head->value;
		T=T->next;
	}
	return ERROR;//û�е��Ӿ�
}

//�����Ӿ䣨���Ӿ����
status solvesimple(struct Clause *root) {
	temp=judgesimple(root);
	if(temp == 0) return ERROR; //��ʾû�е��Ӿ�
	ValueList[abs(temp)].value = temp > 0 ? 1 : 0;
	struct Clause * T = root, *pre_T = CreateClause();
	while(T != NULL) {
		struct Literal * current_Literal = T->head, * pre_Literal = CreateLiteral();
		while(current_Literal != NULL) {
			if(current_Literal->value == temp) { //ɾ�����������ֵ��Ӿ�
				if(T == root) {
					*(root) = *(root->next);
					T = NULL;
				} else {
					pre_T->next = T->next;
					T = pre_T;
				}
				break;
			} else if (current_Literal->value == -temp) { //ɾ����Ϊ�෴��������
				if(current_Literal == T->head) T->head = current_Literal->next;
				else pre_Literal->next = current_Literal->next;
				current_Literal = pre_Literal;
				continue;
			}
			pre_Literal = current_Literal;
			current_Literal = current_Literal->next;
		}
		pre_T = T;
		T = (T == NULL) ? root : T->next;
	}
	return OK;
};

//ѡȡ������ֵ����
status chooseLiteral(struct Clause *root) {
	return root->head->value;
}

//�ж��Ƿ���ڿ��Ӿ�
status judgeEmptyClause(struct Clause * root) {
	struct Clause * T = root;
	while(T != NULL) {
		if(T->head == NULL) return OK;
		T = T->next;
	}
	return ERROR;
}

//�ж��Ƿ�Ϊһ����ʽ���ֵ�����
status judgePureLiteral(struct Clause *root) {
	int *mark=(int *)calloc(wordsum+1,sizeof(int));//�������,����Ϊ0
	struct Clause *T=root;
	while(T) {
		struct Literal *current_literal=T->head;
		while(current_literal) {
			if(mark[abs(current_literal->value)]!=2) {
				temp=mark[abs(current_literal->value)];
				t=judgesign(current_literal->value);
				if(temp==0) mark[abs(current_literal->value)]=t;
				else if(temp==1&&t==-1) mark[abs(current_literal->value)]=2;
				else if(temp==-1&&t==1) mark[abs(current_literal->value)]=2;
				//������ʽ������Ϊ2;
			}
			current_literal=current_literal->next;
		}
		T=T->next;
	}
	for(i=1; i<=wordsum; i++) {
		if(mark[i]==1||mark[i]==-1)
			return i*mark[i]; //����ֻ��һ����ʽ�����ֵ�ֵ
	}
	return 0;//������ֻ��һ����ʽ������
}

//����һ����ʽ������
status solvepureliteral(struct Clause *root) {
	temp=judgePureLiteral(root);//�ҵ�ֻ��һ����ʽ�����֣�
	if(temp==0) return ERROR;//��ʾû���ҵ�
	ValueList[abs(temp)].value=(temp>0)?1:0;//-1Ϊδȷ����ֵ��1Ϊ����ֵ�������룩��0Ϊ����ֵ���������룩
	struct Clause *T=root,*pre_T=CreateClause();
	while(T) {
		struct Literal *current_literal=T->head,*pre_literal=CreateLiteral();
		while(current_literal) {
			if(current_literal->value==temp) {//ɾ�����������ֵ��Ӿ�
				if(T==root) {
					*(root)=*(root->next);
					T=NULL;
				} else {
					pre_T->next=T->next;
					//free(T);
					T=pre_T;
				}
				break;
			} else if(current_literal->value==-temp) { //ɾ����Ϊ�෴��������
				if(current_literal==T->head) {
					T->head=current_literal->next;
					//free(current_literal);
				} else {
					pre_literal->next=current_literal->next;
					//free(current_literal);
				}
				pre_literal=current_literal;
				current_literal=current_literal->next;
			}
			current_literal=current_literal->next;
		}
		pre_T=T;
		T=(T==NULL)?root:T->next;
	}
	return OK;
}
//�ж����־�Ϊһ����ʽ����
status judgeAllliteral(struct Clause *root) {
	int *mark=(int *)calloc(wordsum+1,sizeof(int));//�������,����Ϊ0
	struct Clause *T=root;
	while(T) {
		struct Literal *current_literal=T->head;
		while(current_literal) {
			temp=mark[abs(current_literal->value)];
			t=judgesign(current_literal->value);
			if(temp==0) mark[abs(current_literal->value)]=t;
			else if(temp!=t)  return ERROR;//˵����������ʽ
			current_literal=current_literal->next;
		}
		T=T->next;
	}
	T=root;
	while(T) {
		struct Literal * current_literal = T->head;
		while(current_literal) {
			if(ValueList[abs(current_literal->value)].value==-1) {
				ValueList[abs(current_literal->value)].value=(current_literal->value > 0) ? 1 : 0;
			}
			current_literal=current_literal->next;
		}
		T=T->next;
	}//�Ѿ�ȷ��ȫ��һ����ʽ���ʿ��Ը��±������õ����ս��
	return OK;
}



//�ж��Ƿ�����Ҫ��
status check(struct Clause *root) {
	if(judgeEmptyClause(root)) return ERROR;//������
	if(judgeAllliteral(root)) return OK;//����Ҫ��
	return UNKNOW;//δȷ��״̬
}

//���������Ӿ�
void destroyClause(struct Clause *root) {
	while(root != NULL) {
		struct Clause * next_r = root->next;
		if(root->head != NULL) {
			struct Literal * current_Literal = root ->head;
			while(current_Literal != NULL) {
				struct Literal * next_Literal = current_Literal->next;
				free(current_Literal);
				current_Literal = next_Literal;
			}
		}
		free(root);
		root = next_r;
	}
}

//��¡�Ӿ�
struct Clause *cloneClause(struct Clause *root) {
	struct Clause *new_Clause=CreateClause();
	struct Literal *current_literal=root->head,*pre_literal=NULL;
	while(current_literal) {
		struct Literal *new_literal=CreateLiteral();
		new_literal->value=current_literal->value;
		if(new_Clause->head==NULL) new_Clause->head=new_literal;
		if(pre_literal!=NULL) pre_literal->next=new_literal;
		pre_literal=new_literal;
		current_literal=current_literal->next;
	}
	return new_Clause;
}

//��¡�����Ӿ䲢��ѡȡ��ֵ�����Ӿ��в���Ϊ���Ӿ�
struct Clause *cloneAll(int a,struct Clause *root) {
	ValueList[abs(a)].value=(a>0)?1:0;
	struct Clause *T=root,*new_root=NULL,*pre_Clause=NULL;
	while(T) {
		struct Clause *current_Clause=cloneClause(T);
		if(new_root==NULL) new_root=current_Clause;
		if(pre_Clause!=NULL) pre_Clause->next=current_Clause;
		pre_Clause = current_Clause;
		T=T->next;
	}//���������Ӿ�
	//��������µĵ��Ӿ�
	struct Clause *new_Clause=CreateClause();
	struct Literal *new_Literal=CreateLiteral();
	new_Literal->value=a;
	new_Clause->head=new_Literal;
	new_Clause->next=new_root;
	return new_Clause;
}

//DPLL�㷨���������ٻ��ݷ���
status dpll(struct Clause * root) {
	temp = check(root);
	if(temp != UNKNOW) {
		destroyClause(root);
		return temp;
	}//����δȷ������������ؽ��
	while(1) { //�����Ӿ�
		temp = check(root);
		if(temp != UNKNOW) {
			destroyClause(root);
			return temp;
		}//ͬ��
		if(!solvesimple(root)) break;//�����Ӿ䣬û�еĻ�����ѭ��
	}
	if (WAY == 2) {
		while(1) { //����ֻ��һ����ʽ���ֵ�����
			temp = check(root);
			if(temp != UNKNOW) {
				destroyClause(root);
				return temp;
			}
			if(!solvepureliteral(root)) break;
		}
	}
	int new_count = chooseLiteral(root);//ѡȡ������ֵ
	if(dpll(cloneAll(new_count, root)) == OK) return OK;
	else {
		temp = dpll(cloneAll(-new_count, root));//������ɹ���ѡȡ������ֵ���෴��
		destroyClause(root);
		return temp;
	}
}
/*�����д��ͬ���ļ�*/
void writeResult(char * filename, int f, int time) {
	int len=strlen(filename)-3;
	filename[len ] = 'r',filename[len + 1] = 'e',filename[len + 2] = 's';
	FILE * fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("���������ʧ�ܣ�\n");
		exit(1);
	}
	if(f == 1) {
		fprintf(fp,"s 1\n");
		fprintf(fp,"v ");
		for (i = 1; i <= wordsum; i++) {
			temp=(ValueList[i].value!=0?1:-1)*i;
			fprintf(fp, "%d ", temp);
			if (i%10 == 0) fprintf(fp, "\n");
		}
		fprintf(fp, "\nt %d ms\n", time);
	} else {
		fprintf(fp,"s 0\n");
		fprintf(fp,"v \n");
		fprintf(fp, "t %d ms\n", time);
	}
	printf("���������ɹ���\n");
	fclose(fp);
}

/*------------------------Sudoku--------------------------*/


//��ȡ�����ļ�
void Readqipan(char *filename) {
	FILE *fp;
	fp = fopen(filename,"r");
	ValueList=(struct ValueList *)calloc(730, sizeof(struct ValueList));
	if(fp == NULL) {
		printf("�ļ���ʧ��!\n");
		return ;
	} else {
		char c;
		i=0;
		while(fscanf(fp, "%c", &c) != EOF) {
			if(c!='\n')
			{
			if(c!='.') {
				qipan[i].num=c-'0';
				qipan[i].flag=1;
			} else if(c=='.') qipan[i].num=0,qipan[i].flag=0;
			i++;
			}
		}
	}
	for(i=0; i<=81; i++) {
		x=i/9;
		y=i%9;
		sudoku_table[x][y]=qipan[i].num;
	}
	fclose(fp);
	printf("��ȡ�ɹ���\n");
}

//���������
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
	return OK;
}

//��������Ƿ������
status Checkqipan(struct Qipan qipan[]) {
	for (i = 0; i < 81; i++) {
		if (qipan[i].num == 0) return ERROR;
	}
	return OK;
}

//������ӦSAT��Ԫ��ת��Ϊ��ά����
status CNFSudokuTableTransform(void) {
	int i,j,z;
	for (i=0; i<9; i++) {
		for (j=0; j<9; j++) {
			for (z=1; z<=9; z++) {
				if (ValueList[81*i+9*j+z].value==1) {
					sudoku_table[i][j]=z;
				}
			}
		}
	}
	return OK;
}
//ת��ΪCNF�ļ�
void SetCNFfile(struct Qipan qipan[]) {
	int x,y,z,i,d;
	FILE *fp;
	char sudo_filename[100];
	printf("��������Ҫת�����ļ�����");
	scanf("%s",sudo_filename);
	fp=CreateSudokuFile(sudo_filename);
	if (fp==NULL) {
		printf("�ļ���ʧ�ܣ�\n");
		return ;
	}
	struct Clause *root=Readfile(sudo_filename);
	
	fclose(fp);
	return;

}

//ɾ��ѡ�е�����ֵl���Ӿ�,��ɾ��ѡ�е�����ֵ-l�Ľڵ�
status DeleteCandL(struct Clause *root,int l) {
	temp=l;//ѡ�е����֣�
	if(temp==0) return ERROR;//��ʾû���ҵ�
	ValueList[abs(temp)].value=(temp>0)?1:0;//-1Ϊδȷ����ֵ��1Ϊ����ֵ�������룩��0Ϊ����ֵ���������룩
	struct Clause *T=root,*pre_T=CreateClause();
	while(T) {
		struct Literal *current_literal=T->head,*pre_literal=CreateLiteral();
		while(current_literal) {
			if(current_literal->value==temp) {//ɾ�����������ֵ��Ӿ�
				if(T==root) {
					*(root)=*(root->next);
					T=NULL;
				} else {
					pre_T->next=T->next;
					//free(T);
					T=pre_T;
				}
				break;
			} else if(current_literal->value==-temp) { //ɾ����Ϊ�෴��������
				if(current_literal==T->head) {
					T->head=current_literal->next;
					//free(current_literal);
				} else {
					pre_literal->next=current_literal->next;
					//free(current_literal);
				}
				pre_literal=current_literal;
				current_literal=current_literal->next;
			}
			current_literal=current_literal->next;
		}
		pre_T=T;
		T=(T==NULL)?root:T->next;
	}
	return OK;
}


//������������ת��ΪSAT������cnf�ļ�
FILE * CreateSudokuFile(char *filename) {
	int x,y,z,i,j,k,l;//x�����������У�y�����������У�zȡ1��9�ֱ����ø�����������ֵΪ1��9����һֵ
	FILE *fp;
	fp=fopen(filename, "wb");
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
	fp=fopen(filename, "r");
	return fp;
}

/*�������cnf�ļ���ת��Ϊ����������ʽ���*/
status SolveSudoku(void){
    FILE *fp;
    char filename[100];
    char read;
    int i,j,k,x,y,d,cham;
    int exitnum[10];
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
            }
            else if (read<='9'&&read>='1') {
                sudoku_table[i][j]=(int)(read-'0');
            }
            else j=-1;
            j++;
            fscanf(fp, "%c",&read);
        }
    }
    
    fclose(fp);
    fp=fopen(filename,"r");
    struct Clause *root=Readfile(filename);
    
    /*���������ļ��еõ�����֪������Ϣ��������Ԫ��cell����ֵ�Ķ�Ӧ��ϵ*/
    k=1;
    for (i=0; i<9; i++) {
        for (j=0; j<9; j++)
            if (sudoku_table[i][j]==0) {
                for (d=0; d<=9; d++)
                    exitnum[d]=0;
                for (y=0; y<9; y++) {
                    exitnum[sudoku_table[i][y]]=1;
                }//�����У���¼�����Ѵ��ڵ���
                for (x=0; x<9; x++) {
                    exitnum[sudoku_table[x][j]]=1;
                }//�����У���¼�����Ѵ��ڵ���
                for (x=(i/3)*3; x<(i/3)*3+3; x++) {
                    for (y=(j/3)*3; y<(j/3)*3+3; y++)
                        exitnum[sudoku_table[x][y]]=1;
                }//����region����¼region���Ѵ��ڵ���
                for (cham=1; cham<=9; cham++)
                    if (exitnum[cham]==0) {
                        ValueList[k].x=i;//��¼�ñ�Ԫ��Ӧ������cell������
                        ValueList[k].y=j;//��¼�ñ�Ԫ��Ӧ������cell������
                        ValueList[k].z=cham;//��¼�ñ�Ԫ��Ӧ������������ֵ
                        k++;//������һ��Ԫ
                    }
            }
    }
    
    dpll(root);//��������ļ�
    for (k=1; k<=wordsum; k++) {
        if (ValueList[k].value==1)
            sudoku_table[ValueList[k].x][ValueList[k].y]=ValueList[k].z;
    }//����Ԫ����ֵ��Ϣת��Ϊ��ά�����������Ϣ
    printf("�����Ľ�Ϊ��\n");
    SudokuTablePrint();//�����������
    return OK;
    }
