EXE= shell
FLAGS= -std=gnu90
DIR= bin

BLUE='\x1b[34m'
GREEN='\x1B[32m'
RED='\x1B[31m'
RESET='\x1B[0m'

help: build
	@echo
	@echo -e $(RED)Progetto 1: shell custom$(RESET)
	@echo -e $(BLUE)Componenti del gruppo:$(RESET)
	@echo -e 185701 Giust'\t\t'Alberto
	@echo -e 187599 Bulbarelli'\t'Davide
	@echo -e 196925 Piva'\t\t'Davide
	@echo -e 186145 Agnello'\t\t'Michele
	@echo

build: clean
	@gcc $(FLAGS) -o $(DIR)/$(EXE) src/shell.c src/FormatUtils.c src/FunctionUtils.c src/MyWrappers.c src/SplitLines.c src/ExecuteCommands.c

clean: check
	@rm -f $(DIR)/*

check:
	@if ! [ -d "$(DIR)" ]; then  mkdir $(DIR); fi
