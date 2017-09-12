#!/bin/bash
 
scp .pioenvs/UNO/firmware.hex pi@$1:.
scp pi@$1:/home/pi/.node-red/flows_olasrpi.json flow_LIAM.json

