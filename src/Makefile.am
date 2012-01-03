stylesheet.yy.c: stylesheet.lex
	flex -Psacc_stylesheet --header-file=stylesheet.yy.h -o stylesheet.yy.c stylesheet.lex

stylesheet.tab.c: stylesheet.y
	bison -p sacc_stylesheet --defines=stylesheet.tab.h -o stylesheet.tab.c stylesheet.y



clean-local:
	rm -rf stylesheet.yy.c stylesheet.yy.h
	rm -rf stylesheet.tab.c stylesheet.tab.h
