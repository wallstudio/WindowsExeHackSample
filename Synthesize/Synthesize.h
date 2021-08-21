#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <string>
#include <memory>

namespace Synthesize
{
	enum class AITalkResultCode
	{
		AITALKERR_SUCCESS = 0,
		AITALKERR_INTERNAL_ERROR = -1,
		AITALKERR_UNSUPPORTED = -2,
		AITALKERR_INVALID_ARGUMENT = -3,
		AITALKERR_WAIT_TIMEOUT = -4,
		AITALKERR_NOT_INITIALIZED = -10,
		AITALKERR_ALREADY_INITIALIZED = 10,
		AITALKERR_NOT_LOADED = -11,
		AITALKERR_ALREADY_LOADED = 11,
		AITALKERR_INSUFFICIENT = -20,
		AITALKERR_PARTIALLY_REGISTERED = 21,
		AITALKERR_LICENSE_ABSENT = -100,
		AITALKERR_LICENSE_EXPIRED = -101,
		AITALKERR_LICENSE_REJECTED = -102,
		AITALKERR_TOO_MANY_JOBS = -201,
		AITALKERR_INVALID_JOBID = -202,
		AITALKERR_JOB_BUSY = -203,
		AITALKERR_NOMORE_DATA = 204,
		AITALKERR_OUT_OF_MEMORY = -206,
		AITALKERR_FILE_NOT_FOUND = -1001,
		AITALKERR_PATH_NOT_FOUND = -1002,
		AITALKERR_READ_FAULT = -1003,
		AITALKERR_COUNT_LIMIT = -1004,
		AITALKERR_USERDIC_LOCKED = -1011,
		AITALKERR_USERDIC_NOENTRY = -1012,
	};

	enum class AITalkStatusCode
	{
		AITALKSTAT_WRONG_STATE = -1,
		AITALKSTAT_INPROGRESS = 10,
		AITALKSTAT_STILL_RUNNING = 11,
		AITALKSTAT_DONE = 12
	};

	enum class AITalkJobInOut
	{
		AITALKIOMODE_PLAIN_TO_WAVE = 11,
		AITALKIOMODE_AIKANA_TO_WAVE = 12,
		AITALKIOMODE_JEITA_TO_WAVE = 13,
		AITALKIOMODE_PLAIN_TO_AIKANA = 21,
		AITALKIOMODE_AIKANA_TO_JEITA = 0x20
	};

	enum class AIAudioResultCode
	{
		AIAUDIOERR_SUCCESS = 0,
		AIAUDIOERR_INTERNAL_ERROR = -1,
		AIAUDIOERR_UNSUPPORTED = -2,
		AIAUDIOERR_INVALID_ARGUMENT = -3,
		AIAUDIOERR_WAIT_TIMEOUT = -4,
		AIAUDIOERR_NOT_INITIALIZED = -10,
		AIAUDIOERR_NOT_OPENED = -11,
		AIAUDIOERR_ALREADY_OPENED = 11,
		AIAUDIOERR_NO_AUDIO_HARDWARE = -20,
		AIAUDIOERR_DEVICE_INVALIDATED = -21,
		AIAUDIOERR_INSUFFICIENT = -30,
		AIAUDIOERR_OUT_OF_MEMORY = -100,
		AIAUDIOERR_INVALID_CALL = -101,
		AIAUDIOERR_INVALID_EVENT_ORDINAL = -102,
		AIAUDIOERR_NO_NOTIFICATION_CALLBACK = 103,
		AIAUDIOERR_NO_PLAYING = 111,
		AIAUDIOERR_WRITE_FAULT = -201
	};

	enum class AIAudioFormatType
	{
		AIAUIDOTYPE_NONE = 0,
		AIAUDIOTYPE_PCM_16 = 1,
		AIAUDIOTYPE_PCM_8 = 769,
		AIAUDIOTYPE_MULAW_8 = 7,
		AIAUDIOTYPE_ALAW_8 = 6
	};
	
#pragma pack(1)
	struct AITalk_TConfig
	{
		std::uint32_t hzVoiceDB;
		LPCSTR dirVoiceDBS;
		std::uint32_t msecTimeout;
		LPCSTR pathLicense;
		LPCSTR codeAuthSeed;
		std::uint32_t __reserved__;
	};

	struct AITalk_TJobParam
	{
		AITalkJobInOut modeInOut;
		void* userData;
	};
	
	struct AITalk_TTtsParam
	{
		struct TJeitaParam
		{
			char femaleName[80];
			char maleName[80];
			std::int32_t pauseMiddle;
			std::int32_t pauseLong;
			std::int32_t pauseSentence;
			char control[12];
		};
		struct TSpeakerParam
		{
			char voiceName[80];
			float volume;
			float speed;
			float pitch;
			float range;
			std::uint32_t pauseMiddle;
			std::uint32_t pauseLong;
			std::uint32_t pauseSentence;
			char styleRate[80];
		};
		enum class ExtendFormat
		{
			None = 0U,
			JeitaRuby = 1U,
			AutoBookmark = 16U
		};


		std::uint32_t size;
		void* procTextBuf;
		void* procRawBuf;
		void* procEventTts;
		std::uint32_t lenTextBufBytes;
		std::uint32_t lenRawBufBytes;
		float volume;
		std::int32_t pauseBegin;
		std::int32_t pauseTerm;
		ExtendFormat extendFormat;
		char voiceName[80];
		TJeitaParam Jeita;
		std::uint32_t numSpeakers;
		std::int32_t __reserved__;
		TSpeakerParam speaker[1];
	};

	struct AIAudio_TConfig
	{
		void (*procNotify)(std::uint64_t, void*) ;
		std::uint32_t msecLatency;
		std::uint32_t lenBufferBytes;
		std::uint32_t hzSamplesPerSec;
		AIAudioFormatType formatTag;
		std::int32_t __reserved__;
		LPCSTR descDevice;
	};

	struct AIAudio_TWaveFormat
	{
		std::int32_t header;
		std::uint32_t hzSamplesPerSec;
		AIAudioFormatType formatTag;
	};

	struct Wave
	{
		struct FileHeader
		{
			char wave[4] { 'W', 'A', 'V', 'E' };
			char fmt[4] { 'f', 'm', 't', ' ' };
			std::uint32_t formatDefineSize = 0x00000010;
			std::uint16_t compression = 1;
			std::uint16_t channel = 1;
			std::uint32_t frequency = 44100;
			std::uint32_t dataRate = 44100 * sizeof(short) * 1; // H*Q*C
			std::uint16_t sampleSize = 0x0002; // Q*C
			std::uint16_t quantity = 0x0010;
		};
		struct DataHeader
		{
			char data[4]{ 'd', 'a', 't', 'a' };
			std::uint32_t size;
		};
		
		char riff[4] { 'R', 'I', 'F', 'F' };
		std::uint32_t size;
		FileHeader fileHeader;
		DataHeader dataHeader;
	};
#pragma pack()

	template<typename T>
	inline T GetOriginalFunction(std::string name)
	{
		static auto original = LoadLibraryA("C:\\Program Files\\AHS\\VOICEROID2\\aitalked.dll");
		return (T)GetProcAddress(original, name.c_str());
	}

	inline void PcmToWave(std::vector<std::byte>& pcm, std::vector<std::byte>& wave)
	{
		auto header = Wave
		{
			.size = (std::uint32_t)(sizeof(Wave::FileHeader) + sizeof(Wave::DataHeader) + pcm.size()),
			.dataHeader = Wave::DataHeader {.size = (std::uint32_t)pcm.size() },
		};
		wave = std::vector<std::byte>(sizeof(Wave) + pcm.size());
		memcpy(wave.data(), &header, sizeof(Wave));
		memcpy(wave.data() + sizeof(Wave), pcm.data(), pcm.size());
	}

	inline AIAudioResultCode AIAudioAPI_DeviceInfo(LPCSTR guid, LPCSTR name, std::int32_t bufferLen, std::int32_t& requireLen)
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_DeviceInfo)>("AIAudioAPI_DeviceInfo");
		return base(guid, name, bufferLen, requireLen);
	}
	inline std::uint32_t AITalkAPI_ModuleFlag()
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_ModuleFlag)>("AITalkAPI_ModuleFlag");
		return base();
	}
	inline AITalkResultCode AITalkAPI_LicenseInfo(LPCSTR key, LPCSTR str, std::int32_t len)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_LicenseInfo)>("AITalkAPI_LicenseInfo");
		return base(key, str, len);
	}
	inline AITalkResultCode AITalkAPI_LicenseDate(LPCSTR data)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_LicenseDate)>("AITalkAPI_LicenseDate");
		return base(data);
	}
	inline AIAudioResultCode AIAudioAPI_Open(AIAudio_TConfig& config)
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_Open)>("AIAudioAPI_Open");
		return base(config);
	}
	inline AIAudioResultCode AIAudioAPI_Close()
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_Close)>("AIAudioAPI_Close");
		return base();
	}
	inline AIAudioResultCode AIAudioAPI_PushData(UINT8* buf, std::uint32_t len, std::int32_t stop)
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_PushData)>("AIAudioAPI_PushData");
		return base(buf, len, stop);
	}
	inline AIAudioResultCode AIAudioAPI_PushEvent(UINT64 tick, void* userData)
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_PushEvent)>("AIAudioAPI_PushEvent");
		return base(tick, userData);
	}
	inline AIAudioResultCode AIAudioAPI_ClearData()
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_ClearData)>("AIAudioAPI_ClearData");
		return base();
	}
	inline AIAudioResultCode AIAudioAPI_Suspend()
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_Suspend)>("AIAudioAPI_Suspend");
		return base();
	}
	inline AIAudioResultCode AIAudioAPI_Resume()
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_Resume)>("AIAudioAPI_Resume");
		return base();
	}
	inline AIAudioResultCode AIAudioAPI_SaveWave(LPCSTR path, AIAudio_TWaveFormat& format, UINT8* buf, std::uint32_t len)
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_SaveWave)>("AIAudioAPI_SaveWave");
		return base(path, format, buf, len);
	}
	inline AIAudioResultCode AIAudioAPI_GetDescriptor(std::int32_t target, std::byte* desc, std::uint32_t bufferLen, std::uint32_t& requireLen)
	{
		static auto base = GetOriginalFunction<decltype(&AIAudioAPI_GetDescriptor)>("AIAudioAPI_GetDescriptor");
		return base(target, desc, bufferLen, requireLen);
	}
	inline AITalkResultCode AITalkAPI_Init(AITalk_TConfig& config)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_Init)>("AITalkAPI_Init");
		return base(config);
	}
	inline AITalkResultCode AITalkAPI_End()
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_End)>("AITalkAPI_End");
		return base();
	}
	inline AITalkResultCode AITalkAPI_VersionInfo(std::int32_t verbose, LPCSTR sjis, std::uint32_t bufferLen, std::uint32_t& requireLen)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_VersionInfo)>("AITalkAPI_VersionInfo");
		return base(verbose, sjis, bufferLen, requireLen);
	}
	inline AITalkResultCode AITalkAPI_GetStatus(std::int32_t jobID, AITalkStatusCode& status)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_GetStatus)>("AITalkAPI_GetStatus");
		return base(jobID, status);
	}
	inline AITalkResultCode AITalkAPI_SetParam(AITalk_TTtsParam& param)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_SetParam)>("AITalkAPI_SetParam");
		return base(param);
	}
	inline AITalkResultCode AITalkAPI_GetParam(AITalk_TTtsParam& param, std::uint32_t& size)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_GetParam)>("AITalkAPI_GetParam");
		return base(param, size);
	}
	inline AITalkResultCode AITalkAPI_LangLoad(LPCSTR dirLang)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_LangLoad)>("AITalkAPI_LangLoad");
		return base(dirLang);
	}
	inline AITalkResultCode AITalkAPI_LangClear()
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_LangClear)>("AITalkAPI_LangClear");
		return base();
	}
	inline AITalkResultCode AITalkAPI_VoiceLoad(LPCSTR voiceName)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_VoiceLoad)>("AITalkAPI_VoiceLoad");
		return base(voiceName);
	}
	inline AITalkResultCode AITalkAPI_VoiceClear()
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_VoiceClear)>("AITalkAPI_VoiceClear");
		return base();
	}
	inline AITalkResultCode AITalkAPI_TextToSpeech(std::int32_t& jobID, AITalk_TJobParam& param, LPCSTR text)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_TextToSpeech)>("AITalkAPI_TextToSpeech");
		return base(jobID, param, text);
	}
	inline AITalkResultCode AITalkAPI_CloseSpeech(std::int32_t jobID, std::int32_t useEvent)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_CloseSpeech)>("AITalkAPI_CloseSpeech");
		return base(jobID, useEvent);
	}
	inline AITalkResultCode AITalkAPI_GetData(std::int32_t jobID, std::int16_t* rawBuf, std::uint32_t bufferLen, std::uint32_t& requireLen)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_GetData)>("AITalkAPI_GetData");
		return base(jobID, rawBuf, bufferLen, requireLen);
	}
	inline AITalkResultCode AITalkAPI_TextToKana(std::int32_t& jobID, AITalk_TJobParam& param, LPCSTR text)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_TextToKana)>("AITalkAPI_TextToKana");
		return base(jobID, param, text);
	}
	inline AITalkResultCode AITalkAPI_CloseKana(std::int32_t jobID, std::int32_t useEvent)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_CloseKana)>("AITalkAPI_CloseKana");
		return base(jobID, useEvent);
	}
	inline AITalkResultCode AITalkAPI_GetKana(std::int32_t jobID, LPCSTR textBuf, std::uint32_t bufferLen, std::uint32_t& requireLen, std::uint32_t& pos)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_GetKana)>("AITalkAPI_GetKana");
		return base(jobID, textBuf, bufferLen, requireLen,  pos);
	}
	inline AITalkResultCode AITalkAPI_GetJeitaControl(std::int32_t jobID, LPCSTR control)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_GetJeitaControl)>("AITalkAPI_GetJeitaControl");
		return base(jobID, control);
	}
	inline AITalkResultCode AITalkAPI_BLoadWordDic()
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_BLoadWordDic)>("AITalkAPI_BLoadWordDic");
		return base();
	}
	inline AITalkResultCode AITalkAPI_ReloadWordDic(LPCSTR path)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_ReloadWordDic)>("AITalkAPI_ReloadWordDic");
		return base(path);
	}
	inline AITalkResultCode AITalkAPI_ReloadPhraseDic(LPCSTR path)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_ReloadPhraseDic)>("AITalkAPI_ReloadPhraseDic");
		return base(path);
	}
	inline AITalkResultCode AITalkAPI_ReloadSymbolDic(LPCSTR path)
	{
		static auto base = GetOriginalFunction<decltype(&AITalkAPI_ReloadSymbolDic)>("AITalkAPI_ReloadSymbolDic");
		return base(path);
	}
}
