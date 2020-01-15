#!/bin/bash -e

lockfile=/tmp/cron.lock

wdir="/var/www/html/dygraphs/data"

day_filter=$(env TZ=America/North_Dakota/Center date +"%m/%d" --date="7 days ago")

seven_day () {
	cat ${wdir}/${wfile} | grep -v ${day_filter} > /tmp/temp.csv && mv /tmp/temp.csv ${wdir}/${wfile}
}

new_day () {
	wfile="solar_voltage_data.csv" ; seven_day
	wfile="solar_current_data.csv" ; seven_day

	wfile="battery_voltage_data.csv" ; seven_day
	wfile="battery_current_data.csv" ; seven_day

	wfile="fivev_voltage_data.csv" ; seven_day
	wfile="fivev_current_data.csv" ; seven_day

	wfile="twelvev_voltage_data.csv" ; seven_day
	wfile="twelvev_current_data.csv" ; seven_day

	wfile="battery_hdc_temp_data.csv" ; seven_day

	wfile="pth_pressure_data.csv" ; seven_day
	wfile="pth_temp_data.csv" ; seven_day
	wfile="pth_humidity_data.csv" ; seven_day
}

new_day
