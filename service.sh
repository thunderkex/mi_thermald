#!/system/bin/sh

MODDIR=${0%/*}

# Wait for boot to complete
until [ "$(getprop sys.boot_completed)" = "1" ]; do
    sleep 1
done

# Kill existing mi_thermald processes
killall mi_thermald 2>/dev/null

# Start our custom mi_thermald
chmod 755 "$MODDIR/mi_thermald"
"$MODDIR/mi_thermald" &