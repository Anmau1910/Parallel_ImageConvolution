#include "image_proc.h"

int main(int argc, char **argv) {
	DIR *in_dir, *out_dir;

	int world_size, tag = 99; // number of processes / tag
	int my_id; // the id of the process
	MPI_Status status;
	struct dirent *de;

	//Error checking --------------------------------------------------
	if (argc != 3) {
		fprintf(stderr, "Usage %s input_dir output_dir\n", argv[0]);
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
	//End error checking --------------------------------------------------

	MPI_Init(NULL, NULL);      // initialize MPI environment
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	if (!my_id)
	{
		int s,i = 0;
		char file[256];
		char end = '\0';
		while ((de = readdir(in_dir))) {

			if (de->d_type != DT_REG || !strstr(de->d_name, ".bmp")) 
				continue;

			//process(de->d_name);

			if( !(i % world_size) )
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

		for( i = 1; i < world_size; i++ )
			MPI_Send(&end, 256, MPI_CHAR, i, tag, MPI_COMM_WORLD);
	}
	else
	{
		int size;
		char file[256];
		while(1)
		{
			MPI_Recv(file,256,MPI_CHAR,MPI_ANY_SOURCE,tag, MPI_COMM_WORLD, &status);

			if(!file[0])
				break;

			// file = input location
			// file + strlen(file) + 1 = output location
			printf("%s\n", file + strlen(file) + 1);
			process(file, file + strlen(file) + 1);
			memset(file, 0, 256);
		}
	}


	MPI_Finalize();
	exit(EXIT_SUCCESS);
}
