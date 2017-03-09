#!/bin/bash

port="/dev/ttyUSB0"

stty -F ${port} raw speed 57600 &> /dev/null

# Loop
while [ 1 ];
do
	echo 'LOADING...'
	READ=` grabserial -d /dev/ttyUSB0 -b 57600 -m "$" -q "^\n"`
	echo $READ
	load_busvolt=$(echo $READ | sed 's/ /\n/g' | grep '$LOAD:' | grep BusVolt | awk -F ':' '{print $4}' | awk -F 'V*' '{print $1}')
	load_current=$(echo $READ | sed 's/ /\n/g' | grep '$LOAD:' | grep Current | awk -F ':' '{print $4}' | awk -F 'mA*' '{print $1}')

	charger_busvolt=$(echo $READ | sed 's/ /\n/g' | grep '$CHARGER:' | grep BusVolt | awk -F ':' '{print $4}' | awk -F 'V*' '{print $1}')
	charger_current=$(echo $READ | sed 's/ /\n/g' | grep '$CHARGER:' | grep Current | awk -F ':' '{print $4}' | awk -F 'mA*' '{print $1}')

#	echo "$load_busvolt V"
#	echo "$load_current ma"
	get_time=$(date +"%Y/%m/%d %k:%M:%S")
#	echo "time:$get_time"
	if [ ! -f /var/www/html/dygraphs/data.csv ] ; then
		echo "Date,Load Voltage (V),Load Current (mA),Charger Voltage (V), Charger Current (mA)" > /var/www/html/dygraphs/data.csv
	fi
	if [ ! -f /var/www/html/dygraphs/load_voltage_data.csv  ] ; then
		echo "Date,Load Voltage (V)" > /var/www/html/dygraphs/load_voltage_data.csv
	fi
	if [ ! -f /var/www/html/dygraphs/load_current_data.csv  ] ; then
		echo "Date,Load Current (mA)" > /var/www/html/dygraphs/load_current_data.csv
	fi

        if [ ! -f /var/www/html/dygraphs/charger_voltage_data.csv  ] ; then
                echo "Date,Charger Voltage (V)" > /var/www/html/dygraphs/charger_voltage_data.csv
        fi
        if [ ! -f /var/www/html/dygraphs/charger_current_data.csv  ] ; then
                echo "Date,Charger Current (mA)" > /var/www/html/dygraphs/charger_current_data.csv
        fi

        if [ ! -f /var/www/html/dygraphs/voltage_data.csv  ] ; then
                echo "Date,Load Voltage (V),Charger Voltage (V)" > /var/www/html/dygraphs/voltage_data.csv
        fi

        if [ ! -f /var/www/html/dygraphs/current_data.csv  ] ; then
                echo "Date,Load Current (mA),Charger Current (mA)" > /var/www/html/dygraphs/current_data.csv
        fi

	echo "$get_time,$load_busvolt,$load_current,$charger_busvolt,$charger_current" >> /var/www/html/dygraphs/data.csv
	echo "$get_time,$load_busvolt" >> /var/www/html/dygraphs/load_voltage_data.csv
	echo "$get_time,$load_current" >> /var/www/html/dygraphs/load_current_data.csv

        echo "$get_time,$charger_busvolt" >> /var/www/html/dygraphs/charger_voltage_data.csv
        echo "$get_time,$charger_current" >> /var/www/html/dygraphs/charger_current_data.csv

	echo "$get_time,$load_busvolt,$charger_busvolt" >> /var/www/html/dygraphs/voltage_data.csv
	echo "$get_time,$load_current,$charger_current" >> /var/www/html/dygraphs/current_data.csv


    echo '[PRESS Ctrl + C TO EXIT]'
done
