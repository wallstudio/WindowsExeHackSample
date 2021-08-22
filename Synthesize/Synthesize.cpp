#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <random>
#include <regex>
#include <codecvt>
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

std::string GetSeed()
{
    auto random = std::random_device();
    auto tmpFile = std::filesystem::temp_directory_path() / std::format("seed_{}", random());

	std::system(std::format("Extract.exe > {}", tmpFile.string().c_str()).c_str());
    long fileSize = std::filesystem::file_size(tmpFile);
    auto srcBuff = std::vector<char>(fileSize);
    {
        auto sr = std::ifstream(tmpFile);
        sr.read(srcBuff.data(), fileSize);
    }
    std::filesystem::remove(tmpFile);

	return std::regex_replace(srcBuff.data(), std::regex("\\n"), "");
}

int main()
{
	using namespace Synthesize;
	try
	{
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
		Check(AITalkAPI_ReloadPhraseDic("C:\\Users\\huser\\Documents\\VOICEROID2\\�t���[�Y����\\user.pdic"));
		Check(AITalkAPI_ReloadWordDic("C:\\Users\\huser\\Documents\\VOICEROID2\\�P�ꎫ��\\user.wdic"));
		Check(AITalkAPI_ReloadSymbolDic("C:\\Users\\huser\\Documents\\VOICEROID2\\�L���|�[�Y����\\user.sdic"));
		Check(AITalkAPI_VoiceLoad("tamiyasu_44"));
		//auto params = AITalk_TTtsParam{ .size = sizeof(AITalk_TTtsParam), };
		//Check(AITalkAPI_GetParam(params, params.size));
		//Check(AITalkAPI_SetParam(params));

		int job = 0;
		AITalk_TJobParam jobParam;

		auto kanaBuff = std::vector<char>(2048);
		std::uint32_t req, prop;
		jobParam = AITalk_TJobParam{ .modeInOut = AITalkJobInOut::AITALKIOMODE_PLAIN_TO_AIKANA };
		Check(AITalkAPI_TextToKana(job, jobParam, "�A�t���J�����w�@"));
		Check(AITalkAPI_GetKana(job, kanaBuff.data(), kanaBuff.size(), req, prop));
		Check(AITalkAPI_CloseKana(job, 0));
		auto kana = std::string(kanaBuff.data());

		jobParam = AITalk_TJobParam{ .modeInOut = AITalkJobInOut::AITALKIOMODE_AIKANA_TO_WAVE };
		Check(AITalkAPI_TextToSpeech(job, jobParam, kana.c_str()));
		auto pcm = std::vector<short>();
		auto pcmBuff = std::array<short, 128 * 1024>();
		for (auto result = AITalkResultCode::AITALKERR_SUCCESS; result == AITalkResultCode::AITALKERR_SUCCESS;)
		{
			std::uint32_t written = 0;
			result = AITalkAPI_GetData(job, pcmBuff.data(), pcmBuff.size(), written);
			pcm.insert(pcm.end(), pcmBuff.begin(), pcmBuff.begin() + written);
		}

		auto wave = PcmToWave(pcm);
		auto file = std::ofstream("C:\\Users\\huser\\Desktop\\b.wav", std::ios_base::binary | std::ios_base::out);
		file.write((char*)wave.data(), wave.size());
	}
	catch (AITalkResultCode code)
	{

	}
}
