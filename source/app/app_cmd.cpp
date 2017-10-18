#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/cmd_line.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_config.h"
#include "app_cmd.h"

#include "task_list.h"
#include "task_list_if.h"

static int32_t i_shell_ver(uint8_t* argv);
static int32_t i_shell_help(uint8_t* argv);
static int32_t i_shell_cfg(uint8_t* argv);
static int32_t i_shell_dbg(uint8_t* argv);
static int32_t i_shell_fw(uint8_t* argv);

cmd_line_t lgn_cmd_table[] = {
	{(const int8_t*)"ver",		i_shell_ver,			(const int8_t*)"get kernel version"},
	{(const int8_t*)"help",		i_shell_help,			(const int8_t*)"help command info"},
	{(const int8_t*)"cfg",		i_shell_cfg,			(const int8_t*)"config"},
	{(const int8_t*)"dbg",		i_shell_dbg,			(const int8_t*)"debug"},
	{(const int8_t*)"fw",		i_shell_fw,				(const int8_t*)"firmware update"},

	/* End Of Table */
	{(const int8_t*)0,(pf_cmd_func)0,(const int8_t*)0}
};

int32_t i_shell_ver(uint8_t* argv) {
	(void)argv;
	APP_PRINT("version: %s\n", AK_VERSION);
	return 0;
}

int32_t i_shell_help(uint8_t* argv) {
	uint32_t idx = 0;
	switch (*(argv + 4)) {
	default:
		APP_PRINT("\nCOMMANDS INFORMATION:\n\n");
		while(lgn_cmd_table[idx].cmd != (const int8_t*)0) {
			APP_PRINT("%s\n\t%s\n\n", lgn_cmd_table[idx].cmd, lgn_cmd_table[idx].info);
			idx++;
		}
		break;
	}
	return 0;
}

int32_t i_shell_cfg(uint8_t* argv) {
	switch (*(argv + 4)) {
	case '0': {
		app_config_parameter_t config;

		/* lora gateway */
		strcpy(config.lora_gateway.lora_host,			"1.1.1.1");
		strcpy(config.lora_gateway.mqtt_host,			"118.69.135.199");
		config.lora_gateway.mqtt_port =					1883;
		strcpy(config.lora_gateway.mqtt_user_name,		"y55fYL");
		strcpy(config.lora_gateway.mqtt_psk,			"eJwKMNV2BQwC69PC");

		/* mqtt server */
		strcpy(config.pop_gateway.gateway_id_prefix,	"iot-");
		strcpy(config.pop_gateway.gateway_id,			"pop-dev");
		strcpy(config.pop_gateway.host,					"118.69.135.199");
		config.pop_gateway.port =						1883;
		strcpy(config.pop_gateway.user_name_view,		"fiot");
		strcpy(config.pop_gateway.user_psk_view	,		"ZmlvdEA5MTFmaW90");
		strcpy(config.pop_gateway.user_name_control,	"fciot");
		strcpy(config.pop_gateway.user_psk_control,		"ZmNpb3RAOTExOTExZmNpb3Q=");

		gateway_configure.write_config_data(&config);
		gateway_configure.parser_config_file(&config);

		APP_DBG("lora_gateway.lora_host:%s\n"			, config.lora_gateway.lora_host);
		APP_DBG("lora_gateway.mqtt_host:%s\n"			, config.lora_gateway.mqtt_host);
		APP_DBG("lora_gateway.mqtt_port:%d\n"			, config.lora_gateway.mqtt_port);
		APP_DBG("lora_gateway.mqtt_user_name:%s\n"		, config.lora_gateway.mqtt_user_name);
		APP_DBG("lora_gateway.mqtt_psk:%s\n"			, config.lora_gateway.mqtt_psk);

		APP_DBG("mqtt_server.gateway_id_prefix:%s\n"	, config.pop_gateway.gateway_id_prefix);
		APP_DBG("mqtt_server.gateway_id:%s\n"			, config.pop_gateway.gateway_id);
		APP_DBG("mqtt_server.host:%s\n"					, config.pop_gateway.host);
		APP_DBG("mqtt_server.port:%d\n"					, config.pop_gateway.port);
		APP_DBG("mqtt_server.user_name_view:%s\n"		, config.pop_gateway.user_name_view);
		APP_DBG("mqtt_server.user_psk_view:%s\n"		, config.pop_gateway.user_psk_view);
		APP_DBG("mqtt_server.user_name_control:%s\n"	, config.pop_gateway.user_name_control);
		APP_DBG("mqtt_server.user_psk_control:%s\n"		, config.pop_gateway.user_psk_control);
	}
		break;

	default:
		break;
	}

	return 0;
}

int32_t i_shell_dbg(uint8_t* argv) {
	switch (*(argv + 4)) {

	default:
		break;
	}
	return 0;
}

int32_t i_shell_fw(uint8_t* argv) {
	switch (*(argv + 3)) {
	case 'b': {
		APP_DBG("[i_shell_fw] update slave boot request\n");
		gateway_fw_dev_update_req_t gateway_fw_dev_update_req;
		memset(&gateway_fw_dev_update_req, 0, sizeof(gateway_fw_dev_update_req_t));
		strcpy(gateway_fw_dev_update_req.dev_bin_path, "/home/thannt/workspace/projects/thannt/arm_cortex_m3_base_source/boot/build_arm_cortex_m3_base_boot_stm32l/arm_cortex_m3_base_boot.bin");
		gateway_fw_dev_update_req.type_update   = TYPE_UPDATE_TARTGET_BOOT;
		gateway_fw_dev_update_req.source_if_type = IF_TYPE_UART_GW;
		gateway_fw_dev_update_req.target_task_id = AC_TASK_FW_ID;
		gateway_fw_dev_update_req.target_if_type = IF_TYPE_UART_AC;

		ak_msg_t* s_msg = get_dynamic_msg();
		set_msg_sig(s_msg, GW_FW_OTA_REQ);
		set_data_dynamic_msg(s_msg, (uint8_t*)&gateway_fw_dev_update_req, sizeof(gateway_fw_dev_update_req_t));
		set_msg_src_task_id(s_msg, GW_TASK_CONSOLE_ID);
		task_post(GW_TASK_FW_ID, s_msg);
	}
		break;

	case 'a': {
		APP_DBG("[i_shell_fw] update slave app request\n");
		gateway_fw_dev_update_req_t gateway_fw_dev_update_req;
		memset(&gateway_fw_dev_update_req, 0, sizeof(gateway_fw_dev_update_req_t));
		strcpy(gateway_fw_dev_update_req.dev_bin_path, "/home/thannt/workspace/projects/thannt/arm_cortex_m3_base_source/application/build_arm_cortex_m3_base_application_stm32l/arm_cortex_m3_base_application.bin");
		gateway_fw_dev_update_req.type_update   = TYPE_UPDATE_TARTGET_APP;
		gateway_fw_dev_update_req.source_if_type = IF_TYPE_UART_GW;
		gateway_fw_dev_update_req.target_task_id = AC_TASK_FW_ID;
		gateway_fw_dev_update_req.target_if_type = IF_TYPE_UART_AC;

		ak_msg_t* s_msg = get_dynamic_msg();
		set_msg_sig(s_msg, GW_FW_OTA_REQ);
		set_data_dynamic_msg(s_msg, (uint8_t*)&gateway_fw_dev_update_req, sizeof(gateway_fw_dev_update_req_t));
		set_msg_src_task_id(s_msg, GW_TASK_CONSOLE_ID);
		task_post(GW_TASK_FW_ID, s_msg);
	}
		break;

	default:
		break;
	}
	return 0;
}