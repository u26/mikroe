
#ifndef CAPSLIDER2_H
#define CAPSLIDER2_H

/**
 * \defgroup error_code Error Code
 * \{
 */
#define CAPSLIDER2_RETVAL  uint8_t

#define CAPSLIDER2_OK           0x00
#define CAPSLIDER2_INIT_ERROR   0xFF
/** \} */

/**
 * \defgroup device_address Device address
 * \{
 */
#define CAPSLDR2_DEVICE_ADDR                            0x64
/** \} */

/**
 * \defgroup registers Registers
 * \{
 */
#define CAPSLDR2_DEVICE_INFO_REG                        0x00
#define CAPSLDR2_SYSTEM_FLAGS_REG                       0x01
#define CAPSLDR2_WHEEL_COORDNT_REG                      0x02
#define CAPSLDR2_TOUCH_BYTES_REG                        0x03
#define CAPSLDR2_COUNTS_REG                             0x04
#define CAPSLDR2_LTA_REG                                0x05
#define CAPSLDR2_MULTIPLIERS_REG                        0x06
#define CAPSLDR2_COMPENS_REG                            0x07
#define CAPSLDR2_PROXSETT_REG                           0x08
#define CAPSLDR2_THRESH_REG                             0x09
#define CAPSLDR2_TIMINGS_REG                            0x0A
#define CAPSLDR2_TARGETS_REG                            0x0B
#define CAPSLDR2_PWM_DUTY_REG                           0x0C
#define CAPSLDR2_PWM_LIM_REG                            0x0D
#define CAPSLDR2_ACTIVE_CHANN_REG                       0x0E
#define CAPSLDR2_BUZZER_REG                             0x0F
/** \} */

/**
 * \defgroup bytes Bytes
 * \{
 */
#define CAPSLDR2_DEV_INFO_NBYTES                        2
#define CAPSLDR2_SYST_FLAGS_NBYTES                      1
#define CAPSLDR2_WHEEL_NBYTES                           4
#define CAPSLDR2_TOUCH_NBYTES                           2
#define CAPSLDR2_COUNTS_NBYTES                          18
#define CAPSLDR2_LTA_NBYTES                             18
#define CAPSLDR2_MULTIPL_NBYTES                         10
#define CAPSLDR2_COMPENS_NBYTES                         10
#define CAPSLDR2_PROXSETT_NBYTES                        6
#define CAPSLDR2_THRESH_NBYTES                          10
#define CAPSLDR2_TIMINGS_NBYTES                         13
#define CAPSLDR2_TARGETS_NBYTES                         10
#define CAPSLDR2_PWM_DUTY_NBYTES                        8
#define CAPSLDR2_PWM_LIM_NBYTES                         2
#define CAPSLDR2_ACT_CHANN_NBYTES                       2
#define CAPSLDR2_BUZZER_NBYTES                          1
/** \} */

/**
 * \defgroup buzzer Buzzer
 * \{
 */
#define CAPSLDR2_BUZZER_EN                              0x80
#define CAPSLDR2_BUZZER_DC_HIGH                         0x04
#define CAPSLDR2_BUZZER_DC_LOW                          0x00
#define CAPSLDR2_BUZZER_PERM_EN                         0x02
#define CAPSLDR2_BUZZER_BURST_EN                        0x01
/** \} */

/**
 * \defgroup chanel Chanel
 * \{
 */
#define CAPSLDR2_CH0_PROX_EN                           0x001
#define CAPSLDR2_CH1_EN                                0x002
#define CAPSLDR2_CH2_EN                                0x004
#define CAPSLDR2_CH3_EN                                0x008
#define CAPSLDR2_CH4_EN                                0x010
#define CAPSLDR2_CH5_EN                                0x020
#define CAPSLDR2_CH6_EN                                0x040
#define CAPSLDR2_CH7_EN                                0x080
#define CAPSLDR2_CH8_EN                                0x100
#define CAPSLDR2_CH9_EN                                0x200
/** \} */

/**
 * \defgroup pwm_constants PWM constants
 * \{
 */
#define CAPSLDR2_PWM_OFF                                0x00
#define CAPSLDR2_PWM_CONST                              0x20
#define CAPSLDR2_PWM_DECR_0_PERCENTS                    0x80
#define CAPSLDR2_PWM_DECR_LIM                           0xA0
#define CAPSLDR2_PWM_INCR_100_PERCENTS                  0xC0
#define CAPSLDR2_PWM_INCR_LIM                           0xE0
#define CAPSLDR2_PWM_DUTY_0_PERCENTS                    0x00
#define CAPSLDR2_PWM_DUTY_25_PERCENTS                   0x10
#define CAPSLDR2_PWM_DUTY_50_PERCENTS                   0x18
#define CAPSLDR2_PWM_DUTY_75_PERCENTS                   0x1C
#define CAPSLDR2_PWM_DUTY_100_PERCENTS                  0x1F
/** \} */

/**
 * \defgroup data Data
 * \{
 */
#define CAPSLDR2_DATA_READY                             0
#define CAPSLDR2_DATA_NOT_READY                         1
#define CAPSLDR2_INVALID_ADDR                           0xFF
#define CAPSLDR2_INVALID_NBYTES                         0xFE
#define CAPSLDR2_INVALID_CHANNEL                        0xFD
#define CAPSLDR2_OK                                     0x00
/** \} */

/**
 * \defgroup led LED
 * \{
 */
#define CAPSLDR2_LED_INTENSITY                          0
#define CAPSLDR2_LED_NUMBER                             1
/** \} */


#ifdef __cplusplus
extern "C"{
#endif


#ifdef __cplusplus
}
#endif

#endif  // _CAPSLIDER2_H_
