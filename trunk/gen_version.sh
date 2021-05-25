#!/bin/bash

echo -n "Build version : "
echo `git describe --tags --always`.`git rev-list --count HEAD`
echo -n "Build date : "
echo `date`
echo -n "Merlin Compiler version : "
month=`date +%m`
month=${month#0}
echo `date +%Y`.$(((${month}-1)/3+1))
