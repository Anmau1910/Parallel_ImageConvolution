#include "image_proc.h"

int main(int argc, char **argv) {
	DIR *in_dir, *out_dir;
	int filt;
	double time_start, time_finish;

	int world_size, tag = 99; // number of processes / tag
	int my_id; // the id of the process
	MPI_Status status;
	struct dirent *de;

	//Error checking --------------------------------------------------
	if (argc != 4) {
		fprintf(stderr, "Usage %s input_dir output_dir filter\n", argv[0]);
		fprintf(stderr, "Filters:\n");
		fprintf(stderr, "\t0: blur\n");
		fprintf(stderr, "\t1: bottom sobel\n");
		fprintf(stderr, "\t2: emboss\n");
		fprintf(stderr, "\t3: identity\n");
		fprintf(stderr, "\t4: left sobel\n");
		fprintf(stderr, "\t5: outline\n");
		fprintf(stderr, "\t6: right sobel\n");
		fprintf(stderr, "\t7: sharpen\n");
		fprintf(stderr, "\t8: top sobel\n");
		exit(EXIT_FAILURE);
	}

	if (!(in_dir = opendir(argv[1]))) {
		perror("Error abriendo directorio de entrada: ");
		exit(EXIT_FAILURE);
	}

	if (!(out_dir = opendir(argv[2]))) {
		perror("Error abriendo directorio de salida: ");
		closedir(in_dir);
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[3]) > 1 || argv[3][0] - '0' > 8 || argv[3][0] - '0' < 1) {
		fprintf(stderr, "Filtro no reconocido\n");
		exit(EXIT_FAILURE);
	}

	// End error checking --------------------------------------------------
	filt = argv[3][0] - '0';

	MPI_Init(NULL, NULL);      // initialize MPI environment
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	if (!my_id)
	{
		int s,i = 0;
		char file[256];
		char end = '\0';
		time_start = MPI_Wtime();
		while ((de = readdir(in_dir))) {

			if (de->d_type != DT_REG || !strstr(de->d_name, ".bmp")) 
				continue;

			//process(de->d_name);

			if (!(i % world_size))
				i++;

			//building string with input and output path to send:
			strcpy(file, argv[1]);
			s = strlen(file);
			file[s] = '/';
			strcpy(file +s +1, de->d_name);
			strcpy(file + strlen(file) + 1, argv[2]);
			s = strlen(file) + strlen(argv[2]);
			file[s + 1] = '/';
			strcpy(file + s + 2, de->d_name);

			MPI_Send(file, 256, MPI_CHAR, i % world_size, tag, MPI_COMM_WORLD);

			i++;
			memset(file, 0, 256);
		}

		for (i = 1; i < world_size; i++)
			MPI_Send(&end, 256, MPI_CHAR, i, tag, MPI_COMM_WORLD);
	} else {
		int size;
		char file[256];
		while (1)
		{
			MPI_Recv(file, 256, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);

			if (!file[0])
				break;

			// file = input location
			// file + strlen(file) + 1 = output location
			// printf("%s\n", file + strlen(file) + 1);
			process(file, file + strlen(file) + 1, filt);
			memset(file, 0, 256);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (!my_id) {
		time_finish = MPI_Wtime();
		printf("Time elapsed:\t%fs.\n", time_finish - time_start);
	}

	MPI_Finalize();
	exit(EXIT_SUCCESS);
}