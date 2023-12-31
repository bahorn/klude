TARGET = stage1
ccflags-y := -std=gnu99 \
	-Wno-declaration-after-statement
	
obj-m += $(TARGET).o

stage1-objs += ./src/stage1.o ./src/cmd.o \
	./src/hooks/all.o ./src/hooks/random_dev.o \
	./src/utils/ftrace_helper.o ./src/utils/mem.o
