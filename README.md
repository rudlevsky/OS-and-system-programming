# OS-and-system-programming (OSISP)

Laboratory task #1. 

Write a script that finds all the files of the specified extension in the specified directory and all its subdirectories and creates a hard link for each file found in the specified directory. The file extension and directory for hard links are specified as the first and second command-line arguments.

Laboratory task #2.

Sort in the specified directory (argument 1 of the command line) and in all its subdirectories files by the following criteria (argument 2 of the command line, given as an integer): 1 - by file size, 2 - by file name. To save without saving the directory structure, the sorted files are shared
list, into the new directory (argument 3 of the command line).

In connection with the indexing of files in directories for file systems ext 2,3,4, before starting the program, it is necessary to temporarily disable the indexing option of the file system as follows:
sudo tune2fs -O ^ dir_index /dev /sdaXY

Check the result using, ls -l -f.

Laboratory task #3.

Write a program for finding the array of values ​​of the function y[i]=sin(2*PI*i/N) (i=0,1,2...N-1) using the Taylor series. The user sets the values ​​of N and the number of n members of the Taylor series. To calculate one member of the Taylor series, a separate process is started. Each process displays on the screen and in the intermediate results file

(create in the / tmp directory) its pid, i and the calculated value of the member of the series. The head process reads from the intermediate results file the values ​​of all calculated members of the Taylor series for each i, sums them up and the resulting value y[i] writes to the result file as: y[i]=value. Check the program for N=256 n=5; N=1024 n=10.

Laboratory task #4.

Create a process tree according to the variant of the individual task. Processes continuously exchange signals according to Table. 2. A record in Table 1 of the form: 1 -> (2,3,4,5) means that the initial process 0 creates a child process 1, which in turn creates child processes 2,3,4,5. An entry in table 2 of the form: 1 -> (2,3,4) SIGUSR1 means that process 1 sends the child processes 2,3,4 simultaneously (ie, for one kill () call) the signal SIGUSR1. Each process on receipt or sending a signal outputs information to the console in the following form:
N pid ppid sent / received USR1 / USR2 current time (usec)
where N is the number of the son according to Table. 1
Process 1, after receiving the 101st SIGUSR signal, sends the SIGTERM signal to the sons and waits for the completion of all the sons, and then ends up alone. Process 0 waits for process 1 to terminate, and then ends itself. Sons, having received a signal SIGTERM, finish the work with output to the console messages of the form:
pid ppid completed after the Xth signal SIGUSR1 and the Yth signal SIGUSR2
where X, Y is the number of SIGUSR1 and SIGUSR2 signals sent over the entire period of work of this son.

Processes tree:

2	1->(2,3,4)   2->(5,6)   6->7  7->8

Signals exchanges: 

1->(8,7,6) SIGUSR1   8->4 SIGUSR1  7->4SIGUSR2   
 6->4 SIGUSR1  4->(3,2) SIGUSR1 2->1 SIGUSR2
 
Laboratory task #5. 
 
Write a program for finding an array of N consecutive values of the function y [i] = sin (2 * PI * i / N) (i = 0,1,2 ... N-1) using the Taylor series.
The user sets the values of N and the number of n members of the Taylor series. To calculate each member of the Taylor series, a new separate thread is started.
Each thread displays its id and calculated value of the series on the screen and in the file. The head process reads from the file the values of all calculated members of the Taylor series,
sums them and the resulting value y [i] writes to the file. Check the program for N = 256 n = 5; N = 1024 n = 10.

Лабораторное задание #1.

Написать скрипт, находящий в заданном каталоге и всех его подкаталогах все файлы заданного расширения и создающий для каждого найденного файла жесткую ссылку в заданном каталоге. Расширение файла и каталог для жестких ссылок задаются в качестве первого и второго аргумента командной строки.

Лабораторное задание #2.

Отсортировать в заданном каталоге (аргумент 1 командной строки) и во всех его подкаталогах файлы по следующим критериям (аргумент 2 командной строки, задаётся в виде целого числа): 1 – по размеру файла, 2 – по имени файла. Записать без сохранения структуры каталогов отсортированные файлы общим
списком, в новый каталог (аргумент 3 командной строки). 

В связи с индексированием файлов в каталогах для файловых систем ext 2,3,4 перед запуском программы необходимо временно отключить опцию индексирования файловой системы следующим образом:
sudo tune2fs –O ^dir_index /dev/sdaXY

Проверить результат, используя, ls -l –f.

Лабораторное задание #3.

Написать программу нахождения массива значений функции y[i]=sin(2*PI*i/N) (i=0,1,2…N-1) с использованием ряда Тейлора. Пользователь задаёт значения N и количество n членов ряда Тейлора. Для расчета одного члена ряда Тейлора запускается отдельный процесс. Каждый процесс выводит на экран и в файл промежуточных результатов

(создать в каталоге /tmp) свой pid, i и рассчитанное значение члена ряда. Головной процесс считывает из файла промежуточных результатов значения всех рассчитанных членов ряда Тейлора для каждого i, суммирует их и полученное значение y[i] записывает в файл результата в виде: y[i] =значение. Проверить работу программы для N=256 n=5; N=1024 n=10.

Лабораторное задание #4.

Процессы непрерывно обмениваются сигналами. Запись в таблице 1 вида:  1->(2,3,4,5) означает, что исходный процесс 0 создаёт дочерний процесс 1, который, в свою очередь, создаёт дочерние процессы 2,3,4,5. Запись в таблице 2 вида:  1->(2,3,4) SIGUSR1 означает, что процесс 1 посылает  дочерним процессам  2,3,4 одновременно (т.е. за один вызов kill() ) сигнал SIGUSR1.Каждый процесс при получении или посылке сигнала выводит на консоль информацию в следующем виде:
N pid    ppid   послал/получил  USR1/USR2 текущее время (мксек)
где N-номер сына по табл. 1
Процесс 1, после получения  101 –го по счету сигнала SIGUSR,  посылает    сыновьям сигнал SIGTERM и ожидает  завершения всех сыновей, после чего завершается сам. Процесс 0 ожидает завершения работы процесса 1 после чего завершается сам. Сыновья, получив сигнал SIGTERM, завершают работу с выводом на консоль сообщения вида:
pid    ppid   завершил работу после X-го сигнала SIGUSR1 и Y-го сигнала SIGUSR2
где X,Y – количество посланных за все время работы данным сыном сигналов SIGUSR1 и SIGUSR2.

Дерево процессов:

2	1->(2,3,4)   2->(5,6)   6->7  7->8

Последовательность обмена сигналами: 

1->(8,7,6) SIGUSR1   8->4 SIGUSR1  7->4SIGUSR2   
 6->4 SIGUSR1  4->(3,2) SIGUSR1 2->1 SIGUSR2

Лабораторное задание #5.

Написать программу нахождения массива N последовательных значений функции y[i]=sin(2*PI*i/N) (i=0,1,2…N-1) с использованием ряда Тейлора. 
Пользователь задаёт значения N и количество n членов ряда Тейлора. Для расчета каждого члена ряда Тейлора запускается новый отдельный поток. 
Каждый поток выводит на экран и в файл свой id и рассчитанное значение ряда. Головной процесс считывает из файла значения всех рассчитанных членов ряда Тейлора, 
суммирует их и полученное значение y[i] записывает в файл. Проверить работу программы для N=256 n=5; N=1024 n=10.