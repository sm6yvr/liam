
#ifndef _DEFINITION_H_
#define _DEFINITION_H_


#define wifi_ssid ""
#define wifi_password ""
#define mqtt_server ""
#define mqtt_port 1883

#define TOPIC_LIAM_COMMAND "/liam/command"
#define TOPIC_LIAM_CONSOLE "/liam/console"
#define TOPIC_ESP_CONSOLE "/esp/console"


// Include LocalDefinition.h if it exists
#if __has_include("LocalDefinition.h")
#include "LocalDefinition.h"
#endif

#endif /* _DEFINITION_H_ */