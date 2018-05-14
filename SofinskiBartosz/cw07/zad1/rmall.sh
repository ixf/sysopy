#!/bin/bash
ipcs -m | grep bsofinsk | awk '{print $2}' | while read in; do ipcrm shm $in; done
ipcs -s | grep bsofinsk | awk '{print $2}' | while read in; do ipcrm sem $in; done
