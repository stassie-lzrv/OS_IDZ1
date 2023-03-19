
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


const int buf_size = 5000;

int main(int argc, char ** argv) {
    if (argc < 4) {
        printf("Используются: main <input1> <input2> <output>\n");
        exit(0);
    }
    int input1;
    int input2;
    int output;
    int fifo_desc;
    int result;
    char* pipe_read = "prc.fifo";
    char* pipe_write = "pcw.fifo";
    char buffer[buf_size];
    ssize_t read_b;
    ssize_t read_b_2;
    ssize_t write_b;
    ssize_t write_b_2;

    mknod(pipe_read, S_IFIFO | 0666, 0);
    mknod(pipe_write, S_IFIFO | 0666, 0);

    result = fork();
    if (result < 0) {
        printf("Невозможно создать дочерний процесс\n");
        exit(-1);
    } else if (result > 0) {
        /* Процесс на чтение */
        if((fifo_desc = open(pipe_read, O_WRONLY)) < 0){
            printf("Процесс 1: Невозможно открыть канал на чтение\n");
            exit(-1);
        }
         printf("Процесс 1: Чтение из файла %s...\n", argv[1]);
        input1 = open(argv[1], O_RDONLY);
    input2 = open(argv[2], O_RDONLY);
        if (input1 < 0) {
            printf("Процесс 1: Невозможно открыть файл на чтение\n");
            exit(1);
        }
        read_b = read(input1, buffer, buf_size);
    read_b_2 = read(input2, buffer + read_b, buf_size);
        if (read_b > 0) {
            printf("Процесс 1: Запись в процесс %ld байт\n", read_b);
            write_b = write(fifo_desc, buffer, read_b);
        write_b_2 = write(fifo_desc, buffer + write_b, read_b_2);
        read_b += read_b_2;
        write_b += write_b_2;
            if (write_b != read_b) {
                printf("Процесс 1: Невозможно записать всю строку в процесс\n");
                exit(-1);
            }
        }
        close(input1);
        close(fifo_desc);
        printf("Процесс 1: Успешное завершение процесса\n");
    } else {
        result = fork();
        if (result < 0) {
            printf("Невозможно создать дочерний процесс\n");
            exit(-1);
        } else if (result > 0) {
            /* Процесс на запись */
            output = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
            if (output < 0) {
                 printf("Процесс 2: Невозможно создать файл\n");
                exit(1);
            }
            if((fifo_desc = open(pipe_write, O_RDONLY)) < 0){
                printf("Процесс 2: Невозможно открыть канал на запись\n");
                exit(-1);
            }
            printf("Процесс 2: Чтение из канала\n");
            read_b = read(fifo_desc, buffer, buf_size);
            printf("Процесс 2: Запись в файл %s %ld байт\n", argv[3], read_b);
            write_b = write(output, buffer, read_b);
            close(output);
            close(fifo_desc);
            printf("Процесс 2: Успешное завершение процесса\n");
        } else {
            /* Процесс на обработку */
            if((fifo_desc = open(pipe_read, O_RDONLY)) < 0){
                printf("Процесс 3: Невозможно открыть канал на чтение\n");
                exit(-1);
            }
            read_b = read(fifo_desc, buffer, buf_size);
            printf("Процесс 3: Обработка строки %ld байт\n", read_b);
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
                printf("Процесс 3: Невозможно открыть канал на запись\n");
                exit(-1);
            }
            printf("Процесс 3: Запись в канал %d байт\n", counter);
            write_b = write(fifo_desc, buffer, counter);
            close(fifo_desc);
            printf("Процесс 3: Успешное завершение процесса\n");
        }
    }
    return 0;
}