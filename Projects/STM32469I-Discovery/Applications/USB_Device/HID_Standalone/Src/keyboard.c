#include "keyboard.h"

KeyboardKey keys[] =
{
	{ KEY_L, { GPIOA, GPIO_PIN_0 }, KEY_STATE_UP },
	{ KEY_B, { GPIOA, GPIO_PIN_1 }, KEY_STATE_UP },
	{ KEY_C, { GPIOA, GPIO_PIN_3 }, KEY_STATE_UP },
	{ KEY_MACRO_0, { GPIOA, GPIO_PIN_4 }, KEY_STATE_UP }
};
const int keyCount = sizeof(keys) / sizeof(keys[0]);

uint8_t anyKeyDown = 0;
uint8_t macroKeyDown = 0;
uint8_t macroStep = 0;
uint8_t macroCount = 0;
uint8_t macroNextKeySame = 0;
uint32_t macroMillis = 0;
char macroContent[100] = {0};
__ALIGN_BEGIN uint8_t _reportBuffer[REPORT_BUF_SIZE] __ALIGN_END;

void SetupKeyboard(void) 
{
	static GPIO_InitTypeDef GPIO_InitStruct;

	USBD_Init(&USBD_Device, &HID_Desc, 0);
	USBD_RegisterClass(&USBD_Device, &USBD_HID);
	USBD_Start(&USBD_Device);

	// Configure the key pins for input
	for (int i = 0; i < keyCount; i++)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Pin = keys[i].Pin.Pin;

		HAL_GPIO_Init(keys[i].Pin.Port, &GPIO_InitStruct);
	}
}

#if 0
void UpdateKeyboard(void)
{
	// Only scan for key states if we aren'y processing a macro
	if (!macroKeyDown)
	{
		ScanKeys();
//		printf("ScanKeys()\r\n");
	}

	if (macroKeyDown)
	{
		HandleMacroKey();
//		printf("HandleMacroKey()\r\n");
	}
	else
	{
		HandleStandardKeys();
//		printf("HandleStandardKeys()\r\n");
	}
}
#else
void UpdateKeyboard(void)
{
	// Only scan for key states if we aren'y processing a macro
	
	ScanKeys();

	HandleStandardKeys();
}
#endif


#if 0
void ScanKeys(void)
{
	uint32_t millis = HAL_GetTick();
	anyKeyDown = 0;

	for (int i = 0; i < keyCount; i++)
	{
		uint8_t pinState = HAL_GPIO_ReadPin(keys[i].Pin.Port, keys[i].Pin.Pin);
		if (pinState != keys[i].State)
		{
			if (millis - keys[i].StateChangeMillis > DEBOUNCE_MILLIS)
			{
				keys[i].State = pinState;
				keys[i].StateChangeMillis = millis;
				printf(" keys[%d].State = %d\r\n",i, keys[i].State);
				printf(" keys[%d].StateChangeMillis = %d\r\n",i, keys[i].StateChangeMillis);
			}
		}

		if (keys[i].State == KEY_STATE_DOWN)
		{
			anyKeyDown = 1;
		}

		if ((keys[i].Key == KEY_MACRO_0 || keys[i].Key == KEY_MACRO_1)
						&& keys[i].State == KEY_STATE_DOWN)
		{
			BeginMacroKey(keys[i]);
		}
	}
}
#else
void ScanKeys(void)
{
	uint32_t millis = HAL_GetTick();
	anyKeyDown = 0;

	uint8_t pinState = HAL_GPIO_ReadPin(keys[0].Pin.Port, keys[0].Pin.Pin);
	if (pinState != keys[0].State)
	{
		if (millis - keys[0].StateChangeMillis > DEBOUNCE_MILLIS)
		{
			keys[0].State = pinState;
			keys[0].StateChangeMillis = millis;
			printf(" keys[%d].State = %d\r\n",0, keys[0].State);
			printf(" keys[%d].StateChangeMillis = %d\r\n",0, keys[0].StateChangeMillis);
		}
		
		if (keys[0].State == KEY_STATE_DOWN)
		{
			anyKeyDown = 1;
		}
	}

//	if (keys[0].State == KEY_STATE_DOWN)
//	{
//		anyKeyDown = 1;
//	}
	
}
#endif

void BeginMacroKey(KeyboardKey key)
{
	macroKeyDown = key.Key;
	macroMillis = HAL_GetTick();

	// Generate the output for the macrow
	if (key.Key == KEY_MACRO_0)
	{
		macroCount = GenerateGuid(macroContent);
	}
}

void EndMacroKey(void)
{
	macroKeyDown = 0;
	macroStep = 0;
	macroCount = 0;
	macroNextKeySame = 0;

	// Zero out the macro content
	memset(macroContent, 0, 100);
}
#if 0
void HandleStandardKeys(void)
{
	uint8_t currentReportKey = 0;
	HIDKeyboardReport report = {0};

	if (!anyKeyDown)
	{
		printf("KeyDown\r\n");
		SendNullReport();
		return;
	}

	for (int i = 0; i < keyCount; i++)
	{
		if (keys[i].State == KEY_STATE_DOWN)
		{
			report.Keys[currentReportKey] = keys[i].Key;
			currentReportKey++;

			printf(" keys[%d].State = %d\r\n",i, keys[i].State);
		}

		// We can only send up to REPORT_MAX_KEYS keys at once
		if (currentReportKey >= REPORT_MAX_KEYS)
		{
			break;
		}
	}

	SendReport(&report);
}
#else
#define SYS_SLEEP		0x01
#define SYS_WAKEUP		0x02
/*键盘发送给PC的数据每次8个字节
      data0 data1 data2 data3 data4 data5 data6 data7 
      定义分别是：
      data0 --
      |--bit0: Left Control是否按下，按下为1
      |--bit1: Left Shift 是否按下，按下为1
      |--bit2: Left Alt 是否按下，按下为1
      |--bit3: Left GUI 是否按下，按下为1
      |--bit4: Right Control是否按下，按下为1
      |--bit5: Right Shift 是否按下，按下为1
      |--bit6: Right Alt 是否按下，按下为1
      |--bit7: Right GUI 是否按下，按下为1
      data1 -- 保留
      data2--data7 -- 普通按键
refer to hid spec 8.3*/
#define 	LEFT_CTRL		0
#define 	LEFT_SHIFT		1
#define	LEFT_ALT		2
#define	LEFT_WIN		3
#define	RIGHT_CTRL		4
#define	RIGHT_SHIFT	5
#define	RIGHT_ALT		6
#define	RIGHT_WIN		7
void HandleStandardKeys(void)
{
	uint8_t currentReportKey = 0;
	HIDKeyboardReport report = {0};

	if (!anyKeyDown)
	{
		SendNullReport();
		return;
	}
	
	if (keys[0].State == KEY_STATE_DOWN)
	{
//		#if 0
//		report.Modifiers |= (1<<3);		//Lock screen
//		report.Keys[0] = KEY_L;
//		#else
//		report.Modifiers |= (1<<LEFT_SHIFT);		//disable screen and Lock screen
//		report.Keys[0] = KEY_ESC;
//		#endif
		report.Modifiers =0;
		report.Keys[0] = KEY_A;
//		report.Keys[0] = keys[0].Key;
//		report.Keys[1] = KEY_LEFTMETA;
		printf("HandleStandardKeys()\r\n");
#ifdef SYS_CTL_MODE
//		report.Modifiers =0x02;
//		report.Keys[0] = SYS_SLEEP;
#endif
		SendReport(&report);
	}

	
}
#endif

void HandleMacroKey(void)
{
	HIDKeyboardReport report = {0};
	uint32_t currentTick = HAL_GetTick();

	if (macroNextKeySame && currentTick - macroMillis > MACRO_KEY_DELAY)
	{
		SendNullReport();
		macroNextKeySame = 0;
		macroMillis = HAL_GetTick();
		return;
	}

	if (!macroNextKeySame && macroStep < macroCount && currentTick - macroMillis > MACRO_KEY_DELAY)
	{
		report.Keys[0] = CharToKeyCode(macroContent[macroStep]);
		SendReport(&report);

		macroStep++;
		macroMillis = HAL_GetTick();

		// If the next and prev characters are the same, send a null report
		if (macroStep < macroCount && macroContent[macroStep] == macroContent[macroStep - 1])
		{
			macroNextKeySame = 1;
		}

		return;
	}

	if (macroStep >= macroCount)
	{
		SendNullReport();
		EndMacroKey();
	}
}

void SendNullReport(void) {
	HIDKeyboardReport report = {
		.Modifiers = 0,
		.Keys[0] = 0,
		.Keys[1] = 0,
		.Keys[2] = 0,
		.Keys[3] = 0,
		.Keys[4] = 0,
		.Keys[5] = 0,
	};
	SendReport(&report);
}

void SendReport(const HIDKeyboardReport* report) {
	if (report == NULL) 
	{
		return;
	}

	CopyReportToBuffer(report, _reportBuffer);

	USBD_HID_SendReport(&USBD_Device, _reportBuffer, REPORT_BUF_SIZE);
}

void CopyReportToBuffer(const HIDKeyboardReport* report, uint8_t* buffer)
{
	buffer[0] = report->Modifiers;
	buffer[1] = 0;
	buffer[2] = report->Keys[0];
	buffer[3] = report->Keys[1];
	buffer[4] = report->Keys[2];
	buffer[5] = report->Keys[3];
	buffer[6] = report->Keys[4];
	buffer[7] = report->Keys[5];
}

void ResetReport(HIDKeyboardReport* report)
{
	report->Modifiers = 0;
	report->Keys[0] = 0;
	report->Keys[1] = 0;
	report->Keys[2] = 0;
	report->Keys[3] = 0;
	report->Keys[4] = 0;
	report->Keys[5] = 0;
}
