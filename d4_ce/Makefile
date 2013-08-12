all:
	bison d4_ce_parser.yy
	flex d4_ce_scanner.ll
	g++ -I../GNU -I. lex.yy.cc d4_ce_parser.tab.cc D4CEDriver.cc Main.cpp -o d4_ce -L../.libs -ldap
	
clean:
	rm -rf d4_ce_parser.tab.cc d4_ce_parser.tab.hh location.hh position.hh stack.hh
	rm -rf lex.yy.cc
	rm -rf d4_ce

hack:
	g++ lex.yy.c d4_ce_parser.tab.cc D4CEDriver.cc Main.cpp -o d4_ce