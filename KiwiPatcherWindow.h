/*
 ==============================================================================
 
 This file is part of the KIWI library.
 Copyright (c) 2014 Pierre Guillot & Eliott Paris.
 
 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3
 
 Details of these licenses can be found at: www.gnu.org/licenses
 
 KIWI is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 ------------------------------------------------------------------------------
 
 To release a closed-source product which uses KIWI, contact : guillotpierre6@gmail.com
 
 ==============================================================================
*/

#ifndef __DEF_KIWI_PATCHER_WINDOW__
#define __DEF_KIWI_PATCHER_WINDOW__

#include "KiwiPatch.h"

namespace Kiwi
{
    // ================================================================================ //
    //                              PATCHER WINDOW                                      //
    // ================================================================================ //

    class Patcher::Window : public GuiWindow
    {
    private:
        const sPatcher      m_patcher;
        const sGuiViewport  m_viewport;
    public:
        
        //! The patcher controller constructor.
        /** The function does nothing.
         @param patcher The patcher.
         */
        Window(sPatcher patcher) noexcept;
        
        //! The controller destructor.
        /** The function does nothing.
         */
        ~Window() noexcept;
    };
}


#endif


