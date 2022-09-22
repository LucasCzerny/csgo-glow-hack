#include "ExternalProcess.h"
#include <iostream>

struct offsets
{
	uintptr_t localPlayer = 0xD8C2CC;
	uintptr_t glowIndex = 0xA438;
	uintptr_t glowObjectManager = 0x52EC550;
	uintptr_t team = 0xF4;
	uintptr_t entityList = 0x4DA3F5C;
} offsets;

struct values
{
	uintptr_t clientDLL;
	uintptr_t localPlayer;
	uintptr_t glowObjectManager;
} values;

struct GlowStruct
{
	BYTE base[4];
	float red;
	float green;
	float blue;
	float alpha;
	BYTE buffer[16];
	bool renderWhenOccluded;
	bool renderWhenUnOccluded;
	bool fullBloom;
	BYTE buffer1[5];
	int glowStyle;
};

ExternalProcess csgo(L"csgo.exe");

void SetGlow(int glowIndex, GlowStruct newGlow)
{
	uintptr_t location = values.glowObjectManager + 0x38 * glowIndex;
	GlowStruct glow = csgo.ReadMemory<GlowStruct>(location);

	glow.red = newGlow.red;
	glow.green = newGlow.green;
	glow.blue = newGlow.blue;
	glow.alpha = newGlow.alpha;
	glow.renderWhenOccluded = true;
	glow.renderWhenUnOccluded = true;

	csgo.WriteMemory<GlowStruct>(location, glow);
}

void HandleGlow()
{
	values.glowObjectManager = csgo.ReadMemory<uintptr_t>(values.clientDLL + offsets.glowObjectManager);

	for (unsigned int i = 0; i < 64; i++)
	{
		uintptr_t entity = csgo.ReadMemory<uintptr_t>(values.clientDLL + offsets.entityList + i * 0x10);

		if (entity)
		{
			// ent team
			int glowIndex = csgo.ReadMemory<int>(entity + offsets.glowIndex);

			GlowStruct glow;
			glow.red = 1.0f;
			glow.green = 0.0f;
			glow.blue = 1.0f;
			glow.alpha = 1.0f;

			SetGlow(glowIndex, glow);
		}
	}
}

int main()
{
	values.clientDLL = csgo.GetModuleBaseAddress(L"client.dll");
	values.localPlayer = 0;

	while (values.localPlayer == 0)
	{
		values.localPlayer = csgo.ReadMemory<uintptr_t>(values.clientDLL + offsets.localPlayer);
	}

	std::cout << "LocalPlayer: " << std::hex << values.localPlayer << "\n";

	bool glow = true;

	while (true)
	{
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			glow = !glow;
			std::cout << "Glow " << (glow ? "enabled" : "disabled") << "\n";
		}

		if (glow)
		{
			HandleGlow();
		}
	}
}