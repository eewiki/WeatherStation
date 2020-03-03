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

	battery_hdc_min="-100"
	max_temperature="140"
	max_pressure="2000"

	echo "Logging to: /var/www/html/dygraphs/data/log.txt"

	# Loop
	while [ 1 ];
	do
		READ=`grabserial -d /dev/ttyUSB0 -b 57600 -m "MsgFrom:" -q ":#"`
		echo "[$READ]:[`date`]" >> /var/www/html/dygraphs/data/log.txt

		solar_volt=$(echo $READ | grep -a '0013A20041A7AE31:Solar:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		echo "solar_volt=[$solar_volt]" >> /var/www/html/dygraphs/data/log.txt

		solar_mamp=$(echo $READ | grep -a '0013A20041A7AE31:Solar:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		echo "solar_mamp=[$solar_mamp]" >> /var/www/html/dygraphs/data/log.txt

		battery_volt=$(echo $READ | grep -a '0013A20041A7AE31:Battery:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		echo "battery_volt=[$battery_volt]" >> /var/www/html/dygraphs/data/log.txt

		battery_mamp=$(echo $READ | grep -a '0013A20041A7AE31:Battery:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		echo "battery_mamp=[$battery_mamp]" >> /var/www/html/dygraphs/data/log.txt

		fivev_volt=$(echo $READ | grep -a '0013A20041A7AE31:5V_Rail:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		if [ "x$fivev_volt" != "xIN" ] ; then
			echo "fivev_volt=[$fivev_volt]" >> /var/www/html/dygraphs/data/log.txt
		else
			unset fivev_volt
		fi

		fivev_mamp=$(echo $READ | grep -a '0013A20041A7AE31:5V_Rail:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		if [ "x$fivev_mamp" != "xINVALID" ] ; then
			echo "fivev_mamp=[$fivev_mamp]" >> /var/www/html/dygraphs/data/log.txt
		else
			unset fivev_mamp
		fi


		twelvev_volt=$(echo $READ | grep -a '0013A20041A7AE31:12V_Rail:' | grep -a BusVolt | awk -F ':' '{print $6}' | awk -F 'V' '{print $1}' || true)
		if [ "x$twelvev_volt" != "xIN" ] ; then
			echo "twelvev_volt=[$twelvev_volt]" >> /var/www/html/dygraphs/data/log.txt
		else
			unset twelvev_volt
		fi

		twelvev_mamp=$(echo $READ | grep -a '0013A20041A7AE31:12V_Rail:' | grep -a Current | awk -F ':' '{print $8}' | awk -F 'mA' '{print $1}' || true)
		if [ "x$twelvev_mamp" != "xINVALID" ] ; then
			echo "twelvev_mamp=[$twelvev_mamp]" >> /var/www/html/dygraphs/data/log.txt
		else
			unset twelvev_mamp
		fi

		battery_hdc_temp=$(echo $READ | grep -a '0013A20041A7AE31:HDC1080:' | grep -a Temperature | awk -F ':' '{print $6}' | awk -F 'F' '{print $1}' || true)
		if [ "x$battery_hdc_temp" != "xINVALID" ] ; then
			echo "battery_hdc_temp=[$battery_hdc_temp]" >> /var/www/html/dygraphs/data/log.txt
		else
			unset battery_hdc_temp
		fi

		pth_pressure=$(echo $READ | grep -a '0013A20041981B29:PTH_sensor:' | grep -a Press | awk -F ':' '{print $6}' | awk -F 'mB' '{print $1}' || true)
		echo "pth_pressure=[$pth_pressure]" >> /var/www/html/dygraphs/data/log.txt

		pth_temp=$(echo $READ | grep -a '0013A20041981B29:PTH_sensor:' | grep -a Temp | awk -F ':' '{print $8}' | awk -F 'F' '{print $1}' || true)
		echo "pth_temp=[$pth_temp]" >> /var/www/html/dygraphs/data/log.txt

		mcp_temp=$(echo $READ | grep -a '0013A20041981B29:PTH_sensor:' | grep -a Temp1 | awk -F ':' '{print $10}' | awk -F 'F' '{print $1}' || true)
		echo "mcp_temp=[$mcp_temp]" >> /var/www/html/dygraphs/data/log.txt

		pth_humidity=$(echo $READ | grep -a '0013A20041981B29:PTH_sensor:' | grep -a Humidity | awk -F ':' '{print $12}' | awk -F '%' '{print $1}' || true)
		echo "pth_humidity=[$pth_humidity]" >> /var/www/html/dygraphs/data/log.txt

		get_time=$(env TZ=America/North_Dakota/Center date +"%Y/%m/%d %k:%M:%S")
		get_time_weather=$(env TZ=America/North_Dakota/Center date +"%Y-%m-%dT%k:%M:%S")
		get_day=$(env TZ=America/North_Dakota/Center date +"%d")

		if [ "x$solar_volt" != "x" ] && [ "x$solar_mamp" != "x" ] ; then
			echo "$get_time,$solar_volt" >> ${wdir}/solar_voltage_data.csv
			if [ 1 -eq "$(echo "${solar_mamp} > 0" | bc)" ] ; then
				echo "$get_time,$solar_mamp" >> ${wdir}/solar_current_data.csv
			else
				echo "$get_time,0" >> ${wdir}/solar_current_data.csv
			fi
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

		if [ "x$battery_hdc_temp" != "x" ] ; then
			if [ 0 -eq "$(echo "${battery_hdc_temp} > ${max_temperature}" | bc)" ] ; then
				echo "$get_time,$battery_hdc_temp" >> ${wdir}/battery_hdc_temp_data.csv
				if [ -f /home/debian/send.data ] ; then
					curl -X POST 'http://192.168.0.114:8100/v1/data/for/WeatherStationAgain?Key1=$battery_hdc_temp'
				fi
			fi
		fi

		if [ "x$pth_pressure" != "x" ] ; then
			if [ 0 -eq "$(echo "${pth_pressure} > ${max_pressure}" | bc)" ] ; then
				echo "$get_time,$pth_pressure" >> ${wdir}/pth_pressure_data.csv
			fi
		fi

		if [ "x$pth_temp" != "x" ] ; then
			if [ 0 -eq "$(echo "${pth_temp} > ${max_temperature}" | bc)" ] ; then
				echo "$get_time,$pth_temp" >> ${wdir}/pth_temp_data.csv
			fi
		fi

		if [ "x$mcp_temp" != "x" ] ; then
			if [ 0 -eq "$(echo "${mcp_temp} > ${max_temperature}" | bc)" ] ; then
				echo "$get_time,$mcp_temp" >> ${wdir}/mcp_temp_data.csv
			fi
		fi

		if [ "x$pth_temp" != "x" ] && [ "x$mcp_temp" != "x" ] ; then
			if [ 0 -eq "$(echo "${pth_temp} > ${max_temperature}" | bc)" ] && [ 0 -eq "$(echo "${mcp_temp} > ${max_temperature}" | bc)" ] ; then
				echo "$get_time,$pth_temp,$mcp_temp" >> ${wdir}/1B29_temp_data.csv
			fi
		fi

		if [ "x$pth_humidity" != "x" ] ; then
			echo "$get_time,$pth_humidity" >> ${wdir}/pth_humidity_data.csv
		fi

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

