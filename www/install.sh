#!/bin/bash

git pull --rebase

cp -v dygraph.css /var/www/html/dygraphs/
cp -v dygraph.min.js /var/www/html/dygraphs/
cp -v power.html /var/www/html/dygraphs/

