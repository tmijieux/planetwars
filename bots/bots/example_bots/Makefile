INSTALL_DIR=../../bin/java

TARGETS=$(INSTALL_DIR)/RandomBot.jar \
	$(INSTALL_DIR)/DualBot.jar \
	$(INSTALL_DIR)/ProspectorBot.jar \
	$(INSTALL_DIR)/RageBot.jar\
	$(INSTALL_DIR)/BullyBot.jar \

JAVAC=javac

OBJS=Planet.class Fleet.class PlanetWars.class

CFLAGS=-Wall -std=c99
LIBS=-lm


all: $(TARGETS)

.PHONY: java all clean mrproper

java:
	make -C example_bots

$(INSTALL_DIR)/%Bot.jar: $(OBJS) %Bot.class
	echo "Main-Class:" $(shell echo "$@" | cut -d/ -f5 | sed 's/\.jar//g')  > Manifest.txt
	jar cvfm $@ Manifest.txt $^ 

%.class: %.java
	$(JAVAC) $<

clean:
	$(RM) *~ $(OBJS) *.bak
	$(RM) Manifest.txt

mrproper: clean
	$(RM) $(TARGETS)
