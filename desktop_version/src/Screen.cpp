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

    mode = ScaleMode::ASPECT;

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
  if (mode == ScaleMode::ASPECT)
    mode = ScaleMode::STRETCHED;
  else
    mode = ScaleMode::ASPECT;
}

void Screen::FlipScreen()
{
  if (mode == ScaleMode::ASPECT)
  {
    FillRect(hw_screen, 0x000);
    scale_NN_AllowOutOfScreen(m_screen, hw_screen, 240, 180);
  }
  else if (mode == ScaleMode::STRETCHED)
    scale_NN_AllowOutOfScreen(m_screen, hw_screen, 240, 240);

  //BlitSurfaceStandard(m_screen, NULL, hw_screen, NULL);
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

