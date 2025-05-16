#include <stdio.h>
#include <string.h>
#include <unistd.h>

void control_charge_speed(const char *file_path, int target_value)
{
    FILE *file = fopen(file_path, "r+"); // 打开文件以读写模式
    if (file != NULL)
    {
        int current_value;
        fscanf(file, "%d", &current_value); // 读取当前值
        if (current_value != target_value)
        {                                      // 如果当前值与目标值不同
            rewind(file);                      // 重置文件指针到文件开头
            fprintf(file, "%d", target_value); // 将目标值（单位：微安，µA）写入文件
        }
        fclose(file); // 关闭文件
    }
}

int main()
{
    char buffer[128];      // 用于存储从文件读取的电池状态
    char charge_type[128]; // 用于存储充电类型
    char capacity_str[16]; // 用于存储电池电量字符串
    int capacity;          // 用于存储电池电量整数值

    while (1)
    { 
        // 打开电池状态文件以读取模式
        FILE *fp = fopen("/sys/class/power_supply/battery/status", "r");

        // 读取电池状态
        if (fp && fgets(buffer, sizeof(buffer), fp))
        {
            buffer[strcspn(buffer, "\n")] = 0; // 去除读取内容中的换行符

            // 只有在状态为 Charging 时才进行电流控制
            if (strstr(buffer, "Charging"))
            {
                FILE *charge_type_fp = fopen("/sys/class/power_supply/battery/charge_type", "r");
                FILE *capacity_fp = fopen("/sys/class/power_supply/battery/capacity", "r");

                // 读取充电类型
                if (charge_type_fp && fgets(charge_type, sizeof(charge_type), charge_type_fp))
                {
                    charge_type[strcspn(charge_type, "\n")] = 0; 
                }

                if (strcmp(charge_type, "Standard") == 0)
                { // 如果充电类型为标准
                    if (capacity_fp && fgets(capacity_str, sizeof(capacity_str), capacity_fp))
                    {
                        capacity_str[strcspn(capacity_str, "\n")] = 0; 
                        sscanf(capacity_str, "%d", &capacity);      
                    }

                    if (capacity <= 80)
                    {
                        control_charge_speed("/sys/class/power_supply/battery/constant_charge_current", 7000000); // 设置充电电流为7000000微安（7A，35W）
                    }
                    else
                    {
                        control_charge_speed("/sys/class/power_supply/battery/constant_charge_current", 4000000); // 设置充电电流为4000000微安（4A，20W）
                    }
                }
                else if (strcmp(charge_type, "Fast") == 0)
                { // 如果充电类型为快充
                    if (capacity_fp && fgets(capacity_str, sizeof(capacity_str), capacity_fp))
                    {
                        capacity_str[strcspn(capacity_str, "\n")] = 0;
                        sscanf(capacity_str, "%d", &capacity);     
                    }

                    if (capacity <= 30)
                    {
                        control_charge_speed("/sys/class/power_supply/battery/constant_charge_current", 10000000); // 50W（10A）
                    }
                    else if (capacity <= 80)
                    {
                        int current = 10000000 - (capacity - 30) * 100000;
                        control_charge_speed("/sys/class/power_supply/battery/constant_charge_current", current);
                    }
                    else
                    {
                        control_charge_speed("/sys/class/power_supply/battery/constant_charge_current", 5000000); // 25W（5A）
                    }
                }

                if (charge_type_fp)
                    fclose(charge_type_fp);
                if (capacity_fp)
                    fclose(capacity_fp);
            }
        }

        if (fp)
            fclose(fp);

        sleep(2);
    }
    return 0;
}
