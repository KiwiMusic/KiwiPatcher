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

#include "KiwiInstance.h"
#include "KiwiConsole.h"
#include "../KiwiObjects/KiwiObjects.h"

namespace Kiwi
{
    bool libraries_loaded = false;
    
    // ================================================================================ //
    //                                      INSTANCE                                    //
    // ================================================================================ //
    
    Instance::Instance(sGuiDeviceManager guiDevice, sDspDeviceManager dspDevice, sTag name) noexcept :
    GuiContext(guiDevice),
    DspContext(dspDevice),
    m_name(name)
    {
		;
    }
    
    Instance::~Instance()
    {
        lock_guard<mutex> guard(m_patchers_mutex);
        m_patchers.clear();
        m_listeners.clear();
    }
    
    sInstance Instance::create(sGuiDeviceManager guiDevice, sDspDeviceManager dspDevice, string const& name)
    {
        return make_shared<Instance>(guiDevice, dspDevice, Tag::create(name));
    }
    
    sPatcher Instance::createPatcher()
    {
        Dico dico;
        return createPatcher(dico);
    }

    sPatcher Instance::createPatcher(Dico& dico)
    {
        sPatcher patcher;
        bool state(false);
        {
            patcher = Patcher::create(getShared(), dico);
            lock_guard<mutex> guard(m_patchers_mutex);
            if(patcher && m_patchers.insert(patcher).second)
            {
                DspContext::add(patcher);
                state = true;
            }
        }
        if(state)
        {
            m_listeners.call(&Listener::patcherCreated, getShared(), patcher);
        }
        return patcher;
    }
    
    void Instance::removePatcher(sPatcher patcher)
    {
        bool success = false;
        {
            lock_guard<mutex> guard(m_patchers_mutex);
            success = m_patchers.erase(patcher);
        }
        if(success)
        {
            DspContext::remove(patcher);
            m_listeners.call(&Listener::patcherRemoved, getShared(), patcher);
        }
    }
    
    vector<sPatcher> Instance::getPatchers()
    {
        lock_guard<mutex> guard(m_patchers_mutex);
        return vector<sPatcher>(m_patchers.begin(), m_patchers.end());
    }
    
    void Instance::addListener(sListener listener)
    {
        m_listeners.add(listener);
    }
    
    void Instance::removeListener(sListener listener)
    {
        m_listeners.remove(listener);
    }
    
    sGuiWindow Instance::createWindow()
    {
        sWindow window(make_shared<Window>(getShared()));
        if(window)
        {
            window->addToDesktop();
        }
        return window;
    }
    
    // ================================================================================ //
    //                                  INSTANCE WINDOW                                 //
    // ================================================================================ //
    
    Instance::Window::Window(sInstance instance) : GuiWindow(instance),
    m_instance(instance)
    {
        setHeader(make_shared<GuiWindow::Header>(instance, "Kiwi Studio"));
        setContent(make_shared<GuiViewport>(instance));
    }
    
    void Instance::Window::viewCreated(sGuiView view) noexcept
    {
        sGuiContext ctxt(getContext());
        if(view && ctxt)
        {
            sGuiController ctrl(view->getController());
            if(ctrl)
            {
                //ctrl->setBounds(ctxt->getScreenBounds(ctrl->getBounds().centre()));
                ctrl->setBounds(Rectangle(30., 30., 800, 600));
            }
        }
    }
    
    void Instance::Window::patcherCreated(sInstance instance, sPatcher patcher) {}
    
    void Instance::Window::patcherRemoved(sInstance instance, sPatcher patcher) {}
    
    void Instance::Window::dspStarted(sInstance instance) {}
    
    void Instance::Window::dspStopped(sInstance instance) {}
}

