# OS-and-system-programming (OSISP)

Laboratory task #1. 

Write a script that finds all the files of the specified extension in the specified directory and all its subdirectories and creates a hard link for each file found in the specified directory. The file extension and directory for hard links are specified as the first and second command-line arguments.

Laboratory task #2.

Sort in the specified directory (argument 1 of the command line) and in all its subdirectories files by the following criteria (argument 2 of the command line, given as an integer): 1 - by file size, 2 - by file name. To save without saving the directory structure, the sorted files are shared
list, into the new directory (argument 3 of the command line).

In connection with the indexing of files in directories for file systems ext 2,3,4, before starting the program, it is necessary to temporarily disable the indexing option of the file system as follows:
sudo tune2fs -O ^ dir_index /dev /sdaXY

Check the result using, ls -l -f.

Лабораторное задание #1.

Написать скрипт, находящий в заданном каталоге и всех его подкаталогах все файлы заданного расширения и создающий для каждого найденного файла жесткую ссылку в заданном каталоге. Расширение файла и каталог для жестких ссылок задаются в качестве первого и второго аргумента командной строки.

Лабораторное задание #2.

Отсортировать в заданном каталоге (аргумент 1 командной строки) и во всех его подкаталогах файлы по следующим критериям (аргумент 2 командной строки, задаётся в виде целого числа): 1 – по размеру файла, 2 – по имени файла. Записать без сохранения структуры каталогов отсортированные файлы общим
списком, в новый каталог (аргумент 3 командной строки). 

В связи с индексированием файлов в каталогах для файловых систем ext 2,3,4 перед запуском программы необходимо временно отключить опцию индексирования файловой системы следующим образом:
sudo tune2fs –O ^dir_index /dev/sdaXY

Проверить результат, используя, ls -l –f.