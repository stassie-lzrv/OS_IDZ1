#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


const int buf_size = 5000;

int main(int argc, char ** argv) {

    int input;
    int output;
    int fifo_desc;
    int result;
    char* pipe_read = "prc.fifo";
    char* pipe_write = "pcw.fifo";
    char buffer[buf_size];
    ssize_t read_b;
    ssize_t write_b;

    mknod(pipe_read, S_IFIFO | 0666, 0);
    mknod(pipe_write, S_IFIFO | 0666, 0);
    /*  Процесс на обработку*/

    
        if((fifo_desc = open(pipe_read, O_RDONLY)) < 0){
            printf("Процесс 2: Невозможно открыть канал на чтение\n");
            exit(-1);
        }
        read_b = read(fifo_desc, buffer, buf_size);
        printf("Процесс 2: Обработка строки %ld байт\n", read_b);
        int arr[128];
	    int counter = 0;
	    for(int i = 0; i < 128; i++){
		arr[i] = 0;
	    }
            for (int i = 0; i < read_b ; i++) {
                arr[buffer[i]] = 1; 
            }
	    char tmp = "";
	    for (int i = 0; i < read_b; i++){
		buffer[i] = tmp;
	    }
	    for (int i = 0; i < 128; i++){
		if(i != 59){
			if(arr[i] != 0){
				buffer[counter] = i;
				counter++;
			}
		}
	    }
        close(fifo_desc);
        if((fifo_desc = open(pipe_write, O_WRONLY)) < 0){
            printf("Процесс 2: Невозможно открыть канал на запись\n");
            exit(-1);
        }
        printf("Процесс 2: Запись в канал %d байт\n", counter);
        write_b = write(fifo_desc, buffer, read_b);
        close(fifo_desc);
        printf("Процесс 2: Успешное завершение процесса\n");
    
    return 0;
}