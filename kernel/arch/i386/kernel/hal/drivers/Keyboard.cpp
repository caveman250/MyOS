#include <string.h>
#include <ctype.h>
#include <kernel/hal/HAL.h>
#include <kernel/hal/drivers/Keyboard.h>

//uncommen for printf debugging
//#include <stdio.h>


namespace kernel::hal::drivers
{
	enum class EncoderIO
	{
		InputBuffer = 0x60,
		CommandRegister = 0x60
	};

	enum class EncoderCommand
	{
		SetLED = 0xED,
		Echo = 0xEE,
		ScanCodeSet = 0xF0,
		ID = 0xF2,
		AutoPlay = 0xF3,
		Enable = 0xF4,
		ResetWait = 0xF5,
		ResetScan = 0xF6,
		AllAuto = 0xF7,
		MakeBreak = 0xF8,
		MakeOnly = 0xF9,
		AllMakeBreakAuto = 0xFA,
		SingleAutoRepeat = 0xFB,
		SingleMakeBreak = 0xFC,
		SingleBreakOnly = 0xFD,
		Resend = 0xFE,
		Reset = 0xFF
	};

	enum class ControlIO
	{
		StatusRegister = 0x64,
		CommandRegister = 0x64
	};

	enum class ControlStatusMask 
	{
		OutBuffer = 1,			//00000001
		InBuffer = 2,			//00000010
		System = 4,				//00000100
		CommandData = 8,		//00001000
		Locked = 0x10,			//00010000
		AuxiliaryBuffer = 0x20,	//00100000
		Timeout = 0x40,			//01000000
		Parity = 0x80			//10000000
	};

	enum class ControlCommand
	{
		Read = 0x20,
		Write = 0x60,
		SelfTest = 0xAA,
		InterfaceTest = 0xAB,
		Disable = 0xAD,
		Enable = 0xAE,
		ReadInPort = 0xC0,
		ReadOutPort = 0xD0,
		WriteOutPort = 0xD1,
		ReadTestInputs = 0xE0,
		SystemReset = 0xFE,
		MouseDisable = 0xA7,
		MouseEnable = 0xA8,
		MousePortTest = 0xA9,
		MouseWrite = 0xD4
	};

	enum class ErrorCode
	{
		BufferOverrun =	0,
		IDRet = 0x83AB,
		BAT = 0xAA,
		EchoRet = 0xEE,
		ACK = 0xFA,
		BATFailed = 0xFC,
		DiagnosticFailed = 0xFD,
		ResendCommand = 0xFE,
		Key = 0xFF
	};

	Keyboard Keyboard::s_Instance;

	static KeyCode s_KeyboardScanCodes[] = 
	{
		KeyCode::Unknown,		//0
		KeyCode::Escape,		//1
		KeyCode::Key1,			//2
		KeyCode::Key2,			//3
		KeyCode::Key3,			//4
		KeyCode::Key4,			//5
		KeyCode::Key5,			//6
		KeyCode::Key6,			//7
		KeyCode::Key7,			//8
		KeyCode::Key8,			//9
		KeyCode::Key9,			//0xa
		KeyCode::Key0,			//0xb
		KeyCode::Minus,			//0xc
		KeyCode::Equal,			//0xd
		KeyCode::BackSpace,		//0xe
		KeyCode::Tab,			//0xf
		KeyCode::Q,				//0x10
		KeyCode::W,				//0x11
		KeyCode::E,				//0x12
		KeyCode::R,				//0x13
		KeyCode::T,				//0x14
		KeyCode::Y,				//0x15
		KeyCode::U,				//0x16
		KeyCode::I,				//0x17
		KeyCode::O,				//0x18
		KeyCode::P,				//0x19
		KeyCode::LeftBracket,	//0x1a
		KeyCode::RightBracket,	//0x1b
		KeyCode::Return,		//0x1c
		KeyCode::LeftControl,	//0x1d
		KeyCode::A,				//0x1e
		KeyCode::S,				//0x1f
		KeyCode::D,				//0x20
		KeyCode::F,				//0x21
		KeyCode::G,				//0x22
		KeyCode::H,				//0x23
		KeyCode::J,				//0x24
		KeyCode::K,				//0x25
		KeyCode::L,				//0x26
		KeyCode::Semicolon,		//0x27
		KeyCode::Quote,			//0x28
		KeyCode::Grave,			//0x29
		KeyCode::LeftShift,		//0x2a
		KeyCode::BackSlash,		//0x2b
		KeyCode::Z,				//0x2c
		KeyCode::X,				//0x2d
		KeyCode::C,				//0x2e
		KeyCode::V,				//0x2f
		KeyCode::B,				//0x30
		KeyCode::N,				//0x31
		KeyCode::M,				//0x32
		KeyCode::Comma,			//0x33
		KeyCode::Dot,			//0x34
		KeyCode::Slash,			//0x35
		KeyCode::RightShift,	//0x36
		KeyCode::NumAsterisk,	//0x37
		KeyCode::RightAlt,		//0x38
		KeyCode::Space,			//0x39
		KeyCode::CapsLock,		//0x3a
		KeyCode::F1,			//0x3b
		KeyCode::F2,			//0x3c
		KeyCode::F3,			//0x3d
		KeyCode::F4,			//0x3e
		KeyCode::F5,			//0x3f
		KeyCode::F6,			//0x40
		KeyCode::F7,			//0x41
		KeyCode::F8,			//0x42
		KeyCode::F9,			//0x43
		KeyCode::F10,			//0x44
		KeyCode::NumLock,		//0x45
		KeyCode::ScrollLock,	//0x46
		KeyCode::Home,			//0x47
		KeyCode::Up,			//0x48
		KeyCode::PageUp,		//0x49
		KeyCode::Num2,			//0x50
		KeyCode::Num3,			//0x51
		KeyCode::Num0,			//0x52
		KeyCode::NumDecimal,	//0x53
		KeyCode::Unknown,		//0x54
		KeyCode::Unknown,		//0x55
		KeyCode::Down,		//0x56
		KeyCode::F11,			//0x57
		KeyCode::F12			//0x58
	};

	void Keyboard::Install(int irq) 
	{
		HAL::Get().SetInterruptRoutine(irq, (uint32_t)keyboard_irq);

		//assume BAT test is good. irq handler will override this if need be.
		m_BATTestResult = true;

		m_CurrentScanCode = 0;
		m_NumLock = m_ScrollLock = m_CapsLock = false;
		m_Shift = m_Alt = m_Control = false;
		UpdateLEDState();
	}

	bool Keyboard::GetScrollock()
	{
		return m_ScrollLock;
	}

	bool Keyboard::GetNumLock()
	{
		return m_NumLock;
	}

	bool Keyboard::GetCapsLock()
	{
		return m_CapsLock;
	}

	void Keyboard::ToggleNumLock()
	{
		m_NumLock = !m_NumLock;
		UpdateLEDState();
	}

	void Keyboard::ToggleCapsLock()
	{
		m_CapsLock = !m_CapsLock;
		UpdateLEDState();
	}

	void Keyboard::ToggleScrollLock()
	{
		m_ScrollLock = !m_ScrollLock;
		UpdateLEDState();
	}

	bool Keyboard::GetControl()
	{
		return m_Control;
	}

	bool Keyboard::GetAlt()
	{
		return m_Alt;
	}

	bool Keyboard::GetShift()
	{
		return m_Shift;
	}

	void Keyboard::IgnoreShouldResendLastCommand()
	{
		m_ShouldResendLastCommand = false;
	}

	bool Keyboard::ShouldResendLastCommand()
	{
		return m_ShouldResendLastCommand;
	}

	void Keyboard::SetShouldResendLastCommand()
	{
		m_ShouldResendLastCommand = true;
	}

	bool Keyboard::GetBATResult()
	{
		return m_BATTestResult;
	}

	bool Keyboard::GetDisagnosticResult()
	{
		return m_DiagnosticResult;
	}

	uint8_t Keyboard::GetCurrentScanCode()
	{
		return m_CurrentScanCode;
	}

	void Keyboard::UpdateLEDState() 
	{
		uint8_t data = 0;

		//set or clear approppriate bits
		data = (m_ScrollLock) ? (data | 1) : (data & 1);
		data = (m_NumLock) ? (m_NumLock | 2) : (m_NumLock & 2);
		data = (m_CapsLock) ? (m_NumLock | 4) : (m_NumLock & 4);

		//send the command
		EncoderSendCommand((uint8_t)EncoderCommand::SetLED);
		EncoderSendCommand(data);
	}

	KeyCode Keyboard::GetLastKeyCode() 
	{
		//if(m_CurrentScanCode != s_InvalidScancode)
		//{
		//	printf("%i\n", m_CurrentScanCode);
		//	printf("%i\n", s_KeyboardScanCodes[(int)m_CurrentScanCode]);
		//}

		return (m_CurrentScanCode != s_InvalidScancode) ? ((KeyCode)s_KeyboardScanCodes[(int)m_CurrentScanCode]) : (KeyCode::Unknown);
	}

	void Keyboard::DiscardLastKeyCode()
	{
		m_CurrentScanCode = s_InvalidScancode;
	}

	char Keyboard::KeyCodeToAscii(KeyCode code) 
	{
		uint8_t key = (uint8_t)code;

		if (isascii(key))
		{
			//if shift key is down or caps lock is on, make the key uppercase
			if (m_Shift || m_CapsLock)
			{
				if (key >= 'a' && key <= 'z')
				{
					key -= 32;
				}
			}

			if (m_Shift && !m_CapsLock)
			{
				if (key >= '0' && key <= '9')
				{
					switch (key)
					{
						case '0':
							key = (uint8_t)KeyCode::RightParenthisis;
							break;
						case '1':
							key = (uint8_t)KeyCode::Exclamation;
							break;
						case '2':
							key = (uint8_t)KeyCode::At;
							break;
						case '3':
							key = (uint8_t)KeyCode::Exclamation;
							break;
						case '4':
							key = (uint8_t)KeyCode::Hash;
							break;
						case '5':
							key = (uint8_t)KeyCode::Percent;
							break;
						case '6':
							key = (uint8_t)KeyCode::Carret;
							break;
						case '7':
							key = (uint8_t)KeyCode::Ampersand;
							break;
						case '8':
							key = (uint8_t)KeyCode::Asterisk;
							break;
						case '9':
							key = (uint8_t)KeyCode::LeftParenthisis;
							break;
					}
				}
				else
				{
					switch (code)
					{
						case KeyCode::Comma:
							key = (uint8_t)KeyCode::Less;
							break;

						case KeyCode::Dot:
							key = (uint8_t)KeyCode::Greater;
							break;

						case KeyCode::Slash:
							key = (uint8_t)KeyCode::Question;
							break;

						case KeyCode::Semicolon:
							key = (uint8_t)KeyCode::Colon;
							break;

						case KeyCode::Quote:
							key = (uint8_t)KeyCode::DoubleQuote;
							break;

						case KeyCode::LeftBracket :
							key = (uint8_t)KeyCode::LeftCurl;
							break;

						case KeyCode::RightBracket :
							key = (uint8_t)KeyCode::RightCurl;
							break;

						case KeyCode::Grave:
							key = (uint8_t)KeyCode::Tilde;
							break;

						case KeyCode::Minus:
							key = (uint8_t)KeyCode::UnderScore;
							break;

						case KeyCode::Plus:
							key = (uint8_t)KeyCode::Equal;
							break;

						case KeyCode::BackSlash:
							key = (uint8_t)KeyCode::Bar;
							break;
						default:
							break;
					}
				}
			}

			return key;
		}

		return 0;
	}

	void Keyboard::Disable() 
	{
		ControlSendCommand ((uint8_t)ControlCommand::Disable);
		m_Disabled = true;
	}

	void Keyboard::Enable()
	{
		ControlSendCommand ((uint8_t)ControlCommand::Enable);
		m_Disabled = false;
	}

	bool Keyboard::IsEnabled() 
	{
		return !m_Disabled;
	}

	void Keyboard::ResetSystem()
	{
		//write 11111110 to the output port (sets reset system line low)
		ControlSendCommand((uint8_t)ControlCommand::WriteOutPort);
		EncoderSendCommand(0xfe);
	}

	uint8_t Keyboard::ControlReadStatus()
	{
		return HAL::Get().InB((unsigned short)ControlIO::StatusRegister);
	}

	void Keyboard::ControlSendCommand(uint8_t cmd)
	{
		while ((ControlReadStatus() & (uint8_t)ControlStatusMask::InBuffer) != 0)
		{
			//wait for input buffer
		}

		kernel::hal::HAL::Get().OutB((unsigned short)ControlIO::CommandRegister, cmd);
	}

	uint8_t Keyboard::EncoderReadBuffer() 
	{
		return HAL::Get().InB((unsigned short)EncoderIO::InputBuffer);
	}

	void Keyboard::EncoderSendCommand (uint8_t cmd)
	{
		while ((ControlReadStatus() & (uint8_t)ControlStatusMask::InBuffer) != 0)
		{
			//wait for input buffer
		}

		HAL::Get().OutB((unsigned short)EncoderIO::CommandRegister, cmd);
	}

	void Keyboard::HandleInterrupt()
	{
		int code = 0;

		//check if the out buffer is full (contains a scan code)
		if (Keyboard::Get().ControlReadStatus () & (uint8_t)ControlStatusMask::OutBuffer)
		{
			code = Keyboard::Get().EncoderReadBuffer();

			if (code == 0xE0 || code == 0xE1)
			{
				//TODO Extended scan codes.
			}
			else 
			{
				//check if this is a break code (key released)
				if (code & 0x80) 
				{	
					code -= 0x80;

					KeyCode key = s_KeyboardScanCodes[code];

					//check if a modifier key was released.
					switch (key) 
					{
						case KeyCode::LeftControl:
						case KeyCode::RightControl:
							m_Control = false;
							break;

						case KeyCode::LeftShift:
						case KeyCode::RightShift:
							m_Shift = false;
							break;

						case KeyCode::LeftAlt:
						case KeyCode::RightAlt:
							m_Alt = false;
							break;
						default:
							break;
					}
				}
				else 
				{
					KeyCode key = s_KeyboardScanCodes[code];

					//check if a modifier key was pressed
					switch(key)
					{
						case KeyCode::LeftControl:
						case KeyCode::RightControl:
							m_Control = true;
							break;

						case KeyCode::LeftShift:
						case KeyCode::RightShift:
							m_Shift = true;
							break;

						case KeyCode::LeftAlt:
						case KeyCode::RightAlt:
							m_Alt = true;
							break;

						case KeyCode::CapsLock:
							ToggleCapsLock();
							break;

						case KeyCode::NumLock:
							ToggleNumLock();
							break;

						case KeyCode::ScrollLock:
							ToggleScrollLock();
							break;
						default:
							m_CurrentScanCode = code;
							break;
					}
				}
			}

			//check for error
			switch (code)
			{
				case (int)ErrorCode::BATFailed:
					m_BATTestResult = false;
					break;

				case (int)ErrorCode::DiagnosticFailed:
					m_DiagnosticResult = false;
					break;

				case (int)ErrorCode::ResendCommand:
					m_ShouldResendLastCommand = true;
					break;
			}
		}

		HAL::Get().InterruptFinished(0);
	}

	extern "C" void keyboard_irq_main ()
	{
		Keyboard::Get().HandleInterrupt();
	}
}