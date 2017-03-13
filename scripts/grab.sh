#!/bin/bash

lockfile=/tmp/cron.lock

wdir="/var/www/html/dygraphs/data"

new_day () {
	day=${get_day}
	wbt_max=${wbt}
	wbt_min=${wbt}
	awk '!(NR%2)' ${wdir}/load_voltage_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/load_voltage_data.csv
	awk '!(NR%2)' ${wdir}/load_current_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/load_current_data.csv
	awk '!(NR%2)' ${wdir}/voltage_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/voltage_data.csv

	awk '!(NR%2)' ${wdir}/charger_voltage_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/charger_voltage_data.csv
	awk '!(NR%2)' ${wdir}/charger_current_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/charger_current_data.csv
	awk '!(NR%2)' ${wdir}/current_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/current_data.csv

	awk '!(NR%2)' ${wdir}/wbh_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/wbh_data.csv
	awk '!(NR%2)' ${wdir}/wbp_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/wbp_data.csv
	awk '!(NR%2)' ${wdir}/wbt_data.csv > ${wdir}/temp.csv && mv ${wdir}/temp.csv ${wdir}/wbt_data.csv
}

run () {
	port="/dev/ttyUSB0"

	stty -F ${port} raw speed 57600 &> /dev/null

	wbt_min="140"
	wbt_max="-100"
	day="40"
	wbt_min_error="-100"

	# Loop
	while [ 1 ];
	do
		READ=`grabserial -d /dev/ttyUSB0 -b 57600 -m "$" -q "^\n"`
		echo "[$READ]"
		load_busvolt=$(echo $READ | sed 's/ /\n/g' | grep '$LOAD:' | grep BusVolt | tail -1 | awk -F ':' '{print $4}' | awk -F 'V*' '{print $1}' || true)
		echo "load_busvolt=[$load_busvolt]"
		load_current=$(echo $READ | sed 's/ /\n/g' | grep '$LOAD:' | grep Current | tail -1 | awk -F ':' '{print $4}' | awk -F 'mA*' '{print $1}' || true)
		echo "load_current=[$load_current]"

		charger_busvolt=$(echo $READ | sed 's/ /\n/g' | grep '$CHARGER:' | grep BusVolt | tail -1 | awk -F ':' '{print $4}' | awk -F 'V*' '{print $1}' || true)
		echo "charger_busvolt=[$charger_busvolt]"
		charger_current=$(echo $READ | sed 's/ /\n/g' | grep '$CHARGER:' | grep Current | tail -1 | awk -F ':' '{print $4}' | awk -F 'mA*' '{print $1}' || true)
		echo "charger_current=[$charger_current]"

		wbh=$(echo $READ | sed 's/ /\n/g' | grep '$WBH:' | grep Humid | tail -1 | awk -F ':' '{print $4}' | awk -F 'P*' '{print $1}' || true)
		wbp=$(echo $READ | sed 's/ /\n/g' | grep '$WBP:' | grep Pres | tail -1 | awk -F ':' '{print $4}' | awk -F 'Pa*' '{print $1}' || true)
		wbt=$(echo $READ | sed 's/ /\n/g' | grep '$WBT:' | grep TempF | tail -1 | awk -F ':' '{print $4}' | awk -F 'F*' '{print $1}' || true)

		get_time=$(env TZ=America/North_Dakota/Center date +"%Y/%m/%d %k:%M:%S")
		get_time_weather=$(env TZ=America/North_Dakota/Center date +"%Y-%m-%dT%k:%M:%S")
		get_day=$(env TZ=America/North_Dakota/Center date +"%d")

		if [ "x$load_busvolt" != "x" ] && [ "x$load_current" != "x" ] ; then
			echo "$get_time,$load_busvolt" >> ${wdir}/load_voltage_data.csv
			echo "$get_time,$load_current" >> ${wdir}/load_current_data.csv
			if [ "x$charger_busvolt" != "x" ] ; then
				echo "$get_time,$load_busvolt,$charger_busvolt" >> ${wdir}/voltage_data.csv
				echo "save:[$get_time,$load_busvolt,$charger_busvolt]"
			fi
		fi

		if [ "x$charger_busvolt" != "x" ] && [ "x$charger_current" != "x" ] ; then
			echo "$get_time,$charger_busvolt" >> ${wdir}/charger_voltage_data.csv
			echo "$get_time,$charger_current" >> ${wdir}/charger_current_data.csv
			if [ "x$load_current" != "x" ] ; then
				echo "$get_time,$load_current,$charger_current" >> ${wdir}/current_data.csv
				echo "save:[$get_time,$load_current,$charger_current]"
			fi
		fi

		if [ "x$wbh" != "x" ] ; then
			echo "wbh=[$wbh]"
			echo "$get_time,$wbh" >> ${wdir}/wbh_data.csv
		fi

		if [ "x$wbp" != "x" ] ; then
			echo "wbp=[$wbp]"
			echo "$get_time,$wbp" >> ${wdir}/wbp_data.csv
		fi

		if [ "x$wbt" != "x" ] ; then
			if [ 1 -eq "$(echo "${wbt} < ${wbt_min_error}" | bc)" ] ; then
				unset wbt
			fi
		fi

		if [ "x$wbt" != "x" ] ; then
			echo "wbt=[$wbt]"
			echo "$get_time,$wbt" >> ${wdir}/wbt_data.csv

			if [ "x${get_day}" != "x${day}" ] ; then
				new_day
			fi

			if [ 1 -eq "$(echo "${wbt} < ${wbt_min}" | bc)" ] ; then
				wbt_min=${wbt}
			fi
			echo "min=[$wbt_min]"

			if [ 1 -eq "$(echo "${wbt} > ${wbt_max}" | bc)" ] ; then
				wbt_max=${wbt}
			fi
			echo "max=[$wbt_max]"
		fi

		if [ "x$wbh" != "x" ] && [ "x$wbp" != "x" ] && [ "x$wbt" != "x" ] ; then
			wfile="${wdir}/current.xml"
			echo "<current>" > ${wfile}
			echo "    <temperature value=\"$wbt\" min=\"$wbt_min\" max=\"$wbt_max\" unit=\"fahrenheit\"/>" >> ${wfile}
			echo "    <humidity value=\"$wbh\" unit=\"%\">" >> ${wfile}
			echo "    <pressure value=\"$wbp\" unit=\"hPa\">" >> ${wfile}
	#		echo "    <wind>" >> ${wfile}
	#		echo "     <speed value=/"7.78/" name=/"Moderate breeze/">" >> ${wfile}
	#		echo "     <direction value=/"140/" code=/"SE/" name="SouthEast">" >> ${wfile}
	#		echo "    </wind>" >> ${wfile}
			echo "    <lastupdate value=\"$get_time_weather\">" >> ${wfile}
			echo "</current>" >> ${wfile}
		fi

	done
}

if [ ! -f ${lockfile} ] ; then
	touch ${lockfile}

	run
fi

