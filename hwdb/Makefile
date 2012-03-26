.PHONY: toxml

all: library_report.xml

/usr/bin/tidy:
	sudo apt-get install tidy

library_report.xml: library_report.doc /usr/bin/tidy
	tidy -c -b -n -asxhtml library_report.doc  > library_report.xml

fixup:
	%s/&nbsp;/\&\#160;/g
