#!/bin/bash

echo "tar cvzf SofinskiBartosz-cw$1.tar.gz SofinskiBartosz/cw$1"
source='SofinskiBartosz-cw'$1'.tar.gz'
to='SofinskiBartosz/cw'$1
tar cvzf $source $to
