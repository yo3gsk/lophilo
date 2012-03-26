.PHONY: toxml
toxml: library_report.doc
	tidy -asxhtml library_report.doc  > library_report.xml

fixup:
	%s/&nbsp;/\&\#160;/g
