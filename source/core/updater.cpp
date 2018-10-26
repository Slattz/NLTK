#include <3ds.h>
#include <string>
#include "json.hpp"
#include "httpc.h"
#include "gfx.h"
#include "nfs.h"
#include "config.hpp"
#include "utils.h"
#include "common.h"
#include "updater.hpp"

extern Config *config;

static char *newVerString = nullptr;
static char *newChangelog = nullptr;
static char *urlDownload = nullptr;
static char *Filename = nullptr;
static char UserAgentStr[50] = TITLE "/" VERSION;
HTTPC httpc;

bool CheckVersion(const char *releaseName)
{
    int major = 0;
    int minor = 0;
    int revision = 0;
    int beta_ver = 0;
    char *next = NULL;

    if (releaseName && *releaseName == 'v')
        releaseName++;

    major = strtol(releaseName, &next, 10); //Get major version
    if (config->IsDebug) MsgDisp(top, Format("Major: %d, Result: %d", major, major >= MAJOR_VERSION));

    if (next && *next == '.') //If minor version
        next++;
    else
        return major > MAJOR_VERSION;

    minor = strtol(next, &next, 10); //Get minor version
    if (config->IsDebug) MsgDisp(top, Format("Minor: %d, Result %d", minor, minor >= MINOR_VERSION));

    if (next && *next == '.') //If revision version
        next++;

    else if (next && *next == 'B') //If there's no revision ver but it's beta
    {
        next++;
        beta_ver = strtol(next, NULL, 10); //Get beta version
        if (config->IsDebug) MsgDisp(top, Format("Beta 1: %d, Result %d", beta_ver, beta_ver >= BETA_VERSION));

        return major >= MAJOR_VERSION && minor >= MINOR_VERSION && beta_ver > BETA_VERSION;
    }

    else //If there's no revision ver & beta ver
        return major >= MAJOR_VERSION && minor > MINOR_VERSION;

    revision = strtol(next, &next, 10); //Get revision version
    if (config->IsDebug) MsgDisp(top, Format("Revision: %d, Result %d", revision, revision >= REV_VERSION));

    if (next && *next == 'B') //If there's a beta ver after revision ver
        next++;

    else
        return major >= MAJOR_VERSION && minor >= MINOR_VERSION && revision > REV_VERSION;

    beta_ver = strtol(next, NULL, 10); //Get beta version
    if (config->IsDebug)
        MsgDisp(top, Format("Beta 2: %d, Result %d", beta_ver, beta_ver >= BETA_VERSION));
    return major >= MAJOR_VERSION && minor >= MINOR_VERSION && revision >= REV_VERSION && beta_ver > BETA_VERSION;
}

bool HTTPDownload(const char *src, u8 **output, u32 *outSize)
{
    Result res = 0;
    httpcContext context;
    u32 responseCode = 0;
    u32 size=0;
    u8 *buffer;
    char url[1024] = {0};
    bool resolved = false;
    bool ret = false;

    strncpy(url, src, 1023);
    while(R_SUCCEEDED(res) && !resolved)
    {
        if (R_SUCCEEDED(res = httpc.OpenContext(&context, HTTPC_METHOD_GET, url, 1)))
        {
            httpc.SetSSLOpt(&context, SSLCOPT_DisableVerify);
            httpc.AddRequestHeaderField(&context, (char*)"User-Agent", UserAgentStr);
            httpc.AddRequestHeaderField(&context, (char*)"Connection", (char*)"Keep-Alive");
            httpc.BeginRequest(&context);
            res = httpc.GetResponseStatusCode(&context, &responseCode);

            if (responseCode >= 301 && responseCode <= 303)
            {
                if (config->IsDebug)
                    MsgDisp(top, "redirecting URL");
                memset(url, '\0', strlen(url));
                if (R_SUCCEEDED(res = httpc.GetResponseHeader(&context, (char*)"Location", url, 1024)))
                    httpc.CloseContext(&context);
            }

            else
            {
                resolved = true;

                if (responseCode != 200)
                {
                    if (config->IsDebug)
                        MsgDisp(top, Format("URL returned status: %ld", responseCode));
                    httpc.CloseContext(&context);
                    return false;       
                }
            }
        }

        else MsgDisp(top, "Could not open HTTP context!");
    }

    if (R_SUCCEEDED(res = httpc.GetDownloadSizeState(&context, NULL, &size)))
    {
        buffer = (u8*)calloc(1, size);
        if (buffer != NULL)
        {
            if (R_SUCCEEDED(res = httpc.ReceiveData(&context, (u8*)buffer, size)))
            {
                *output = buffer;
                *outSize = size;
            }
            ret = true;
        }

        else
            ret = false;
    }

    else
        ret = false;

    return ret;
}

bool CheckForUpdate(void)
{
    Json    *json;
    char    *jsonstring = NULL;
    u32     size = 0;
    bool    ret = false;

    if (HTTPDownload("https://api.github.com/repos/Slattz/NLTK/releases/latest", (u8 **)&jsonstring, &size))
    {
        if (envIsHomebrew()) strcpy(Filename, TITLE ".3dsx");
        else strcpy(Filename, TITLE ".cia");

        json = new Json(jsonstring, size);

        /* Assume the top-level element is an object */
        if (!json_is_object(json->m_json))
        {
            if (config->IsDebug) MsgDisp(top, "Json object expected");
            return 0;
        }

        if (json->GetValue("tag_name", newVerString, 40) <= -1) {
            if (config->IsDebug) MsgDisp(top, "No tag_name found");
            return 0;
        }

        if (!CheckVersion(newVerString))
            return 0; //if no update

        json_t *arr = json_object_get(json->m_json, "assets");
        if (arr == NULL) {
            if (config->IsDebug) MsgDisp(top, "No 'assets' array found");
            return 0;
        }

        for (size_t i = 0; i < json_array_size(arr); i++) //Go through the objects in the array
        {
            ret = false;
            int res;
            char name[50] = {0};
            Json *object = new Json(json_array_get(arr, i));

            res = object->GetValue("name", name, 49); //Used to check if correct file based on if hb user or not
            if (res <= -1 && strncmp(Filename, name, 49) != 0)
                continue;

            res = object->GetValue("browser_download_url", urlDownload, 1000);
            
            if (res <= -1) {
                if (config->IsDebug) MsgDisp(top, "No 'browser_download_url' string found");
                return 0;
            }

            ret = true;
            break;
        }

        if (json->GetValue("body", newChangelog, 2000) <= -1) {
            strcpy(newChangelog, " "); //if body didn't exist, just copy a blank space.
        }
    }
    return ret;
}

bool InstallUpdate(void)
{
    u32 downloadsize = 0;
    u8* downloadbuf;
    u32 bytesWritten;
    Handle ciaHandle;
    Result res;

    if (HTTPDownload(urlDownload, &downloadbuf, &downloadsize))
    {
        if (config->IsDebug)
            MsgDisp(top, "Update Downloaded!");

        if (envIsHomebrew())
        {
            if (file_write(downloadbuf, "sdmc:/3ds/NLTK/NLTK.3dsx", downloadsize))
            {
                MsgDisp(top, "The update has been installed!\nNow exiting the app");
                return true;
            }

            else
            {
                MsgDisp(top, "Failed to write 3dsx!");
                return false;
            }
        }

        else //cia
        {
            if(R_SUCCEEDED(res = AM_StartCiaInstall(MEDIATYPE_SD, &ciaHandle)))
            {
                FSFILE_Write(ciaHandle, &bytesWritten, 0, downloadbuf, downloadsize, 0);
                if (R_SUCCEEDED(res = AM_FinishCiaInstall(ciaHandle)))
                {
                    MsgDisp(top, "The update has been installed!\nNow exiting the app");
                    return true;
                }

                else
                {
                    AM_CancelCIAInstall(ciaHandle);
                    MsgDisp(top, Format("Update install failed (FinishCiaInstall): %d", res));
                    return false;
                }
            }

            else
            {
                MsgDisp(top, Format("Update install failed (StartCiaInstall): %d", res));
                return false;
            }
        }

        delete[] downloadbuf;
    }
    return false;
}

bool Updater::Launch(void)
{
    bool ret = false;
    urlDownload = new char[1024];
    newChangelog = new char[2048];
    newVerString = new char[50];
    Filename = new char[50];

    static const char* ProceedUpdate = "Would you like to proceed\nwith installing the update?";

    if(CheckForUpdate())
    {

        if (config->IsDebug)
            MsgDisp(top, "Check Update: True!");

        MsgDisp(top, Format("New Version: %s\n Changelog:\n%s", newVerString, newChangelog));

        if (MsgDisp(top, ProceedUpdate, MsgTypeConfirm))
            ret = InstallUpdate();        

    }

    else
    {
        ret = false;
        if (config->IsDebug) MsgDisp(top, "Check Update: False!");
    }

    delete[] urlDownload;
    delete[] newChangelog;
    delete[] newVerString;
    delete[] Filename;

    return ret;
}