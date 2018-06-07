#include "updater.h"
#include "utils.h"
#include "jsmn.h"
#include "httpc.h"

static char     newVerString[50];
static char     newChangelog[2048];
static char     urlDownload[1024];
static int      urlDownloadSize;
static char     UserAgentStr[50] = TITLE "/" VERSION;
HTTPC httpc;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

void str_replace(char *target, const char *needle, const char *replacement)
{
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}

bool    CheckVersion(const char *releaseName)
{
    int     major = 0;
    int     minor = 0;
    int     revision = 0;
    int     beta_ver = 0;
    char    *next = NULL;
    
    if (releaseName && *releaseName == 'v')
        releaseName++;

    major = strtol(releaseName, &next, 10); //Get major version
    if (config.isdebug)
        MsgDisp(top, Format("Major: %d, Result: %d", major, major >= MAJOR_VERSION));

    if (next && *next == '.') //If minor version
        next++;
    else
        return major > MAJOR_VERSION;

    minor = strtol(next, &next, 10); //Get minor version
    if (config.isdebug)
        MsgDisp(top, Format("Minor: %d, Result %d", minor, minor >= MINOR_VERSION));
    if (next && *next == '.') //If revision version
        next++;

    else if (next && *next == 'B') //If there's no revision ver but it's beta
    {
        next++;
        beta_ver = strtol(next, NULL, 10); //Get beta version
        if (config.isdebug)
            MsgDisp(top, Format("Beta 1: %d, Result %d", beta_ver, beta_ver >= BETA_VERSION));
        return major >= MAJOR_VERSION && minor >= MINOR_VERSION && beta_ver > BETA_VERSION;
    }
            
    else //If there's no revision ver & beta ver
        return major >= MAJOR_VERSION && minor >= MINOR_VERSION;

    revision = strtol(next,  &next, 10); //Get revision version
    if (config.isdebug)
        MsgDisp(top, Format("Revision: %d, Result %d", revision, revision >= REV_VERSION));
    if (next && *next == 'B') //If there's a beta ver after revision ver
        next++;

    else
        return major >= MAJOR_VERSION && minor >= MINOR_VERSION && revision > REV_VERSION;
    
    beta_ver = strtol(next, NULL, 10); //Get beta version
    if (config.isdebug)
        MsgDisp(top, Format("Beta 2: %d, Result %d", beta_ver, beta_ver >= BETA_VERSION));
    return major >= MAJOR_VERSION && minor >= MINOR_VERSION 
            && revision >= REV_VERSION && beta_ver > BETA_VERSION;
}

bool http_download(const char *src, u8 **output, u32 *outSize)
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
                if (config.isdebug)
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
                    if (config.isdebug)
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

bool checkUpdate(void)
{
    char            *json = NULL;
    u32             size = 0;
    Result          res;
    bool            ret = false;
    jsmn_parser     jParser;
    jsmntok_t       tokens[500];
    jsmntok_t       obj_key;
    jsmntok_t       obj_val;
    jsmntok_t       assetName;
    jsmntok_t       assetUrl;
    bool            updateavailable = false;
    int             value_length;
    int             i; //Loop
    int             j; //Loop 2
    int             tok = 0;
    int             tokoffset = 0;
    int             objnum = 0;
    int             objoffset = 0;
    int             assetNameLoopVal = -1;
    int             assetUrlLoopVal = -2;//Start value different to assetNameLoopVal

    if (http_download("https://api.github.com/repos/Slattz/NLTK/releases/latest", (u8 **)&json, &size))
    {
        jsmn_init(&jParser);
        res = jsmn_parse(&jParser, json, size, tokens, sizeof(tokens)/sizeof(tokens[0]));

        /* Check if the parse was a success */
        if (res < 0) 
        {
                if (config.isdebug)
                    MsgDisp(top, Format("Failed to parse JSON: %ld", res));
            return 0;
        }

        /* Assume the top-level element is an object */
        if (res < 1 || tokens[0].type != JSMN_OBJECT)
        {
            if (config.isdebug)
                MsgDisp(top, "Object expected");
            return 0;
        }

        /* Loop over all keys of the root object */
        for (i = 1; i < res; i++)
        {
            jsmntok_t json_key = tokens[i];
            jsmntok_t json_value = tokens[i+1];
            value_length = json_value.end - json_value.start;

            if (jsoneq(json, &json_key, "tag_name") == 0 && json_value.type == JSMN_STRING)
            {
                Sstrncpy(newVerString, json + json_value.start, value_length);
                if (CheckVersion(newVerString)) {
                    updateavailable = true;
                }

                else return 0; //if no update
            }

            else if (jsoneq(json, &json_key, "assets") == 0 && updateavailable && json_value.type == JSMN_ARRAY)
            {
                u32 tempsize = json_value.size; //store size of assets so it can be added to i later
                for (j = 0; j < json_value.size; j++) //Loop for amount of assets found
                {
                    /*
                    i = position in orig loop
                    2 = to bring to start of first object of array
                    j = which array
                    tokoffset = calculated to get to start of next array
                    */
                    tok = i+2+j+tokoffset;
                    jsmntok_t val = tokens[tok]; //Use tok to determine the token in the json
                    if (val.type == JSMN_OBJECT) //If an object is found
                    {
                        /* Increment tokoffset by how big the obj is * 2 to include keys & values */
                        tokoffset += val.size*2;
                        for (int k = 1; k < (val.size*2)+1; k++) //start at 1 to skip original object
                        {
                            objnum = tok+k+objoffset;
                            obj_key = tokens[objnum]; //object keys
                            obj_val = tokens[objnum+1]; //object values

                            /* skip uploader object */
                            if (jsoneq(json, &obj_key, "uploader") == 0 && obj_val.type == JSMN_OBJECT)
                            {
                                objoffset += 1+(obj_val.size*2); //+1 to get to proper end of object
                                tokoffset += (obj_val.size*2);
                            }

                            if (jsoneq(json, &obj_key, "name") == 0 && obj_val.type == JSMN_STRING)
                            {
                                assetName = obj_val;
                                assetNameLoopVal = j; //Ensure name is from same asset as url
                            }

                            else if (jsoneq(json, &obj_key, "browser_download_url") == 0 && obj_val.type == JSMN_STRING)
                            {
                                assetUrl = obj_val;
                                assetUrlLoopVal = j; //Ensure url is from same asset as name
                            }

                            if (envIsHomebrew())
                            {
                                if (jsoneq(json, &assetName, TITLE ".3dsx") == 0
                                    && assetUrl.type == JSMN_STRING
                                    && assetNameLoopVal == assetUrlLoopVal)
                                {
                                    urlDownloadSize = assetUrl.end - assetUrl.start;
                                    Sstrncpy(urlDownload, json + assetUrl.start, urlDownloadSize);
                                    ret = true;
                                }
                            }

                            else
                            {
                                if (jsoneq(json, &assetName, TITLE ".cia") == 0
                                    && assetUrl.type == JSMN_STRING
                                    && assetNameLoopVal == assetUrlLoopVal)
                                {
                                    urlDownloadSize = assetUrl.end - assetUrl.start;
                                    Sstrncpy(urlDownload, json + assetUrl.start, urlDownloadSize);
                                    ret = true;
                                }
                            }
                        }
                        objoffset = 0; //Reset objoffset for the next object
                    }
                }
                i += tempsize; //Add size of assets array to loop to skip it
            }

            else if (jsoneq(json, &json_key, "body") == 0 && updateavailable && json_value.type == JSMN_STRING)
            {
                Sstrncpy(newChangelog, json + json_value.start, value_length);
                str_replace(newChangelog, "\\r", "");
                str_replace(newChangelog, "\\n", "\n");
            }
        }
    }
    return ret;
}

bool installUpdate(void)
{
    u32 downloadsize = 0;
    u8* downloadbuf;
    u32 bytesWritten;
    Handle ciaHandle;
    Result res;

    if (http_download(urlDownload, &downloadbuf, &downloadsize))
    {
        if (config.isdebug)
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

bool launchUpdater(void)
{
    bool ret = false;
    static const char* ProceedUpdate = "Would you like to proceed\nwith installing the update?";

    if(checkUpdate())
    {

        if (config.isdebug)
            MsgDisp(top, "Check Update: True!");

        MsgDisp(top, Format("New Version: %s\n Changelog:\n%s", newVerString, newChangelog));

        if (MsgDisp(top, ProceedUpdate, MsgTypeConfirm))
            ret = installUpdate();        

        return ret;
    }

    else
    {
        if (config.isdebug)
            MsgDisp(top, "Check Update: False!");

        return false;
    }
}