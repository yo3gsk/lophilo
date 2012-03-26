<lophilo>
{
for $pins in doc("library_report_modified.xml")//h5
for $table in $pins/following-sibling::table[1]
for $row in $table/tr
let $component := $pins/preceding-sibling::table[1]/tr[2]/td[2]/span/text()
let $designator := $row/td[1]/span/text()
let $pinname := $row/td[2]/span/text()
where contains($pins/text(), 'Pins')
return 
		<pin> 
				<component>{$component}</component>
				<name>{$pinname}</name>
				<designator>{$designator}</designator>
			</pin>
}
</lophilo>
