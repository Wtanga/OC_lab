#!/bin/bash

case "$1" in
-h | --help) printf "Авторы: Владислав Каширин, Далия Бесаева, Никоалай Заруднев\n
Все доступные аргументы:
-n: Вывод всех сетевых интерфейсов;
-t: Включение/отключение заданных интерфейсов (в т.ч. сразу нескольких);
-s: Установка IP/Mask/Gateway для определенного интерфейса;
-k: Убийство процесса по занимаемому порту;
-d: Отображение сетевой статистики;\n
Краткое описание проекта: Управление сетевыми настройками системы\n
Примеры запуска:\n ./lab1.sh -o on/off <name>\n ./lab1.sh -k <port>";;

-n)
    ip -o link show | awk '{print $2,$9}' ;;
-t)
    if [[ "$2" == "on" ]]; then
        shift
        shift
        for intrfc in $@
        do
            #Включение интерфейса
            sudo ip link set $intrfc up
            #echo $intrfc "- switch on"
        done
    elif [[ "$2" == "off" ]]; then
        shift
        shift
        for intrfc in $@
        do
            #Отключение интерфейса
            sudo ip link set $intrfc down
            #echo $intrfc "- switch off"
        done
    else
        echo "./lab1.sh -t on/off <name>"
    fi;;
-s)
    #добавление IP/Mask для сетевого интерфейса
    if (($# == 5)); then
        ip addr add $2/$3 dev $4
        if (($? == 0)); then
            echo "IP/Mask добавлен."
        fi

        #Добавление шлюза
        ip route add default via $5
        if (($? == 0)); then
            echo "Шлюз добавлен."
        fi
    else
        echo "Недостатоно параметров"
        echo "./lab1.sh -s <ip> <mask> <name> <gateway>"
    fi;;
-k) 
    kill -9 $(lsof -t -i:$2);;
-d) 
    cat /proc/net/dev
    name=$(ls /sys/class/net)
    printf "Interfaces\t\tReceive\t\t\tTransmit\n"
    printf "\t\tbytes\t\tpackets\t\t\tbytes\t\tpackets\n"
    for var in $name
    do
        echo -e "$(cat /proc/net/dev | grep $var | awk '{print($1 "\t\t" $2 "\t\t" $3 "\t\t\t" $10 "\t\t" $11)}')"
    done
    echo;;
*) echo "'$1' is not an option" ;;
esac

echo
