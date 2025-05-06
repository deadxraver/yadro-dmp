MOD_NAME=dmp
obj-m+=$(MOD_NAME).o

all:	insert

insert:	build
	@sudo insmod $(MOD_NAME).ko 2> /dev/null && echo "inserted module" $(MOD_NAME) "into kernel" \
		|| echo "already inserted or name is taken"

remove:
	@sudo rmmod $(MOD_NAME) 2> /dev/null && echo "removed module" $(MOD_NAME) "from kernel" \
		|| echo "module already removed"

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
