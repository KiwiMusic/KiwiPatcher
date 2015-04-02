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

#include "KiwiPatch.h"
#include "KiwiInstance.h"
#include "KiwiConsole.h"

namespace Kiwi
{    
    // ================================================================================ //
    //                                      PAGE                                        //
    // ================================================================================ //
    
    Patcher::Patcher(sInstance instance) : DspChain(instance), GuiPatcher(instance),
    m_instance(instance)
    {
        ;
    }
	
    Patcher::~Patcher()
    {
        m_links.clear();
        m_objects.clear();
        m_free_ids.clear();
    }
    
    sPatcher Patcher::create(sInstance instance, Dico& dico)
    {
        sPatcher patcher = make_shared<Patcher>(instance);
        if(patcher)
        {
            instance->DspContext::add(patcher);
			patcher->initialize();
			
            auto it = dico.find(Tag::List::patcher);
            if(it != dico.end())
            {
                patcher->add(it->second);
            }
        }
        return patcher;
    }
    
    void Patcher::createObject(Dico const& dico)
    {
        sObject object;
        if(dico.count(Tag::List::name) && dico.at(Tag::List::name).isTag() &&
           dico.count(Tag::List::text) && dico.at(Tag::List::text).isTag() &&
           dico.count(Tag::List::id) && dico.at(Tag::List::id).isNumber() &&
           dico.count(Tag::List::arguments) && dico.at(Tag::List::arguments).isVector())
        {
            const sTag text = dico.at(Tag::List::text);
            object = Factory::create(dico.at(Tag::List::name), Detail(getInstance(), getShared(), dico.at(Tag::List::id), dico.at(Tag::List::name), text->getName(), dico, dico.at(Tag::List::arguments)));
            if(object)
            {
                sDspNode dspnode = dynamic_pointer_cast<DspNode>(object);
                if(dspnode)
                {
                    DspChain::add(dspnode);
                }
                GuiPatcher::add(object);
                m_objects.push_back(object);
            }
        }
    }
    
    void Patcher::createLink(Dico const& dico)
    {
        if(dico.count(Tag::List::from) && dico.at(Tag::List::from).isVector() &&
           dico.count(Tag::List::to) && dico.at(Tag::List::to).isVector())
        {
            Vector const& vfrom  = dico.at(Tag::List::from);
            Vector const& vto    = dico.at(Tag::List::to);
            if(vfrom.size() > 1 && vto.size() > 1)
            {
                const sObject from = getObjectWithId(vfrom[0]);
                const sObject to   = getObjectWithId(vto[0]);
                if(from && to)
                {
                    const Object::sOutlet outlet  = from->getOutlet(vfrom[1]);
                    const Object::sInlet inlet    = to->getInlet(vto[1]);
                    if(outlet && inlet)
                    {
                        if(outlet->getType() >= Object::Io::Signal && inlet->getType() >= Object::Io::Signal)
                        {
                            sDspNode pfrom = dynamic_pointer_cast<DspNode>(from);
                            sDspNode pto   = dynamic_pointer_cast<DspNode>(to);
                            if(from && to)
                            {
                                ulong poutlet, pinlet;
                                try
                                {
                                    pinlet = from->getDspInletIndex(vfrom[1]);
                                }
                                catch(Error& e)
                                {
                                    Console::post(e.what());
                                    return;
                                }
                                
                                try
                                {
                                    poutlet = from->getDspOutletIndex(vto[1]);
                                }
                                catch(Error& e)
                                {
                                    Console::post(e.what());
                                    return;
                                }
                                
                                outlet->append(to, vfrom[1]);
                                inlet->append(from, vto[1]);
                                Object::Io::Type type = Object::Io::Signal;
                                if(outlet->getType() == Object::Io::Both && inlet->getType() == Object::Io::Both)
                                {
                                    type = Object::Io::Both;
                                }
                                shared_ptr<Link::SignalLink> link = make_shared<Link::SignalLink>(getShared(), from, vfrom[1], to, vto[1], type, pfrom, poutlet, pto, pinlet);
                                
                                DspChain::add(link);
                                m_links.push_back(static_pointer_cast<Link>(link));
                                GuiPatcher::add(link);

                            }
                        }
                        else if(outlet->getType() == inlet->getType() || inlet->getType() == Object::Io::Both || outlet->getType() == Object::Io::Both)
                        {
                            
                            outlet->append(to, vfrom[1]);
                            inlet->append(from, vto[1]);
                            sLink link = make_shared<Link>(getShared(), from, vfrom[1], to, vto[1], Object::Io::Message);
                            m_links.push_back(link);
                            GuiPatcher::add(link);
                        }
                    }
                }
                else
                {
                    throw Error("The dico isn't valid for a link creation.");
                }
            }
            else
            {
                throw Error("The dico isn't valid for a link creation.");
            }
        }
        else
        {
            throw Error("The dico isn't valid for a link creation.");
        }
    }
    
    void Patcher::add(Dico const& dico)
    {
        Dico rdico = dico;
        Vector objects, links;
        auto it = dico.find(Tag::List::objects);
        if(it != dico.end())
        {
            objects = it->second;
        }
        it = dico.find(Tag::List::links);
        if(it != dico.end())
        {
            links = it->second;
        }

        lock_guard<mutex> guard(m_mutex);
        for(vector<sObject>::size_type i = 0; i < objects.size(); i++)
        {
            Dico objdico(objects[i]);
            if(!objdico.empty())
            {
                const ulong r_id = objdico[Tag::List::id];
                const ulong n_id = m_free_ids.empty() ? m_objects.size() + 1 : m_free_ids[0];
                if(!m_free_ids.empty())
                {
                    m_free_ids.erase(m_free_ids.begin());
                }
                objdico[Tag::List::id] = (long)n_id;
                Vector atoms;
                for(vector<sLink>::size_type i = 0; i < links.size(); i++)
                {
                    atoms = objdico[Tag::List::from];
                    if(atoms.size() > 1 && r_id == (ulong)atoms[0])
                    {
                        objdico[Tag::List::from] = {(long)n_id, atoms[1]};
                    }
            
                    atoms = objdico[Tag::List::to];
                    if(atoms.size() > 1 && r_id == (ulong)atoms[0])
                    {
                        objdico[Tag::List::to] = {(long)n_id, atoms[1]};
                    }
                }
                createObject(objdico);
            }
        }
        
        for(vector<sLink>::size_type i = 0; i < links.size(); i++)
        {
            Dico linkdico(links[i]);
            if(!linkdico.empty())
            {
                createLink(linkdico);
            }
        }
    }
    
    void Patcher::remove(sObject object)
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_objects.begin(), m_objects.end(), object);
            if(it != m_objects.end())
            {
                for(auto li = m_links.begin(); li != m_links.end();)
                {
                    if((*li)->getObjectFrom() == object || (*li)->getObjectTo() == object)
                    {
                        sDspLink dsplink = dynamic_pointer_cast<DspLink>((*li));
                        if(dsplink)
                        {
                            DspChain::remove(dsplink);
                        }
                        GuiPatcher::remove(*li);
                    }
                    else
                    {
                        ++li;
                    }
                }
                sDspNode dspnode = dynamic_pointer_cast<DspNode>(object);
                if(dspnode)
                {
                    DspChain::remove(dspnode);
                }
                GuiPatcher::remove(object);
                m_objects.erase(it);
                m_free_ids.push_back(object->getId());
            }
        }
    }
    
    void Patcher::remove(sLink link)
    {
        if(link)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_links.begin(), m_links.end(), link);
            if(it != m_links.end())
            {
                sDspLink dsplink = dynamic_pointer_cast<DspLink>(link);
                if(dsplink)
                {
                    DspChain::remove(dsplink);
                }
                GuiPatcher::remove(link);
                m_links.erase(it);
            }
        }
    }
    
    void Patcher::toFront(sObject object)
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_objects.begin(), m_objects.end(), object);
            if(it != m_objects.end())
            {
                m_objects.erase(it);
                m_objects.push_back(object);
            }
        }
    }
    
    void Patcher::toBack(sObject object)
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            auto it = find(m_objects.begin(), m_objects.end(), object);
            if(it != m_objects.end())
            {
                m_objects.erase(it);
                m_objects.insert(m_objects.begin(), object);
            }
        }
    }
	
    void Patcher::write(Dico& dico) const
    {
        if(1)
        {
            /*
			Dico& subpatcher = Dico::create();
			if(subpatcher)
			{
                Vector atoms;
				lock_guard<mutex> guard(m_mutex);
                
				for(vector<sObject>::size_type i = 0; i < m_objects.size(); i++)
				{
					Dico& object = Dico::create();
					if(object)
					{
						m_objects[i]->write(object);
                        atoms.push_back(object);
					}
				}
				subpatcher->set(Tag::List::objects, atoms);
				
				atoms.clear();
				
				for(vector<sLink>::size_type i = 0; i < m_links.size(); i++)
				{
                    Dico& link = Dico::create();
					if(link)
					{
						m_links[i]->write(link);
						atoms.push_back(link);
					}
				}
				subpatcher->set(Tag::List::links, atoms);
				dico->set(Tag::List::patcher, subpatcher);
			}*/
        }
    }
}




