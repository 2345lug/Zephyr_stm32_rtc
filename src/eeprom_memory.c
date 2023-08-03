#include "eeprom_memory.h"

#include <zephyr/drivers/counter.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/eeprom.h>

#define EEPROM_RTC_MEMORY_OFFSET 0

static uint32_t previous_shutdown_time = 0;

/**
       * Function for get EEPROM device from dtm
       * @return Pointer to device structure. If womething goes wrong - return NULL
       */
const struct device *get_eeprom_device(void)
{
	const struct device *const dev = DEVICE_DT_GET_ONE(atmel_at24);

	if (!device_is_ready(dev)) {
		printk("\nError: Device \"%s\" is not ready; "
		       "check the driver initialization logs for errors.\n",
		       dev->name);
		return NULL;
	}

	printk("Found EEPROM device \"%s\"\n", dev->name);
	return dev;
}

/**
       * Function for compare current time with las time, stored in eeprom memory
       * @param eeprom_device EEPROM device structure pointer.
       * @param now Current time value.
       * @return Time different in seconds
       */
uint32_t compare_previous_time (const struct device * eeprom_device, uint32_t now)
{
	uint32_t readed_value = 0;
	uint32_t time_different = 0;
	int rc = 0;
	rc = eeprom_read(eeprom_device, EEPROM_RTC_MEMORY_OFFSET, &readed_value, sizeof(uint32_t));
	
	if (rc < 0) {
		printk("Error: Couldn't read eeprom: err: %d.\n", rc);
		return 0;
	}
	else
	{
		printk("Readed value is: %d. now value is: %d.\n", readed_value, now);
		if ((now - readed_value) > 0)
		{
            previous_shutdown_time = readed_value;
			time_different = now - readed_value;
			printk("Downtime is: %02d m %02ds. \n", time_different / 60, time_different % 60);
		}
		
		return time_different;
	}
}

/**
       * Function for store current time as last written time
       * @param now Current time value.
       * @return 0 if eeprom write command is OK, error code if not
       */
int set_previous_time (const struct device * eeprom_device, uint32_t now)
{
    int rc = 0;
    rc = eeprom_write(eeprom_device, EEPROM_RTC_MEMORY_OFFSET, &now, sizeof(uint32_t));
	if (rc < 0) 
    {
	    printk("Error: Couldn't write eeprom: err:%d.\n", rc);
	}
    return rc;
}

/**
       * Function for get last shutdown time
       * @return last shutdown time
       */
uint32_t get_previous_shutdown_time (void)
{
    return previous_shutdown_time;
}