#!/bin/bash

port="/dev/ttyUSB0"

stty -F ${port} raw speed 57600 &> /dev/null

# Loop
while [ 1 ];
do
	READ=` grabserial -d /dev/ttyUSB0 -b 57600 -m "$" -q "^\n"`
	load_busvolt=$(echo $READ | sed 's/ /\n/g' | grep '$LOAD:' | grep BusVolt | awk -F ':' '{print $4}' | awk -F 'V*' '{print $1}' || true)
	load_current=$(echo $READ | sed 's/ /\n/g' | grep '$LOAD:' | grep Current | awk -F ':' '{print $4}' | awk -F 'mA*' '{print $1}' || true)

	charger_busvolt=$(echo $READ | sed 's/ /\n/g' | grep '$CHARGER:' | grep BusVolt | awk -F ':' '{print $4}' | awk -F 'V*' '{print $1}' || true)
	charger_current=$(echo $READ | sed 's/ /\n/g' | grep '$CHARGER:' | grep Current | awk -F ':' '{print $4}' | awk -F 'mA*' '{print $1}' || true)

	get_time=$(env TZ=America/North_Dakota/Center date +"%Y/%m/%d %k:%M:%S")

	if [ "x$load_busvolt" != "x" ] && [ "x$load_current" != "x" ] ; then
		echo "$get_time,$load_busvolt" >> /var/www/html/dygraphs/load_voltage_data.csv
		echo "$get_time,$load_current" >> /var/www/html/dygraphs/load_current_data.csv
		echo "$get_time,$load_busvolt,$charger_busvolt" >> /var/www/html/dygraphs/voltage_data.csv
	fi

	if [ "x$charger_busvolt" != "x" ] && [ "x$charger_current" != "x" ] ; then
		echo "$get_time,$charger_busvolt" >> /var/www/html/dygraphs/charger_voltage_data.csv
		echo "$get_time,$charger_current" >> /var/www/html/dygraphs/charger_current_data.csv
		echo "$get_time,$load_current,$charger_current" >> /var/www/html/dygraphs/current_data.csv
	fi
done
