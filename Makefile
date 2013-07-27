CROSS_COMPILE:=
TARGET=$(notdir $(CURDIR))
C_FLAGS+=-Wall -g
LD_FLAGS+=-lncurses
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJFILES=$(SOURCES:%.c=%.o)
all:clean $(TARGET)
$(TARGET):$(OBJFILES)
	$(CROSS_COMPILE)gcc $^ -o $@ $(LD_FLAGS)
$(OBJFILES):%.o:%.c $(HEADERS)
	$(CROSS_COMPILE)gcc $(C_FLAGS) -c $< -o $@

clean:
	@echo Removing files
	@rm -rf $(OBJFILES) $(TARGET) *~*.d.dep
