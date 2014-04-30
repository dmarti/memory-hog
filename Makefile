CFLAGS = -std=gnu99 -fPIC -levent
LDFLAGS = -shared 

all : memory-hog.so

memory-hog.so : memory-hog.c
	$(CC) -o $@ -std=gnu99 -fPIC -shared $^

hooks : .git/hooks/pre-commit

.git/hooks/% : Makefile
	echo "#!/bin/sh" > $@
	echo "make `basename $@`" >> $@
	chmod 755 $@

pre-commit :
	git diff-index --check HEAD

clean :
	find . -print0 | git check-ignore -z --stdin | xargs -0 rm -f

.PHONY : all clean hooks
