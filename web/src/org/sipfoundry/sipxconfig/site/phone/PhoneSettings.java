/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.site.phone;

import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.phone.Endpoint;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.PhoneContext;
import org.sipfoundry.sipxconfig.setting.SettingModel;

/**
 * Edit vendor specific phone setttings in abstract manor using setting model of meta
 * data
 */
public abstract class PhoneSettings extends BasePage implements PageRenderListener {
    
    public static final String PAGE = "PhoneSettings"; 

    public abstract int getPhoneId();
    
    /** REQUIRED PAGE PARAMETER */
    public abstract void setPhoneId(int id);
    
    public abstract String getModelName();
    
    /** REQUIRED PAGE PARAMETER */
    public abstract void setModelName(String name); 

    public abstract Phone getPhone();
    
    public abstract void setPhone(Phone phone);   
    
    public abstract SettingModel getSettingModel();
    
    public abstract void setSettingModel(SettingModel settings);
    
    public void pageBeginRender(PageEvent event) {
        PhoneContext context = PhonePageUtils.getPhoneContext(event.getRequestCycle());
        Phone phone = context.getPhone(getPhoneId()); 
        setPhone(phone);
        SettingModel model = phone.getSettingModel();
        SettingModel section = (SettingModel) model.getMeta(getModelName());
        Endpoint endpoint = phone.getEndpoint();
        SettingModel copy = section.populateCopy(endpoint.getSettings());
        setSettingModel(copy);
    }
}
