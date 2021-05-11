#!/bin/bash

#
#   Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
#   Sergey Manoylo <info@smansoft.com>
#

export SM_FILE_BUILD=Debug;

make clean;
#make SM_FILE_BUILD=Debug;
make;
make install;
