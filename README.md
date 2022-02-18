# skunwind
使用Linux自带的unwind接口实现对程序段错误信息的捕捉


# compile
gcc test.c skunwind.c -o test -ldl -funwind-tables -rdynamic

