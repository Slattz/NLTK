#include <codecvt>
#include <locale>
#include <memory>
#include "utils.h"

cursorinfo_s g_cursorpos;
static const char*  iconfilenametable[] = {"icn_000", "icn_001", "icn_001", "icn_001", "icn_001", "icn_001", "icn_001", "icn_001", "icn_001", "icn_001", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_010", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_039", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_069", "icn_109", "icn_109", "icn_109", "icn_109", "icn_109", "icn_109", "icn_109", "icn_109", "icn_109", "icn_109", "icn_119", "icn_127", "icn_128", "icn_129", "icn_130", "icn_131", "icn_132", "icn_133", "icn_134", "icn_135", "icn_136", "icn_137", "icn_138", "icn_139", "icn_140", "icn_141", "icn_142", "icn_143", "icn_144", "icn_145", "icn_146", "icn_147", "icn_148", "icn_149", "icn_150", "icn_151", "icn_152", "icn_153", "icn_154", "icn_155", "icn_156", "icn_157", "icn_158", "icn_159", "icn_160", "icn_161", "icn_162", "icn_163", "icn_164", "icn_165", "icn_166", "icn_167", "icn_168", "icn_169", "icn_170", "icn_171", "icn_172", "icn_173", "icn_174", "icn_175", "icn_176", "icn_177", "icn_178", "icn_179", "icn_180", "icn_181", "icn_182", "icn_183", "icn_184", "icn_185", "icn_186", "icn_187", "icn_188", "icn_189", "icn_190", "icn_191", "icn_192", "icn_193", "icn_194", "icn_195", "icn_196", "icn_197", "icn_198", "icn_199", "icn_200", "icn_201", "icn_202", "icn_203", "icn_204", "icn_205", "icn_206", "icn_207", "icn_208", "icn_209", "icn_210", "icn_211", "icn_212", "icn_213", "icn_214", "icn_215", "icn_216", "icn_217", "icn_218", "icn_219", "icn_220", "icn_221", "icn_222", "icn_223", "icn_224", "icn_225", "icn_226", "icn_227", "icn_228", "icn_230", "icn_231", "icn_232", "icn_233", "icn_234", "icn_235", "icn_236", "icn_237", "icn_238", "icn_239", "icn_240", "icn_241", "icn_242", "icn_243", "icn_245", "icn_246", "icn_247", "icn_248", "icn_249", "icn_250", "icn_251", "icn_252", "icn_253", "icn_254", "icn_255", "icn_256", "icn_257", "icn_258", "icn_259", "icn_260", "icn_261", "icn_262", "icn_263", "icn_264", "icn_265", "icn_266", "icn_267", "icn_268", "icn_269", "icn_270", "icn_271", "icn_272", "icn_273", "icn_274", "icn_275", "icn_276", "icn_277", "icn_278", "icn_279", "icn_280", "icn_281", "icn_282", "icn_283", "icn_284", "icn_285", "icn_286", "icn_287", "icn_288", "icn_289", "icn_290", "icn_291", "icn_292", "icn_293", "icn_294", "icn_295", "icn_296", "icn_297", "icn_298", "icn_299", "icn_300", "icn_301", "icn_302", "icn_303", "icn_304", "icn_305", "icn_306", "icn_307", "icn_308", "icn_309", "icn_310", "icn_311", "icn_312", "icn_313", "icn_314", "icn_315", "icn_316", "icn_317", "icn_318", "icn_319", "icn_320", "icn_321", "icn_322", "icn_323", "icn_324", "icn_325", "icn_326", "icn_327", "icn_328", "icn_329", "icn_330", "icn_331", "icn_332", "icn_333", "icn_334", "icn_335", "icn_337", "icn_338", "icn_339", "icn_342", "icn_343", "icn_345", "icn_346", "icn_347", "icn_348", "icn_349", "icn_360", "icn_361", "icn_362", "icn_351", "icn_352", "icn_353", "icn_354", "icn_355", "icn_356", "icn_357", "icn_358", "icn_359", "icn_363", "icn_364", "icn_365", "icn_368", "icn_370", "icn_371", "icn_372", "icn_373", "icn_374", "icn_375", "icn_376", "icn_377", "icn_378", "icn_379", "icn_380", "icn_381", "icn_382", "icn_383", "icn_384", "icn_385", "icn_386", "icn_387", "icn_388", "icn_389", "icn_390", "icn_391", "icn_392", "icn_394", "icn_395", "icn_396", "icn_397", "icn_398", "icn_399", "icn_400", "icn_401", "icn_402", "icn_403", "icn_404", "icn_405", "icn_406", "icn_407", "icn_408", "icn_409", "icn_410", "icn_411", "icn_412", "icn_413", "icn_414", "icn_415", "icn_416", "icn_417", "icn_418", "icn_419", "icn_420", "icn_421", "icn_422", "icn_423", "icn_424", "icn_425", "icn_426", "icn_427", "icn_428", "icn_429", "icn_430", "icn_431", "icn_432", "icn_433", "icn_434", "icn_435", "icn_436", "icn_437", "icn_438", "icn_439", "icn_440", "icn_441", "icn_442", "icn_443", "icn_444", "icn_446", "icn_447", "icn_448", "icn_449", "icn_450", "icn_451", "icn_452", "icn_453", "icn_454", "icn_455", "icn_456", "icn_457", "icn_458", "icn_459", "icn_460", "icn_462", "icn_463", "icn_464", "icn_465", "icn_466", "icn_467", "icn_469", "icn_470", "icn_471", "icn_472", "icn_473", "icn_474", "icn_475", "icn_476", "icn_477", "icn_478", "icn_479", "icn_480", "icn_481", "icn_482", "icn_483", "icn_484", "icn_485", "icn_486", "icn_487", "icn_488", "icn_489", "icn_490", "icn_491", "icn_492", "icn_493", "icn_494", "icn_495", "icn_496", "icn_497", "icn_498", "icn_499", "icn_500", "icn_501", "icn_502", "icn_503", "icn_504", "icn_505", "icn_506", "icn_507", "icn_001", "icn_509", "icn_510", "icn_511", "icn_512", "icn_513", "icn_514", "icn_515", "icn_516", "icn_517", "icn_518", "icn_519", "icn_520", "icn_521", "icn_393", "icn_508", "icn_468", "icn_369", "icn_366", "icn_522"};

/*
	bool IsSDCardInserted(void)
		=> returns whether or not the SD Card is inserted
*/
bool IsSDCardInserted() {
	bool inserted = false;
	FSUSER_IsSdmcDetected(&inserted);
	return inserted;
}

/*
	bool IsGameCartInserted(void)
		=> returns whether or not a GameCard is inserted
*/
bool IsGameCartInserted() {
	bool inserted = false;
	FSUSER_CardSlotIsInserted(&inserted);
	return inserted;
}

int    RandomNum(int start, int end)
{
    srand(svcGetSystemTick());
    int r[20];

    for (int i = 0; i < 20; i++)
        r[i] = rand() % (end - start + 1) + start;

    return (r[rand() % 20]);
}

//Credit to SciresM for this code! :)
u32     DecryptACNLValue(u64 encval)
{
    // Unpack 64-bit value into (u32, u16, u8, u8) values.
    u32 enc = (encval & 0xFFFFFFFF);
    u16 adjust = ((encval >> 32) & 0xFFFF);
    u8  shift_val = ((encval >> 48) & 0xFF);
    u8  chk = ((encval >> 56) & 0xFF);
    // Validate 8-bit checksum
    if ((((enc >> 0) + (enc >> 8) + (enc >> 16) + (enc >> 24) + 0xBA) & 0xFF) != chk) return 0;
        
    u8  left_shift = ((0x1C - shift_val) & 0xFF);
    u8  right_shift = 0x20 - left_shift;

    // Handle error case: Invalid shift value.
    if (left_shift >= 0x20)
    {
        return 0 + (enc << right_shift) - (adjust + 0x8F187432);
    }

    // This case should occur for all game-generated values.
    return (enc << left_shift) + (enc >> right_shift) - (adjust + 0x8F187432);
}

u64     EncryptACNLValue(u32 decval)
{
    // Make a new RNG
    u16 adjust = RandomNum(0, 0x10000);
    u8  shift_val = RandomNum(0, 0x1A);

    // Encipher value
    u32 enc = decval + adjust + 0x8F187432;
    enc = (enc >> (0x1C - shift_val)) + (enc << (shift_val + 4));

    // Calculate Checksum
    u8  chk = (((enc >> 0) + (enc >> 8) + (enc >> 16) + (enc >> 24) + 0xBA) & 0xFF);

    // Pack result
    return ((u64)enc << 0) | ((u64)adjust << 32) | ((u64)shift_val << 48) | ((u64)chk << 56);
}

char Sstrncpy(char *dest, const char *src, size_t n) //'Safe' strncpy, always null terminates
{
    strncpy(dest, src, n);
    return dest[n] = '\0';
}

bool is_ACNL(u64 tid)
{
    if (tid == JPN_TID || tid == USA_TID || tid == EUR_TID || tid == KOR_TID)
        return true;

    else if (tid == JPN_WA_TID || tid == USA_WA_TID || tid == EUR_WA_TID || tid == KOR_WA_TID)
        return true;

    else return false;
}

bool DebugCode(bool complete)
{
    static const u32 debug_code[] =
    {
        KEY_X, KEY_Y, KEY_X, KEY_Y, KEY_UP,
        KEY_RIGHT, KEY_DOWN, KEY_LEFT, KEY_X, KEY_A
    };

    const u32 maxpos = sizeof(debug_code)/sizeof(debug_code[0]);
    static u32 pos = 0;
    static u32 timeout = 60;
    u32 down = hidKeysDown();

    if (!complete)
    {
        if (timeout > 0)
            timeout--;

        if (timeout == 0)
            pos = 0;

        if (pos == maxpos)
        {
            pos = 0;
            timeout = 30;
            return true;
        }

        else if (down)
        {
            pos = (down & debug_code[pos]) ? pos+1 : 0;
            timeout = 30;
        }
    }
    return false;
}

void Sleep(u64 nanoseconds)
{
    svcSleepThread(nanoseconds);
}

s64 Seconds(float amount)
{
    return (s64)(amount*1000000000);
}

void load_player_invitems(Save *saveFile , int selectedplayer)
{
    for (int num = 0; num < 16; num++)
    {
		Item item = saveFile->players[selectedplayer].Pockets[num];

        item.Name = GetItemName(&item);
        item.Icon = GetItemIcon(&item);
    }
}

float getCenterText_X(Text* msg, float StartX, float WidthX)
{
    float CharWidth = msg->GetWidth();
    float CenterX = StartX+(WidthX - CharWidth) /2;
    return CenterX;
}

float getCenterText_Y(Text* msg, float StartY, float HeightY)
{
    float CharHeight = msg->GetHeight();
    float CenterY = StartY+(HeightY-CharHeight) /2;
    return CenterY;
}

void draw_centered_text(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* text)
{
    Text* Msg = new Text(Color(color), text, scaleX, scaleY);
    float XCoord = getCenterText_X(Msg, StartX, WidthX);
    float YCoord = getCenterText_Y(Msg, StartY, HeightY);
    Msg->Draw(XCoord, YCoord, true);
}

void draw_centered_textf(float StartX, float WidthX, float StartY, float HeightY,
                        float scaleX, float scaleY, u32 color, const char* string, ...)
{
    char       buffer[1024];
    va_list    args;
    va_start(args, string);
    vsnprintf(buffer, sizeof(buffer), string, args);
    draw_centered_text(StartX, WidthX, StartY, HeightY, scaleX, scaleY, color, buffer);
    va_end(args);
}

void updatePointerPosition(void)
{
    hidScanInput();

    if (hidKeysHeld() & KEY_A)
    {
        g_cursorpos.A_held = true;
        return;
    }

    else
        g_cursorpos.A_held = false;
    //CPad
    if (hidKeysHeld() & KEY_CPAD)
    {
        if (hidKeysHeld() & KEY_CPAD_RIGHT)
            g_cursorpos.x += 3;
        
        if (hidKeysHeld() & KEY_CPAD_LEFT)
            g_cursorpos.x -= 3;
        
        if (hidKeysHeld() & KEY_CPAD_UP)
            g_cursorpos.y -= 3;
        
        if (hidKeysHeld() & KEY_CPAD_DOWN)
            g_cursorpos.y += 3;

        g_cursorpos.show = true;
    }

    //C-Stick
    if (hidKeysHeld() & KEY_CSTICK)
    {
        if (hidKeysHeld() & KEY_CSTICK_RIGHT)
            g_cursorpos.x += 4;
        
        if (hidKeysHeld() & KEY_CSTICK_LEFT)
            g_cursorpos.x -= 4;
        
        if (hidKeysHeld() & KEY_CSTICK_UP)
            g_cursorpos.y -= 4;
        
        if (hidKeysHeld() & KEY_CSTICK_DOWN)
            g_cursorpos.y += 4;

        g_cursorpos.show = true;
    }

    if (hidKeysHeld() & KEY_TOUCH)
        g_cursorpos.show = false;

    if (g_cursorpos.y < -5)
        g_cursorpos.y = 235;

    if (g_cursorpos.y > 235)
        g_cursorpos.y = -5;

    if (g_cursorpos.x < -5)
        g_cursorpos.x = 315;

    if (g_cursorpos.x > 315)
        g_cursorpos.x = -5;
}

void updateCursorInfo(void)
{
    touchPosition touch;
    hidTouchRead(&touch);
    
    g_CheckX[0] = touch.px;
    g_CheckY[0] = touch.py;
    g_CheckX[1] = g_cursorpos.x;
    g_CheckY[1] = g_cursorpos.y;
    g_disabled[0] = !g_cursorpos.show;
    g_disabled[1] = g_cursorpos.show;
}

bool CheckUnreadMail(int selectedplayer)
{
    /* STUB until player struct finished*/
	if (selectedplayer == 0) { // TODO: REMOVE THIS (It's here to stop compiler warnings)
		return true;
	}
    return true;
}

Item GetItemFromSave(Save *saveFile, u32 offset)
{
    Item item;
    item.Id = saveFile->ReadU16(offset);
    item.Flag1 = saveFile->ReadU16(offset + 2);
	item.Flag2 = saveFile->ReadU16(offset + 3);
    return item;
}

int CheckItemValidity(Item *item)
{
    //Real Max ID is 0x372B (exclude wrap paper) but for checks game -= 0x2000 from orig item ID
    static const s16 maxID = 0x172B;
    int chk;

    u16 ID = item->Id & ~0x8000;
    chk = ID - 0x2000; //To cover items lower than 0x2000 (Enviroment Items)

    if (chk < maxID)
    return chk;

    else return -1;
}

u32 CheckIfValidItem(Item *item)
{
    static const u16 maxID = 0x172B;
    int chk = CheckItemValidity(item);

    if (chk < 0 || chk > maxID)
        return 0;

    else /* chk <= maxID */
    {
        chk = ((chk << 4) - chk) << 1;
        return chk;
    }
}

u16 GetAxeDamageValue(Item *item)
{
    /* Used for Normal Axe and Silver Axe */
    if (item->Id != 0x334D && item->Id != 0x334E)
        return 0;

    return (item->Flag2 & 0xF); // TODO: Confirm Flag2 is correct
}

u16 GetAxeDamageIcon(Item *item, u16 ItemIcon)
{
    static u8 axedamagetable[] = {0, 0, 0, 1, 1, 1, 2, 2};
    u16 DamageValue = GetAxeDamageValue(item);
    if (DamageValue >= 8)
        return ItemIcon;

    return ItemIcon + axedamagetable[DamageValue];
}

u8 GetItemCategory(Item *item)
{
    if (g_ItemBin == NULL)
        return 0;

    u32 offset = CheckIfValidItem(item);
    if (offset == 0)
        return 0x9B;

    u8 category = g_ItemBin[offset+0xA];
    if (category >= 0x9B)
        return 0;

    return category;
}

u16 GetIconID(Item *item)
{

    if (g_ItemBin == NULL)
        return 0;

    u32 offset = CheckIfValidItem(item);
    if (offset == 0)
        return 0;

    u16 iconID = (g_ItemBin[offset + 1] << 8) + g_ItemBin[offset]; //Reading from BE file; 2nd u8 comes first to convert to LE
	if (iconID >= 0x1FB) {
		return 0;
	}

    u8 category = GetItemCategory(item);
	if (category == 0xE && (item->Id == 0x334D || item->Id == 0x334E)) {
		iconID = GetAxeDamageIcon(item, iconID);
	}

    return iconID;
}

std::string GetItemIcon(Item *item)
{
    u16 iconID = 0;
    if (g_ItemBin != NULL)
        iconID = GetIconID(item);

    return iconfilenametable[iconID];

}

std::string GetItemName(Item *item)
{
    char itemid[10];
    char* line = new char[100];
    snprintf(itemid, 8, "0x%04X", item->Id);

    rewind(g_Items_en);
    while (fgets(line, 100, g_Items_en) != NULL) /* read each line */
    {
        if (strncmp(itemid, line, 6) == 0) /* If itemID in txt is same as our ID */
        {
            int len = strlen(line + 8) - 1;
            if ((len > 0) && (line[len] == '\n'))
            line[len] = '\0';
			auto str = std::string(line + 8); // +8 so the itemID and spaces before actual name aren't included
			delete[] line;
            return str;
        }
    }

	delete[] line;
	return std::string("???");
}

std::string u16tou8(std::u16string src)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	std::string dst = convert.to_bytes(src);
	return dst;
}

std::u16string u8tou16(const char* src)
{
	char16_t tmp[256] = { 0 };
	utf8_to_utf16((uint16_t *)tmp, (uint8_t *)src, 256);
	return std::u16string(tmp);
}

/*
	bool checkGameCartIsACNL
		=> Returns whether or not the game cart is an ACNL game cart.
*/
bool checkGameCartIsACNL() {
	if (currentTitleId == 0) {
		return getInstalledTitles(MEDIATYPE_GAME_CARD).HasACNLData;
	}
	else {
		return checkGameCartTitleSame(currentTitleId);
	}
}

std::string Format(const char* fmt, ...)
{
    char        buffer[0x100] = { 0 };
    va_list     argList;

    va_start(argList, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argList);
    va_end(argList);

    return (std::string(buffer));
}

std::vector<u32> findPlayerReferences(Save *saveFile, Player *player) {
	// Read u16 ID + Name
	u16 *dataArray = new u16[11];
	for (u32 i = 0; i < 11; i++) {
		dataArray[i] = saveFile->ReadU16(player->m_offset + 0x55A6 + i * 2);
	}

	std::vector<u32> references = std::vector<u32>();
	u32 Size = saveFile->GetSaveSize() - 22;
	for (u32 i = 0; i < Size; i += 2) { // subtract 22 so we don't read past the end of the file
		bool foundMatch = true;
		for (int x = 0; x < 11; x++) {
			if (saveFile->ReadU16(i + x * 2) != dataArray[x]) {
				foundMatch = false;
				break;
			}
		}
		// add the offset to the vector
		if (foundMatch) {
			references.push_back(i);
			i += 20; // skip the rest of this data
		}
	}

	delete[] dataArray;
	return references;
}