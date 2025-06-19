#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* Battery system file paths */
#define BATTERY_STATUS_PATH "/sys/class/power_supply/battery/status"
#define BATTERY_CHARGE_TYPE_PATH "/sys/class/power_supply/battery/charge_type"
#define BATTERY_CAPACITY_PATH "/sys/class/power_supply/battery/capacity"
#define BATTERY_CHARGE_CURRENT_PATH "/sys/class/power_supply/battery/constant_charge_current"

/* Charging current limits (in microamps) */
#define FAST_CHARGE_MAX_CURRENT 10000000    /* 10A - Maximum fast charging current */
#define STANDARD_CHARGE_MAX_CURRENT 7000000  /* 7A - Maximum standard charging current */
#define FAST_CHARGE_MIN_CURRENT 5000000      /* 5A - Minimum fast charging current */
#define STANDARD_CHARGE_MIN_CURRENT 4000000  /* 4A - Minimum standard charging current */

/**
 * Controls the charging speed by writing to the specified sysfs file
 * @param file_path Path to the charging current control file
 * @param target_value Desired charging current in microamps
 */
void control_charge_speed(const char *file_path, int target_value)
{
    FILE *file = fopen(file_path, "r+");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening %s: %s\n", file_path, strerror(errno));
        return;
    }

    int current_value;
    if (fscanf(file, "%d", &current_value) != 1)
    {
        fprintf(stderr, "Error reading current value from %s\n", file_path);
        fclose(file);
        return;
    }

    if (current_value != target_value)
    {
        rewind(file);
        if (fprintf(file, "%d", target_value) < 0)
        {
            fprintf(stderr, "Error writing to %s: %s\n", file_path, strerror(errno));
        }
    }
    fclose(file);
}

/**
 * Reads the current battery capacity
 * @return Battery capacity percentage (0-100) or -1 on error
 */
static int read_battery_capacity(void)
{
    FILE *fp = fopen(BATTERY_CAPACITY_PATH, "r");
    if (!fp)
    {
        fprintf(stderr, "Error opening capacity file: %s\n", strerror(errno));
        return -1;
    }

    char capacity_str[16];
    int capacity = -1;
    
    if (fgets(capacity_str, sizeof(capacity_str), fp))
    {
        capacity_str[strcspn(capacity_str, "\n")] = 0;
        sscanf(capacity_str, "%d", &capacity);
    }
    
    fclose(fp);
    return capacity;
}

/**
 * Handles standard charging mode
 * Applies appropriate current limits based on battery capacity:
 * - ≤80%: Maximum current (7A/35W)
 * - >80%: Reduced current (4A/20W)
 * 
 * @param capacity Current battery capacity (0-100)
 */
static void handle_standard_charging(int capacity)
{
    if (capacity < 0) return;
    
    int target_current = (capacity <= 80) ? 
        STANDARD_CHARGE_MAX_CURRENT : STANDARD_CHARGE_MIN_CURRENT;
    
    control_charge_speed(BATTERY_CHARGE_CURRENT_PATH, target_current);
}

/**
 * Handles fast charging mode
 * Applies dynamic current limits based on battery capacity:
 * - ≤30%: Maximum current (10A/50W)
 * - 31-80%: Linear reduction from 10A to 5A
 * - >80%: Minimum current (5A/25W)
 * 
 * @param capacity Current battery capacity (0-100)
 */
static void handle_fast_charging(int capacity)
{
    if (capacity < 0) return;
    
    int target_current;
    if (capacity <= 30)
    {
        target_current = FAST_CHARGE_MAX_CURRENT;
    }
    else if (capacity <= 80)
    {
        target_current = FAST_CHARGE_MAX_CURRENT - (capacity - 30) * 100000;
    }
    else
    {
        target_current = FAST_CHARGE_MIN_CURRENT;
    }
    
    control_charge_speed(BATTERY_CHARGE_CURRENT_PATH, target_current);
}

int main()
{
    char buffer[128];
    char charge_type[128];

    while (1)
    {
        FILE *fp = fopen(BATTERY_STATUS_PATH, "r");
        if (!fp)
        {
            fprintf(stderr, "Error opening battery status: %s\n", strerror(errno));
            sleep(2);
            continue;
        }

        if (fgets(buffer, sizeof(buffer), fp))
        {
            buffer[strcspn(buffer, "\n")] = 0;

            if (strstr(buffer, "Charging"))
            {
                FILE *charge_type_fp = fopen(BATTERY_CHARGE_TYPE_PATH, "r");
                if (!charge_type_fp)
                {
                    fprintf(stderr, "Error opening charge type file: %s\n", strerror(errno));
                    fclose(fp);
                    sleep(2);
                    continue;
                }

                if (fgets(charge_type, sizeof(charge_type), charge_type_fp))
                {
                    charge_type[strcspn(charge_type, "\n")] = 0;
                    int capacity = read_battery_capacity();

                    if (strcmp(charge_type, "Standard") == 0)
                    {
                        handle_standard_charging(capacity);
                    }
                    else if (strcmp(charge_type, "Fast") == 0)
                    {
                        handle_fast_charging(capacity);
                    }
                }

                fclose(charge_type_fp);
            }
        }

        fclose(fp);
        sleep(2);
    }
    return 0;
}
