SUBDIR = $(shell ls -d */)

.PHONY:clean default all $(SUBDIR)

default:all

all clean:
	$(MAKE) $(SUBDIR) TARGET=$@

$(SUBDIR):
	$(MAKE) -C $@ $(TARGET)

