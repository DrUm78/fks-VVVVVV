#include "Screen.h"

#include "FileSystemUtils.h"
#include "GraphicsUtil.h"

#include <stdlib.h>

// Used to create the window icon
extern "C"
{
	extern unsigned lodepng_decode24(
		unsigned char** out,
		unsigned* w,
		unsigned* h,
		const unsigned char* in,
		size_t insize
	);
}

Screen::Screen()
{
    hw_screen = NULL;
    m_screen = NULL;
    isWindowed = true;
    stretchMode = 0;
    isFiltered = false;
    filterSubrect.x = 1;
    filterSubrect.y = 1;
    filterSubrect.w = 318;
    filterSubrect.h = 238;

    mode = ScaleMode::CROPPED;

    hw_screen = SDL_SetVideoMode(240, 240, 32, SDL_DOUBLEBUF | SDL_HWSURFACE);

	  m_screen = SDL_CreateRGBSurface(
		  0,
		  320,
		  240,
		  32,
		  0x00FF0000,
		  0x0000FF00,
		  0x000000FF,
		  0xFF000000
	  );

    SDL_SetAlpha(m_screen, 0, 0);

    badSignalEffect = false;
    glScreen = true;
}

void Screen::ResizeScreen(int x , int y)
{
	static int resX = 320;
	static int resY = 240;
	if (x != -1 && y != -1)
	{
		// This is a user resize!
		resX = x;
		resY = y;
	}
}

void Screen::GetWindowSize(int* x, int* y)
{
  *x = 240;
  *y = 240;
}

void Screen::UpdateScreen(SDL_Surface* buffer, SDL_Rect* rect )
{
    if((buffer == NULL) && (m_screen == NULL) )
    {
        return;
    }

    if(badSignalEffect)
    {
        buffer = ApplyFilter(buffer);
    }


    FillRect(m_screen, 0x000);
    BlitSurfaceStandard(buffer,NULL,m_screen,rect);

    if(badSignalEffect)
    {
        SDL_FreeSurface(buffer);
    }

}

const SDL_PixelFormat* Screen::GetFormat()
{
    return hw_screen->format;
}

void Screen::nextScaleMode()
{
  const char* name = "STRETCHED";
  
  if (mode == ScaleMode::SCALED)
    mode = ScaleMode::STRETCHED;
  else if (mode == ScaleMode::STRETCHED)
  {
    FillRect(hw_screen, 0x000);
    mode = ScaleMode::CROPPED;
    name = "CROPPED";
  }
  else
  {
    FillRect(hw_screen, 0x000);
    mode = ScaleMode::SCALED;
    name = "SCALED";
  }

#if !_WIN32
  const char* SHELL_CMD_NOTIF_SET = "notif set";
  constexpr int NOTIF_SECONDS_DISP = 2;

  FILE *fp;
  char command[100];
  sprintf(command, "%s %d \"     DISPLAY MODE: %s\"",
    SHELL_CMD_NOTIF_SET, NOTIF_SECONDS_DISP, name);
  fp = popen(command, "r");
  if (fp == NULL) {
    printf("Failed to run command %s\n", command);
  }
#endif
}

void Screen::setPlayerPosition(int x, int y)
{
  this->x = x;
  this->y = y;
}

void Screen::FlipScreen()
{
  if (mode == ScaleMode::SCALED)
  {
    //scale_NN_AllowOutOfScreen(m_screen, hw_screen, 240, 180);
    downscale_320x240_to_240x180_bilinearish(m_screen, hw_screen);
  }
  else if (mode == ScaleMode::STRETCHED)
  {
    //scale_NN_AllowOutOfScreen(m_screen, hw_screen, 240, 240);
    downscale_320x240_to_240x240_bilinearish(m_screen, hw_screen);
  }
  else if (mode == ScaleMode::CROPPED)
  {
    //if (x != -1)
    {
      int base = 40; 

      if (x != -1)
      {
        if (x < 120)
          base = 0;
        else if (x >= 200)
          base = 80;
        else
          base = x - 120;
      }

      SDL_Rect src = { base, 0, 240, 240 };
      BlitSurfaceStandard(m_screen, &src, hw_screen, NULL);
    }
    //else
    //  scale_NN_AllowOutOfScreen(m_screen, hw_screen, 240, 180);
  }

  SDL_Flip(hw_screen);
}

void Screen::toggleFullScreen()
{
	isWindowed = !isWindowed;
	ResizeScreen(-1, -1);
}

void Screen::toggleStretchMode()
{
	stretchMode = (stretchMode + 1) % 3;
	ResizeScreen(-1, -1);
}

void Screen::toggleLinearFilter()
{

}

void Screen::ClearScreen( int colour )
{
    //FillRect(m_screen, colour) ;
}

#define RES_HW_SCREEN_VERTICAL 240
#define RES_HW_SCREEN_HORIZONTAL 240

void Screen::scale_NN_AllowOutOfScreen(SDL_Surface *src_surface, SDL_Surface *dst_surface, int new_w, int new_h)
{

  /// Sanity check
  if (src_surface->format->BytesPerPixel != dst_surface->format->BytesPerPixel) {
    printf("Error in %s, src_surface bpp: %d != dst_surface bpp: %d", __func__,
      src_surface->format->BytesPerPixel, dst_surface->format->BytesPerPixel);
    return;
  }

  int BytesPerPixel = src_surface->format->BytesPerPixel;
  int w1 = src_surface->w;
  //int h1=src_surface->h;
  int w2 = new_w;
  int h2 = new_h;
  int x_ratio = (int)((src_surface->w << 16) / w2);
  int y_ratio = (int)((src_surface->h << 16) / h2);
  int x2, y2;

  /// --- Compute padding for centering when out of bounds ---
  int y_padding = (RES_HW_SCREEN_VERTICAL - new_h) / 2;
  int x_padding = 0;
  if (w2 > RES_HW_SCREEN_HORIZONTAL) {
    x_padding = (w2 - RES_HW_SCREEN_HORIZONTAL) / 2 + 1;
  }
  int x_padding_ratio = x_padding * w1 / w2;
  //printf("src_surface->h=%d, h2=%d\n", src_surface->h, h2);

  for (int i = 0; i < h2; i++)
  {
    if (i >= RES_HW_SCREEN_VERTICAL) {
      continue;
    }

    uint8_t* t = (uint8_t*)(dst_surface->pixels) + ((i + y_padding) * ((w2 > RES_HW_SCREEN_HORIZONTAL) ? RES_HW_SCREEN_HORIZONTAL : w2))*BytesPerPixel;
    y2 = ((i*y_ratio) >> 16);
    uint8_t* p = (uint8_t*)(src_surface->pixels) + (y2*w1 + x_padding_ratio)*BytesPerPixel;
    int rat = 0;
    for (int j = 0; j < w2; j++)
    {
      if (j >= RES_HW_SCREEN_HORIZONTAL) {
        continue;
      }
      x2 = (rat >> 16);
      //*t++ = p[x2];
      memcpy(t, &p[x2*BytesPerPixel], BytesPerPixel);
      t += BytesPerPixel;
      rat += x_ratio;
    }
  }
}

// Support math
#define Half(A) (((A) >> 1) & 0x7F7F7F7F)
#define Quarter(A) (((A) >> 2) & 0x3F3F3F3F)
// Error correction expressions to piece back the lower bits together
#define RestHalf(A) ((A) & 0x01010101)
#define RestQuarter(A) ((A) & 0x03030303)

// Error correction expressions for quarters of pixels
#define Corr1_3(A, B)     Quarter(RestQuarter(A) + (RestHalf(B) << 1) + RestQuarter(B))
#define Corr3_1(A, B)     Quarter((RestHalf(A) << 1) + RestQuarter(A) + RestQuarter(B))

// Error correction expressions for halves
#define Corr1_1(A, B)     ((A) & (B) & 0x01010101)

// Quarters
#define Weight1_3(A, B)   (Quarter(A) + Half(B) + Quarter(B) + Corr1_3(A, B))
#define Weight3_1(A, B)   (Half(A) + Quarter(A) + Quarter(B) + Corr3_1(A, B))

// Halves
#define Weight1_1(A, B)   (Half(A) + Half(B) + Corr1_1(A, B))

void Screen::downscale_320x240_to_240x240_bilinearish(SDL_Surface *src_surface, SDL_Surface *dst_surface)
{
  int w1 = src_surface->w;
  int h1 = src_surface->h;
  int w2 = dst_surface->w;
  int h2 = dst_surface->h;

  if (w1 != 320) {
    printf("src_surface->w (%d) != 320\n", src_surface->w);
    return;
  }

  //printf("src = %dx%d\n", w1, h1);
  int y_ratio = (int)((h1 << 16) / h2);
  int y_padding = (RES_HW_SCREEN_VERTICAL - h2) / 2;
  int y1;
  uint32_t *src_screen = (uint32_t *)src_surface->pixels;
  uint32_t *dst_screen = (uint32_t *)dst_surface->pixels;

  /* Interpolation */
  for (int i = 0; i < h2; i++)
  {
    if (i >= RES_HW_SCREEN_VERTICAL) {
      continue;
    }
    uint32_t* t = (uint32_t*)(dst_screen +
      (i + y_padding)*((w2 > RES_HW_SCREEN_HORIZONTAL) ? RES_HW_SCREEN_HORIZONTAL : w2));

    // ------ current and next y value ------
    y1 = ((i*y_ratio) >> 16);
    uint32_t* p = (uint32_t*)(src_screen + (y1*w1));

    for (int j = 0; j < 80; j++)
    {
      /* Horizontaly:
       * Before(4):
       * (a)(b)(c)(d)
       * After(3):
       * (aaab)(bc)(cddd)
       */
      uint32_t _a = *(p);
      uint32_t _b = *(p + 1);
      uint32_t _c = *(p + 2);
      uint32_t _d = *(p + 3);
      *(t) = Weight3_1(_a, _b);
      *(t + 1) = Weight1_1(_b, _c);
      *(t + 2) = Weight1_3(_c, _d);

      // ------ next dst pixel ------
      t += 3;
      p += 4;
    }
  }
}

void Screen::downscale_320x240_to_240x180_bilinearish(SDL_Surface *src_surface, SDL_Surface *dst_surface)
{
  if (src_surface->w != 320)
  {
    printf("src_surface->w (%d) != 320 \n", src_surface->w);
    return;
  }
  if (src_surface->h != 240)
  {
    printf("src_surface->h (%d) != 240 \n", src_surface->h);
    return;
  }

  /// Compute padding for centering when out of bounds
  int y_padding = (RES_HW_SCREEN_VERTICAL - 180) / 2;

  uint32_t *Src32 = (uint32_t *)src_surface->pixels;
  uint32_t *Dst32 = (uint32_t *)dst_surface->pixels + y_padding * RES_HW_SCREEN_HORIZONTAL;

  // There are 80 blocks of 2 pixels horizontally, and 48 of 3 horizontally.
  // Horizontally: 320=80*4 240=80*3
  // Vertically: 240=60*4 180=60*3
  // Each block of 4*4 becomes 3*3
  uint32_t BlockX, BlockY;
  uint32_t *BlockSrc;
  uint32_t *BlockDst;
  for (BlockY = 0; BlockY < 60; BlockY++)
  {
    BlockSrc = Src32 + BlockY * 320 * 4;
    BlockDst = Dst32 + BlockY * 240 * 3;
    for (BlockX = 0; BlockX < 80; BlockX++)
    {
      /* Horizontaly:
       * Before(4):
       * (a)(b)(c)(d)
       * After(3):
       * (aaab)(bc)(cddd)
       */

       /* Verticaly:
        * Before(2):
        * (1)(2)(3)(4)
        * After(4):
        * (1112)(23)(3444)
        */

        // -- Data --
      uint32_t _a1 = *(BlockSrc);
      uint32_t _b1 = *(BlockSrc + 1);
      uint32_t _c1 = *(BlockSrc + 2);
      uint32_t _d1 = *(BlockSrc + 3);
      uint32_t _a2 = *(BlockSrc + 320 * 1);
      uint32_t _b2 = *(BlockSrc + 320 * 1 + 1);
      uint32_t _c2 = *(BlockSrc + 320 * 1 + 2);
      uint32_t _d2 = *(BlockSrc + 320 * 1 + 3);
      uint32_t _a3 = *(BlockSrc + 320 * 2);
      uint32_t _b3 = *(BlockSrc + 320 * 2 + 1);
      uint32_t _c3 = *(BlockSrc + 320 * 2 + 2);
      uint32_t _d3 = *(BlockSrc + 320 * 2 + 3);
      uint32_t _a4 = *(BlockSrc + 320 * 3);
      uint32_t _b4 = *(BlockSrc + 320 * 3 + 1);
      uint32_t _c4 = *(BlockSrc + 320 * 3 + 2);
      uint32_t _d4 = *(BlockSrc + 320 * 3 + 3);

      uint32_t _a2a2a2b2 = Weight3_1(_a2, _b2);
      uint32_t _a3a3a3b3 = Weight3_1(_a3, _b3);
      uint32_t _b2c2 = Weight1_1(_b2, _c2);
      uint32_t _b3c3 = Weight1_1(_b3, _c3);
      uint32_t _c2d2d2d2 = Weight1_3(_c2, _d2);
      uint32_t _c3d3d3d3 = Weight1_3(_c3, _d3);

      // -- Line 1 --
      *(BlockDst) = Weight3_1(Weight3_1(_a1, _b1), _a2a2a2b2);
      *(BlockDst + 1) = Weight3_1(Weight1_1(_b1, _c1), _b2c2);
      *(BlockDst + 2) = Weight3_1(Weight1_3(_c1, _d1), _c2d2d2d2);

      // -- Line 2 --
      *(BlockDst + 240 * 1) = Weight1_1(_a2a2a2b2, _a3a3a3b3);
      *(BlockDst + 240 * 1 + 1) = Weight1_1(_b2c2, _b3c3);
      *(BlockDst + 240 * 1 + 2) = Weight1_1(_c2d2d2d2, _c3d3d3d3);

      // -- Line 3 --
      *(BlockDst + 240 * 2) = Weight1_3(_a3a3a3b3, Weight3_1(_a4, _b4));
      *(BlockDst + 240 * 2 + 1) = Weight1_3(_b3c3, Weight1_1(_b4, _c4));
      *(BlockDst + 240 * 2 + 2) = Weight1_3(_c3d3d3d3, Weight1_3(_c4, _d4));

      BlockSrc += 4;
      BlockDst += 3;
    }
  }
}
