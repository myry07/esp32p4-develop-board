idf_component_register(SRCS "btn_reg.c" "button.c"
                    INCLUDE_DIRS "include"
                    REQUIRES freertos driver esp_timer log led)
