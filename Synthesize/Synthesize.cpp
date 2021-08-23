#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <random>
#include <regex>
#include <codecvt>
#include <thread>
#include <chrono>
#include "Synthesize.h"

void Check(Synthesize::AITalkResultCode status)
{
	if (status != Synthesize::AITalkResultCode::AITALKERR_SUCCESS)
		throw status;
}

void Check(Synthesize::AIAudioResultCode status)
{
	if (status != Synthesize::AIAudioResultCode::AIAUDIOERR_SUCCESS)
		throw status;
}

int main()
{
	using namespace Synthesize;
	try
	{
		// 初期化
		auto seed = GetSeed();
		std::filesystem::current_path("C:\\Program Files\\AHS\\VOICEROID2");
		auto config = AITalk_TConfig
		{
			.hzVoiceDB = 44100,
			.dirVoiceDBS = "C:\\Program Files (x86)\\AHS\\VOICEROID2\\Voice",
			.msecTimeout = 10000,
			.pathLicense = "aitalk.lic",
			.codeAuthSeed = seed.c_str(),
			.__reserved__ = 0,
		};
		Check(AITalkAPI_End());
		Check(AITalkAPI_Init(config));
		Check(AITalkAPI_LangLoad("C:\\Program Files (x86)\\AHS\\VOICEROID2\\Lang\\standard"));
		Check(AITalkAPI_ReloadPhraseDic("C:\\Users\\huser\\Documents\\VOICEROID2\\フレーズ辞書\\user.pdic"));
		Check(AITalkAPI_ReloadWordDic("C:\\Users\\huser\\Documents\\VOICEROID2\\単語辞書\\user.wdic"));
		Check(AITalkAPI_ReloadSymbolDic("C:\\Users\\huser\\Documents\\VOICEROID2\\記号ポーズ辞書\\user.sdic"));
		Check(AITalkAPI_VoiceLoad("tamiyasu_44"));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//auto params = AITalk_TTtsParam{ .size = sizeof(AITalk_TTtsParam), };
		//Check(AITalkAPI_GetParam(params, params.size));
		//Check(AITalkAPI_SetParam(params));

		static int job = 0;
		static AITalk_TJobParam jobParam;

		// 制御文字列へ変換
		auto kanaBuff = std::vector<char>(2048);
		std::uint32_t req, prop;
		jobParam = AITalk_TJobParam{ .modeInOut = AITalkJobInOut::AITALKIOMODE_PLAIN_TO_AIKANA };
		Check(AITalkAPI_TextToKana(job, jobParam, "アフリカ王立学院"));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		Check(AITalkAPI_GetKana(job, kanaBuff.data(), kanaBuff.size(), req, prop));
		auto kana = std::string(kanaBuff.data());
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// PCMへ変換
		jobParam = AITalk_TJobParam{ .modeInOut = AITalkJobInOut::AITALKIOMODE_AIKANA_TO_WAVE };
		Check(AITalkAPI_TextToSpeech(job, jobParam, kana.c_str()));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		auto pcm = std::vector<short>();
		auto pcmBuff = std::array<short, 1024>();
		for (auto result = AITalkResultCode::AITALKERR_SUCCESS; result == AITalkResultCode::AITALKERR_SUCCESS;)
		{
			std::uint32_t written = 0;
			result = AITalkAPI_GetData(job, pcmBuff.data(), pcmBuff.size(), written);
			pcm.insert(pcm.end(), pcmBuff.begin(), pcmBuff.begin() + written);
		}

		// Waveへ変換
		auto wave = PcmToWave(pcm);
		auto file = std::ofstream("C:\\Users\\huser\\Desktop\\b.wav", std::ios_base::binary | std::ios_base::out);
		file.write((char*)wave.data(), wave.size());
	}
	catch (AITalkResultCode code)
	{

	}
}
