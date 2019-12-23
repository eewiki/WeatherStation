#!/bin/bash

lockfile=/tmp/cron.lock

wdir="/var/www/html/dygraphs/data"

day=$(env TZ=America/North_Dakota/Center date +"%d")
day_filter=$(env TZ=America/North_Dakota/Center date +"%m/%d" --date="5 days ago")

seven_day () {
	cat ${wdir}/${wfile} | grep -v ${day_filter} > /tmp/temp.csv && mv /tmp/temp.csv ${wdir}/${wfile}
}

cut_half () {
	awk '!(NR%2)' ${wdir}/${wfile} > /tmp/temp.csv && mv /tmp/temp.csv ${wdir}/${wfile}
}

new_day () {
	day=${get_day}
	wbt_max=${wbt}
	wbt_min=${wbt}
#	wfile="load_voltage_data.csv" ; seven_day ; cut_half
#	wfile="load_current_data.csv" ; seven_day ; cut_half
#	wfile="voltage_data.csv" ; seven_day ; cut_half

#	wfile="charger_voltage_data.csv" ; seven_day ; cut_half
#	wfile="charger_current_data.csv" ; seven_day ; cut_half
#	wfile="current_data.csv" ; seven_day ; cut_half

#	wfile="wbh_data.csv" ; seven_day ; cut_half
#	wfile="wbp_data.csv" ; seven_day ; cut_half
#	wfile="wbt_data.csv" ; seven_day ; cut_half
}

run () {
	port="/dev/ttyUSB0"

	stty -F ${port} raw speed 57600 &> /dev/null

	wbt_min="140"
	wbt_max="-100"
	wbt_min_error="-100"

	wlt_min="140"
	wlt_max="-100"
	wlt_min_error="-100"

	wbh_max="100"
	wlh_max="100"

	wlp_max="1800"

	# Loop
	while [ 1 ];
	do
		READ=`grabserial -d /dev/ttyUSB0 -b 57600 -m "MsgFrom:" -q ":#"`
		echo "[$READ]"

		solar_volt=$(echo $READ | grep -a '0013A20041A7AE31:Solar:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		echo "solar_volt=[$solar_volt]"
		solar_mamp=$(echo $READ | grep -a '0013A20041A7AE31:Solar:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		echo "solar_mamp=[$solar_mamp]"

		battery_volt=$(echo $READ | grep -a '0013A20041A7AE31:Battery:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		echo "battery_volt=[$battery_volt]"
		battery_mamp=$(echo $READ | grep -a '0013A20041A7AE31:Battery:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		echo "battery_mamp=[$battery_mamp]"

		fivev_volt=$(echo $READ | grep -a '0013A20041A7AE31:5V_Rail:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		echo "fivev_volt=[$fivev_volt]"
		fivev_mamp=$(echo $READ | grep -a '0013A20041A7AE31:5V_Rail:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		echo "fivev_mamp=[$fivev_mamp]"

		twelvev_volt=$(echo $READ | grep -a '0013A20041A7AE31:12V_Rail:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		echo "twelvev_volt=[$twelvev_volt]"
		twelvev_mamp=$(echo $READ | grep -a '0013A20041A7AE31:12V_Rail:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		echo "twelvev_mamp=[$twelvev_mamp]"

		get_time=$(env TZ=America/North_Dakota/Center date +"%Y/%m/%d %k:%M:%S")
		get_time_weather=$(env TZ=America/North_Dakota/Center date +"%Y-%m-%dT%k:%M:%S")
		get_day=$(env TZ=America/North_Dakota/Center date +"%d")

		if [ "x$solar_volt" != "x" ] && [ "x$solar_mamp" != "x" ] ; then
			echo "$get_time,$solar_volt" >> ${wdir}/solar_voltage_data.csv
			echo "$get_time,$solar_mamp" >> ${wdir}/solar_current_data.csv
		fi

		if [ "x$battery_volt" != "x" ] && [ "x$battery_mamp" != "x" ] ; then
			echo "$get_time,$battery_volt" >> ${wdir}/battery_voltage_data.csv
			echo "$get_time,$battery_mamp" >> ${wdir}/battery_current_data.csv
		fi

		if [ "x$fivev_volt" != "x" ] && [ "x$fivev_mamp" != "x" ] ; then
			echo "$get_time,$fivev_volt" >> ${wdir}/fivev_voltage_data.csv
			echo "$get_time,$fivev_mamp" >> ${wdir}/fivev_current_data.csv
		fi

		if [ "x$twelvev_volt" != "x" ] && [ "x$twelvev_mamp" != "x" ] ; then
			echo "$get_time,$twelvev_volt" >> ${wdir}/twelvev_voltage_data.csv
			echo "$get_time,$twelvev_mamp" >> ${wdir}/twelvev_current_data.csv
		fi

#
#		if [ "x$wbh" != "x" ] ; then
#			if [ 1 -eq "$(echo "${wbh} > ${wbh_max}" | bc)" ] ; then
#				wbh=${wbh_max}
#			fi
#
#			echo "wbh=[$wbh]"
#			echo "$get_time,$wbh" >> ${wdir}/wbh_data.csv
#		fi
#
#		if [ "x$wlh" != "x" ] ; then
#			if [ 1 -eq "$(echo "${wlh} > ${wlh_max}" | bc)" ] ; then
#				wlh=${wlh_max}
#			fi
#
#			echo "wlh=[$wlh]"
#			echo "$get_time,$wlh" >> ${wdir}/wlh_data.csv
#		fi
#
#		if [ "x$wbp" != "x" ] ; then
#			echo "wbp=[$wbp]"
#			echo "$get_time,$wbp" >> ${wdir}/wbp_data.csv
#		fi
#
#		if [ "x$wlp" != "x" ] ; then
#			if [ 1 -eq "$(echo "${wlp} > ${wlp_max}" | bc)" ] ; then
#				unset wlp
#			else
#				echo "wlp=[$wlp]"
#				echo "$get_time,$wlp" >> ${wdir}/wlp_data.csv
#			fi
#		fi
#
#		if [ "x$wbt" != "x" ] ; then
#			if [ 1 -eq "$(echo "${wbt} < ${wbt_min_error}" | bc)" ] ; then
#				unset wbt
#			fi
#		fi
#
#		if [ "x$wlt" != "x" ] ; then
#			if [ 1 -eq "$(echo "${wlt} < ${wlt_min_error}" | bc)" ] ; then
#				unset wlt
#			fi
#		fi
#
#		if [ "x$wlt" != "x" ] ; then
#			echo "wlt=[$wlt]"
#			echo "$get_time,$wlt" >> ${wdir}/wlt_data.csv
#		fi
#
#		if [ "x$wbt" != "x" ] ; then
#			echo "wbt=[$wbt]"
#			echo "$get_time,$wbt" >> ${wdir}/wbt_data.csv
#
#			if [ "x${get_day}" != "x${day}" ] ; then
#				new_day
#			fi
#
#			if [ 1 -eq "$(echo "${wbt} < ${wbt_min}" | bc)" ] ; then
#				wbt_min=${wbt}
#			fi
#			echo "min=[$wbt_min]"
#
#			if [ 1 -eq "$(echo "${wbt} > ${wbt_max}" | bc)" ] ; then
#				wbt_max=${wbt}
#			fi
#			echo "max=[$wbt_max]"
#		fi
#
#		if [ "x$wbh" != "x" ] && [ "x$wbp" != "x" ] && [ "x$wbt" != "x" ] ; then
#			wfile="${wdir}/current.xml"
#			echo "<current>" > ${wfile}
#			echo "    <temperature value=\"$wbt\" min=\"$wbt_min\" max=\"$wbt_max\" unit=\"fahrenheit\"/>" >> ${wfile}
#			echo "    <humidity value=\"$wbh\" unit=\"%\">" >> ${wfile}
#			echo "    <pressure value=\"$wbp\" unit=\"hPa\">" >> ${wfile}
#	#		echo "    <wind>" >> ${wfile}
#	#		echo "     <speed value=/"7.78/" name=/"Moderate breeze/">" >> ${wfile}
#	#		echo "     <direction value=/"140/" code=/"SE/" name="SouthEast">" >> ${wfile}
#	#		echo "    </wind>" >> ${wfile}
#			echo "    <lastupdate value=\"$get_time_weather\">" >> ${wfile}
#			echo "</current>" >> ${wfile}
#		fi

	done
}

if [ ! -f ${lockfile} ] ; then
	touch ${lockfile}

	run
fi

