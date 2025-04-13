#!/bin/sh
zip=${ORACC}/www/downloads/oracc-schemas.zip
rm -fr $zip
cp -f schemas.xml ${ORACC}/lib/schemas
(cd ${ORACC}/lib ; zip -r $zip schemas)
chmod o+r $zip
