all :
	cd ~/exam && git pull && gcc -c ft_printf.c && ar rc libftprintf.a ft_printf.o && cd pft_exam && make && ./test && cat results.txt