#!/bin/bash
ipcs -q | awk '{print $2}' | head -n -1 | while read in; do ipcrm msg $in; done
