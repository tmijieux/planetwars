all: bots server

.PHONY: bots server jserver rapport clean mrproper doc

bots:
	make -C bots/

server:
	make -C server/

clean:
	make -C bots/ clean
	make -C server/ clean

mrproper: clean
	make -C bots/ mrproper
	make -C server/ mrproper
