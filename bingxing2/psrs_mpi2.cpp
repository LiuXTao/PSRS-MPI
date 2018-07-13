//#include<algorithm>
//#include<fstream>
//#include<iostream>
//#include<mpi.h>
//#include<cstdlib>
//
//using namespace std;
//
//const int MBSIZE = 1024*1024;
//int i, j, k;
//
//// ������
//void readFile(int *data, long num) {
//
//	ifstream fin("../data512M.txt", ios::binary);
//
//	fin.read((char *)data, num * sizeof(int));
//	fin.close();
//}
//
//void splitPartition(int process,int *localSizeAll, int *newSizes, int **localPartition, int *temp) {
//	for (i = 0; i < process; i++) {
//		int* indexes = (int *)malloc(process * sizeof(int));
//		for (j = 0; j < process; j++) {
//			indexes[j] = localSizeAll[j*process + i];
//		}
//		int* partitionEnds = (int *)malloc(process * sizeof(int));
//		for (j = 0; j < process; j++) {
//			if (j == 0)
//				partitionEnds[j] = 0;
//			else
//				partitionEnds[j] = partitionEnds[j - 1] + indexes[j - 1];
//		}
//		MPI_Gatherv(localPartition[i], newSizes[i], MPI_INT, temp, indexes, partitionEnds, MPI_INT, i, MPI_COMM_WORLD);
//	}
//}
//
//void swapAll(int process, int **localPartition,int *sizes, int *newSizes, int localLength, int *pivot, int *local_data){
//	for (int i = 0; i < process; i++) {
//		localPartition[i] = (int*)malloc(newSizes[i] * sizeof(int));
//	}
//
//	for (i = 0; i < localLength; i++) {
//		for (j = 0; j < process - 1; j++) {
//			if (local_data[i] < pivot[j]) {
//				localPartition[j][sizes[j]++] = local_data[i];
//				break;
//			}
//			if (local_data[i] >= pivot[j] && j + 2 == process) {
//				localPartition[j + 1][sizes[j + 1]++] = local_data[i];
//				break;
//			}
//		}
//	}
//}
//void PSRS(long num);
//
//
//
//
//int main(int argc, char *argv[]) {
//
//	int num =256;
//	MPI_Init(&argc, &argv);
//	// ����
//	PSRS(num*MBSIZE);
//
//	MPI_Finalize();
//	return 0;
//
//
//}
//
//void PSRS(long num) {
//	int *data = NULL;
//	int *local_data = NULL;
//	int my_rank, process;
//	int partition_length;
//	
//	int *origin_sizes, *new_sizes, k;
//	int *local_size = new int[1];
//	int *localSizeAll;
//	int *temp;
//
//	int *pivot_ready, *pivot_sample, *pivot;    // �ܵĳ��ȣ� ��Ԫ
//	MPI_Comm_size(MPI_COMM_WORLD, &process);
//	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//
//
//	partition_length = num / process;
//	local_data = (int*)malloc(sizeof(int)*partition_length);
//	if (my_rank == 0) {
//		data = new int[num];
//		readFile(data, num);
//	}
//	if (process == 1) {
//		double start_time = MPI_Wtime();
//		sort(data, data + num);
//		double end_time = MPI_Wtime();
//		cout << "����128M," << "������" << process << endl;
//		cout << "using" << end_time - start_time << " s" << endl;
//		return;
//	}
//	localSizeAll = (int *)malloc(sizeof(int)*process*process);
//	double startTime = MPI_Wtime();
//
//	// ��data�ַ���ÿһ���ֲ���localDta
//	MPI_Scatter(data, num / process, MPI_INT, local_data, num / process, MPI_INT, 0, MPI_COMM_WORLD);
//
//
//	origin_sizes = (int *)malloc(process * sizeof(int));
//	new_sizes = (int *)malloc(process * sizeof(int));
//	for (i = 0; i < process; i++) {
//		new_sizes[i] = 0;
//		origin_sizes[i] = 0;
//	}
//
//	// ���Ȼ���
//
//	// �ֲ�����
//	sort(local_data, local_data + num / process);
//
//	// ��ѡ��Ԫ
//	pivot_ready = (int*)malloc(sizeof(int)*(process));
//	//�������
//	for (i = 0; i < process; i++) {
//		pivot_ready[i] = local_data[i*(partition_length / process)];
//	}
//
//
//	pivot_sample = (int*)malloc(process*process * sizeof(int));
//	//��Ԫ
//	pivot = (int*)malloc((process - 1) * sizeof(int));
//	int index = 0;
//
//	MPI_Gather(pivot_ready, process, MPI_INT, pivot_sample, process, MPI_INT, 0, MPI_COMM_WORLD);
//	if (my_rank == 0) {
//	////��������
//		sort(pivot_sample, pivot_sample + process*process);
//	//ѡ����Ԫ
//		for (i = 0; i < process; i++) {
//			pivot[i] = pivot_sample[(i + 1)*process + 1];
//		}
//
//	}
//	// ��pivot��ֵ���͸�ÿһ������
//	MPI_Bcast(pivot, process - 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//	// ������Ԫ����
//	index = 0;
//	for (i = 0; i < partition_length; i++) {
//		for (j = 0; j < process - 1; j++) {
//			if (local_data[i] < pivot[j]) {
//				new_sizes[j] = new_sizes[j] + 1;
//				break;
//			}
//			if (local_data[i] >= pivot[j] && j + 2 == process) {
//				new_sizes[j + 1] = new_sizes[j + 1] + 1;
//				break;
//			}
//		}
//
//	}
//
//	//ȫ�ֽ���
//	///////////////////////////
//	int **localPartition = new int *[process];
//	swapAll(process, localPartition, origin_sizes, new_sizes, partition_length,pivot,local_data);
//
//	//��ÿ���ڵ�����ݹ�Լ
//	for (i = 0; i < process; i++) {
//		MPI_Reduce(new_sizes + i, local_size, 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);
//	}
//	for (i = 0; i < process; i++) {
//		MPI_Gather(new_sizes, process, MPI_INT, localSizeAll, process, MPI_INT, i, MPI_COMM_WORLD);
//	}
//
//	temp = (int*)malloc(sizeof(int)*local_size[0]);
//
//	//��ÿ��ֵ�����·���
//	splitPartition(process,localSizeAll, new_sizes,localPartition,temp);
//	
//	// ÿ���·�������
//	sort(temp, temp + local_size[0]);
//
//	// ���ÿ�����������Ƿ���ȷ
//	for (int i = 1; i < local_size[0]; i++) {
//		if (temp[i] < temp[i - 1]) {
//			cout << "false"<< temp[i] <<"   "<< my_rank <<"   "<< temp[i - 1] <<"  "<<i-1<<" "<<endl;
//			break;
//		}
//	}
//
//	free(temp);
//	free(origin_sizes);
//	free(new_sizes);
//	free(pivot_ready);
//	
//	//��¼����ʱ��
//	double endTime = MPI_Wtime();
//	MPI_Barrier(MPI_COMM_WORLD);
//	if (my_rank == 0) {
//		cout << "����256M," << "������" << process << endl;
//		cout << "using" << endTime - startTime << " s" << endl;
//	}
//}
