CC	= g++

CFLAGS += -g
CFLAGS += -I./CentOS 
CFLAGS += -I./SOURCE

SRCS = ./CentOS/OS_CPU_C.C ./CentOS/Utils.C ./SOURCE/uCOS_II.C 

EXEC = ./TEST//bin.exec  
EXEC_1 = ./EX1_x86L/bin.exec
EXEC_2 = ./EX2_x86L/bin.exec
EXEC_3 = ./EX3_x86L/bin.exec

all: $(EXEC) $(EXEC_1) $(EXEC_2) $(EXEC_3) 

$(EXEC):$(SRCS)  #complie appcation
	$(CC) $(CFLAGS) $(SRCS) -I./TEST ./TEST/TEST.C -o $(EXEC) 
	@echo "-- Build Target Appcation ($(EXEC)) Successful --"

$(EXEC_1):$(SRCS)  #complie appcation
	$(CC) $(CFLAGS) $(SRCS) -I./EX1_x86L ./EX1_x86L/TEST.C -o $(EXEC_1) 
	@echo "-- Build Target Appcation ($(EXEC_1)) Successful --"

$(EXEC_2):$(SRCS)  #complie appcation
	$(CC) $(CFLAGS) $(SRCS) -I./EX2_x86L ./EX2_x86L/TEST.C -o $(EXEC_2) 
	@echo "-- Build Target Appcation ($(EXEC_2)) Successful --"

$(EXEC_3):$(SRCS)  #complie appcation
	$(CC) $(CFLAGS) $(SRCS) -I./EX3_x86L ./EX3_x86L/TEST.C -o $(EXEC_3) 
	@echo "-- Build Target Appcation ($(EXEC_3)) Successful --"

clean:
	@echo ">>>>>>>>>>>> make clean <<<<<<<<<<<<<<"

	@rm -rf $(EXEC) $(EXEC_1) $(EXEC_2) $(EXEC_3) 