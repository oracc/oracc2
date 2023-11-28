#!/bin/sh
project=$1
if [ "$project" = "" ];
then
    echo newproj-dirs.sh: must give project name as argument
    exit 1
fi
if [ ! -d ${ORACC_HOME}/$project ];
then
    echo newproj-dirs.sh: user directory ${ORACC_HOME}/$project does not exist
    exit 1
fi
if [ "$ORACC_MODE" = "multi" ]; then
    home=/home
else
    home=$ORACC_HOME
fi
cd $home/$project
echo creating project directories
for a in any atf bak bib bin cat lib map res tei web xml ; do \
    mkdir -p 00$a ; \
done
mkdir -p 00lib/lists 00res/downloads 00res/images 01tmp
mkdir -p ${ORACC}/{bld,pub,tmp,www,xml}/$project
# Just don't do this now that oracc build etc. autocreates these things
#ln -sf ${ORACC_HOME}/$project ${ORACC}/
echo setting permissions
if [[ $ORACC_MODE = "single" ]]; then
    ouser=$(cat $ORACC_BUILDS/.oraccuser)
    ogroup=$(cat $ORACC_BUILDS/.oraccgroup)
    chown -R $ouser:$ogroup $ORACC_HOME/$project ${ORACC}/{bld,pub,tmp,www,xml}/$project
else
    chown -R $project:$project 00* 01tmp ${ORACC}/{bld,pub,tmp,www,xml}/$project
fi
chmod u+rw ${ORACC}/{bld,pub,tmp,www,xml}/$project
chmod go+r ${ORACC}/{bld,pub,tmp,www,xml}/$project
chmod -R u+rw .
chmod -R g+r .
chmod -R o-rw *
echo done
exit 0
