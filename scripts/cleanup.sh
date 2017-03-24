#!/bin/bash

lockfile=/tmp/cron.lock

wdir="/var/www/html/dygraphs/data"

day_filter=$(env TZ=America/North_Dakota/Center date +"%m/%d" --date="14 days ago")

seven_day () {
	cat ${wdir}/${wfile} | grep -v '$day_filter' > /tmp/temp.csv && mv /tmp/temp.csv ${wdir}/${wfile}
}

new_day () {
	wfile="load_voltage_data.csv" ; seven_day
	wfile="load_current_data.csv" ; seven_day
	wfile="voltage_data.csv" ; seven_day

	wfile="charger_voltage_data.csv" ; seven_day
	wfile="charger_current_data.csv" ; seven_day
	wfile="current_data.csv" ; seven_day

	wfile="wbh_data.csv" ; seven_day
	wfile="wbp_data.csv" ; seven_day
	wfile="wbt_data.csv" ; seven_day
}

new_day
