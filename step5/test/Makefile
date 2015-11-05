all:
	@chmod a+x run.sh
	@cd ./simulator && make
	@./run.sh
clean:
	@cd ./simulator && make clean
	rm -rf result output
