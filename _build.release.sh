#!/bin/bash

#
#   Copyright (c) 2021 SManSoft <http://www.smansoft.com/>
#   Sergey Manoylo <info@smansoft.com>
#

export SM_FILE_BUILD=Release;

make clean;
#make SM_FILE_BUILD=Release;
make;
make install;
