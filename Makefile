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
LDFLAGS=-Wl,-rpath,$(ROOTSYS)/lib # Fix for El Capitan

# Executables

all: nTupleAnalyzer.exe jsoncpp_cheat.exe readcfg.exe

nTupleAnalyzer.exe: nTupleAnalyzer.cc $(OBJDIR)/nTupleAnalyzer.o $(OBJDIR)/json2tchain.o $(OBJDIR)/jsoncpp.o $(OBJDIR)/analysis.o $(OBJDIR)/assign_branches.o
	$(CPP) $(OBJDIR)/nTupleAnalyzer.o $(OBJDIR)/analysis.o $(OBJDIR)/jsoncpp.o $(OBJDIR)/json2tchain.o $(OBJDIR)/assign_branches.o $(LROOT) $(LDFLAGS) -o $@

jsoncpp_cheat.exe: jsoncpp_cheat.cc $(OBJDIR)/jsoncpp.o
	$(CPP) jsoncpp_cheat.cc $(OBJDIR)/jsoncpp.o $(CPPFLAGS) $(LDFLAGS) -o $@

readcfg.exe: readcfg.cc $(OBJDIR)/cfgreader.o $(OBJDIR)/jsoncpp.o
	$(CPP) $(CPPFLAGS) readcfg.cc $(OBJDIR)/cfgreader.o $(OBJDIR)/jsoncpp.o $(LDFLAGS) -o $@


#Objects

$(OBJDIR)/assign_branches.o: assign_branches.cc | $(OBJDIR)
	$(CPP) -c assign_branches.cc $(CPPFLAGS) $(CROOT) -o $@

$(OBJDIR)/analysis.o: analysis.cc analysis.h | $(OBJDIR)
	$(CPP) -c analysis.cc $(CPPFLAGS) $(CROOT) -o $@

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
	$(RM) *.exe $(OBJDIR)/*.o