LIBS		=	-lc C:/SaturnOrbit/SH_$(OBJFMT)/sh-$(OBJFMT)/lib/gcc/sh-$(OBJFMT)/3.4-GNUSH_v0501/libgcc.a
OBJDIR	=	.
OBJ		=     $(OBJDIR)/CRT0.o	\
			$(OBJDIR)/main.o	

CC		=	sh-$(OBJFMT)-gcc
AS		=	sh-$(OBJFMT)-as
LD		=	sh-$(OBJFMT)-ld

EXE_FILENAME		=	main_$(OBJFMT).bin
ISO_FILENAME		=	$(EXE_FILENAME:.bin=.iso)
IPFILE 	=	C:/SaturnOrbit/COMMON/IP.BIN

LDFLAGS	=	-nostartfiles --script bart.lnk -Map $(EXE_FILENAME:.bin=.map)
FLAGS		=	-Wall -Werror -m2 #-O2 -fomit-frame-pointer

all	:	$(EXE_FILENAME) $(ISO_FILENAME)

$(EXE_FILENAME)	:	$(OBJ)
		$(LD) $(LDFLAGS) $(OBJ) $(LIBS) -o $(EXE_FILENAME)
		$(LD) $(LDFLAGS) $(OBJ) $(LIBS) -o cd/0.bin

$(ISO_FILENAME)	: $(EXE_FILENAME)
		mkisofs -quiet -sysid "SEGA SATURN" -volid "SaturnApp" -volset "SaturnApp" -publisher "SEGA ENTERPRISES, LTD." -preparer "SEGA ENTERPRISES, LTD." -appid "SaturnApp" -abstract "ABS.TXT" -copyright "CPY.TXT" -biblio "BIB.TXT" -generic-boot $(IPFILE) -full-iso9660-filenames -o $(ISO_FILENAME) cd  
	        
$(OBJDIR)/%.o : 	%.c %.h
		$(CC) -c $< -o $@ $(FLAGS) $(_FLAGS)
	        
$(OBJDIR)/%.o :	%.S
		$(AS) $< -o $@
	        
pack	:	        
		sh-$(OBJFMT)-strip $(EXE_FILENAME) --strip-all
clean	:	        
		rm -f $(OBJ)
		rm -f $(EXE_FILENAME)
		rm -f $(EXE_FILENAME:.bin=.map)
		rm -f $(ISO_FILENAME)
		rm -f cd/0.bin

makedir :
		mkdir obj