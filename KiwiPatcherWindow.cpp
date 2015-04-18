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

#include "KiwiPatcherWindow.h"
#include "KiwiInstance.h"
#include "KiwiConsole.h"

namespace Kiwi
{    
    // ================================================================================ //
    //                              PATCHER WINDOW                                      //
    // ================================================================================ //
    
    Patcher::Window::Window(sPatcher patcher) noexcept :
    GuiWindow(patcher->GuiSketcher::getContext()),
    m_patcher(patcher)
    {
        setPosition(patcher->getPosition());
        setSize(Size(patcher->getSize().width(), patcher->getSize().height() + 24.));
    }
    
    Patcher::Window::~Window() noexcept
    {
        
    }
    
    void Patcher::Window::initialize() noexcept
    {
        m_header = GuiWindow::Header::create(dynamic_pointer_cast<GuiWindow>(shared_from_this()), "Zaza");
        m_container = make_shared<GuiContainer>(getContext());
        m_container->setPosition(Point(0., 24.));
        m_container->setSize(Size(getSize().width(), getSize().height() - 24));
        addContent(m_header);
        addContent(m_container);
        m_container->addContent(m_patcher);
    }
}




