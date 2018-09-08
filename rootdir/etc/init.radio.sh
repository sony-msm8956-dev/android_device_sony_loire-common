#! /vendor/bin/sh

# Set radio props based on hardware configs
simslot_id=`cat /proc/simslot_id`
case "$simslot_id" in
    "ss")
    setprop ro.telephony.default_network 9
    ;;
    "dsds")
    setprop persist.radio.multisim.config dsds
    setprop ro.telephony.default_network 9,1
    ;;
esac
