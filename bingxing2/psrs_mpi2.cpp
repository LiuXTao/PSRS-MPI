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
//// 读数据
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
//	// 运行
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
//	int *pivot_ready, *pivot_sample, *pivot;    // 总的长度， 主元
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
//		cout << "运行128M," << "进程数" << process << endl;
//		cout << "using" << end_time - start_time << " s" << endl;
//		return;
//	}
//	localSizeAll = (int *)malloc(sizeof(int)*process*process);
//	double startTime = MPI_Wtime();
//
//	// 将data分发给每一个局部的localDta
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
//	// 均匀划分
//
//	// 局部排序
//	sort(local_data, local_data + num / process);
//
//	// 候选主元
//	pivot_ready = (int*)malloc(sizeof(int)*(process));
//	//正则采样
//	for (i = 0; i < process; i++) {
//		pivot_ready[i] = local_data[i*(partition_length / process)];
//	}
//
//
//	pivot_sample = (int*)malloc(process*process * sizeof(int));
//	//主元
//	pivot = (int*)malloc((process - 1) * sizeof(int));
//	int index = 0;
//
//	MPI_Gather(pivot_ready, process, MPI_INT, pivot_sample, process, MPI_INT, 0, MPI_COMM_WORLD);
//	if (my_rank == 0) {
//	////样本排序
//		sort(pivot_sample, pivot_sample + process*process);
//	//选择主元
//		for (i = 0; i < process; i++) {
//			pivot[i] = pivot_sample[(i + 1)*process + 1];
//		}
//
//	}
//	// 将pivot的值传送给每一个部分
//	MPI_Bcast(pivot, process - 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//	// 按照主元划分
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
//	//全局交换
//	///////////////////////////
//	int **localPartition = new int *[process];
//	swapAll(process, localPartition, origin_sizes, new_sizes, partition_length,pivot,local_data);
//
//	//将每个节点的数据规约
//	for (i = 0; i < process; i++) {
//		MPI_Reduce(new_sizes + i, local_size, 1, MPI_INT, MPI_SUM, i, MPI_COMM_WORLD);
//	}
//	for (i = 0; i < process; i++) {
//		MPI_Gather(new_sizes, process, MPI_INT, localSizeAll, process, MPI_INT, i, MPI_COMM_WORLD);
//	}
//
//	temp = (int*)malloc(sizeof(int)*local_size[0]);
//
//	//将每个值划入新分区
//	splitPartition(process,localSizeAll, new_sizes,localPartition,temp);
//	
//	// 每个新分区排序
//	sort(temp, temp + local_size[0]);
//
//	// 检查每个分区排序是否正确
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
//	//记录结束时间
//	double endTime = MPI_Wtime();
//	MPI_Barrier(MPI_COMM_WORLD);
//	if (my_rank == 0) {
//		cout << "运行256M," << "进程数" << process << endl;
//		cout << "using" << endTime - startTime << " s" << endl;
//	}
//}
