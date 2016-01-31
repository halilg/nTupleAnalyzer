ifndef ROOTSYS                                                                                                                                         
$(error *** Please set up ROOT)                                                                                                                        
endif

CPP=g++
CPPFLAGS=-I.
# -Wall
LROOT=`root-config --glibs`
CROOT=`root-config --cflags`
LOTHER= #-lboost_system
OBJDIR=tmp
#OBJS := $(addprefix $(OBJDIR)/, jsoncpp.o json2tchain.o)
#nTupleAnalyzer.o

#all: $(OBJS)
all: nTupleAnalyzer.exe jsoncpp_cheat.exe readcfg.exe

nTupleAnalyzer.exe: nTupleAnalyzer.cc $(OBJDIR)/nTupleAnalyzer.o $(OBJDIR)/json2tchain.o $(OBJDIR)/jsoncpp.o $(OBJDIR)/analysis.o
	$(CPP) $(OBJDIR)/nTupleAnalyzer.o $(OBJDIR)/analysis.o $(OBJDIR)/jsoncpp.o $(OBJDIR)/json2tchain.o $(LROOT) -o $@

jsoncpp_cheat.exe: jsoncpp_cheat.cc $(OBJDIR)/jsoncpp.o
	$(CPP) jsoncpp_cheat.cc $(OBJDIR)/jsoncpp.o $(CPPFLAGS) -o $@

readcfg.exe: readcfg.cc $(OBJDIR)/cfgreader.o $(OBJDIR)/jsoncpp.o
	$(CPP) $(CPPFLAGS) readcfg.cc $(OBJDIR)/cfgreader.o $(OBJDIR)/jsoncpp.o -o $@

#$(OBJDIR)/%.o : %.cc
	#$(CPP) $(CPPFLAGS) $(CROOT) -o $@ $<



#$(OBJS): | $(OBJDIR)

$(OBJDIR)/analysis.o: analysis.cc analysis.h | $(OBJDIR)
	$(CPP) -c analysis.cc $(CPPFLAGS) -o $@

$(OBJDIR)/cfgreader.o: cfgreader.cc cfgreader.h | $(OBJDIR)
	$(CPP) -c cfgreader.cc $(CPPFLAGS) -o $@

$(OBJDIR)/nTupleAnalyzer.o: nTupleAnalyzer.cc nTupleAnalyzer.h | $(OBJDIR)
	$(CPP) -c nTupleAnalyzer.cc $(CPPFLAGS) $(CROOT) -o $@

$(OBJDIR)/jsoncpp.o: jsoncpp.cc json/json-forwards.h json/json.h | $(OBJDIR)
	$(CPP) -c jsoncpp.cc $(CPPFLAGS) -o $@

$(OBJDIR)/json2tchain.o: json2tchain.cc json2tchain.h | $(OBJDIR)
	$(CPP) -c json2tchain.cc $(CPPFLAGS) $(CROOT) -o $@


$(OBJDIR):
	mkdir -p $(OBJDIR)
	
clean:
	$(RM) $(OBJDIR)/*.o *.exe

# http://stackoverflow.com/questions/99132/how-to-prevent-directory-already-exists-error-in-a-makefile-when-using-mkdir#99174