build:
	gcc process_generator.c -o process_generator.out
	gcc ./Clock/clk.c -o clk.out
	gcc ./Scheduler/scheduler.c -o scheduler.out
	gcc ./Process/process.c -o process.out
	gcc ./InputGenerator/test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./test_generator.out && ./process_generator.out
