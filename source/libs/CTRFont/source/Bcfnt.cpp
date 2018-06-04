#include <3ds.h>
#include <citro3d.h>

struct C2D_Glyph
{
    u32     lineNo{0};
    C3D_Tex *sheet{nullptr};
    float   xPos{0.f};
    float   width{0.f};
    struct
    {
        float left, top, right, bottom;
    } texcoord{0.f, 0.f, 0.f, 0.f};
};

#include "FontInternal.hpp"

using namespace CTRFontImpl;

static inline void  Relocate(void *addr, const u32 base)
{
    *(u32 *)addr += base;
}

static void     RelocateCMAP(CMAP_s *node, const u32 base)
{
    if (node == NULL || node->next == NULL)
        return;

    Relocate(&node->next, base);
    RelocateCMAP(node->next, base);
}

static void     RelocateCWDH(CWDH_s *node, const u32 base)
{
    if (node == NULL || node->next == NULL)
        return;

    Relocate(&node->next, base);
    RelocateCWDH(node->next, base);
}

int     Bcfnt::CreateGlyphsheet(void)
{
    if (glyphSheets != nullptr)
        return -2;

    int         i;
    TGLP_s  *   glyphInfo = GetGlyphInfo();

    glyphSheets = new C3D_Tex[glyphInfo->nSheets];

    if (glyphSheets == NULL)
        svcBreak(USERBREAK_PANIC);

    for (i = 0; i < glyphInfo->nSheets; ++i)
    {
        C3D_Tex *tex = &glyphSheets[i];

        tex->data = GetGlyphSheetTex(i);
        tex->fmt = (GPU_TEXCOLOR)glyphInfo->sheetFmt;
        tex->size = glyphInfo->sheetSize;
        tex->width = glyphInfo->sheetWidth;
        tex->height = glyphInfo->sheetHeight;
        tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
            | GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);
        tex->border = 0xFFFFFFFF;
        tex->lodParam = 0;
    }

    return 0;
}

int     Bcfnt::LoadFromFile(FILE *file)
{
    u32     base;
    void    *data;
    FINF_s  *finf;
    Result  res = 0;
    CFNT_s  header;

    if (file == NULL)
        return MAKERESULT(RL_USAGE, RS_NOTFOUND, RM_APPLICATION, RD_NOT_FOUND);

    if (fread((void *)&header, 1, sizeof(CFNT_s), file) != sizeof(CFNT_s))
    {
        res = MAKERESULT(RL_USAGE, RS_WRONGARG, RM_APPLICATION, RD_INVALID_SIZE);
        goto exit;
    }

    if (header.signature != 0x554E4643 && header.signature != 0x544E4643) /// CFNU && CFNT
    {
        res = MAKERESULT(RL_USAGE, RS_WRONGARG, RM_APPLICATION, RD_INVALID_SIZE);
        goto exit;
    }

    data = linearAlloc(header.fileSize);
    if (data == NULL)
    {
        res = MAKERESULT(RL_USAGE, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY);
        goto exit;
    }

    fseek(file, 0, SEEK_SET);
    if (fread(data, 1, header.fileSize, file) != header.fileSize)
    {
        linearFree(data);
        res = MAKERESULT(RL_USAGE, RS_WRONGARG, RM_APPLICATION, RD_INVALID_SIZE);
        goto exit;
    }

    cfnt = (CFNT_s *)data;

    finf = &cfnt->finf;
    base = (u32)cfnt;
    Relocate(&finf->tglp, base);
    Relocate(&finf->cwdh, base);
    Relocate(&finf->cmap, base);
    Relocate(&finf->tglp->sheetData, base);
    RelocateCMAP(finf->cmap, base);
    RelocateCWDH(finf->cwdh, base);

    res = CreateGlyphsheet();

    charPerSheet = finf->tglp->nRows * finf->tglp->nLines;

    cfnt->signature = 0x554E4643; ///< CFNU

exit:
    fclose(file);
    return res;
}

int     Bcfnt::GlyphIndexFromCodePoint(u32 codePoint) const
{
    int ret = cfnt->finf.alterCharIndex;

    if (codePoint < 0x10000)
    {
        CMAP_s* cmap;

        for (cmap = cfnt->finf.cmap; cmap; cmap = cmap->next)
        {
            if (codePoint < cmap->codeBegin || codePoint > cmap->codeEnd)
                continue;

            if (cmap->mappingMethod == CMAP_TYPE_DIRECT)
            {
                ret = cmap->indexOffset + (codePoint - cmap->codeBegin);
                break;
            }

            if (cmap->mappingMethod == CMAP_TYPE_TABLE)
            {
                ret = cmap->indexTable[codePoint - cmap->codeBegin];
                break;
            }

            int j;
            for (j = 0; j < cmap->nScanEntries; j ++)
                if (cmap->scanEntries[j].code == codePoint)
                    break;
            if (j < cmap->nScanEntries)
            {
                ret = cmap->scanEntries[j].glyphIndex;
                break;
            }
        }
    }

    return ret;
}

charWidthInfo_s & Bcfnt::GetCharWidthInfo(int glyphIndex) const
{
    charWidthInfo_s *info = NULL;
    CWDH_s          *cwdh;

    for (cwdh = cfnt->finf.cwdh; cwdh && !info; cwdh = cwdh->next)
    {
        if (glyphIndex < cwdh->startIndex || glyphIndex > cwdh->endIndex)
            continue;

        info = &cwdh->widths[glyphIndex - cwdh->startIndex];
    }

    if (!info)
        info = &cfnt->finf.defaultWidth;

    return *info;
}

float   Bcfnt::CalcGlyphPos(C2D_Glyph& out, int glyphIndex, C3D_Tex *glyphSheets) const
{
    TGLP_s          * tglp = GetGlyphInfo();
    charWidthInfo_s & cwi = GetCharWidthInfo(glyphIndex);

    int     sheetId = glyphIndex / charPerSheet;
    int     glInSheet = glyphIndex % charPerSheet;

    out.sheet = &glyphSheets[sheetId];
    out.xPos = cwi.left;
    out.width = cwi.glyphWidth;

    int     lineId = glInSheet / tglp->nRows;
    int     rowId = glInSheet % tglp->nRows;

    float   tx = (float)(rowId * (tglp->cellWidth + 1) + 1) / tglp->sheetWidth;
    float   ty = 1.0f - (float)(lineId * (tglp->cellHeight + 1) + 1) / tglp->sheetHeight;
    float   tw = (float)cwi.glyphWidth / tglp->sheetWidth;
    float   th = (float)tglp->cellHeight / tglp->sheetHeight;

    out.texcoord.left = tx;
    out.texcoord.top = ty;
    out.texcoord.right = tx + tw;
    out.texcoord.bottom = ty - th;

    return cwi.charWidth;
}
