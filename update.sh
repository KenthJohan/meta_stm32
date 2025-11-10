wget -O STM32_open_pin_data.zip https://github.com/STMicroelectronics/STM32_open_pin_data/archive/refs/heads/master.zip
unzip STM32_open_pin_data.zip
mv STM32_open_pin_data-master/mcu/IP IP
mv STM32_open_pin_data-master/mcu/ mcu
mv STM32_open_pin_data-master/boards/ boards


find svd -type f -name "*.svd" | sort > svds.txt
find IP -type f -name "*_gpio_v1_0_Modes.xml" | sort > modes.txt
find mcu -maxdepth 1 -type f -name "*.xml" | sort > mcus.txt
find boards -maxdepth 1 -type f -name "*.ioc" | sort > boards.txt

