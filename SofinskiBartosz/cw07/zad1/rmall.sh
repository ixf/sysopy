#!/bin/bash
ipcs -m | grep 644 | awk '{print $2}' | while read in; do ipcrm shm $in; done
ipcs -s | grep 644 | awk '{print $2}' | while read in; do ipcrm sem $in; done
