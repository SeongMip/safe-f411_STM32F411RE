/****************************************************************************
 * @file    test_baremetal_runner.c
 * @brief   bare-metal 환경에서 선택된 테스트를 실행한다.
 *
 * @details
 * - 단일 TC 실행 흐름을 관리한다.
 * - TEST_IN_REVIEW 상태를 주기적으로 재평가하여 최종 PASS/FAIL로 수렴시킨다.
 ****************************************************************************/

#include "test_baremetal_runner.h"
#include "log.h"
#include "platform_port.h"
#include "test_config.h"
#include "tc_000_firmware_execution_verification.h"
#include "tc_001_debug_attach_reset.h"
#include "tc_002_clock_sanity.h"
#include "tc_010_led_gpio.h"
#include "tc_011_button_polling.h"
#include "tc_012_single_click_toggle.h"
#include "tc_013_long_press_toggle_once.h"
#include "tc_020_uart_sanity.h"
#include "tc_021_boot_log_format.h"
#include "tc_022_button_event_uart_mapping.h"
#include "tc_030_timing_interval_accuracy_validation.h"
#include "tc_031_button_debounce_stability_validation.h"
#include "tc_032_cpu_load_impact_on_button_detection.h"
#include "tc_040_button_bounce_bug_injection.h"
#include "tc_041_long_press_repeated_event_defect.h"
#include "tc_042_uart_event_omission_defect.h"
#include "tc_050_debounce_fix_validation.h"
#include "tc_051_long_press_one_shot_fix_validation.h"
#include "tc_052_uart_event_mapping_fix_validation.h"
#include "tc_060_regression_after_button_fix.h"
#include "tc_061_related_function_regression_summary.h"
#include "tc_062_rapid_input_stress_test.h"
#include "tc_063_long_duration_stability_test.h"
#include "tc_064_power_cycle_recovery_validation.h"
#include "tc_065_reset_recovery_observation.h"
#include "tc_066_fault_log_safe_state_validation.h"

static const TestCaseEntry g_test_cases[] =
{
    { RUN_TC_000, "TC_000", TC_000_FirmwareExecutionVerification_Run },
    { RUN_TC_001, "TC_001", TC_001_DebugAttachReset_Run },
    { RUN_TC_002, "TC_002", TC_002_ClockSanity_Run },
    { RUN_TC_010, "TC_010", TC_010_LedGpioOutput_Run },
    { RUN_TC_011, "TC_011", TC_011_ButtonPolling_Run },
    { RUN_TC_012, "TC_012", TC_012_SingleClickToggle_Run },
    { RUN_TC_013, "TC_013", TC_013_LongPressToggleOnce_Run },
    { RUN_TC_020, "TC_020", TC_020_UartSanity_Run },
    { RUN_TC_021, "TC_021", TC_021_BootLogFormat_Run },
    { RUN_TC_022, "TC_022", TC_022_ButtonEventUartMap_Run },
    { RUN_TC_030, "TC_030", TC_030_TimingIntervalAccuracyValidation_Run },
    { RUN_TC_031, "TC_031", TC_031_ButtonDebounceStabilityValidation_Run },
    { RUN_TC_032, "TC_032", TC_032_CpuLoadImpactOnButtonDetection_Run },
    { RUN_TC_040, "TC_040", TC_040_ButtonBounceBugInjection_Run },
    { RUN_TC_041, "TC_041", TC_041_LongPressRepeatedEventDefect_Run },
    { RUN_TC_042, "TC_042", TC_042_UartEventOmissionDefect_Run },
    { RUN_TC_050, "TC_050", TC_050_DebounceFixValidation_Run },
    { RUN_TC_051, "TC_051", TC_051_LongPressOneShotFixValidation_Run },
    { RUN_TC_052, "TC_052", TC_052_UartEventMappingFixValidation_Run },
    { RUN_TC_060, "TC_060", TC_060_RegressionAfterButtonFix_Run },
    { RUN_TC_061, "TC_061", TC_061_RelatedFunctionRegressionSummary_Run },
    { RUN_TC_062, "TC_062", TC_062_RapidInputStressTest_Run },
    { RUN_TC_063, "TC_063", TC_063_LongDurationStabilityTest_Run },
    { RUN_TC_064, "TC_064", TC_064_PowerCycleRecoveryValidation_Run },
    { RUN_TC_065, "TC_065", TC_065_ResetRecoveryObservation_Run },
    { RUN_TC_066, "TC_066", TC_066_FaultLogSafeStateValidation_Run },
};

static const TestCaseEntry* FindSelectedTestCase(RunnerSelection selection)
{
    uint32_t i;

    for (i = 0U; i < (sizeof(g_test_cases) / sizeof(g_test_cases[0])); i++)
    {
        if (g_test_cases[i].selection == selection)
        {
            return &g_test_cases[i];
        }
    }

    return 0;
}

void TestBaremetalRunner_RunSelected(void)
{
    const TestCaseEntry* test_case = FindSelectedTestCase(TEST_BAREMETAL_TARGET);
    TestResult result;

    if (test_case == 0)
    {
        Log_Printf(LOG_LEVEL_ERROR,
                  "[TEST] invalid selection=%d\r\n",
                  (int)TEST_BAREMETAL_TARGET);
        return;
    }

    Log_Printf(LOG_LEVEL_INFO,
              "[TEST] START %s\r\n",
              test_case->id);

    do
    {
		result = test_case->run();

		if (result == TEST_IN_REVIEW)
		{
			Platform_DelayMs(10U);
		}
    } while (result == TEST_IN_REVIEW);

	Log_Printf(LOG_LEVEL_INFO,
			  "[TEST] END %s => %s\r\n",
			  test_case->id,
			  (result == TEST_PASS) ? "PASS" : "FAIL");
}
