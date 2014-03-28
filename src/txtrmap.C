/*
    Sabre Flight Simulator 
    Copyright (C) 1997	Dan Hammer

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/*************************************************
 *           Sabre Flight Simulator              *
 * File   : txtrmap.C                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *          08/20/97                             *
 *          Antti Barck                          *
 *          Use unsigned int when read/writing   *
 *          compressed files to make compatible  *
 *          with 64bit machines                  *
 *************************************************/
#ifdef SABREWIN
#include <windows.h>
#endif
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include <string.h>
#include "swap.h"
#include "defs.h"
#include "grafix.h"
#include "vga_13.h"
#include "vmath.h"
#include "simerr.h"
#include "simfile.h"
#include "colormap.h"
#include "txtrmap.h"
#include "comp.h"
#include "unc.h"

TextrMap nullmap;
TextrMap_Manager *map_man = NULL;
int TextrMap_Manager::reserved = 64;

void TextrMap::copy(TextrMap &tm)
{
	bytes = tm.bytes;
	size = tm.size;
	cbytes = tm.cbytes;
	csize = tm.csize;
	map_w = tm.map_w;
	map_h = tm.map_h;
	trans_colr = tm.trans_colr;
	delf = 1;
	tm.delf = 0;
	flags = tm.flags;
	memcpy(id,tm.id,sizeof(id));
	if (tm.cmaps != NULL)
	{
		cmaps = new ColorMap[N_COLORS];
		MYCHECK(cmaps != NULL);
		for (int i=0;i<N_COLORS;i++)
			cmaps[i] = tm.cmaps[i];
	}
	if (tm.mc != NULL && tm.nm > 0)
	{
		nm = tm.nm;
		mc = new int[nm];
		MYCHECK(mc != NULL);
		for (int i=0;i<nm;i++)
			mc[i] = tm.mc[i];
	}
}

void TextrMap::read(std::istream &is)
{
char *cptr;
int ii;
char c = ' ';

	memset(id,' ',sizeof(id));
	READ_TOK('{',is,c)
	is >> id;
	cptr = id + strlen(id) - 1;
	c = *cptr;
	if (c == '*' || c == '@')
	{
		is >> trans_colr;
		*cptr = '\0';
		if (c == '@')
			read_compressed(0);
		else
			read_compressed(1);
	}
	else
	{
		is >>  map_w >> map_h >> trans_colr;
		id[31] = 0;
		int nbytes = map_w * map_h;
		if (bytes != NULL)
			delete [] bytes;
		bytes = new unsigned char[nbytes];
		MYCHECK(bytes != NULL);
		delf = 1;
		for (int i=0;i<nbytes;i++)
		{
			is >> ii;
			bytes[i] = (unsigned char) ii;
		}
	}

	is >> c;
	if (c == '*')
	{
		is >> nm;
		cmaps = new ColorMap[N_COLORS];
		MYCHECK(cmaps != NULL);
		mc = new int[nm];
		MYCHECK(mc != NULL);
		ColorMap tmp;
		for (int i=0;i<nm;i++)
		{
			is >> tmp;
			mc[i] = tmp.id_color;
			cmaps[tmp.id_color] = tmp;
		}
	}
	READ_TOK('}',is,c);
}

void TextrMap::write(std::ostream &os)
{
  os << "{\n" << id << "*\n" << trans_colr << "\n";
  compress();
  os << "}\n";
}

std::istream & operator >>(std::istream &is, TextrMap &tm)
{
  tm.read(is);
  return (is);
}

std::ostream & operator <<(std::ostream &os, TextrMap &tm)
{
  tm.write(os);
  return (os);
}

void TextrMap::compress()
{
unsigned int  n;
unsigned long tgtsize;
unsigned long srcsize;
char     *tgt;
char     *fname;
std::ofstream os;

	if (bytes != NULL)
	{
		tgtsize = (unsigned long) map_w * map_h;
		srcsize = tgtsize;
		tgt = new char[tgtsize];
		if ((n = memcompress(tgt,tgtsize,(char *)bytes,srcsize)) != 0)
		{
			fname = new char[strlen(id) + 10];
#ifdef SABREWIN
			sprintf(fname,"tzp\\%s.tzp",id);
#else
			sprintf(fname,"tzp/%s.tzp",id);
#endif
			if (open_libos(os,fname))
			{
				uint32_t tmp;
				tmp = ltohl(map_w);
				os.write((char *)&tmp,sizeof(tmp));
				tmp = ltohl(map_h);
				os.write((char *)&tmp,sizeof(tmp));
				tmp = ltohl(n);
				os.write((char *)&tmp,sizeof(tmp));
				os.write(tgt,n);
			}
			delete [] fname;
		}
		else
			printf("compression error: %d\n",n);
		delete [] tgt;
	}
}

void TextrMap::read_compressed(int cflg)
{
uint32_t          n;
char              *fname;
FILE              *f;
int               nread;
const char              *path;
const char              *open_params;

	fname = new char[strlen(id) + 10];
#ifdef SABREWIN
	/* ain't DOS dumb? */
	sprintf(fname,"tzp\\%s.tzp",id);
	open_params = "rb";
#else
	sprintf(fname,"tzp/%s.tzp",id);
	open_params = "r";
#endif
	path = build_libpath(fname);
	if ((f = fopen(path,open_params)) != NULL)
	{
		fread(&map_w,sizeof(map_w),1,f);
		map_w = ltohl(map_w);
		fread(&map_h,sizeof(map_h),1,f);
		map_h = ltohl(map_h);
		fread(&n,sizeof(n),1,f);
		n = ltohl(n);
		size = map_w * map_h;
		csize = n;
		cbytes = new unsigned char[csize];
		MYCHECK(cbytes != NULL);
		nread = fread(cbytes,1,csize,f);
		if (ferror(f))
		{
			error_jump("TextrMap: error reading from file %s %d bytes\n",
			 fname,nread);
			delete [] cbytes;
			cbytes = NULL;
			return;
		}
		fclose(f);
		if (bytes != NULL)
		{
			delete [] bytes;
			bytes = NULL;
		}
		if (!cflg)
			getBytes(0);
	}
}

unsigned char *TextrMap::getBytes(int cflg)
{
long r;

	if (bytes != NULL)
		return (bytes);
	if (cbytes == NULL)
	{
		error_jump("TextrMap:: cbytes are NULL\n");
		return (NULL);
	}
	bytes = new unsigned char[size];
	MYCHECK(bytes != NULL);
	r = unc_mem_to_mem(bytes,size,cbytes,csize);
	if (r <= 0)
	{
		error_jump("TextrMap %s: decompression error\n", id);
		delete [] bytes;
		bytes = NULL;
	}
	else if (!cflg)
	{
		delete [] cbytes;
		cbytes = NULL;
	}
	return (bytes);
}

void TextrMap::clearBytes()
{
  if (bytes != NULL)
    {
      delete [] bytes;
      bytes = NULL;
    }
}

void TextrMap::setColorMaps(float r)
{
  if (cmaps != NULL)
    for (int i=0;i<nm;i++)
      cmaps[mc[i]].calcMappedColor(r);
}

void TextrMap::setColorMaps(int base, int range, int col)
{
  if (cmaps != NULL)
    for (int i=0;i<nm;i++)
      cmaps[mc[i]].calcMappedColor(base,range,col);
}

#ifdef SABREWIN

extern RGBQUAD *wvga_get_rgb_colors();

long TextrMap::createDIB(long hdc)
{
HBITMAP result = (HBITMAP) NULL;
struct tagBMINFO
{
BITMAPINFOHEADER bmiHeader;
RGBQUAD          bmiColors[256];
} bmInfo;
int i;
RGBQUAD *wvga_colors;
unsigned char *bmBytes;

	wvga_colors = wvga_get_rgb_colors();
	for (i=0;i<256;i++)
		bmInfo.bmiColors[i] = wvga_colors[i];

   bmInfo.bmiHeader.biSize          = sizeof (BITMAPINFOHEADER);
   bmInfo.bmiHeader.biWidth         = map_w;
   bmInfo.bmiHeader.biHeight        = -abs(map_h); // Top-down bitmap
   bmInfo.bmiHeader.biPlanes        = 1;
   bmInfo.bmiHeader.biBitCount      = 8;
   bmInfo.bmiHeader.biCompression   = BI_RGB;
   bmInfo.bmiHeader.biSizeImage     = NULL;
   bmInfo.bmiHeader.biXPelsPerMeter = NULL;
   bmInfo.bmiHeader.biYPelsPerMeter = NULL;
   bmInfo.bmiHeader.biClrUsed       = 256;
   bmInfo.bmiHeader.biClrImportant  = 256;

	getBytes();
	if (bytes == NULL)
		return 0L;

   result = CreateDIBSection ((HDC)hdc,(BITMAPINFO *) &bmInfo, DIB_RGB_COLORS,
                              (VOID * *) &bmBytes, NULL, NULL);

	memcpy(bmBytes,bytes,size);
	hBitmap = (long) result;
	return hBitmap;
}

void TextrMap::freeDIB()
{
	if (hBitmap)
	{
		DeleteObject((HBITMAP)hBitmap);
		hBitmap = 0;
	}
}

#ifdef USES_DDRAW
#define D3D_OVERLOADS

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <d3d.h>
/**************************************************************************
 DEBUG junk
 **************************************************************************/
#if defined(DEBUG) || defined(_DEBUG)
static void DPF(char *fmt, ...)
{
    char ach[128];
    va_list va;
    va_start( va, fmt );
    wvsprintf( ach, fmt, va );
    va_end( va );
    OutputDebugString("TEXTURE: ");
    OutputDebugString(ach);
    OutputDebugString("\r\n");
}
#else
#define DPF if (0) 
#endif

/**************************************************************************
 ChooseTextureFormat
 **************************************************************************/

typedef struct {
    DWORD           bpp;        // we want a texture format of this bpp
    DDPIXELFORMAT   ddpf;       // place the format here
}   FindTextureData;

static HRESULT CALLBACK FindTextureCallback(DDSURFACEDESC *DeviceFmt, LPVOID lParam)
{
    FindTextureData * FindData = (FindTextureData *)lParam;
    DDPIXELFORMAT ddpf = DeviceFmt->ddpfPixelFormat;

    DPF("FindTexture: %d %s%s%s %08X %08X %08X %08X", 
	ddpf.dwRGBBitCount, 
        (ddpf.dwFlags & (DDPF_ALPHA|DDPF_ALPHAPIXELS)) ? "ALPHA " : "", 
        (ddpf.dwFlags &	(DDPF_RGB)) ? "RGB " : "", 
        (ddpf.dwFlags &	(DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4)) ? "PAL " : "", 
	ddpf.dwRBitMask,
	ddpf.dwGBitMask,
	ddpf.dwBBitMask,
	ddpf.dwRGBAlphaBitMask);

    //
    // we use GetDC/BitBlt to init textures so we only
    // want to use formats that GetDC will support.
    //
    if (ddpf.dwFlags & (DDPF_ALPHA|DDPF_ALPHAPIXELS))
        return DDENUMRET_OK;

    if (ddpf.dwRGBBitCount <= 8 &&
        !(ddpf.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4)))
        return DDENUMRET_OK;

    if (ddpf.dwRGBBitCount > 8 && !(ddpf.dwFlags & DDPF_RGB))
        return DDENUMRET_OK;

    //
    // BUGBUG GetDC does not work for 1 or 4bpp YET!
    //
    if (ddpf.dwRGBBitCount < 8)
        return DDENUMRET_OK;

    //
    // keep the texture format that is nearest to the bitmap we have
    //
    if (FindData->ddpf.dwRGBBitCount == 0 ||
       (ddpf.dwRGBBitCount >= FindData->bpp &&
       (UINT)(ddpf.dwRGBBitCount - FindData->bpp) < (UINT)(FindData->ddpf.dwRGBBitCount - FindData->bpp)))
    {
        FindData->ddpf = ddpf;
    }

    return DDENUMRET_OK;
}

static void ChooseTextureFormat(IDirect3DDevice2 *Device, DWORD bpp, DDPIXELFORMAT *pddpf)
{
    FindTextureData FindData;
    ZeroMemory(&FindData, sizeof(FindData));
    FindData.bpp = bpp;
    Device->EnumTextureFormats(FindTextureCallback, (LPVOID)&FindData);
    *pddpf = FindData.ddpf;

    DPF("ChooseTexture: %d %s%s%s %08X %08X %08X %08X", 
	pddpf->dwRGBBitCount, 
        (pddpf->dwFlags & (DDPF_ALPHA|DDPF_ALPHAPIXELS)) ? "ALPHA " : "", 
        (pddpf->dwFlags &	(DDPF_RGB)) ? "RGB " : "", 
        (pddpf->dwFlags &	(DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4)) ? "PAL " : "", 
	pddpf->dwRBitMask,
	pddpf->dwGBitMask,
	pddpf->dwBBitMask,
	pddpf->dwRGBAlphaBitMask);
}

/**************************************************************************
 PaletteFromBitmap
 **************************************************************************/
static IDirectDrawPalette * PaletteFromBitmap(IDirectDraw *DirectDraw, HBITMAP Bitmap)
{
    IDirectDrawPalette *    Palette = NULL;
    HDC                     BitmapDC;
    DWORD                   adw[256];
    int                     colors, i;

    //
    // get the color table from the DIBSection
    //
    BitmapDC = CreateCompatibleDC(NULL);
    SelectObject(BitmapDC, Bitmap);
    colors = GetDIBColorTable(BitmapDC, 0, 256, (RGBQUAD *)adw);
    DeleteDC(BitmapDC);

    if (colors != 0)
    {
        //
        // convert BGR to RGB (stupid IBM!)
        //
        for (i=0; i<colors; i++)
            adw[i] = RGB(GetBValue(adw[i]),GetGValue(adw[i]),GetRValue(adw[i]));

        //
        // create a DirectDraw palette for the texture.
        //
        DirectDraw->CreatePalette(colors <= 16 ? DDPCAPS_4BIT : DDPCAPS_8BIT,
            (PALETTEENTRY *)adw, &Palette, NULL);
    }

    return Palette;
}

/**************************************************************************
 GetDD

 get the IDirectDraw from a IDirect3DDevice, we need the DD
 to create surfaces.

 this function does not do a AddRef on the DirectDraw object
 (ie you dont need to Release)

 NOTE if your app has this around as a global you can use
 the global instead of this code.

 **************************************************************************/
static IDirectDraw * GetDD(IDirect3DDevice2 *Device)
{
    IDirectDraw *       DirectDraw;
    IDirectDrawSurface *Target;
    IDirectDrawSurface2*Target2;

    //
    // get the render target (we need it to get the IDirectDraw)
    //
    if (Device==NULL || Device->GetRenderTarget(&Target) != DD_OK)
        return NULL;

    //
    // get the DirectDraw object, but first we need a IDirectDrawSurface2
    //
    if (Target->QueryInterface(IID_IDirectDrawSurface2, (void**)&Target2) != DD_OK)
        return NULL;
    Target->Release();
    Target2->GetDDInterface((void**)&DirectDraw);
    Target2->Release();
    DirectDraw->Release();  // dont up ref count

    return DirectDraw;
}

/**************************************************************************
 TextrMap::Load

 load a bitmap from the resource, or bmp file and create a
 D3D texture map

 **************************************************************************/
BOOL TextrMap::Load(IDirect3DDevice2 *Device, char *BitmapName)
{
BITMAP              bm;
DDSURFACEDESC       ddsd;
HBITMAP             Bitmap;
IDirectDraw *       DirectDraw;

    DPF("TextrMap::Load(%s)", BitmapName);
	 if (Handle)
		 return TRUE;

    //
    // we need a IDirectDraw so we can create a surface.
    //
    if ((DirectDraw = GetDD(Device)) == NULL)
        return FALSE;

	if (BitmapName != NULL)
	{
		 //
		 // load the bitmap from a resource or file.
		 //
		 Bitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), BitmapName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		 if (Bitmap == NULL)
			  Bitmap = (HBITMAP)LoadImage(NULL, BitmapName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

		 if (Bitmap == NULL)
			  return FALSE;
	}
	else
	{
		createDIB((long) GetModuleHandle(NULL));
		Bitmap = (HBITMAP) hBitmap;
	}
    // free any existing texture.
    Release();

    GetObject(Bitmap, sizeof(bm), &bm);      // get size of bitmap

    //
    // find the best texture format to use.
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ChooseTextureFormat(Device, bm.bmBitsPixel, &ddsd.ddpfPixelFormat);
    ddsd.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;

    //
    // create a video memory texture
    //
    // if we are dealing with a HAL create in video memory, else
    // create in system memory.
    //
    D3DDEVICEDESC hal, hel;
    ZeroMemory(&hal, sizeof(hal));
    hal.dwSize = sizeof(hal);
    ZeroMemory(&hel, sizeof(hel));
    hel.dwSize = sizeof(hel);
    Device->GetCaps(&hal, &hel);

    // BUGBUG should we check for texture caps?
    if (hal.dcmColorModel)
        ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
    else
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY| DDSCAPS_TEXTURE;

    if (DirectDraw->CreateSurface(&ddsd, &DeviceSurface, NULL) != DD_OK)
        goto error;

    //
    // create a system memory surface for the texture.
    //
    // we use this system memory surface for the ::Load call
    // and this surface does not get lost.
    //
    if (hal.dcmColorModel)
    {
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;

        if (DirectDraw->CreateSurface(&ddsd, &MemorySurface, NULL) != DD_OK)
            goto error;
    }
    else
    {
        //
        // when dealing with a SW rasterizer we dont need to make two
        // surfaces.
        //
        MemorySurface = DeviceSurface;
        DeviceSurface->AddRef();
    }

    //
    // create a palette for the texture
    //
    if (ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
    {
        Palette = PaletteFromBitmap(DirectDraw, Bitmap);

        //
        // now we have a palette, attach the palette to the Surface
        // and the texture
        //
        MemorySurface->SetPalette(Palette);
        DeviceSurface->SetPalette(Palette);
    }

    //
    // copy the bitmap to our surface
    //
    if (!Copy(Bitmap))
        goto error;

    //
    // get the texture handle
    //
    IDirect3DTexture2 *	Texture;
    DeviceSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&Texture);
    Texture->GetHandle(Device, &Handle);
    Texture->Release();

    //
    // we are done, delete the bitmap (we made a copy) and return.
    //
    DeleteObject(Bitmap);

	 delete bytes;
	 bytes = NULL;
	 
    return TRUE;

error:
    if (Bitmap)
        DeleteObject(Bitmap);
    Release();
    return FALSE;
}

/**************************************************************************
 TextrMap::Copy

 init the system memory surface from a GDI Bitmap

**************************************************************************/
BOOL TextrMap::Copy(HBITMAP Bitmap)
{
    BITMAP  bm;
    HDC     BitmapDC;
    HDC     SurfaceDC;
    BOOL    result = FALSE;

    DPF("TextrMap::Copy");

    GetObject(Bitmap, sizeof(bm), &bm);    // get size of bitmap

    BitmapDC = CreateCompatibleDC(NULL);
    SelectObject(BitmapDC, Bitmap);

    if (MemorySurface->GetDC(&SurfaceDC) == DD_OK)
    {
        BitBlt(SurfaceDC, 0, 0, bm.bmWidth, bm.bmHeight, BitmapDC, 0, 0, SRCCOPY);
        MemorySurface->ReleaseDC(SurfaceDC);
        result = Restore();
    }
    DeleteDC(BitmapDC);
    return result;
}


/**************************************************************************
 TextrMap::Restore

 restore the texture image

 **************************************************************************/

BOOL TextrMap::Restore()
{
    HRESULT             err;
    IDirect3DTexture2  *MemoryTexture;
    IDirect3DTexture2  *DeviceTexture;

    DPF("TextrMap::Restore");

    if (DeviceSurface == NULL || MemorySurface == NULL)
        return FALSE;

    //
    // we dont need to do this step for system memory surfaces.
    //
    if (DeviceSurface == MemorySurface)
        return TRUE;

    //
    // restore the video memory texture.
    //
    if (DeviceSurface->Restore() != DD_OK)
        return FALSE;

    //
    // call IDirect3DTexture::Load() to copy the texture to the device.
    //
    DeviceSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&DeviceTexture);
    MemorySurface->QueryInterface(IID_IDirect3DTexture2, (void**)&MemoryTexture);

    err = DeviceTexture->Load(MemoryTexture);

    DeviceTexture->Release();
    MemoryTexture->Release();

    return err == DD_OK;
}

/**************************************************************************
 TextrMap::Release

 free the texture, free all objects associated with this texture

 NOTE we cant do this in the destructor because DirectDraw
 will clean up all the surfaces when the main direct draw object
 is destroyed, before a static destructor will be called.

 **************************************************************************/

void TextrMap::Release(void)
{
	if (MemorySurface)
		MemorySurface->Release();
	MemorySurface = 0;
	if (DeviceSurface)
		DeviceSurface->Release();
	DeviceSurface = 0;
	if (Palette)
		Palette->Release();
	Palette = 0;
	Handle = 0;
}

#endif

#endif

/*************************************************************
 * TextrMap_Manager methods                                  *
 *************************************************************/
TextrMap_Manager::TextrMap_Manager(int n)
{
  n_maps = n;
  tmaps = new TextrMap[n_maps];
  MYCHECK(tmaps != NULL);
  nxt = 0;
}

int TextrMap_Manager::add_map(TextrMap &tm)
{
  if (nxt < n_maps - 1)
    {
      tmaps[nxt++] = tm;
      return 1;
    }
  else
    return 0;
}

void TextrMap_Manager::read(std::istream &is)
{
int n;

	if (tmaps == NULL)
	{
		is >> n_maps;
		if (n_maps <= 0)
			return;
		tmaps = new TextrMap[n_maps];
		MYCHECK(tmaps != NULL);
		for (int i=0;i<n_maps;i++)
			is >> tmaps[i];
			nxt = n_maps - 1;
	}
	else
	{
		is >> n;
		for (int i=0;i<n;i++)
		{
			TextrMap tmp;
			if (nxt < n_maps - 1)
			{
				is >> tmaps[nxt];
				nxt++;
			}
			else
				is >> tmp;
		}
	}
}

std::istream &operator >>(std::istream &is, TextrMap_Manager &tm)
{
  tm.read(is);
  return (is);
}

void TextrMap_Manager::read_file(const char *path)
{
  std::ifstream is;
  if (open_is(is,path))
    is >> *this;
}

void TextrMap_Manager::write(std::ostream &os)
{
  os << nxt << '\n';
  for (int i=0;i<nxt;i++)
    os << tmaps[i] << '\n';
}


std::ostream &operator <<(std::ostream &os, TextrMap_Manager &tm)
{
  tm.write(os);
  return (os);
}

void TextrMap_Manager::write_file(const char *path)
{
  std::ofstream os;
  if (open_libos(os,path))
    write(os);
}


TextrMap *TextrMap_Manager::get_map_ptr(const char *id)
{
	TextrMap *result = NULL;
	for (int i=0;i<n_maps;i++)
	{
		if (!strcmp(id,tmaps[i].id))
		{
			result = &tmaps[i];
			break;
		}
	}
	return (result);
}




