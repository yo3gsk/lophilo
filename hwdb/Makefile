.PHONY: toxml

all: library_report.xml

/usr/bin/tidy:
	sudo apt-get install tidy

/usr/bin/xml_pp:
	sudo apt-get install xml-twig-tools

/usr/bin/xqilla:
	sudo apt-get install xqilla

library_report.xml: library_report.doc /usr/bin/tidy /usr/bin/xml_pp
	tidy -c -b -n -asxhtml library_report.doc  | xml_pp > library_report.xml

library_report_modified_output.xml: library_report_modified.xml parser.xquery /usr/bin/xml_pp /usr/bin/xqilla
	xqilla parser.xquery  | xml_pp > library_report_modified_output.csv
