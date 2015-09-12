
//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#ifndef _SDLWindow_H_
#define _SDLWindow_H_

#include "../api.h"
#include "Window.h"
#include "KeyEvent.h"

#include "../graphics/GLConfig.h"
#include "../base/Rect.h"

#include <SDL2/SDL.h>
#include <boost/shared_ptr.hpp>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace avg {

class XInputMTInputDevice;

class AVG_API SDLWindow: public Window
{
    public:
        SDLWindow(const DisplayParams& dp, const WindowParams& wp, GLConfig glConfig);
        virtual ~SDLWindow();

        void setTitle(const std::string& sTitle);
        void swapBuffers() const;

        std::vector<EventPtr> pollEvents();
        void setXIMTInputDevice(XInputMTInputDevice* pInputDevice);
        void setMousePos(const IntPoint& pos);
        void setGamma(float red, float green, float blue);
#ifdef _WIN32
        HWND getWinHWnd();
#endif

    private:
        EventPtr createMouseEvent
                (Event::Type Type, const SDL_Event & SDLEvent, long Button);
        EventPtr createMouseButtonEvent(Event::Type Type, const SDL_Event & SDLEvent);
        KeyEventPtr createKeyEvent(Event::Type Type, const SDL_Event & SDLEvent);

        SDL_Window* m_pSDLWindow;
        SDL_GLContext m_SDLGLContext;

        // Event handling.
        glm::vec2 m_LastMousePos;
        XInputMTInputDevice * m_pXIMTInputDevice;
};

typedef boost::shared_ptr<SDLWindow> SDLWindowPtr;

}

#endif
