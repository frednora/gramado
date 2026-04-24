
all:

# Entries: 
#  + Boot time entry
#  + AP initialization entry
#  + Interrupt time entry
	make -C arch/

# Top level routines
	make -C kwrap/

# libk and rtl
	make -C libk/

# Kernel-side support for user interaction
	make -C wink/


