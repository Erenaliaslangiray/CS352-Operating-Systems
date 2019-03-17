#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define m_Threads 27

int check[m_Threads] = {0}; //
int threads_completed=0; //Whether threads or not


typedef struct {//Define rows and columns
	int row;
	int column;
} parameters;

int m_Sudoku[9][9] = { //Sudoku initialized
	{6, 2, 4, 5, 3, 9, 1, 8, 7},
	{5, 1, 9, 7, 2, 8, 6, 3, 4},
	{8, 3, 7, 6, 1, 4, 2, 9, 5},
	{1, 4, 3, 8, 6, 5, 7, 2, 9},
	{9, 5, 8, 2, 4, 7, 3, 6, 1},
	{7, 6, 2, 3, 9, 1, 4, 5, 8},
	{3, 7, 1, 9, 5, 6, 8, 4, 2},
	{4, 9, 6, 1, 8, 2, 5, 7, 3},
	{2, 8, 5, 4, 7, 3, 9, 1, 6}
};

void *Rows(void *param){//Row Checkers
	parameters *_param =(parameters*) param; //Parameter
	int row=_param->row; //Order of Ro
	int column=_param->column; // Order of Column

	int rowValidator[9]={1,2,3,4,5,6,7,8,9}; //Numbers that has to be in row
	int _rowValidator[9]={0};
	int i,j;

	for(i=0;i<9;i++)
	{
	for(j=0;j<9;j++){
	if(m_Sudoku[row][j]==rowValidator[i]){ //Check how many iterations have been done
		_rowValidator[i]++;
	}
	}
	}

	for(i=0;i<9;i++){

	if(_rowValidator[i]>1){ //If iteration is 2 ERROR
		printf("Error!\n");
		threads_completed++;
		pthread_exit(NULL);
	}
	}

	check[row] = 1;


	threads_completed++;
	pthread_exit(NULL);
}

void *Columns(void *param){
	parameters *_param =(parameters*) param;
	int row=_param->row;
	int column=_param->column;

	int columnValidator[9]={1,2,3,4,5,6,7,8,9};
	int _columnValidator[9]={0};
	int i,j;

	for(i=0;i<9;i++)
	{

	for(j=0;j<9;j++){
	if(m_Sudoku[row][j]==columnValidator[i]){
		_columnValidator[i]++;
	}
	}
	}

	for(i=0;i<9;i++){

	if(_columnValidator[i]>1){ //If iteration is 2 ERROR
		printf("Error!\n");
		threads_completed++;
		pthread_exit(NULL);
	}
	}

	check[9+column] = 1;
	threads_completed++;
	pthread_exit(NULL);
}

void *SubGrids(void *param){

	parameters *_param =(parameters*) param;
	int row=_param->row;
	int column=_param->column;

	int subgridValidator[9]={1,2,3,4,5,6,7,8,9};
	int _subgridValidator[9]={0};
	int i,j;

	for(i=row;i<row+3;i++) //3x3 square
	{
	for(j=column;j<column+3;j++){
	if(m_Sudoku[i][j]==subgridValidator[i]){
		_subgridValidator[i]++;
	}

	}

	}

	for(i=0;i<9;i++){

	if(_subgridValidator[i]>1){
		printf("Error!\n");
		threads_completed++;
		pthread_exit(NULL);
	}
	}




	check[18+(row+column/3)]=1;
	threads_completed++;
	pthread_exit(NULL);
	}

int main() {
	pthread_t threads[m_Threads];
	int threadIndex = 0; //Initialize threads
	int i,j;

	for (i = 0; i < 9; i++) {

	for(j=0;j<9;j++){

	if(j==0){
	parameters *rData = (parameters *) malloc(sizeof(parameters)); // Allocate Location
	rData->row = i;
	rData->column = j;
	pthread_create(&threads[threadIndex++], NULL, Rows, rData); //Sends threads
	}

	if(i==0){
	parameters *cData = (parameters *) malloc(sizeof(parameters));
	cData->row = i;
	cData->column = j;
	pthread_create(&threads[threadIndex++], NULL, Columns, cData); //Sends threads
	}

	if(i%3==0&&j%3==0){
	parameters *Data = (parameters *) malloc(sizeof(parameters));
	Data->row = i;
	Data->column = j;
	pthread_create(&threads[threadIndex++], NULL, SubGrids, Data);
	}
	}
	}

	for (i = 0; i < 9; i++) {

	for(j=0;j<9;j++){

	printf("%d ",m_Sudoku[i][j]);
	}
	printf("\n");
	}


	if(threads_completed==27){
	for(i=0;i<m_Threads;i++){
    printf("Threads Completed %d\n",threads_completed);

	if (check[i] == 0) {
	printf("Invalid",i);
	return 0;
	}
	}

	printf("Valid! \n");
	system("pause");
	return 0;

	}



}
