
#pragma once

#include <stdint.h>

namespace kernel::hal::drivers
{
	enum class KeyCode 
	{
		Space             	= ' ',
		Key0                 = '0',
		Key1                 = '1',
		Key2                 = '2',
		Key3                 = '3',
		Key4                 = '4',
		Key5                 = '5',
		Key6                 = '6',
		Key7                 = '7',
		Key8                 = '8',
		Key9                 = '9',

		A                 = 'a',
		B                 = 'b',
		C                 = 'c',
		D                 = 'd',
		E                 = 'e',
		F                 = 'f',
		G                 = 'g',
		H                 = 'h',
		I                 = 'i',
		J                 = 'j',
		K                 = 'k',
		L                 = 'l',
		M                 = 'm',
		N                 = 'n',
		O                 = 'o',
		P                 = 'p',
		Q                 = 'q',
		R                 = 'r',
		S                 = 's',
		T                 = 't',
		U                 = 'u',
		V                 = 'v',
		W                 = 'w',
		X                 = 'x',
		Y                 = 'y',
		Z                 = 'z',

		Return            = '\r',
		Escape            = 0x1001,
		BackSpace         = '\b',

		Up                = 0x1100,
		Down              = 0x1101,
		Left              = 0x1102,
		Right             = 0x1103,

		F1                = 0x1201,
		F2                = 0x1202,
		F3                = 0x1203,
		F4                = 0x1204,
		F5                = 0x1205,
		F6                = 0x1206,
		F7                = 0x1207,
		F8                = 0x1208,
		F9                = 0x1209,
		F10               = 0x120a,
		F11               = 0x120b,
		F12               = 0x120b,
		F13               = 0x120c,
		F14               = 0x120d,
		F15               = 0x120e,

		Dot               = '.',
		Comma             = ',',
		Colon             = ':',
		Semicolon         = ';',
		Slash             = '/',
		BackSlash         = '\\',
		Plus              = '+',
		Minus             = '-',
		Asterisk          = '*',
		Exclamation       = '!',
		Question          = '?',
		DoubleQuote       = '\"',
		Quote             = '\'',
		Equal             = '=',
		Hash              = '#',
		Percent           = '%',
		Ampersand         = '&',
		UnderScore        = '_',
		LeftParenthisis   = '(',
		RightParenthisis  = ')',
		LeftBracket       = '[',
		RightBracket      = ']',
		LeftCurl          = '{',
		RightCurl         = '}',
		Dollar            = '$',
		Less              = '<',
		Greater           = '>',
		Bar               = '|',
		Grave             = '`',
		Tilde             = '~',
		At                = '@',
		Carret            = '^',

		Num0              = '0',
		Num1              = '1',
		Num2              = '2',
		Num3              = '3',
		Num4              = '4',
		Num5              = '5',
		Num6              = '6',
		Num7              = '7',
		Num8              = '8',
		Num9              = '9',
		NumPlus           = '+',
		NumMinus          = '-',
		NumDecimal        = '.',
		NumDivide         = '/',
		NumAsterisk       = '*',
		NumLock           = 0x300f,
		NumEnter          = 0x3010,

		Tab               = 0x4000,
		CapsLock          = 0x4001,

		LeftShift         = 0x4002,
		LeftControl       = 0x4003,
		LeftAlt           = 0x4004,
		LeftMod           = 0x4005,
		RightShift        = 0x4006,
		RightControl      = 0x4007,
		RightAlt          = 0x4008,
		RightMod          = 0x4009,

		Inset             = 0x400a,
		Delete            = 0x400b,
		Home              = 0x400c,
		End               = 0x400d,
		PageUp            = 0x400e,
		PageDown          = 0x400f,
		ScrollLock        = 0x4010,
		Pause             = 0x4011,

		Unknown,
		Count
	};

	class Keyboard
	{
	public:
		static Keyboard& Get() { return s_Instance; }

		void Install(int irq);

		bool GetScrollock();
		bool GetNumLock();
		bool GetCapsLock();

		bool GetAlt();
		bool GetControl();
		bool GetShift();
	
		bool GetBATResult();
		bool GetDisagnosticResult();

		uint8_t GetCurrentScanCode();
		void SetCurrentScanCode(uint8_t scanCode);
		KeyCode GetLastKeyCode();
		void DiscardLastKeyCode();

		char KeyCodeToAscii(KeyCode code);

		void Enable();
		void Disable();
		bool IsEnabled();

		void ResetSystem();

		void HandleInterrupt();

	private:
		void UpdateLEDState();

		void ToggleNumLock();
		void ToggleCapsLock();
		void ToggleScrollLock();

		void IgnoreShouldResendLastCommand();
		bool ShouldResendLastCommand();
		void SetShouldResendLastCommand();

		uint8_t ControlReadStatus();
		void ControlSendCommand(uint8_t cmd);
		uint8_t EncoderReadBuffer();
		void EncoderSendCommand(uint8_t cmd);

		bool m_NumLock, m_ScrollLock, m_CapsLock;
		bool m_Shift, m_Alt, m_Control;

		char m_CurrentScanCode;
		bool m_ShouldResendLastCommand = false;
		bool m_Disabled = false;

		bool m_BATTestResult;
		bool m_DiagnosticResult;

		static constexpr int s_InvalidScancode = 0;
		static Keyboard s_Instance;
	};

	extern "C" void keyboard_irq();
	extern "C" void  keyboard_irq_main();
}
