#include "ir_protocols.h"

/**
 * @brief  红外接收
 */

void ir_rx_task(void *arg)
{
    uint32_t addr = 0;
    uint32_t cmd = 0;
    size_t length = 0;
    bool repeat = false;
    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;



    rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(CONFIG_EXAMPLE_RMT_RX_GPIO, RMT_CHANNEL_2);
    rmt_config(&rmt_rx_config);
    rmt_driver_install(RMT_CHANNEL_2, 1000, 0);
    ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)RMT_CHANNEL_2);
    ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
    ir_parser_t *ir_parser = NULL;
#if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
    ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);
#elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
    ir_parser = ir_parser_rmt_new_rc5(&ir_parser_config);
#endif

    // get RMT RX ringbuffer
    rmt_get_ringbuf_handle(RMT_CHANNEL_2, &rb);
    assert(rb != NULL);
    // Start receive
    rmt_rx_start(RMT_CHANNEL_2, true);
    while (1)
    {
        items = (rmt_item32_t *)xRingbufferReceive(rb, &length, portMAX_DELAY);
        if (items)
        {
            length /= 4; // one RMT = 4 Bytes
            if (ir_parser->input(ir_parser, items, length) == ESP_OK)
            {
                if (ir_parser->get_scan_code(ir_parser, &addr, &cmd, &repeat) == ESP_OK)
                {
                    ESP_LOGI("IR", "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
                    if (cmd == 0xe718)
                    {
                        printf("前\n");
                        brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
                    }
                    else if (cmd == 0xad52)
                    {
                        printf("后\n");
                        brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
                    }
                    else if (cmd == 0xf708)
                    {
                        printf("左\n");
                        brushed_motor_left(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
                    }
                    else if (cmd == 0xa55a)
                    {
                        printf("右\n");
                        brushed_motor_right(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
                    }
                    else if(cmd==0xe31c)
                    {
                        brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_UNIT_1, MCPWM_TIMER_1, 0.0);
                    }
                }
            }
            vRingbufferReturnItem(rb, (void *)items);
        }
    }
    ir_parser->del(ir_parser);
    rmt_driver_uninstall(RMT_CHANNEL_2);
    vTaskDelete(NULL);
}

/**
 * @brief 红外发送
 *
 */
/*void ir_tx_task(void *arg)
{
    uint32_t addr = 0x10;
    uint32_t cmd = 0x20;
    rmt_item32_t *items = NULL;
    size_t length = 0;
    ir_builder_t *ir_builder = NULL;

    rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(CONFIG_EXAMPLE_RMT_TX_GPIO, RMT_CHANNEL_0);
    rmt_tx_config.tx_config.carrier_en = true;
    rmt_config(&rmt_tx_config);
    rmt_driver_install(RMT_CHANNEL_0, 0, 0);
    ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)RMT_CHANNEL_0);
    ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
#if CONFIG_EXAMPLE_IR_PROTOCOL_NEC
    ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
#elif CONFIG_EXAMPLE_IR_PROTOCOL_RC5
    ir_builder = ir_builder_rmt_new_rc5(&ir_builder_config);
#endif
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI("IR", "Send command 0x%x to address 0x%x", cmd, addr);
        // Send new key code
        ESP_ERROR_CHECK(ir_builder->build_frame(ir_builder, addr, cmd));
        ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
        // To send data according to the waveform items.
        rmt_write_items(RMT_CHANNEL_0, items, length, false);
        // Send repeat code
        vTaskDelay(pdMS_TO_TICKS(ir_builder->repeat_period_ms));
        ESP_ERROR_CHECK(ir_builder->build_repeat_frame(ir_builder));
        ESP_ERROR_CHECK(ir_builder->get_result(ir_builder, &items, &length));
        rmt_write_items(RMT_CHANNEL_0, items, length, false);
        cmd++;
    }
    ir_builder->del(ir_builder);
    rmt_driver_uninstall(RMT_CHANNEL_0);
    vTaskDelete(NULL);
}
*/