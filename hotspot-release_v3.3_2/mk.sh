#!/bin/bash
cd ./sllib/&&./cp.sh
cd ..
cd ./img-make-hotspot/&&./make.sh&&./tftp.sh
cd ..

echo "ALL OK"

