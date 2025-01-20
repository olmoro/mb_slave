/*=====================================================================================
 * Description:
 *    ESP32
 *====================================================================================*/


#include "staff.h"
#include <stdio.h>
#include <stdint.h>
#include "esp_err.h"
//#include "mbcontroller.h"       // for mbcontroller defines and api
//#include "modbus_params.h"      // for modbus parameters structures
#include "esp_log.h"            // for log_write
#include "sdkconfig.h"




static void staff_task(void *arg)
{



  while (1)
  {
    /* code */
  }
  vTaskDelete(NULL);
}  


void staffTaskStart()
{
  //xTaskCreate(slave_task, "mb_slave", 4096, NULL, 10, NULL);

  static StackType_t stTaskStack[4096];
  static StaticTask_t stTaskBuffer;

  xTaskCreateStatic(staff_task,
                       "staff",
                          4096,
                          NULL,
                            10,
                   stTaskStack,
                 &stTaskBuffer);
}
