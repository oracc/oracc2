#!/bin/dash
echo '<div id="p4GlossaryEntry">'
xmllint --xpath '/*/*[2]/*' $1
echo '</div>'
