for $pins in doc("library_report_modified.xml")//h5
for $table in $pins/following-sibling::table[1]
for $row in $table/tr
let $component := $pins/preceding-sibling::table[1]/tr[2]/td[2]/span/text()
let $designator := $row/td[1]/span/text()
let $pinname := $row/td[2]/span/text()
let $output := ($component, $designator, $pinname)
where contains($pins/text(), 'Pins')
and not(contains($row/td[1]/span/text(), 'Designator'))
return fn:normalize-space(fn:string-join($output, ';'))
