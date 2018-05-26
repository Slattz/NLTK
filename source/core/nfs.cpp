#include "nfs.h"
#include <unistd.h>
#include <time.h>

NLTK_Titles_Info MediaInfo;
bool loadedFromCart = true;
AM_TitleEntry* titleInfo;

/*
	bool openSaveArchive(FS_Archive *out, u64 id, FS_MediaType mediaType)
		=> Attempts to open the save data of the specified id from mediaType.
			=> returns bool successful
*/
bool openSaveArchive(FS_Archive *out, u64 id, FS_MediaType mediaType) {
	u32 pathInfo[3] = { mediaType, static_cast<u32>(id), static_cast<u32>(id >> 32) };
	return R_SUCCEEDED(FSUSER_OpenArchive(out, ARCHIVE_USER_SAVEDATA, { PATH_BINARY, 0xC, pathInfo }));
}

/*
	bool tryOpenSaveArchive(FS_Archive *out, u64 id, FS_MediaType *mediaTypeOut)
		=> Attempts to open a save file of the specified id from any media.
			=> returns bool successful
*/
bool tryOpenSaveArchive(FS_Archive *out, u64 id, FS_MediaType *mediaTypeOut) {
	if (IsGameCartInserted() && openSaveArchive(out, id, MEDIATYPE_GAME_CARD)) {
		*mediaTypeOut = MEDIATYPE_GAME_CARD;
		return true;
	}
	else if (IsSDCardInserted() && openSaveArchive(out, id, MEDIATYPE_SD)) {
		*mediaTypeOut = MEDIATYPE_SD;
		return true;
	}

	return false;
}

/*
	NLTK_Media_Installed getInstalledTitles(FS_MediaType mediaType)
		=> returns the installed media info for a specific media type.
*/
NLTK_Media_Installed getInstalledTitles(FS_MediaType mediaType) {
	NLTK_Media_Installed installInfo;

	if ((mediaType == MEDIATYPE_SD && IsSDCardInserted()) || (mediaType == MEDIATYPE_GAME_CARD && IsGameCartInserted())) {
		u32  num_titles;
		u64* titles;

		AM_GetTitleCount(mediaType, &num_titles); //Get how many titles are installed
		titles = new u64[num_titles]; //Allocate memory based on amount of titles
		AM_GetTitleList(NULL, mediaType, num_titles, titles); //Get all installed title ids

		if (num_titles > 0)
		{
			for (u32 i = 0; i < num_titles; i++) //Go through each tid and see if it matches an ACNL tid
			{

				if (titles[i] == JPN_TID)
					installInfo.InstalledTitles.ORIG_JPN_installed = true;
				else if (titles[i] == USA_TID)
					installInfo.InstalledTitles.ORIG_USA_installed = true;
				else if (titles[i] == EUR_TID)
					installInfo.InstalledTitles.ORIG_EUR_installed = true;
				else if (titles[i] == KOR_TID)
					installInfo.InstalledTitles.ORIG_KOR_installed = true;

				else if (titles[i] == JPN_WA_TID)
					installInfo.InstalledTitles.WA_JPN_installed = true;
				else if (titles[i] == USA_WA_TID)
					installInfo.InstalledTitles.WA_USA_installed = true;
				else if (titles[i] == EUR_WA_TID)
					installInfo.InstalledTitles.WA_EUR_installed = true;
				else if (titles[i] == KOR_WA_TID)
					installInfo.InstalledTitles.WA_KOR_installed = true;
			}
		}

		if (titles) {
			delete[] titles;
		}

		//Organise what's installed and what isn't
		if (installInfo.InstalledTitles.ORIG_JPN_installed || installInfo.InstalledTitles.ORIG_USA_installed ||
			installInfo.InstalledTitles.ORIG_EUR_installed || installInfo.InstalledTitles.ORIG_KOR_installed)
		{
			installInfo.InstalledTitles.ORIG_installed = true;
			installInfo.HasACNLData = true;
		}

		if (installInfo.InstalledTitles.WA_JPN_installed || installInfo.InstalledTitles.WA_USA_installed ||
			installInfo.InstalledTitles.WA_EUR_installed || installInfo.InstalledTitles.WA_KOR_installed)
		{
			installInfo.InstalledTitles.WA_installed = true;
			installInfo.HasACNLData = true;
		}
	}

	return installInfo;
}

/*
	void checkInstalledTitles(void)
		=> Sets global MediaInfo's SD Card & Game Card infos.
*/
void checkInstalledTitles(void)
{
	MediaInfo.SDCardInfo = getInstalledTitles(FS_MediaType::MEDIATYPE_SD);
	MediaInfo.GameCartInfo = getInstalledTitles(FS_MediaType::MEDIATYPE_GAME_CARD);
}

/*
	bool checkGameCartTitleSame(void)
		=> Checks if the currently inserted game cart is the same type as the one being edited.
			=> Returns bool: IsCardSame
*/
bool checkGameCartTitleSame(u64 titleId) {
	if (currentMediaType != MEDIATYPE_GAME_CARD)
		return true; // We don't need to check if we aren't editing the game cart save file

	bool cardInserted = false;
	FSUSER_CardSlotIsInserted(&cardInserted);

	if (!cardInserted)
		return false;

	u32 num_gameCartTitles = 0;

	AM_GetTitleCount(MEDIATYPE_GAME_CARD, &num_gameCartTitles);
	u64 *gameCartTitles = new u64[num_gameCartTitles];
	AM_GetTitleList(NULL, MEDIATYPE_GAME_CARD, num_gameCartTitles, gameCartTitles);

	for (u32 i = 0; i < num_gameCartTitles; i++) {
		if (gameCartTitles[i] == titleId) {
			delete[] gameCartTitles;
			return true;
		}
	}

	delete[] gameCartTitles;
	return false;
}

int get_file_size(FILE *file)
{
    int size = 0;
    if (fseek(file, 0, SEEK_END) != 0)
    {
        infoDisp(top, "Error in get_file_size: fseek");
        return -1;
    }
    size = ftell(file);
    rewind(file);

    return size;
}

bool fileExists(const char *path)
{
    if (!path) return false;
    if (access(path, F_OK) == 0) //-1: doesn't exist, 0: exists
        return true; 
    else
        return false;
}

bool file_write(void* sourcebuf, const char* path, int size)
{
    FILE *file = fopen(path, "wb");
    if (!file) return false;
    fwrite(sourcebuf, 1, size, file);
    fclose(file);
    return true;
}

bool file_read(void* destbuf, const char *path, int size)
{
    if (fileExists(path))
    {
        FILE *file = fopen(path, "rb");
        if (!file) return false;
        if (get_file_size(file) == size)
        {
            fread(destbuf, size, 1, file);
            fclose(file);
            return true;
        }
    }

    return false;
}

void saveBackup(Save *saveFile, u64 tid)
{
    char       path[250] = {0};
    char       timestamp[80] = {0};
    time_t     rawtime;
    struct tm *timeinfo;

    snprintf(path, 160, WORKDIR "/saves/%016llX/", tid);
    mkdir(WORKDIR "/saves", 777);
    mkdir(path, 777);

    // Get timestamp for filename
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    /* String Format: Date (YYYY-MM-DD), Hour (12h format), minute, am/pm */
    strftime(timestamp, 80, "[%F_%I-%M%p]", timeinfo);

    strncat(path, "garden_plus", 12);
    strncat(path, timestamp, 80);
    strncat(path, ".dat", 5);
    if (!fileExists(path))
    {
        if (!file_write(saveFile->GetRawSaveData() , path, SIZE_SAVE))
            infoDisp(top, "Error:\nCouldn't write save backup!");
    }
    else
        infoDisp(top, "Error:\nSave backup somehow already exists!");
}